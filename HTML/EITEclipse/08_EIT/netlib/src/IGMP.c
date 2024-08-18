/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if (MN_IGMP)
IGMP_INFO_T igmp_info[MN_IGMP_LIST_SIZE];

static SCHAR igmp_send_report(PIGMP_INFO pigmp) cmx_reentrant;
static PIGMP_INFO igmp_get_entry(void) cmx_reentrant;
static word16 igmp_checksum(byte type, byte mrt, byte *groupaddr) cmx_reentrant;
static void igmp_set_timer(PIGMP_INFO pigmp) cmx_reentrant;

/************************************************************************/

/* Initialize igmp structure */
void mn_igmp_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
   MEMSET(igmp_info, 0, sizeof(igmp_info));
   MN_MUTEX_RELEASE(MUTEX_IGMP);
}

/* Adds an entry to the igmp list and sends a membership report if needed.
   Returns positive number if successful negative number if not.
*/
SCHAR mn_igmp_join(byte ip[], SCHAR interface_no)
cmx_reentrant {
   PIGMP_INFO pigmp;
   byte i;
   SCHAR retval;

   retval = IGMP_ERROR;

   if (((ip[0] >= 224) && (ip[0] <= 239)) && \
         memcmp((void *)ip,(void *)all_hosts_addr,IP_ADDR_LEN) != 0)
      {
      MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
      /* get a table entry */
      pigmp = mn_igmp_search_entry(ip, interface_no);
      if (pigmp != PTR_NULL)
         {
         /* The entry already exists, just increment reference count. */
         pigmp->ref_count++;
         retval = 2;
         }
      else
         {
         /* Add a new entry if there is room. */
         pigmp = igmp_get_entry();
         if (pigmp != PTR_NULL)
            {
            for (i=0;i<IP_ADDR_LEN;i++)
               pigmp->ip_group_addr[i] = ip[i];
            pigmp->igmp_state = IGMP_NON_MEMBER;
            pigmp->igmp_timer = 0;
            pigmp->ref_count = 1;
            pigmp->interface_no = interface_no;
            retval = 1;
            }
         }
      MN_MUTEX_RELEASE(MUTEX_IGMP);

      if (retval == 1)
         {
         retval = igmp_send_report(pigmp);
         if (retval > 0)
            {
            MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
            igmp_set_timer(pigmp);
            MN_MUTEX_RELEASE(MUTEX_IGMP);
            }
         else
            {
            /* We were unable to send the report so remove the entry. */
            MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
            pigmp->ip_group_addr[0] = 0;
            pigmp->ref_count = 0;
            MN_MUTEX_RELEASE(MUTEX_IGMP);
            }
         }
      }

   return (retval);
}

/* Decrements the reference count of the igmp list entry.
   Returns positive number if successful negative number if not.
*/
SCHAR mn_igmp_leave(byte ip[], SCHAR interface_no)
cmx_reentrant {
   PIGMP_INFO pigmp;
   SCHAR retval;

   retval = IGMP_ERROR;

   if (memcmp((void *)ip,(void *)all_hosts_addr,IP_ADDR_LEN) != 0)
      {
      MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
      /* get a table entry */
      pigmp = mn_igmp_search_entry(ip, interface_no);
      if ((pigmp != PTR_NULL) && pigmp->ref_count && \
            ((pigmp->igmp_state == IGMP_DELAY_MEMBER) || \
            (pigmp->igmp_state == IGMP_IDLE_MEMBER)))
         {
         /* The entry exists and is in the proper state. */
         pigmp->ref_count--;
         if (pigmp->ref_count == 0)
            pigmp->ip_group_addr[0] = 0;
         retval = 1;
         }
      MN_MUTEX_RELEASE(MUTEX_IGMP);
      }

   return (retval);
}

/* Processes a received IGMP packet. */
void mn_igmp_process_packet(void)
cmx_reentrant {
   byte type, mrt, csumh, csuml;
   byte group_ip[IP_ADDR_LEN];
   word16 igmp_csum, i;
   byte bad_packet;
   PIGMP_INFO pigmp;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

   bad_packet = FALSE;

   type = (byte)(mn_recv_escaped_byte(TRUE));

   if (((type != IGMP_QUERY) && (type != IGMP_V1_REPORT)) || \
         (interface_ptr->ip_recv_len < IGMP_LEN))
      bad_packet = TRUE;            /* Not an IGMP v1 packet */

   if (!bad_packet)
      {
      /* Read the rest of the IGMP data */
      mrt = (byte)(mn_recv_escaped_byte(TRUE));
      csumh = (byte)(mn_recv_escaped_byte(TRUE));
      csuml = (byte)(mn_recv_escaped_byte(TRUE));
      for (i=0; i<IP_ADDR_LEN; i++)
         group_ip[i] = (byte)(mn_recv_escaped_byte(TRUE));

      /* The checksum must match the calculated checksum and
         IGMP queries must be sent to the all-hosts IP address and
         for reports the group IP address must match the destination IP
         address the packet was sent to.
      */
      igmp_csum = igmp_checksum(type, mrt, group_ip);
      if ((csumh != HIGHBYTE(igmp_csum)) || (csuml != LOWBYTE(igmp_csum)))
         bad_packet = TRUE;
      else if ((type == IGMP_QUERY) && \
            memcmp((void *)interface_ptr->recv_dest_addr,(void *)all_hosts_addr,IP_ADDR_LEN) != 0)
         bad_packet = TRUE;
      else if ((type == IGMP_V1_REPORT) && \
            memcmp((void *)group_ip,(void *)interface_ptr->recv_dest_addr,IP_ADDR_LEN) != 0)
         bad_packet = TRUE;
      }

   if (!bad_packet)
      {
      if (type == IGMP_QUERY)
         {
         /* If a query is received then set report timers for all
            entries in our igmp list that are in the IDLE state.
         */
         MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
         for (i=0;i<MN_IGMP_LIST_SIZE;i++)
            {
            pigmp = &igmp_info[i];
            if (pigmp->igmp_state == IGMP_IDLE_MEMBER)
               igmp_set_timer(pigmp);
            }
         MN_MUTEX_RELEASE(MUTEX_IGMP);
         }
      else if (type == IGMP_V1_REPORT)
         {
         /* Stop the report timer for the group IP address if it is in our
            igmp list and is in the DELAY state.
         */
         MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
         pigmp = mn_igmp_search_entry(group_ip, recv_interface_no);
         if ((pigmp != PTR_NULL) && (pigmp->igmp_state == IGMP_DELAY_MEMBER))
            {
            /* The entry exists and is in the proper state. */
            pigmp->igmp_state = IGMP_IDLE_MEMBER;
            pigmp->igmp_timer = 0;
            }
         MN_MUTEX_RELEASE(MUTEX_IGMP);
         }
      }
}

/* Searches for ip address in igmp list and return pointer to igmp list entry
   if successful or PTR_NULL if not.
   Get MUTEX_IGMP before calling this function.
*/
PIGMP_INFO mn_igmp_search_entry(byte ip[], SCHAR interface_no)
cmx_reentrant {
   word16 i;

   for (i=0;i<MN_IGMP_LIST_SIZE;i++)
      {
      if ((memcmp((void *)ip, (void *)igmp_info[i].ip_group_addr, IP_ADDR_LEN) == 0) && \
            igmp_info[i].interface_no == interface_no)
         return (&igmp_info[i]);
      }

   return (PTR_NULL);
}

#if (RTOS_USED == RTOS_NONE)
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif      /* defined(__C51__) */
/* Update the report timers. Called from tcp_timer ISR. */
#if ( defined(POLECOG1) || defined(POLECOG1E) )
void __irq_code mn_igmp_update_timers(void)
#elif (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_igmp_update_timers(void)
#else
void mn_igmp_update_timers(void)
#endif
cmx_reentrant {
   PIGMP_INFO pigmp;
   word16 i;

   for (i=0;i<MN_IGMP_LIST_SIZE;i++)
      {
      pigmp = &igmp_info[i];
      if ((pigmp->igmp_state == IGMP_DELAY_MEMBER) && pigmp->igmp_timer)
         pigmp->igmp_timer--;
      }
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif      /* defined(__C51__) */

/* Send reports if timers have expired. */
void mn_igmp_check_timers(void)
cmx_reentrant {
   PIGMP_INFO pigmp;
   word16 i;

   for (i=0;i<MN_IGMP_LIST_SIZE;i++)
      {
      pigmp = &igmp_info[i];
      if ((pigmp->igmp_state == IGMP_DELAY_MEMBER) && !pigmp->igmp_timer)
         {
         if (igmp_send_report(pigmp))
            {
            MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
            pigmp->igmp_state = IGMP_IDLE_MEMBER;
            MN_MUTEX_RELEASE(MUTEX_IGMP);
            }
         }
      }
}
#else
/* Update the report timers and send reports if needed.
   Signalled from tcp_timer ISR.
*/
void mn_igmp_timer_task(void)
cmx_reentrant {
   PIGMP_INFO pigmp;
   word16 i;

   while (1)
      {
      if (MN_SIGNAL_WAIT(SIGNAL_IGMP,INFINITE_WAIT) == SIGNAL_SUCCESS)
         {
         for (i=0;i<MN_IGMP_LIST_SIZE;i++)
            {
            pigmp = &igmp_info[i];
            MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
            /* update timer */
            if (pigmp->igmp_state == IGMP_DELAY_MEMBER)
               {
               if (pigmp->igmp_timer)
                  pigmp->igmp_timer--;

               /* send report if timer is zero. */
               if (!pigmp->igmp_timer)
                  {
                  MN_MUTEX_RELEASE(MUTEX_IGMP);
                  if (igmp_send_report(pigmp))
                     {
                     MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
                     pigmp->igmp_state = IGMP_IDLE_MEMBER;
                     }
                  }
               }
            MN_MUTEX_RELEASE(MUTEX_IGMP);
            }     /* for */
         }        /* MN_SIGNAL_WAIT */
      }           /* while (1) */
}
#endif      /* (RTOS_USED == RTOS_NONE) */

/************************************************************************/

/* Sends an IGMP report for the passed entry. Returns 1 if successful or
   negative number on error.
*/
static SCHAR igmp_send_report(PIGMP_INFO pigmp)
cmx_reentrant {
   SCHAR socket_no;
   PSOCKET_INFO socket_ptr;
   SCHAR retval;
   SCHAR i;
   word16 igmp_csum;

   retval = IGMP_ERROR;

   if (pigmp != PTR_NULL)
      {
      /* open a socket to send the igmp report */
      socket_no = mn_open_socket(pigmp->ip_group_addr, 0, 0, PROTO_IGMP, \
         STD_TYPE, 0, 0, pigmp->interface_no);

      if (socket_no >= 0)
         {
         socket_ptr = MK_SOCKET_PTR(socket_no);  /* get pointer to the socket */

         igmp_csum = igmp_checksum(IGMP_V1_REPORT, IGMP_MAX_RESP_TIME, \
            socket_ptr->ip_dest_addr);

         /* start sending the reply */
         if (mn_ip_send_header(socket_ptr, PROTO_IGMP, IGMP_LEN) == 1)
            {
            mn_send_escaped_byte(IGMP_V1_REPORT,TRUE);  
            mn_send_escaped_byte(IGMP_MAX_RESP_TIME,TRUE);
            mn_send_escaped_byte(HIGHBYTE(igmp_csum),TRUE);
            mn_send_escaped_byte(LOWBYTE(igmp_csum),TRUE);
            for (i=0; i<IP_ADDR_LEN; i++)
               mn_send_escaped_byte(socket_ptr->ip_dest_addr[i],TRUE);
            MN_TASK_LOCK;
            (void)mn_close_packet(socket_ptr, 0);
            MN_TASK_UNLOCK;
            retval = 1;
            }

         (void)mn_abort(socket_no);
         }
      }

   return (retval);
}

/* Returns an entry to the igmp table or PTR_NULL is none is available.
   Get MUTEX_IGMP before calling this function.
*/
static PIGMP_INFO igmp_get_entry(void)
cmx_reentrant {
   word16 i;

   for (i=0;i<MN_IGMP_LIST_SIZE;i++)
      {
      if (igmp_info[i].ip_group_addr[0] == 0)
         return (&igmp_info[i]);
      }

   return (PTR_NULL);
}

/* Calculate IGMP checksum */
static word16 igmp_checksum(byte type, byte mrt, byte *groupaddr)
cmx_reentrant {
   word32 csum;

   csum = CSUM_WORD16(type,mrt);
   csum += CSUM_WORD16(groupaddr[0],groupaddr[1]);
   csum += CSUM_WORD16(groupaddr[2],groupaddr[3]);

   return (mn_udp_tcp_end_checksum(csum));
}

/* Sets the report timer with a pseudo-random delay time.
   Get MUTEX_IGMP before calling this function.
*/
static void igmp_set_timer(PIGMP_INFO pigmp)
cmx_reentrant {
   word16 delay_time;

   delay_time = (MN_GET_TICK & 0xfff) + interface_info[0].ip_src_addr[3] + 1;
   if (delay_time > IGMP_DELAY)
      delay_time = IGMP_DELAY;

   pigmp->igmp_state = IGMP_DELAY_MEMBER;
   pigmp->igmp_timer = delay_time;
}
#endif      /* (MN_IGMP) */
