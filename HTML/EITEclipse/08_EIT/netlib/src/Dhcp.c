/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

#include "micronet.h"

#if (MN_DHCP && MN_ETHERNET)

/* Global mode variable.  Set to true, dhcp is actively seeking broadcast
   packets. Set to false, dhcp is inactive.  The bootpMode variable is used
   in other IP functions for checking checksums and filtering packets.
*/

/* Changes marked ##SR courtesy of Dr. Stefan Ritt. */

SCHAR bootpMode;

DHCP_INFO_T dhcp_info;
DHCP_LEASE_T dhcp_lease;

static int mn_dhcp_xchg_packet(byte *, word32) cmx_reentrant;
static word32 make_dhcp_packet(byte *, word32) cmx_reentrant;
static byte *find_tag_type(byte) cmx_reentrant;
static byte get_msg_type(void) cmx_reentrant;
static word32 get_lease_time(void) cmx_reentrant;
static byte * set_lease_time(byte *,word32) cmx_reentrant;
static void copy_server_id(void) cmx_reentrant;
static byte * set_client_id(byte *) cmx_reentrant;
static byte * set_server_id(byte *) cmx_reentrant;
static byte * set_param_list(byte *) cmx_reentrant;
static void copy_subnet_mask(void) cmx_reentrant;
static void copy_gateway(void) cmx_reentrant;
#if (MN_DNS)
static void copy_dns_addr(PINTERFACE_INFO) cmx_reentrant;
#endif      /* (MN_DNS) */

/************************************************************************/
/* initialize dhcp structures */
void mn_dhcp_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_BOOTP_DHCP,INFINITE_WAIT);
   bootpMode = BOOTP_INACTIVE;
   MEMSET((void *)&dhcp_info, 0, sizeof(dhcp_info));
   MEMSET((void *)&dhcp_lease, 0, sizeof(dhcp_lease));
   MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);
}

/* get IP address and lease time when booting up */
int mn_dhcp_start(byte *filename, word32 req_lease_time)
cmx_reentrant {
   int retval;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);
   if (!(interface_ptr->interface_status & IF_OPEN))
      return (INTERFACE_NOT_FOUND);

   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
   MEMSET((void *)interface_ptr->ip_src_addr, 0, IP_ADDR_LEN);
   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
   MN_MUTEX_WAIT(MUTEX_BOOTP_DHCP,INFINITE_WAIT);
   dhcp_lease.dhcp_state = DHCPDISCOVER;
   retval = mn_dhcp_xchg_packet(filename, req_lease_time);
   MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);

#if (RTOS_USED != RTOS_NONE)
   if ((retval == 1) && (K_Task_Start(mn_dhcp_lease_id) != TASK_SUCCESS))
      retval = TASK_DID_NOT_START;
#endif      /* (RTOS_USED != RTOS_NONE) */

   return (retval);
}

/* Release the IP address. */
int mn_dhcp_release(void)
cmx_reentrant {
   int retval;

   retval = 0;

   MN_MUTEX_WAIT(MUTEX_BOOTP_DHCP,INFINITE_WAIT);
   if (dhcp_lease.dhcp_state != DHCP_DEAD)
      {
      dhcp_lease.dhcp_state = DHCPRELEASE;
      retval = mn_dhcp_xchg_packet(PTR_NULL, 0);
      dhcp_lease.dhcp_state = DHCP_DEAD;
      }
   MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);

   return (retval);
}

/* Renew the IP address. When using an RTOS this function should only
   be called from dhcp_lease_task.
*/
int mn_dhcp_renew(word32 req_lease_time)
cmx_reentrant {
   int retval;

   if (!(dhcp_lease.infinite_lease) && (dhcp_lease.dhcp_state >= DHCP_OK))
      {
      /* the dhcp_state here should be DHCP_RENEWING or DHCP_REBINDING */     
      retval = mn_dhcp_xchg_packet(PTR_NULL, req_lease_time);

      /* if the renew fails try once more each, DHCP_RENEWING and
         DHCP_REBINDING, at half the current interval.
      */
      if (retval != 1)
         {
         if ((dhcp_lease.dhcp_state == DHCP_RENEWING) && \
               ((dhcp_lease.org_lease_time / 2) == dhcp_lease.t1_renew_time))
            dhcp_lease.t1_renew_time = dhcp_lease.t1_renew_time / 2;
         else if ((dhcp_lease.dhcp_state == DHCP_REBINDING) && \
               ((dhcp_lease.org_lease_time / 8) == dhcp_lease.t2_renew_time))
            dhcp_lease.t2_renew_time = dhcp_lease.t2_renew_time / 2;
         }

      /* set state to DHCP_OK no matter what the return value so that
         mn_dhcp_renew won't be called continuously if the first call fails.
      */
      dhcp_lease.dhcp_state = DHCP_OK;
      }
   else
      retval = 1;

   return (retval);
}

/* called from timer ISR to update DHCP lease timers and change dhcp_states
   if necessary.
*/
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif      /* defined(__C51__) */
/* This function is called from the timer ISR if an RTOS is not used. It is
   called from mn_timer_update_task if an RTOS is used.
*/
#if (defined(POLECOG1) || defined(POLECOG1E))
void __irq_code mn_dhcp_update_timer(void)
#elif (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_dhcp_update_timer(void)
#else
void mn_dhcp_update_timer(void)
#endif
cmx_reentrant {
   static word16 count = 0;      /* used to convert timer-ticks to seconds */

#if (RTOS_USED != RTOS_NONE)
   if (MN_MUTEX_GET(MUTEX_BOOTP_DHCP) == MUTEX_SUCCESS)
      {
#endif      /* (RTOS_USED != RTOS_NONE) */
      if (!(dhcp_lease.infinite_lease) && (dhcp_lease.dhcp_state >= DHCP_OK))
         {
         count++;

         if (count == (MN_ONE_SECOND))
            {
            count = 0;
            dhcp_lease.lease_time--;
            }

         if (dhcp_lease.lease_time <= dhcp_lease.t1_renew_time)
            {
            if (dhcp_lease.lease_time == 0)
               dhcp_lease.dhcp_state = DHCP_DEAD;
            else if (dhcp_lease.lease_time <= dhcp_lease.t2_renew_time)
               dhcp_lease.dhcp_state = DHCP_REBINDING;
            else if (dhcp_lease.lease_time <= dhcp_lease.t1_renew_time)
               dhcp_lease.dhcp_state = DHCP_RENEWING;
            }
         }
#if (RTOS_USED != RTOS_NONE)
      MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);
      }
#endif      /* (RTOS_USED != RTOS_NONE) */
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif      /* defined(__C51__) */

#if (RTOS_USED != RTOS_NONE)
/* This function is called every DHCP_WAIT_TIME system ticks to renew the
   DHCP lease if needed.
*/
void mn_dhcp_lease_task(void)
cmx_reentrant {
   while (1)
      {
      if (bootpMode == BOOTP_INACTIVE)
         {
         if (MN_MUTEX_GET(MUTEX_BOOTP_DHCP) == MUTEX_SUCCESS)
            {
            if ((dhcp_lease.dhcp_state == DHCP_RENEWING) || \
                  (dhcp_lease.dhcp_state == DHCP_REBINDING))
               {
               /* renew with the previous lease time */
               (void)mn_dhcp_renew(dhcp_lease.org_lease_time);
               }
            MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);
            }
         }

      MN_TASK_WAIT(DHCP_WAIT_TIME);
      }
}
#endif      /* (RTOS_USED != RTOS_NONE) */

/************************************************************************/

/* set dhcp_state before calling this function. returns 1 if ok, returns
   negative number on error.
*/
static int mn_dhcp_xchg_packet(byte *fileName, word32 req_lease_time)
cmx_reentrant {
   int retval;
   int status;
   SCHAR socket_no;
   byte retries;
   PSOCKET_INFO socket_ptr;
   word32 xid;
   word32 recv_xid;
   word16 wait_time;
   word16 dhcp_seconds;
   byte * dest_addr;
   PINTERFACE_INFO interface_ptr;
   TIMER_INFO_T wait_timer;

   retval = 0;
   status = 0;
   dhcp_seconds = 0;

   if (dhcp_lease.dhcp_state != DHCP_DEAD)
      {
      /* broadcast everything except DHCPRELEASE and DHCP_RENEWING */
      if ((dhcp_lease.dhcp_state == DHCPRELEASE) || \
            (dhcp_lease.dhcp_state == DHCP_RENEWING))
         dest_addr = dhcp_lease.server_id;
      else
         dest_addr = broadcast_addr;

      /* Open socket */
      socket_no = mn_open_socket(dest_addr, DHCP_CLIENT_PORT, DHCP_SERVER_PORT, \
                  PROTO_UDP, STD_TYPE, \
                 (byte *)&dhcp_info, sizeof(DHCP_INFO_T),eth_interface_no);
   
      if (socket_no >= 0)
         {
         socket_ptr = MK_SOCKET_PTR(socket_no);
         interface_ptr = MK_INTERFACE_PTR(eth_interface_no);
   
         while (retval == 0)
            {
            for (retries = 0; retries < MN_DHCP_RESEND_TRYS; retries++)
               {
               xid = make_dhcp_packet(fileName, req_lease_time);
   
               dhcp_info.secs[0] = HIGHBYTE(dhcp_seconds);
               dhcp_info.secs[1] = LOWBYTE(dhcp_seconds);
   
               /* don't broadcast if DHCPRELEASE or DHCP_RENEWING */
               if (dest_addr == broadcast_addr)
                  bootpMode = BOOTP_ACTIVE;
               else
                  bootpMode = BOOTP_NO_BCAST;

               /* Send the Data */
               socket_ptr->send_ptr = (byte *)&dhcp_info;
               socket_ptr->send_len = sizeof(DHCP_INFO_T);
               status = (int)mn_udp_send(socket_ptr);

               if (dhcp_lease.dhcp_state == DHCPRELEASE)
                  {
                  /* no reply expected to DHCPRELEASE packet */
                  retval = 1;
                  break;
                  }
   
               wait_time = (retries + 1) * DHCP_RETRY_TIME;

               if (status < 0)        /* error sending? */
                  {
                  MN_TASK_WAIT(wait_time * (MN_ONE_SECOND));
                  continue;
                  }
   
               /* always allow broadcasts when receiving dhcp packets */
               bootpMode = BOOTP_ACTIVE;

               status = 0;
               mn_reset_timer(&wait_timer,(wait_time * (MN_ONE_SECOND)));
               while ((status <= 0) && (!mn_timer_expired(&wait_timer)))
                  {
                  status = (int)mn_recv_wait(socket_no, (byte *)&dhcp_info, \
                     (sizeof(DHCP_INFO_T)), 1);
   
                  if (status > 0)
                     {
                     /* Check that a reply packet from dhcp server has our
                        transaction ID and is a reply to a bootp request.
                     */
                     recv_xid = dhcp_info.xid[3];
                     recv_xid = (recv_xid << 8) | dhcp_info.xid[2];
                     recv_xid = (recv_xid << 8) | dhcp_info.xid[1];
                     recv_xid = (recv_xid << 8) | dhcp_info.xid[0];
   
                     if ( (xid == recv_xid) && (dhcp_info.op == BOOTP_OP_REP) )
                        {
                        switch (dhcp_lease.dhcp_state)
                           {
                           case DHCPDISCOVER:
                              /* check message-type tag for DHCPOFFER */
                              if ( (get_msg_type()) == DHCPOFFER )
                                 {
                                 /* for now accept the first offer we get */
                                 dhcp_lease.dhcp_state = DHCPREQUEST;
                                 copy_server_id();
#if (MN_DNS)
                                 copy_dns_addr(interface_ptr);
#endif      /* (MN_DNS) */
                                 }
                              else
                                 retval = DHCP_ERROR;
                              break;
                           case DHCPREQUEST:
                           case DHCP_RENEWING:
                           case DHCP_REBINDING:
                              if ( (get_msg_type()) == DHCPACK )
                                 {
                                 MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
                                 (void)memcpy((void *)interface_ptr->ip_src_addr, \
                                    (void *)dhcp_info.yiaddr, IP_ADDR_LEN);
                                 MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
                                 /* get subnet mask and gateway from options
                                    field if available.
                                 */
                                 MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
                                 copy_subnet_mask();
                                 copy_gateway();
                                 MN_MUTEX_RELEASE(MUTEX_MN_INFO);
#if (MN_DNS)
                                 copy_dns_addr(interface_ptr);
#endif      /* (MN_DNS) */
                                 dhcp_lease.lease_time = get_lease_time();
                                 dhcp_lease.org_lease_time = dhcp_lease.lease_time;
                                 copy_server_id();
                                 if (dhcp_lease.lease_time == DHCP_INFINITE_LEASE)
                                    {
                                    dhcp_lease.infinite_lease = TRUE;
                                    }
                                 else
                                    {
                                    dhcp_lease.t1_renew_time = dhcp_lease.lease_time / 2;
                                    dhcp_lease.t2_renew_time = dhcp_lease.lease_time / 8;
                                    dhcp_lease.infinite_lease = FALSE;
                                    }
                                 dhcp_lease.dhcp_state = DHCP_OK;

                                 retval = 1;
                                 }
                              else if (get_msg_type() == DHCPOFFER)
                                 {
                                 /* allow for second DHCPOFFER, ##SR */
                                 continue;
                                 }
                              else
                                 retval = DHCP_ERROR;
                              break;
                           default:
                              retval = DHCP_ERROR;
                              break;
                           }
                        }
                     else
                        {
                        /* xid didn't match or not a reply. */
                        status = 0;
                        }
                     }     /* end if status > 0 */
                  }     /* end while wait_timer */

               if ((retval != 0) || (status > 0))
                  break;
               else
                  {
                  /* mn_recv_wait timed out */
                  dhcp_seconds = dhcp_seconds + wait_time;
                  }
               }     /* end for */

            if (status <= 0)        /* error sending or receiving? */
               {
               retval = status;     /* return error */
               break;
               }
            else if (retries >= MN_DHCP_RESEND_TRYS)
               {
               retval = DHCP_ERROR;
               break;
               }
            }        /* end while (retval == 0) */
   
         /* Disallow further broadcast packets. This puts the system back into
            'normal' mode.
         */
         bootpMode = BOOTP_INACTIVE;
   
         /* Clean up before exit */
         (void)mn_abort(socket_no);
         }
      else                          /* couldn't open socket */
         retval = socket_no;     /* return error from mn_open_socket */
      }

   return (retval);
}

/* builds the dhcp packet to send out depending on the dhcp state.
   returns the transaction ID used.
*/
static word32 make_dhcp_packet(byte *fileName, word32 req_lease_time)
cmx_reentrant {
   word32 xid;
   byte *ptr;
   byte i;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   switch (dhcp_lease.dhcp_state)
      {
      case DHCPDISCOVER:
      case DHCPRELEASE:
         /* "xid is set to a 'random' transaction ID." [RFC951 sec 7.1] */
/*         xid = (word32) (( ((word32)MN_GET_TICK+1) << 16) | (MN_GET_TICK+1)); */
         xid = MK_WORD32(eth_src_hw_addr[3], eth_src_hw_addr[4], \
            eth_src_hw_addr[5], (MN_GET_TICK+1));
         dhcp_info.xid[0] = WORD32_BYTE0(xid);
         dhcp_info.xid[1] = WORD32_BYTE1(xid);
         dhcp_info.xid[2] = WORD32_BYTE2(xid);
         dhcp_info.xid[3] = WORD32_BYTE3(xid);
         break;
      case DHCPREQUEST:
      case DHCP_RENEWING:
      case DHCP_REBINDING:
         /* use xid from DHCPOFFER */
         xid = dhcp_info.xid[3];
         xid = (xid << 8) | dhcp_info.xid[2];
         xid = (xid << 8) | dhcp_info.xid[1];
         xid = (xid << 8) | dhcp_info.xid[0];
         break;
      default:
         xid = 0;
         break;
      }

   dhcp_info.op = BOOTP_OP_REQ;
   dhcp_info.htype = ETHERNET_10MB_LB;
   dhcp_info.hlen = ETH_ADDR_LEN;
   MEMSET(dhcp_info.siaddr, 0, IP_ADDR_LEN);
   MEMSET(dhcp_info.giaddr, 0, IP_ADDR_LEN);
   /*  Copy our hardware address. */
   (void)memcpy((void *)dhcp_info.chaddr, (void *)eth_src_hw_addr, ETH_ADDR_LEN);
   /* Set the request file name for TFTP */
   if (fileName != PTR_NULL)
      strcpy((char *)dhcp_info.file, (char *)fileName);
   else
      dhcp_info.file[0] = '\0';

   ptr = &(dhcp_info.options[0]);
   /* setup cookie */
   *ptr++ = 99;
   *ptr++ = 130;
   *ptr++ = 83;
   *ptr++ = 99;

   /* setup message type */
   *ptr++ = DHCP_MSG_TYPE_CODE;
   *ptr++ = DHCP_MSG_TYPE_LEN;

   switch (dhcp_lease.dhcp_state)
      {
      case DHCPDISCOVER:
         dhcp_info.flag[0] = 0x80;     /* Broadcast flag */
         MEMSET(dhcp_info.yiaddr, 0, IP_ADDR_LEN);
         MEMSET(dhcp_info.ciaddr, 0, IP_ADDR_LEN);

         *ptr++ = DHCPDISCOVER;

         ptr = set_client_id(ptr);

         ptr = set_lease_time(ptr,req_lease_time);

         ptr = set_param_list(ptr);

         break;

      case DHCPREQUEST:
         dhcp_info.flag[0] = 0x80;     /* Broadcast flag */
         MEMSET(dhcp_info.ciaddr, 0, IP_ADDR_LEN);

         *ptr++ = DHCPREQUEST;
         ptr = set_client_id(ptr);
         ptr = set_server_id(ptr);

         *ptr++ = REQ_IP_ADDR_CODE;
         *ptr++ = REQ_IP_ADDR_LEN;
         for (i=0;i<IP_ADDR_LEN;i++)
            *ptr++ = dhcp_info.yiaddr[i];

         ptr = set_lease_time(ptr,req_lease_time);

         ptr = set_param_list(ptr);

         MEMSET(dhcp_info.yiaddr, 0, IP_ADDR_LEN);

         break;

      case DHCP_RENEWING:
      case DHCP_REBINDING:
         dhcp_info.flag[0] = 0x00;     /* clear Broadcast flag */
         MEMSET(dhcp_info.yiaddr, 0, IP_ADDR_LEN);
         MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
         (void)memcpy((void *)dhcp_info.ciaddr, \
            (void *)interface_ptr->ip_src_addr, IP_ADDR_LEN);
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

         *ptr++ = DHCPREQUEST;
         ptr = set_client_id(ptr);
         ptr = set_lease_time(ptr,req_lease_time);
         ptr = set_param_list(ptr);
         break;

      case DHCPRELEASE:
         dhcp_info.flag[0] = 0x00;     /* clear Broadcast flag */
         MEMSET(dhcp_info.yiaddr, 0, IP_ADDR_LEN);

         *ptr++ = DHCPRELEASE;
         ptr = set_client_id(ptr);
         ptr = set_server_id(ptr);

         break;

      default:
         /* should never get here */
         break;
      }

   /* mark the end of the options */
   *ptr = END_BYTE;

   return (xid);
}

/*
   this function returns a pointer to the desired tag_type or PTR_NULL
   if the tag-type in the options field is not found.

   we are looking at
      tag-type
      tag-len
      tag_len number of data bytes
      tag-type
      tag-len
      tag_len number of data bytes
      ...
      end_byte
*/
static byte *find_tag_type(byte tag_type)
cmx_reentrant {
   byte *ptr;
   byte *end;
   byte len;

   /* point to start & end of options. first 4 bytes of the options field
      should be the magic number (cookie).
   */
   ptr = &dhcp_info.options[COOKIE_LEN];
   end = &dhcp_info.options[DHCP_OPT_LEN - 1];

   while (1)
      {
      if (*ptr == tag_type)
         {
         /* found what we were looking for */
         break;
         }
      else if ((*ptr == END_BYTE) || (ptr > end))
         {
         /* reached the end without finding what we were looking for */
         ptr = BYTE_PTR_NULL;
         break;
         }
      else if (*ptr == PAD_BYTE)
         {
         /* skip to next tag-type */
         do
            {
            ptr++;
            }
         while (*ptr == PAD_BYTE);
         }
      else
         {
         ptr++;                    /* point to length byte */
         len = (byte)(*ptr + 1);   /* skip to next tag_type */
         ptr = ptr + len;
         }
      }

   return (ptr);
}

/* returns the message type from the received dhcp packet or 0 if no
   message type found (should never happen).
*/
static byte get_msg_type(void)
cmx_reentrant {
   byte retval;
   byte *ptr;

   retval = 0;

   /* get pointer to msg-type tag */
   ptr = find_tag_type(DHCP_MSG_TYPE_CODE);
   if (ptr != PTR_NULL)
      retval = *(ptr+2);
   
   return (retval);
}

/* returns the lease time from the received dhcp packet or 0 if no
   lease time found.
*/
static word32 get_lease_time(void)
cmx_reentrant {
   word32 retval;
   byte *ptr;

   retval = 0;

   /* get pointer to lease-time tag */
   ptr = find_tag_type(REQ_IP_LEASE_CODE);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data */
      /* convert the lease time */
      retval = (word32)*ptr << 24;
      ptr++;
      retval += (word32)*ptr << 16;
      ptr++;
      retval += (word32)*ptr << 8;
      ptr++;
      retval += (word32)*ptr;
      }

   return (retval);
}

/* saves the server_id from the received dhcp packet */
static void copy_server_id(void)
cmx_reentrant {
  char *ptr;

   /* get pointer to server_id tag */
   ptr = (char *)find_tag_type(DHCP_SERVER_ID_CODE);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data */
      (void)memcpy((void *)dhcp_lease.server_id, (void *)ptr, IP_ADDR_LEN);
      }
}

static byte * set_client_id(byte *ptr)
cmx_reentrant {
   byte i;

   /* setup client ID, hardware-type + MAC address */
   *ptr++ = DHCP_CLIENT_ID_CODE;
   *ptr++ = DHCP_CLIENT_ID_LEN;
   *ptr++ = ETHERNET_10MB_LB;
   for (i=0;i<ETH_ADDR_LEN;i++)
      *ptr++ = eth_src_hw_addr[i];

   return (ptr);
}

static byte * set_server_id(byte *ptr)
cmx_reentrant {
   byte i;

   /* setup server ID */
   *ptr++ = DHCP_SERVER_ID_CODE;
   *ptr++ = DHCP_SERVER_ID_LEN;
   for (i=0;i<IP_ADDR_LEN;i++)
      *ptr++ = dhcp_lease.server_id[i];

   return (ptr);
}

static byte * set_lease_time(byte *ptr, word32 req_lease_time)
cmx_reentrant {
   /* setup lease time, use default if needed */
   if (req_lease_time == 0)
      req_lease_time = MN_DHCP_DEFAULT_LEASE_TIME;
   *ptr++ = REQ_IP_LEASE_CODE;
   *ptr++ = REQ_IP_LEASE_LEN;
   *ptr++ = WORD32_BYTE3(req_lease_time);
   *ptr++ = WORD32_BYTE2(req_lease_time);
   *ptr++ = WORD32_BYTE1(req_lease_time);
   *ptr++ = WORD32_BYTE0(req_lease_time);

   return (ptr);
}

static byte * set_param_list(byte *ptr)
cmx_reentrant {
   /* set list of parameters we are requesting */
   *ptr++ = PARAMETER_LIST_CODE;
   *ptr++ = NUM_PARAMETERS;
   *ptr++ = SUBNET_MASK_CODE;
   *ptr++ = ROUTER_CODE;
#if (MN_DNS)
   *ptr++ = DNS_SERVER_CODE;
#endif      /* (MN_DNS) */
   return (ptr);
}

static void copy_subnet_mask(void)
cmx_reentrant {
  char *ptr;

   /* get pointer to subnet mask tag */
   ptr = (char *)find_tag_type(SUBNET_MASK_CODE);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data */
      (void)memcpy((void *)subnet_mask, (void *)ptr, IP_ADDR_LEN);
      }
}

static void copy_gateway(void)
cmx_reentrant {
  char *ptr;

   /* get pointer to router (gateways) tag */
   ptr = (char *)find_tag_type(ROUTER_CODE);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data, use first ip address only */
      (void)memcpy((void *)gateway_ip_addr, (void *)ptr, IP_ADDR_LEN);
      }
}

#if (MN_DNS)
static void copy_dns_addr(PINTERFACE_INFO interface_ptr)
cmx_reentrant {
  char *ptr;

   /* get pointer to DNS server tag */
   ptr = (char *)find_tag_type(DNS_SERVER_CODE);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data, use first ip address only */
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      (void)memcpy((void *)interface_ptr->ip_dns_addr, (void *)ptr, IP_ADDR_LEN);
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      }
}
#endif      /* (MN_DNS) */

#endif      /* MN_DHCP && MN_ETHERNET */


