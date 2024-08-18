/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

#include "micronet.h"
#include "np_includes.h"

#if (defined(MCHP_C18))
#pragma udata mn_sock2
#endif
SOCKET_INFO_T sock_info[MN_NUM_SOCKETS];
#if (defined(MCHP_C18))
#pragma udata
#endif

#if (MN_TCP && MN_TCP_DELAYED_ACKS)
PACKET_RESEND_INFO_T resend_info[MN_TCP_RESEND_BUFFS];

static void packet_resend(PSOCKET_INFO, byte) cmx_reentrant;
#endif

static SCHAR next_socket(void) cmx_reentrant;

/********************************************************************/

/* Perform all non-RTOS initialization needed by MicroNet.
   Returns result from mn_ether_init if ethernet used, otherwise
   always returns 1. If mn_interface_init is not successful
   will return result of the bad mn_interface_init. If an RTOS is
   used this must be called from a task and called after mn_os_init.
*/
int mn_init(void)
cmx_reentrant {
   int status;
#if (MN_TCP && MN_TCP_DELAYED_ACKS)
   byte i;
#endif

#if (RTOS_USED != RTOS_NONE)
    mn_init_done = FALSE;
#endif      /* (RTOS_USED != RTOS_NONE) */

   MEMSET(sock_info, 0, sizeof(sock_info));
#if (RTOS_USED == RTOS_NONE)
   MEMSET(interface_info, 0, sizeof(interface_info));
#endif      /* (RTOS_USED == RTOS_NONE) */

#if (MN_TCP && MN_TCP_DELAYED_ACKS)
   for (i=0; i<MN_TCP_RESEND_BUFFS; i++)
      resend_info[i].socket_no = -1;
#endif

   mn_ip_init();

#if MN_SNMP
   mn_snmp_init();
#endif      /* #if MN_SNMP */

   /* Note: mn_timer_init MUST be called before mn_uart_init and
      mn_ether_init.
   */
   mn_timer_init();

   /* Initialize the interface(s). */
#if (MN_ETHERNET && USE_RECV_BUFF && USE_SEND_BUFF)
   eth_interface_no = mn_interface_init(IF_ETHER, eth_recv_buff, \
      MN_ETH_RECV_BUFF_SIZE, eth_send_buff, MN_ETH_XMIT_BUFF_SIZE);
   if (eth_interface_no < 0)
      return ((int)eth_interface_no);
#endif      /* (MN_ETHERNET && USE_RECV_BUFF && USE_SEND_BUFF) */

#if (MN_PPP)
   uart_interface_no = mn_interface_init(IF_PPP, recv_buff, \
      MN_UART_RECV_BUFF_SIZE, send_buff, MN_UART_XMIT_BUFF_SIZE);
   if (uart_interface_no < 0)
      return ((int)uart_interface_no);
#elif (MN_SLIP)
   uart_interface_no = mn_interface_init(IF_SLIP, recv_buff, \
      MN_UART_RECV_BUFF_SIZE, send_buff, MN_UART_XMIT_BUFF_SIZE);
   if (uart_interface_no < 0)
      return ((int)uart_interface_no);
#endif      /* (MN_PPP) */

#if (MN_IGMP)
   mn_igmp_init();
#endif      /* (MN_IGMP) */

#if (MN_BOOTP)
   mn_bootp_init();
#endif

#if MN_DHCP
   mn_dhcp_init();
#endif      /*  MN_DHCP */

#if MN_VIRTUAL_FILE
   /* this assumes the virtual file system will be set up through the
      mn_??_set_entry functions.
   */
   MEMSET(vf_dir, 0, sizeof(vf_dir));
   MEMSET(page_send_info, 0, sizeof(page_send_info));
#if MN_HTTP
#if (MN_NUM_POST_FUNCS)
   MEMSET(pf, 0, sizeof(pf));
#endif      /* (MN_NUM_POST_FUNCS) */
#if MN_SERVER_SIDE_INCLUDES
   MEMSET(gf, 0, sizeof(gf));
#endif      /* #if MN_SERVER_SIDE_INCLUDES */
#endif      /* #if MN_HTTP */
#endif      /* MN_VIRTUAL_FILE */

#if MN_HTTP
   mn_http_init();
#endif      /*  MN_HTTP */

#if (MN_FTP_SERVER || MN_FTP_CLIENT)
   MEM_POOL_INIT;
#endif      /* #if (MN_FTP_SERVER || MN_FTP_CLIENT) */

#if (MN_FTP_SERVER)
   mn_ftp_server_init();
#endif      /* #if (MN_FTP_SERVER) */

#if MN_SMTP
   mn_smtp_init();
#endif      /* #if MN_SMTP */

   status = 1;
#if MN_ETHERNET
#if MN_ARP
   mn_arp_init();
#endif      /* MN_ARP */
   status = mn_ether_init();
#endif      /* MN_ETHERNET */

#if (MN_PPP && MN_USE_PAP)
   MEMSET(pap_user, 0, sizeof(pap_user));
#endif      /* (MN_PPP && MN_USE_PAP) */

#if (MN_PPP || MN_SLIP)
   if (status > 0)
      mn_uart_init();
#endif      /* (MN_PPP || MN_SLIP) */

   if (status > 0)
      {
      MN_XMIT_BUSY_CLEAR;
      /* allow transmit - assumes semaphore was initialized to zero */
      MN_SIGNAL_POST(SIGNAL_TRANSMIT);
#if (MN_POLLED_ETHERNET)
      /* Signal receive task it is ok to start. */
      MN_SIGNAL_POST(SIGNAL_RECEIVE);
#endif      /* (MN_POLLED_ETHERNET) */
      }

#if (RTOS_USED != RTOS_NONE)
    mn_init_done = TRUE;
#endif      /* (RTOS_USED != RTOS_NONE) */

   return (status);
}

/* fills socket structure and opens TCP connection if needed.
   returns socket_no or negative number on error.
*/
SCHAR mn_open(byte dest_ip[], word16 src_port, word16 dest_port, \
   byte open_mode, byte proto, byte type, byte *buff_ptr, word16 buff_len, \
   SCHAR interface_no)
cmx_reentrant {
   SCHAR socket_no;
#if MN_TCP
   PSOCKET_INFO socket_ptr;
#endif

#if (MN_UDP && !MN_TCP)
   open_mode = open_mode;
#endif

   socket_no = mn_open_socket(dest_ip, src_port, dest_port, proto,type, \
      buff_ptr, buff_len, interface_no);

#if MN_TCP
      if (proto == PROTO_TCP)
         {
         if (socket_no >= 0)
            {
            socket_ptr = MK_SOCKET_PTR(socket_no);
            if (mn_tcp_open(open_mode,socket_ptr) <= 0)
               {
               (void)mn_abort(socket_no);
               return (TCP_OPEN_FAILED);
               }
            }
         }
#endif      /* MN_TCP */

   return (socket_no);
}


/* fills socket structure. returns socket_no or negative number on error.
   mn_open is split into two functions because some linkers were complaining
   about recursion in tcp.c because mn_open was being called from
   tcp_recv_header. tcp_recv_header now calls mn_open_socket.
*/

SCHAR mn_open_socket(byte dest_ip[], word16 src_port, word16 dest_port, \
   byte proto, byte type, byte *buff_ptr, word16 buff_len, SCHAR interface_no)
cmx_reentrant {
   SCHAR socket_no;
   PSOCKET_INFO socket_ptr;
#if (MN_ETHERNET)
   PINTERFACE_INFO interface_ptr;
#endif   /* MN_ETHERNET */

   if (interface_no >= MN_NUM_INTERFACES)
      return (INTERFACE_NOT_FOUND);

   /* Make sure we have a valid proto */
   socket_no = NOT_SUPPORTED;
   switch (proto)
      {
#if (MN_ETHERNET)
      case PROTO_IP:
         if (type == S_ARP_TYPE)
            socket_no = 0;
         break;
#endif
#if (MN_TCP)
      case PROTO_TCP:
#endif
#if (MN_UDP)
      case PROTO_UDP:
#endif
#if (MN_PING_REPLY)
      case PROTO_ICMP:
#endif
#if (MN_IGMP)
      case PROTO_IGMP:
#endif
         socket_no = 0;
         break;
      default:
         break;
      }

   if (socket_no == NOT_SUPPORTED)
      return (socket_no);

#if (MN_ALLOW_MULTICAST)
   if ((dest_ip[0] >= 224) && (dest_ip[0] <= 239))
      type |= MULTICAST_TYPE;
#endif      /* (MN_ALLOW_MULTICAST) */

   if ( (src_port == 0) && ((proto == PROTO_TCP) || (proto == PROTO_UDP)) )
      socket_no = BAD_SOCKET_DATA;
#if (MN_ALLOW_MULTICAST)
   else if ((proto == PROTO_TCP) && (type & MULTICAST_TYPE))
      socket_no = BAD_SOCKET_DATA;
   else if ((dest_ip[0] == 224) && (dest_ip[1] == 0) && (dest_ip[2] == 0) \
         && (dest_ip[3] == 0))
      /* Illegal multicast address */
      socket_no = BAD_SOCKET_DATA;
#else
   else if (type & MULTICAST_TYPE)
      socket_no = BAD_SOCKET_DATA;
#endif      /* (MN_ALLOW_MULTICAST) */
   else
      {
      socket_ptr = mn_find_socket(src_port, dest_port, dest_ip, proto, \
         interface_no);
      if ((socket_ptr != PTR_NULL) && SOCKET_ACTIVE(socket_ptr->socket_no))
         socket_no = SOCKET_ALREADY_EXISTS;
      else
         {
         socket_no = next_socket();
         if (socket_no != NOT_ENOUGH_SOCKETS)
            {
            MN_TASK_LOCK;
            MEMSET(&sock_info[socket_no], 0, sizeof(SOCKET_INFO_T));
            socket_ptr = MK_SOCKET_PTR(socket_no);
            socket_ptr->socket_no = socket_no;
            socket_ptr->interface_no = interface_no;
            socket_ptr->src_port = src_port;
            socket_ptr->dest_port = dest_port;
   
            socket_ptr->ip_dest_addr[0] = dest_ip[0];
            socket_ptr->ip_dest_addr[1] = dest_ip[1];
            socket_ptr->ip_dest_addr[2] = dest_ip[2];
            socket_ptr->ip_dest_addr[3] = dest_ip[3];
   
#if (MN_ETHERNET)
            interface_ptr = MK_INTERFACE_PTR(interface_no);
            if (interface_ptr->interface_type == IF_ETHER)
               {
#if (MN_ARP)
               if ((type == S_ARP_TYPE) && (proto == PROTO_IP))
#endif      /* MN_ARP */
                  {
                  socket_ptr->eth_dest_hw_addr[0] = eth_dest_hw_addr[0];
                  socket_ptr->eth_dest_hw_addr[1] = eth_dest_hw_addr[1];
                  socket_ptr->eth_dest_hw_addr[2] = eth_dest_hw_addr[2];
                  socket_ptr->eth_dest_hw_addr[3] = eth_dest_hw_addr[3];
                  socket_ptr->eth_dest_hw_addr[4] = eth_dest_hw_addr[4];
                  socket_ptr->eth_dest_hw_addr[5] = eth_dest_hw_addr[5];
                  }
               }
#endif   /* (MN_ETHERNET) */
   
            socket_ptr->ip_proto = proto;
            socket_ptr->socket_type = type;
            if (buff_ptr && buff_len)
               {
               socket_ptr->recv_ptr = buff_ptr;
               socket_ptr->recv_end = buff_ptr+buff_len-1;
               }
   
            MN_SIGNAL_RESET(signal_socket[socket_no]);
   
#if MN_TCP
            if (proto == PROTO_TCP)
               mn_tcp_init(socket_ptr);
#endif      /* MN_TCP */
   
            SOCKET_ACTIVATE(socket_no);
            MN_TASK_UNLOCK;
            }
         }
      }

   return (socket_no);
}

#if (MN_SEND_ROM_PKT)
long mn_send_rom(SCHAR socket_no, PCONST_BYTE data_ptr, word16 data_len)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;

   if ((socket_no < 0) || (socket_no >= MN_NUM_SOCKETS))
      return (BAD_SOCKET_DATA);

   socket_ptr = MK_SOCKET_PTR(socket_no);

   /* if no data_ptr passed, use the current value */
   if (data_ptr && data_len)
      {
#if MN_TCP
      /* Can't switch buffers if we are still waiting for an ACK in case
         we have to resend the previous packet.
      */
      if ((socket_ptr->ip_proto == PROTO_TCP) && socket_ptr->tcp_unacked_bytes)
         {
         if (socket_ptr->send_ptr || (socket_ptr->rom_send_ptr != data_ptr) || \
               (socket_ptr->send_len != data_len))
            return 0;
         }
#endif      /* MN_TCP */
      socket_ptr->send_ptr = PTR_NULL;
      socket_ptr->rom_send_ptr = data_ptr;
      socket_ptr->send_len = data_len;
      }

   return (mn_send(socket_no, 0, 0));
}
#endif

/* sends a TCP or UDP packet using the passed socket. returns number of
   bytes sent or negative number on error. If a TCP packet waits for a reply.
*/
long mn_send(SCHAR socket_no, byte *data_ptr, word16 data_len)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
#if (MN_DHCP)
   byte dhcp_state;
#endif      /* (MN_DHCP) */
#if MN_TCP
   long retval;
#if (!MN_TCP_DELAYED_ACKS)
   long recvd;

   recvd = NEED_TO_LISTEN;
#endif
   retval = 0;
#endif

   if ((socket_no < 0) || (socket_no >= MN_NUM_SOCKETS))
      return (BAD_SOCKET_DATA);

   if (!SOCKET_ACTIVE(socket_no))
      return (SOCKET_NOT_FOUND);

   socket_ptr = MK_SOCKET_PTR(socket_no);

   if ((socket_ptr->interface_no < 0) || (socket_ptr->interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

#if MN_PPP
   if (interface_info[socket_ptr->interface_no].interface_type == IF_PPP)
      {
      if (!MN_PPP_STATUS_UP)
         return (PPP_LINK_DOWN);
      }
#endif    /* MN_PPP */

#if MN_DHCP
   if (interface_info[socket_ptr->interface_no].interface_type == IF_ETHER)
      {
      dhcp_state = dhcp_lease.dhcp_state;
      if (dhcp_state == DHCP_DEAD)
         return (DHCP_LEASE_EXPIRED);

#if (RTOS_USED == RTOS_NONE)
      if (bootpMode == BOOTP_INACTIVE)
         {
         if ((dhcp_state == DHCP_RENEWING) || (dhcp_state == DHCP_REBINDING))
            {
            /* renew with the previous lease time */
            (void)mn_dhcp_renew(dhcp_lease.org_lease_time);
            }
         }
#endif      /* (RTOS_USED == RTOS_NONE) */
      }
#endif      /* MN_DHCP */

#if (MN_IGMP && RTOS_USED == RTOS_NONE)
   mn_igmp_check_timers();
#endif      /* (MN_IGMP && RTOS_USED == RTOS_NONE) */

#if (MN_TCP && MN_TCP_DELAYED_ACKS && RTOS_USED == RTOS_NONE)
   mn_resend_tcp();
#endif

   /* if no data_ptr passed, use the current value */
   if (data_ptr && data_len)
      {
#if (MN_TCP && !MN_TCP_DELAYED_ACKS)
      /* Can't switch buffers if we are still waiting for an ACK in case
         we have to resend the previous packet.
      */
      if ((socket_ptr->ip_proto == PROTO_TCP) && socket_ptr->tcp_unacked_bytes)
         {
#if (MN_SEND_ROM_PKT)
         if (socket_ptr->rom_send_ptr || (socket_ptr->send_ptr != data_ptr) || \
               (socket_ptr->send_len != data_len))
#else
         if ((socket_ptr->send_ptr != data_ptr) || \
               (socket_ptr->send_len != data_len))
#endif
            return 0;
         }
#endif      /* (MN_TCP && !MN_TCP_DELAYED_ACKS) */
#if (MN_SEND_ROM_PKT)
      socket_ptr->rom_send_ptr = PTR_NULL;
#endif
      socket_ptr->send_ptr = data_ptr;
      socket_ptr->send_len = data_len;
      }

#if MN_TCP
      if (socket_ptr->ip_proto == PROTO_TCP)
         {
#if (MN_TCP_DELAYED_ACKS)
         /* Don't wait for ACKs if we allow delayed ACKs. */
         retval = mn_tcp_send(socket_ptr);
#else
         /* loop until we get an ack, a tcp_close or error */
         do
            {
            /* Send the Data */
            retval = mn_tcp_send(socket_ptr);
            /* Wait for a reply. */
            if (retval > 0)
               {
               do
                  {
                  recvd = mn_recv_wait(socket_no, 0, 0, (MN_SOCKET_WAIT_TICKS));
                  }
               while (recvd == NEED_TO_LISTEN);

               if ((recvd < 0) && (recvd != SOCKET_TIMED_OUT))  /* recv error */
                  {
                  retval = recvd;
                  break;
                  }
               }
            else        /* send error */
               break;
            }
         while (socket_ptr->tcp_unacked_bytes);
#endif

         return (retval);
         }

#endif      /* MN_TCP */

#if MN_UDP
      if (socket_ptr->ip_proto == PROTO_UDP)
         return (mn_udp_send(socket_ptr));
#endif

      return (0);
}

/* loops until a TCP or UDP packet is received for the passed socket.
   returns number of bytes received or negative number on error.
   Also returns if no packet received in MN_SOCKET_WAIT_TICKS timer ticks.
*/
long mn_recv(SCHAR socket_no, byte *buff_ptr, word16 buff_len)
cmx_reentrant {
   /* this function is set up like this to avoid breaking existing code */
   return (mn_recv_wait(socket_no, buff_ptr, buff_len, (MN_SOCKET_WAIT_TICKS)));
}

/* loops until a TCP or UDP packet is received for the passed socket.
   returns number of bytes received or negative number on error.
   Also returns if no packet received in wait_ticks timer ticks.
   If wait_ticks is 0 will return immediately if no packet available.
*/
long mn_recv_wait(SCHAR socket_no, byte *buff_ptr, word16 buff_len, word16 wait_ticks)
cmx_reentrant {
   long retval;
   PSOCKET_INFO socket_ptr;
#if (RTOS_USED == RTOS_NONE)
   PSOCKET_INFO org_socket_ptr;
   TIMER_INFO_T sock_timer;
   byte packet_type;
#endif      /* (RTOS_USED == RTOS_NONE) */

   retval = 0;

   if ((socket_no < 0) || (socket_no >= MN_NUM_SOCKETS))
      return (BAD_SOCKET_DATA);

   if (!SOCKET_ACTIVE(socket_no))
      return (SOCKET_NOT_FOUND);

   socket_ptr = MK_SOCKET_PTR(socket_no);

   if ((socket_ptr->interface_no < 0) || (socket_ptr->interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

#if MN_DHCP
   if (interface_info[socket_ptr->interface_no].interface_type == IF_ETHER)
      {
      if (dhcp_lease.dhcp_state == DHCP_DEAD)
         return (DHCP_LEASE_EXPIRED);
      }
#endif      /* MN_DHCP */

   /* UDP sockets need a receive buffer, but with TCP we can handle some
      types of packets without a receive buffer for the socket.
   */
   if (buff_ptr && buff_len)
      {
      socket_ptr->recv_ptr = buff_ptr;
      socket_ptr->recv_end = buff_ptr+buff_len-1;
      }

#if MN_UDP
   if ((socket_ptr->ip_proto == PROTO_UDP) && (socket_ptr->recv_ptr == PTR_NULL))
      return (BAD_SOCKET_DATA);
#endif

#if (RTOS_USED == RTOS_NONE)
   org_socket_ptr = socket_ptr;

   if (wait_ticks)
      mn_reset_timer(&sock_timer, wait_ticks);

   while (1)
      {
#if MN_PPP
      if (interface_info[socket_ptr->interface_no].interface_type == IF_PPP)
         {
         if (!MN_PPP_STATUS_UP)
            return (PPP_LINK_DOWN);
         }
#endif    /* MN_PPP */
      /* loop until we get a tcp or udp packet or timeout */
      packet_type = mn_ip_recv();
      if (!(packet_type & (TCP_TYPE|UDP_TYPE|ICMP_TYPE)))
         {
         /* if we get here either there was no packet or we received a
            packet of a bad or unknown type.
         */
         if (!wait_ticks || mn_timer_expired(&sock_timer))
            return (SOCKET_TIMED_OUT);
         else
            {
#if (MN_IGMP && RTOS_USED == RTOS_NONE)
            mn_igmp_check_timers();
#endif      /* (MN_IGMP && RTOS_USED == RTOS_NONE) */

#if (MN_TCP && MN_TCP_DELAYED_ACKS && RTOS_USED == RTOS_NONE)
   mn_resend_tcp();
#endif

            /* call the callback function so something can be done while
               we are waiting.
            */
            if (mn_app_recv_idle() == NEED_TO_EXIT)
               return (NEED_TO_EXIT);
            else
               continue;
            }
         }

      if (packet_type & ICMP_TYPE)
         {
         if (packet_type & ICMP_REPLY_TYPE)
            {
            retval = mn_icmp_recv_reply(&socket_ptr);
            }
         else
            continue;
         }

#if MN_TCP
      if (packet_type & TCP_TYPE)
         {
         retval = mn_tcp_recv(&socket_ptr);
         if ((retval == TCP_NO_CONNECT) && (socket_ptr == org_socket_ptr))
            break;
         }
#endif
#if MN_TCP && MN_UDP
      else
#endif
#if MN_UDP
         {
         if (packet_type & UDP_TYPE)
            {
            retval = mn_udp_recv(&socket_ptr);
            }
         }
#endif
      /* if we got a packet for a different socket, get another packet.
      */
      if ((SOCKET_ACTIVE(org_socket_ptr->socket_no)) && \
            (socket_ptr != org_socket_ptr))
         socket_ptr = org_socket_ptr;
      else
         break;
      }

#else    /* using an RTOS */
   if (!wait_ticks)
      {
      /* don't wait for a signal, just check for one */
      if (MN_SIGNAL_GET(signal_socket[socket_ptr->socket_no]) == SIGNAL_SUCCESS)
         {
         MN_TASK_LOCK;
         retval = socket_ptr->last_return_value;
         MN_TASK_UNLOCK;
         }
      else
         retval = SOCKET_TIMED_OUT;
      }
   else
      {
      if (MN_SIGNAL_WAIT(signal_socket[socket_ptr->socket_no],wait_ticks) == \
            SIGNAL_SUCCESS)
         {
         MN_TASK_LOCK;
         retval = socket_ptr->last_return_value;
         MN_TASK_UNLOCK;
         }
      else
         retval = SOCKET_TIMED_OUT;
      }
#endif      /* (RTOS_USED == RTOS_NONE) */

   return (retval);
}

/* does a tcp_shutdown if a tcp socket and closes the socket. returns
   negative number if a bad socket passed, otherwise returns 0.
*/
int mn_close(SCHAR socket_no)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;

   if ((socket_no < 0) || (socket_no >= MN_NUM_SOCKETS))
      return (BAD_SOCKET_DATA);

   if (!SOCKET_ACTIVE(socket_no))
      return (SOCKET_NOT_FOUND);

   socket_ptr = MK_SOCKET_PTR(socket_no);

   if ((socket_ptr->interface_no < 0) || (socket_ptr->interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

#if MN_DHCP
   if (interface_info[socket_ptr->interface_no].interface_type == IF_ETHER)
      {
      if (dhcp_lease.dhcp_state == DHCP_DEAD)
         {
         (void)mn_abort(socket_no);
         return (DHCP_LEASE_EXPIRED);
         }
      }
#endif      /* MN_DHCP */

#if MN_TCP
   if (socket_ptr->ip_proto == PROTO_TCP)
      {
      mn_tcp_shutdown(socket_ptr);
      }
#endif

   MN_TASK_LOCK;
   CLEAR_SOCKET(socket_no);
   socket_ptr->src_port = 0;
   MN_SIGNAL_RESET(signal_socket[socket_no]);
   MN_TASK_UNLOCK;
   return (0);
}

/* does a tcp_abort if a tcp socket and closes the socket. returns
   negative number if a bad socket passed, otherwise returns 0. The
   difference between mn_close and mn_abort is that mn_abort will not
   send any packets, while mn_close for tcp sockets will send a TCP FIN
   and negotiate a close.
*/
int mn_abort(SCHAR socket_no)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;

   if ((socket_no < 0) || (socket_no >= MN_NUM_SOCKETS))
      return (BAD_SOCKET_DATA);

   if (!SOCKET_ACTIVE(socket_no))
      return (SOCKET_NOT_FOUND);

   socket_ptr = MK_SOCKET_PTR(socket_no);

#if MN_TCP
   if (socket_ptr->ip_proto == PROTO_TCP)
      {
      mn_tcp_abort(socket_ptr);
      }
#endif

   MN_TASK_LOCK;
   CLEAR_SOCKET(socket_no);
   socket_ptr->src_port = 0;
#if (MN_HTTP)
   if (socket_ptr->socket_type && HTTP_TYPE)
      mn_http_close_file(socket_no);
#endif      /* MN_HTTP */
#if (MN_FTP_SERVER)
   if (socket_ptr->socket_type & FTP_TYPE)
      {
      mn_ftp_server_abort(socket_ptr);
      }
#endif      /* #if (MN_FTP_SERVER) */
   MN_SIGNAL_RESET(signal_socket[socket_no]);
   MN_TASK_UNLOCK;
   return (0);
}

/* finish the packet and turn on xmit ISR, len is only used by ethernet.
   returns 1 if successful or less than 0 if an error.

   PPP and SLIP always return 1.
*/
SCHAR mn_close_packet(PSOCKET_INFO socket_ptr, word16 len)
cmx_reentrant {
#if (MN_ETHERNET)
   int status;
#endif      /* MN_ETHERNET */
   byte intface_type;

   if ((send_interface_no < 0) || (send_interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   intface_type = interface_info[send_interface_no].interface_type;

#if MN_DHCP
   if ((intface_type == IF_ETHER) && (dhcp_lease.dhcp_state == DHCP_DEAD))
      return (DHCP_LEASE_EXPIRED);
#endif      /* MN_DHCP */

#if MN_TCP
   if ((socket_ptr != PTR_NULL) && (socket_ptr->ip_proto == PROTO_TCP))
      socket_ptr->tcp_resends -= 1;
   else
#endif
      socket_ptr = socket_ptr;

#if (MN_ETHERNET)
   /* if your ethernet chip doesn't generate the CRC,
      add code to do it here.
   */
   if (intface_type == IF_ETHER)
      {
      MN_XMIT_BUSY_SET;
      status = ETHER_SEND(socket_ptr, len);
#if (MN_POLLED_ETHERNET)
      MN_XMIT_BUSY_CLEAR;
      MN_SIGNAL_POST(SIGNAL_TRANSMIT);    /* allow transmit */
#else
      if (status <= 0)
         {
         MN_XMIT_BUSY_CLEAR;
         MN_SIGNAL_POST(SIGNAL_TRANSMIT);    /* allow transmit */
         }
#endif      /* (MN_POLLED_ETHERNET) */

      if (status > 0)
         return (1);
      else
         return ((SCHAR)status);
      }
#endif      /* MN_ETHERNET */

#if MN_PPP
   if (intface_type == IF_PPP)
      {
      len=len;                   /* keep compiler happy */
      mn_ppp_close_packet();
      return (1);
      }
#elif MN_SLIP
   if (intface_type == IF_SLIP)
      {
      len=len;                   /* keep compiler happy */
      mn_send_byte(SLIP_END);
      XMIT_INT_ON;
      return (1);
      }
#endif

   return (0);
}

/* Determines socket_type from the src_port. The socket_type is used in
   the callback functions to do special processing. (See callback.c).
*/
byte mn_get_socket_type(word16 src_port)
cmx_reentrant {
   byte retval;

   switch (src_port)
      {
      case DEFAULT_HTTP_PORT:
         /* HTTP sockets always use inactivity timers */
         retval = (HTTP_TYPE | INACTIVITY_TIMER);
         break;
      case FTP_DATA_PORT:
      case FTP_CONTROL_PORT:
         retval = (FTP_TYPE | INACTIVITY_TIMER);
         break;
      case SMTP_PORT:
         retval = SMTP_TYPE;
         break;
      case TFTP_SERVER_PORT:
         retval = TFTP_TYPE;
         break;
      default:
         retval = STD_TYPE;
         break;
      }
   if(src_port==HTTP_PORT){					//wendy
	  retval = (HTTP_TYPE | INACTIVITY_TIMER);

   }
   return (retval);
}

/* return a pointer to a socket matching the passed ports, ip_dest_addr and
   prototype.
*/
PSOCKET_INFO mn_find_socket(word16 src_port, word16 dest_port, \
   byte *dstaddr, byte proto, SCHAR interface_no)
cmx_reentrant {
   byte i;
   PSOCKET_INFO socket_ptr;
#if (MN_BOOTP || MN_DHCP)
   SCHAR bp_mode;

   bp_mode = bootpMode;
#endif      /* (MN_BOOTP || MN_DHCP) */

   socket_ptr = (PSOCKET_INFO)PTR_NULL;

   if ((dstaddr != PTR_NULL) && (interface_no < MN_NUM_INTERFACES))
      {
      for (i=0;i<MN_NUM_SOCKETS;i++)
         {
         if ((sock_info[i].src_port == src_port) && \
               (sock_info[i].dest_port == dest_port) && \
               (sock_info[i].ip_proto == proto) && \
               (sock_info[i].interface_no == interface_no))
            {
#if (MN_BOOTP || MN_DHCP)
            if (( (bp_mode == BOOTP_ACTIVE) && \
                  memcmp((void *)sock_info[i].ip_dest_addr, (void *)broadcast_addr, \
                     IP_ADDR_LEN) == 0) || \
                  memcmp((void *)sock_info[i].ip_dest_addr, (void *)dstaddr, \
                     IP_ADDR_LEN) == 0)
#else
            if (memcmp((void *)sock_info[i].ip_dest_addr, (void *)dstaddr, \
                  IP_ADDR_LEN) == 0)
#endif      /* (MN_BOOTP || MN_DHCP) */
               {
               socket_ptr = &sock_info[i];
               break;
               }
            }
         }
      }

   return (socket_ptr);
}

/* searches for an available socket, returns socket number or negative #
   if none found.
*/
static SCHAR next_socket(void)
cmx_reentrant {
   SCHAR i;
   SCHAR socket_no;

   socket_no = NOT_ENOUGH_SOCKETS;

   for (i=0;i<MN_NUM_SOCKETS;i++)
      {
      if (!SOCKET_ACTIVE(i))
         {
         socket_no = i;
         break;
         }
      }
   return (socket_no);
}


#if (MN_SOCKET_INACTIVITY_TIME)
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif      /* defined(__C51__) */
#if (defined(POLECOG1) || defined(POLECOG1E))
void __irq_code mn_update_inactivity_timers(void)
#elif (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_update_inactivity_timers(void)
#else
void mn_update_inactivity_timers(void)
#endif
cmx_reentrant {
   static word16 count = 0;      /* used to convert timer-ticks to seconds */
   PSOCKET_INFO socket_ptr;
   SCHAR i;

   count++;

   if (count == (MN_ONE_SECOND))
      {
      count = 0;

      for (i=0;i<MN_NUM_SOCKETS;i++)
         {
         if (SOCKET_ACTIVE(i))
            {
            socket_ptr = MK_SOCKET_PTR(i);
            if (socket_ptr->socket_type & INACTIVITY_TIMER)
               {
               if (socket_ptr->inactivity_timer)
                  {
                  socket_ptr->inactivity_timer--;
                  if (!(socket_ptr->inactivity_timer))
                     {
#if (MN_FTP_SERVER)
                     /* Reset FTP server if closing the control port. */
                     if (sock_info[i].src_port == FTP_CONTROL_PORT)
                        mn_ftp_server_init();
#endif

#if (defined(POLECOG1) || defined(CMXECOG1) || defined(POLECOG1E) || defined(CMXECOG1E) || \
   (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E) || \
   defined(POLP18F97J60) || defined(CMXP18F97J60)))
#if MN_TCP
                     if (socket_ptr->ip_proto == PROTO_TCP)
                        {
                        socket_ptr->tcp_state = TCP_CLOSED;
                        socket_ptr->send_ptr = BYTE_PTR_NULL;
#if (MN_SEND_ROM_PKT)
                        socket_ptr->rom_send_ptr = PTR_NULL;
#endif
                        socket_ptr->send_len = 0;
                        socket_ptr->tcp_unacked_bytes = 0;
                        }
#endif
                     CLEAR_SOCKET(i);
                     socket_ptr->src_port = 0;
#else
                     (void)mn_abort(i);
#endif
                     }
                  }
               }
            }
         }
      }
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif      /* defined(__C51__) */

void mn_reset_inactivity_timer(PSOCKET_INFO socket_ptr)
cmx_reentrant {
#if (MN_FTP_SERVER || MN_FTP_CLIENT)
   SCHAR i;

   /* Assumes that INACTIVITY_TIMER is always set for FTP sockets. */

   if (socket_ptr->socket_type & FTP_TYPE)
      {
      for (i=0;i<MN_NUM_SOCKETS;i++)
         {
         socket_ptr = MK_SOCKET_PTR(i);
         if (SOCKET_ACTIVE(i) && (socket_ptr->socket_type & FTP_TYPE))
            {
            DISABLE_INTERRUPTS;
            /* If the following is an atomic instruction the calls to
               DISABLE_INTERRUPTS and ENABLE_INTERRUPTS may be removed.
            */
            //socket_ptr->inactivity_timer = MN_SOCKET_INACTIVITY_TIME; //	  Ajay
			socket_ptr->inactivity_timer = terminal_server.timeout;
            ENABLE_INTERRUPTS;
            }
         }
      }
   else
#endif
      {
      if (socket_ptr->socket_type & INACTIVITY_TIMER)
         {
         DISABLE_INTERRUPTS;
         /* If the following is an atomic instruction the calls to
            DISABLE_INTERRUPTS and ENABLE_INTERRUPTS may be removed.
         */
         //socket_ptr->inactivity_timer = MN_SOCKET_INACTIVITY_TIME; //Ajay
		 socket_ptr->inactivity_timer = terminal_server.timeout;
         ENABLE_INTERRUPTS;
         }
      }
}

#endif      /* (MN_SOCKET_INACTIVITY_TIME) */

#if (RTOS_USED != RTOS_NONE)
/* Returns the signal count for a particular socket. */
SIGNAL_COUNT_T mn_socket_signal_count(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   SIGNAL_COUNT_T retval;

   retval = 0;
   if (socket_ptr != PTR_NULL)
      (void)MN_SIGNAL_COUNT(signal_socket[socket_ptr->socket_no], &retval);

   return (retval);
}
#endif      /* (RTOS_USED != RTOS_NONE) */

/* Returns the amount of room left in a socket's receive buffer. */
word16 mn_socket_recv_buff_space(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   word16 retval;

   retval = 0;
   if ((socket_ptr != PTR_NULL) && (socket_ptr->recv_end != PTR_NULL) && \
         (socket_ptr->recv_ptr != PTR_NULL))
      {
      retval = socket_ptr->recv_end - socket_ptr->recv_ptr + 1;
      retval = retval - socket_ptr->recv_len;
      }

   return (retval);
}

#if (MN_TCP && MN_TCP_DELAYED_ACKS)
/* Find an unused resend_info entry. Returns 0 or positive number if
   successful , negative number on error.
*/
int mn_get_next_resend_info(void)
cmx_reentrant {
   int retval;
   int i;

   retval = -1;

   for (i=0; i<MN_TCP_RESEND_BUFFS; i++)
      {
      if (resend_info[i].socket_no == -1)
         {
         retval = i;
         break;
         }
      }

   return (retval);
}

/* Frees resend_info entries used by the passed socket number. */
void mn_free_resend_info(SCHAR socket_no)
cmx_reentrant {
   int i;

   for (i=0; i<MN_TCP_RESEND_BUFFS; i++)
      {
      if (resend_info[i].socket_no == socket_no)
         resend_info[i].socket_no = -1;
      }
}

/* Look for the resend_info entries associated with a socket. */
void mn_get_ri_info(PRI_INFO ri_ptr, PSOCKET_INFO socket_ptr)
cmx_reentrant {
   byte i;

   if ((ri_ptr != PTR_NULL) && (socket_ptr != PTR_NULL))
      {
      ri_ptr->ri1 = 0;
      ri_ptr->ri2 = 0;
      ri_ptr->ri_cnt = 0;

      for (i=0; i<MN_TCP_RESEND_BUFFS; i++)
         {
         if (resend_info[i].socket_no == socket_ptr->socket_no)
            {
            ri_ptr->ri_cnt++;
            if (socket_ptr->packet_count == 1)
               {
               /* We have the resend_info entry */
               ri_ptr->ri1 = i;
               break;
               }
            else
               {
               if (ri_ptr->ri_cnt == 1)
                  {
                  /* We have one resend_info entry */
                  ri_ptr->ri1 = i;
                  }
               else
                  {
                  /* We have the second resend_info entry */
                  ri_ptr->ri2 = i;
                  break;
                  }
               }
            }
         }
      }
}

/* Resend TCP packets if necessary. If two are waiting for an ACK on the same
   socket, then resend the oldest one first.
*/
void mn_resend_tcp(void)
cmx_reentrant {
   RI_INFO_T ri_info;
   byte i;
   PSOCKET_INFO socket_ptr;

   for (i=0; i<MN_NUM_SOCKETS; i++)
      {
      socket_ptr = MK_SOCKET_PTR(i);
      if (SOCKET_ACTIVE(i) && socket_ptr->tcp_unacked_bytes && \
            TCP_TIMER_EXPIRED(socket_ptr) && socket_ptr->tcp_resends)
         {
         /* Get resend_info entries for the socket. */
         mn_get_ri_info(&ri_info, socket_ptr);

         /* Resend the TCP packet(s) associated with this socket. */
         if (ri_info.ri_cnt == 1)
            packet_resend(socket_ptr, ri_info.ri1);
         else if (ri_info.ri_cnt == 2)
            {
            if (ri_info.ri1 < ri_info.ri2)
               {
               packet_resend(socket_ptr, ri_info.ri1);
               packet_resend(socket_ptr, ri_info.ri2);
               }
            else
               {
               packet_resend(socket_ptr, ri_info.ri2);
               packet_resend(socket_ptr, ri_info.ri1);
               }
            }

         if (!socket_ptr->tcp_resends)
            mn_tcp_abort(socket_ptr);
         }
      }
}

static void packet_resend(PSOCKET_INFO socket_ptr, byte ri_index)
cmx_reentrant {
   PRESEND_INFO resend_ptr;
   byte *old_send_ptr;
   SEND_LEN_T old_send_len;
   PINTERFACE_INFO interface_ptr;
#if (MN_NUM_INTERFACES > 1)
   SCHAR old_interface_no;
#endif

   if (!socket_ptr->tcp_resends)
      return;

   /* Save the old send info. */
   old_send_ptr = socket_ptr->send_ptr;
   old_send_len = socket_ptr->send_len;

   /* Clear send_ptr and send_len because the entire packet is in the
      resend buffer.
   */
   socket_ptr->send_ptr = PTR_NULL;
   socket_ptr->send_len = 0;

#if (MN_NUM_INTERFACES > 1)
   /* Change to the interface used. */
   old_interface_no = send_interface_no;
   send_interface_no = socket_ptr->interface_no;
#endif

   /* Resend the packet. */
   if (mn_transmit_ready(send_interface_no))
      {
      /* Set the send info to the saved packet. */
      resend_ptr = MK_RESEND_PTR(ri_index);
      interface_ptr = MK_INTERFACE_PTR(send_interface_no);
      (void)memcpy((void *)interface_ptr->send_in_ptr, \
         (void *)resend_ptr->packet_buff, resend_ptr->packet_size);
      interface_ptr->send_in_ptr = interface_ptr->send_in_ptr + \
         resend_ptr->packet_size;

      (void)mn_close_packet(socket_ptr, 0);
      RESET_TCP_TIMER(socket_ptr);
      }

#if (MN_NUM_INTERFACES > 1)
   /* Restore the send interface. */
   send_interface_no = old_interface_no;
#endif

   /* Restore the send info. */
   socket_ptr->send_ptr = old_send_ptr;
   socket_ptr->send_len = old_send_len;
}

#if (RTOS_USED != RTOS_NONE)
void mn_tcp_resend_task(void)
cmx_reentrant {
   while (1)
      {
      mn_resend_tcp();
      MN_TASK_WAIT(MN_ONE_SECOND);
      }
}
#endif      /* (RTOS_USED != RTOS_NONE) */

#endif

