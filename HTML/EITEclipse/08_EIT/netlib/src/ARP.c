/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

/* Note: even if MN_ARP is set to 0 in mnconfig.h we still need a minimal
   ARP capability if using ethernet. This is implemented in function
   mn_arp_process_packet. Using the full ARP functionality is recommended.
*/

#include "micronet.h"

#if (MN_ETHERNET)

#if MN_ARP
ARP_INFO_T arp_info[MN_ARP_CACHE_SIZE];

static PARP_INFO mn_arp_search_entry(byte [IP_ADDR_LEN]) cmx_reentrant;
static PARP_INFO mn_arp_get_entry(void) cmx_reentrant;
static byte mn_arp_send_request(PSOCKET_INFO) cmx_reentrant;
static byte mn_arp_recv_reply(PSOCKET_INFO) cmx_reentrant;
#endif

/************************************************************************/

/* called from ether_recv_header to check if we got a valid arp packet,
   and if so handle it. on entry the sender's hw addr is in
   eth_dest_hw_addr and the protocol type has been read. the recv_out_ptr
   is pointing to the start of the ARP data. returns 0 if we processed a
   valid arp packet, otherwise returns -1.
*/
SCHAR mn_arp_process_packet(void)
cmx_reentrant {
   byte i, arp_code;
   byte src_ip[IP_ADDR_LEN];
   byte dest_ip[IP_ADDR_LEN];
   byte dest_hw[ETH_ADDR_LEN];
   SCHAR retval;
   SCHAR socket_no;
   PSOCKET_INFO socket_ptr;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   retval = -1;

   /* hardware type correct ? */
   if ( (mn_recv_byte() == ETHERNET_10MB_HB) && \
         (mn_recv_byte() == ETHERNET_10MB_LB) )
      {
      /* protocol space correct ? */
      if ( (mn_recv_byte() == ETH_TYPE_HB) && \
            (mn_recv_byte() == ETH_IP_LB) )
         {
         /* lengths correct ? */
         if ( (mn_recv_byte() == ETH_ADDR_LEN) && \
               (mn_recv_byte() == IP_ADDR_LEN) )
            {
            /* check arp opcode */
            if (mn_recv_byte() == ARP_CODE_HB)
               {
               arp_code = (byte)(mn_recv_byte());
               switch (arp_code)
                  {
                  case ARP_REQUEST:
                     /* save dest hw addr */
                     for (i=0;i<ETH_ADDR_LEN;i++)
                        dest_hw[i] = (byte)(mn_recv_byte());
                     /* save dest ip addr */
                     for (i=0;i<IP_ADDR_LEN;i++)
                        dest_ip[i] = (byte)(mn_recv_byte());
                     /* skip over src hw addr */
                     for (i=0;i<ETH_ADDR_LEN;i++)
                        mn_recv_byte();
                     /* is request for our IP addr ? */
                     for (i=0;i<IP_ADDR_LEN;i++)
                        src_ip[i] = (byte)(mn_recv_byte());
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
                     MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
                     if ( (src_ip[0] == interface_ptr->ip_src_addr[0]) && \
                           (src_ip[1] == interface_ptr->ip_src_addr[1]) && \
                           (src_ip[2] == interface_ptr->ip_src_addr[2]) && \
                           (src_ip[3] == interface_ptr->ip_src_addr[3]) )
                        {
#if MN_ARP
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
                        MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
                        /* update arp cache */
                        mn_arp_update(dest_ip, dest_hw);
#else
                        /* save dest_ip and hw addr.*/
                        for (i=0;i<IP_ADDR_LEN;i++)
                           interface_ptr->ip_dest_addr[i] = dest_ip[i];

                        for (i=0;i<ETH_ADDR_LEN;i++)
                           eth_dest_hw_addr[i] = dest_hw[i];
                        MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif
                        /* valid request for us so send a reply */
                        socket_no = mn_open_socket(dest_ip,ARP_PORT,ARP_PORT,\
                              PROTO_IP,S_ARP_TYPE,0,0,eth_interface_no);
                        if (socket_no >= 0)
                           {
#if (MN_DHCP)
                           if (((bootpMode != BOOTP_ACTIVE) || \
                                 (dhcp_lease.dhcp_state >= DHCP_OK)) && \
                                 mn_transmit_ready(eth_interface_no))
#else
                           if (mn_transmit_ready(eth_interface_no))
#endif      /* (MN_DHCP) */
                              {
                              socket_ptr = MK_SOCKET_PTR(socket_no);
                              start_arp_packet(socket_ptr,FALSE);
                              mn_send_byte(ARP_CODE_HB);
                              mn_send_byte(ARP_REPLY);
                              /* send our hw and ip addresses */
                              MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
                              for (i=0;i<ETH_ADDR_LEN;i++)
                                 mn_send_byte(eth_src_hw_addr[i]);
                              MN_MUTEX_RELEASE(MUTEX_MN_INFO);
                              for (i=0;i<IP_ADDR_LEN;i++)
                                 mn_send_byte(src_ip[i]);
                              /* copy sender's hw and ip addresses */
                              for (i=0;i<ETH_ADDR_LEN;i++)
                                 mn_send_byte(socket_ptr->eth_dest_hw_addr[i]);
                              for (i=0;i<IP_ADDR_LEN;i++)
                                 mn_send_byte(socket_ptr->ip_dest_addr[i]);
                              MN_TASK_LOCK;
                              if (mn_close_packet(socket_ptr, 0) > 0)
                                 retval = 0;
                              MN_TASK_UNLOCK;
                              }
                           (void)mn_abort(socket_no);
                           }
                        }
                     else
                        {
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
                        MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
                        }

                     break;
                  case ARP_REPLY:
#if MN_ARP
                     /* save dest hw addr */
                     for (i=0;i<ETH_ADDR_LEN;i++)
                        dest_hw[i] = (byte)(mn_recv_byte());
                     /* save dest ip addr */
                     for (i=0;i<IP_ADDR_LEN;i++)
                        dest_ip[i] = (byte)(mn_recv_byte());
                     /* skip over our hw addr */
                     for (i=0;i<ETH_ADDR_LEN;i++)
                        mn_recv_byte();
                     /* is reply for our IP addr ? */
                     for (i=0;i<IP_ADDR_LEN;i++)
                        src_ip[i] = (byte)(mn_recv_byte());
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
                     MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
                     if ( (src_ip[0] == interface_ptr->ip_src_addr[0]) && \
                           (src_ip[1] == interface_ptr->ip_src_addr[1]) && \
                           (src_ip[2] == interface_ptr->ip_src_addr[2]) && \
                           (src_ip[3] == interface_ptr->ip_src_addr[3]) )
                        {
                        /* update arp cache */
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
                        MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
                        mn_arp_update(dest_ip, dest_hw);
                        retval = 0;
#if (RTOS_USED != RTOS_NONE)
                        /* send a signal to the waiting ARP socket that
                           we got a reply.
                        */
                        socket_ptr = mn_find_socket(ARP_PORT, ARP_PORT, \
                           dest_ip, PROTO_IP, eth_interface_no);
                        if ((socket_ptr != PTR_NULL) && \
                              SOCKET_ACTIVE(socket_ptr->socket_no))
                           MN_SIGNAL_POST(signal_socket[socket_ptr->socket_no]);
#endif      /* (RTOS_USED != RTOS_NONE) */
                        }
                     else
                        {
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
                        MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
                        }
#endif
                     break;
                  default:
                     /* unknown opcode */
                     break;
                  }
               }
            }
         }
      }

   return (retval);
}

/************************************************************************/

#if MN_ARP
/* initialize arp structure */
void mn_arp_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_ARP_CACHE,INFINITE_WAIT);
   MEMSET(arp_info, 0, sizeof(arp_info));
   MN_MUTEX_RELEASE(MUTEX_ARP_CACHE);
}

void mn_arp_update(byte ip[], byte hw[])
cmx_reentrant {
   PARP_INFO parp;
   byte i;

   MN_MUTEX_WAIT(MUTEX_ARP_CACHE,INFINITE_WAIT);
   /* get a table entry, using the current one if one exists */
   parp = mn_arp_search_entry(ip);
   if (parp == PTR_NULL)
      parp = mn_arp_get_entry();

   /* make sure no one is trying to update the entry at the same time. */
#if (RTOS_USED == RTOS_NONE)
   if (!(parp->flag & ARP_ENTRY_IN_USE))
      {
      parp->flag |= ARP_ENTRY_IN_USE;
#endif      /* (RTOS_USED == RTOS_NONE) */

      /* add the passed info to the table */
      for (i=0;i<IP_ADDR_LEN;i++)
         parp->eth_dest_ip_addr[i] = ip[i];

      for (i=0;i<ETH_ADDR_LEN;i++)
         parp->eth_dest_hw_addr[i] = hw[i];

#if MN_ARP_TIMEOUT
      parp->time_to_live = MN_ARP_KEEP_TICKS;
#endif      /* MN_ARP_TIMEOUT */

#if (RTOS_USED == RTOS_NONE)
      parp->flag &= (byte)(~ARP_ENTRY_IN_USE);
      }
#endif      /* (RTOS_USED == RTOS_NONE) */
   MN_MUTEX_RELEASE(MUTEX_ARP_CACHE);
}

#if MN_ARP_TIMEOUT
/* called from the timer interrupt to decrement the timers in the arp_info
   structure. RFC 826 says that timeouts are desirable but not required.
*/
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif      /* defined(__C51__) */
#if ( defined(POLECOG1) || defined(POLECOG1E) )
void __irq_code mn_arp_update_timer(void)
#elif (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_arp_update_timer(void)
#else
void mn_arp_update_timer(void)
#endif
cmx_reentrant {
   byte i;        /* assumes cache size will be less than 256 */

#if (RTOS_USED != RTOS_NONE)
   if (MN_MUTEX_GET(MUTEX_ARP_CACHE) == MUTEX_SUCCESS)
      {
#endif      /* (RTOS_USED != RTOS_NONE) */
      for (i=0;i<MN_ARP_CACHE_SIZE;i++)
         {
#if (RTOS_USED == RTOS_NONE)
         if ( (arp_info[i].time_to_live > 0) && \
               !(arp_info[i].flag & ARP_ENTRY_IN_USE) )
#else
         if ( arp_info[i].time_to_live > 0 )
#endif      /* (RTOS_USED == RTOS_NONE) */
            arp_info[i].time_to_live--;
         }
#if (RTOS_USED != RTOS_NONE)
      MN_MUTEX_RELEASE(MUTEX_ARP_CACHE);
      }
#endif      /* (RTOS_USED != RTOS_NONE) */
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif      /* defined(__C51__) */
#endif      /* MN_ARP_TIMEOUT */

/* look for an arp entry corresponding to the passed ip address. if a valid
   entry is found in the table return a pointer to it, otherwise send out
   up to MN_ARP_RESEND_TRYS ARP requests. If we get a reply update the arp cache
   and return a pointer to the updated entry. If we don't get a reply return
   PTR_NULL.
*/
PARP_INFO mn_arp_lookup(byte ip[])
cmx_reentrant {
   PARP_INFO parp;
   byte i;
   SCHAR socket_no;
   PSOCKET_INFO socket_ptr;

   /* look for existing entry */
   MN_MUTEX_WAIT(MUTEX_ARP_CACHE,INFINITE_WAIT);
   parp = mn_arp_search_entry(ip);
   MN_MUTEX_RELEASE(MUTEX_ARP_CACHE);
#if MN_ARP_TIMEOUT
   if ((parp == PTR_NULL) || (!parp->time_to_live))
#else
   if (parp == PTR_NULL)
#endif
      {
      parp = PTR_NULL;
      socket_no = mn_open_socket(ip,ARP_PORT,ARP_PORT,PROTO_IP,S_ARP_TYPE,0, \
         0,eth_interface_no);
      if (socket_no >= 0)
         {
         socket_ptr = MK_SOCKET_PTR(socket_no);
         for (i=0;i<MN_ARP_RESEND_TRYS;i++)
            {
            /* send out an arp request */
            if (!mn_arp_send_request(socket_ptr))
               break;
            /* wait for a reply */
            if (mn_arp_recv_reply(socket_ptr))
               {
               MN_MUTEX_WAIT(MUTEX_ARP_CACHE,INFINITE_WAIT);
               parp = mn_arp_search_entry(ip);
               MN_MUTEX_RELEASE(MUTEX_ARP_CACHE);
#if MN_ARP_TIMEOUT
               if ((parp != PTR_NULL) && parp->time_to_live)
#else
               if (parp != PTR_NULL)
#endif
                  break;
#if MN_ARP_TIMEOUT
               else
                  parp = PTR_NULL;
#endif
               }
            }        /* end of for loop */

         (void)mn_abort(socket_no);
         }
      }

   return (parp);
}

/************************************************************************/

/* searches for ip address in arp cache and return pointer to arp cache entry
   if successful or PTR_NULL if not.
*/
static PARP_INFO mn_arp_search_entry(byte ip[])
cmx_reentrant {
   byte i;        /* assumes cache size will be less than 256 */

   for (i=0;i<MN_ARP_CACHE_SIZE;i++)
      {
      if (memcmp((void *)ip, (void *)arp_info[i].eth_dest_ip_addr, IP_ADDR_LEN) == 0)
         return (&arp_info[i]);
      }

   return (PTR_NULL);
}

/* gets an entry to the arp cache table, using an existing spot if
   neccessary. Get MUTEX_ARP_CACHE before calling this function.
*/
static PARP_INFO mn_arp_get_entry(void)
cmx_reentrant {
   byte i;        /* assumes cache size will be less than 256 */

   for (i=0;i<MN_ARP_CACHE_SIZE;i++)
      {
#if MN_ARP_TIMEOUT
      /* check for an expired entry */
      if (!(arp_info[i].time_to_live))
         return (&arp_info[i]);
#endif      /* MN_ARP_TIMEOUT */
      /* there should be no valid IP addresses starting with 0 */
      if (arp_info[i].eth_dest_ip_addr[0] == 0)
         return (&arp_info[i]);
      }

   /* we need to replace an existing entry, so pick one randomly. */
   i = (byte)(MN_GET_TICK % MN_ARP_CACHE_SIZE);
#if (RTOS_USED == RTOS_NONE)
   while (1)
      {
      /* make sure the entry is not being used */
      if (!(arp_info[i].flag & ARP_ENTRY_IN_USE))
         break;
      ++i;
      i = (byte)(i % MN_ARP_CACHE_SIZE);
      }
#endif      /* (RTOS_USED == RTOS_NONE) */

   return (&arp_info[i]);
}

/* send an ARP request. returns 1 if able to send, otherwise returns 0 */
static byte mn_arp_send_request(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   byte i;
   byte retval;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   retval = 0;

#if (MN_DHCP)
   if ((bootpMode != BOOTP_ACTIVE) && mn_transmit_ready(eth_interface_no))
#else
   if (mn_transmit_ready(eth_interface_no))
#endif      /* (MN_DHCP) */
      {
      start_arp_packet(socket_ptr,TRUE);
      mn_send_byte(ARP_CODE_HB);
      mn_send_byte(ARP_REQUEST);
      MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
      /* send our hw and ip addresses */
      for (i=0;i<ETH_ADDR_LEN;i++)
         mn_send_byte(eth_src_hw_addr[i]);
      MN_MUTEX_RELEASE(MUTEX_MN_INFO);
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      for (i=0;i<IP_ADDR_LEN;i++)
         mn_send_byte(interface_ptr->ip_src_addr[i]);
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      /* desired hw and ip addresses */
      for (i=0;i<ETH_ADDR_LEN;i++)
         mn_send_byte(0x00);
      for (i=0;i<IP_ADDR_LEN;i++)
         mn_send_byte(socket_ptr->ip_dest_addr[i]);
      MN_TASK_LOCK;
      if (mn_close_packet(socket_ptr, 0) > 0)
         retval = 1;
      MN_TASK_UNLOCK;
      }

   return (retval);
}

/* wait for an ARP reply. returns 1 if we got an arp reply, otherwise
   returns 0.
*/
static byte mn_arp_recv_reply(PSOCKET_INFO socket_ptr)
cmx_reentrant {
#if (RTOS_USED == RTOS_NONE)
   byte packet_type;
   TIMER_INFO_T sock_timer;

   socket_ptr=socket_ptr;

   mn_reset_timer(&sock_timer, (MN_ARP_WAIT_TICKS));
   while (!mn_timer_expired(&sock_timer))
      {
      /* loop until we get a arp packet or timeout */
      packet_type = mn_ip_get_pkt(eth_interface_no);
      if (packet_type & ARP_TYPE)
         return 1;
      else if (packet_type && !(packet_type & UNKNOWN_TYPE))
         {
         /* Note: if (!packet_type || (packet_type & UNKNOWN_TYPE)) then
            we already called mn_ip_discard_packet() in mn_ip_get_pkt()
         */
         mn_ip_discard_packet();       /* get rid of it. */
         }
      }

   return (0);
#else
   if (MN_SIGNAL_WAIT(signal_socket[socket_ptr->socket_no],(MN_ARP_WAIT_TICKS)) \
         == SIGNAL_SUCCESS)
      return (1);
   else
      return (0);
#endif      /* (RTOS_USED == RTOS_NONE) */
}

#endif      /* MN_ARP */

#endif      /* (MN_ETHERNET) */
