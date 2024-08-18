/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"
#include "np_includes.h"

static byte ip_recv_header(SCHAR) cmx_reentrant;
static byte is_multicast(SCHAR) cmx_reentrant;

byte null_addr[IP_ADDR_LEN];
#if (MN_DHCP || MN_BOOTP || MN_ALLOW_BROADCAST)
byte broadcast_addr[IP_ADDR_LEN];
#endif      /* (MN_DHCP || MN_BOOTP || MN_ALLOW_BROADCAST) */
#if (MN_ALLOW_MULTICAST)
byte all_hosts_addr[IP_ADDR_LEN];
#endif      /* (MN_ALLOW_MULTICAST) */

#if (MN_ETHERNET)
#if (USE_RECV_BUFF)
static void reset_recv(SCHAR) cmx_reentrant;
#endif      /* (USE_RECV_BUFF) */
#endif      /* (MN_ETHERNET) */

#if (MN_PING_REPLY)
static SCHAR get_ping_pkt(byte c) cmx_reentrant;
static void send_ping_reply(void) cmx_reentrant;

/* add 9 to MN_PING_BUFF_SIZE for the partial header we copy also */
static byte ping_reply_buff[MN_PING_BUFF_SIZE+9];
#endif      /* (MN_PING_REPLY) */

/********************************************************************/

/* Initialize IP variables. */
void mn_ip_init(void)
cmx_reentrant {

   MN_MUTEX_WAIT(MUTEX_IP_RECEIVE,INFINITE_WAIT);

   MEMSET(null_addr, 0, sizeof(null_addr));
#if (MN_PING_REPLY)
   MEMSET(ping_reply_buff, 0, sizeof(ping_reply_buff));
#endif      /* (MN_PING_REPLY) */
#if (MN_DHCP || MN_BOOTP || MN_ALLOW_BROADCAST)
   broadcast_addr[0] = broadcast_addr[1] = broadcast_addr[2] = \
      broadcast_addr[3] = 0xff;
#endif      /* (MN_DHCP || MN_BOOTP || MN_ALLOW_BROADCAST) */

#if (MN_ALLOW_MULTICAST)
   all_hosts_addr[0] = 224;
   all_hosts_addr[1] =   0;
   all_hosts_addr[2] =   0;
   all_hosts_addr[3] =   1;
#endif      /* (MN_ALLOW_MULTICAST) */

#if (MN_IP_FRAGMENTATION)
   mn_ip_frag_init();
#endif

   MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
}

/* Waits for a signal from the ISR that a packet has been received or when
   MN_POLLED_ETHERNET is used polls every other system tick for a packet. If
   a packet is present it processes the packet and wakes the appropriate
   task that is waiting.
*/
#if (RTOS_USED != RTOS_NONE)
void mn_receive_task(void)
cmx_reentrant {
   byte packet_type;
   PSOCKET_INFO socket_ptr;
   long retval;

#if (MN_POLLED_ETHERNET)
   /* Wait for signal from mn_init that it is ok to start. */
   if (MN_SIGNAL_WAIT(SIGNAL_RECEIVE,INFINITE_WAIT) == SIGNAL_SUCCESS)
#endif      /* (MN_POLLED_ETHERNET) */
      {
      while (1)
         {
#if (MN_POLLED_ETHERNET)
         MN_TASK_WAIT(1);
#else
         /* wait for a signal from the ethernet or UART receive ISR */
         if (MN_SIGNAL_WAIT(SIGNAL_RECEIVE,INFINITE_WAIT) == SIGNAL_SUCCESS)
#endif      /* (MN_POLLED_ETHERNET) */
            {

            packet_type = mn_ip_recv();
            /* mn_ip_recv will take care of ARP and ICMP packets */
            if (!(packet_type & (TCP_TYPE|UDP_TYPE|ICMP_REPLY_TYPE)))
               continue;

            socket_ptr = PTR_NULL;
            retval = 0;
#if MN_TCP
            if (packet_type & TCP_TYPE)
               {
               retval = mn_tcp_recv(&socket_ptr);
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
            if (packet_type & ICMP_REPLY_TYPE)
               {
               retval = mn_icmp_recv_reply(&socket_ptr);
               }

            /* signal the task associated with the socket that we have
               received a packet for it and save the return value from
               the TCP or UDP receive function.
            */
            if ((socket_ptr != PTR_NULL) && SOCKET_ACTIVE(socket_ptr->socket_no))
               {
               socket_ptr->last_return_value = retval;
               /* Only signal once per socket. */
               if (mn_socket_signal_count(socket_ptr) == 0)
                  MN_SIGNAL_POST(signal_socket[socket_ptr->socket_no]);
               }

            }     /* SIGNAL_WAIT */
         }        /* while (1) */
      }

   MN_TASK_STOP;     /* Should never reach here ... */
}
#endif      /* (RTOS_USED != RTOS_NONE) */

/* ----------------------------------------------------------------------- */
/* Receives ethernet or PPP or SLIP header and IP header. If an ICMP echo  */
/* request is received it will send an ICMP echo reply. Returns the        */
/* packet_type. It should be checked so the proper routine can be called   */
/* to process the rest of the packet. */
byte mn_ip_recv(void)
cmx_reentrant {
   byte packet_type, c;
#if (MN_NUM_INTERFACES > 1)
   static SCHAR current_interface_no = 0;
   byte i;
#endif      /* (MN_NUM_INTERFACES > 1) */

#if (MN_IP_FRAGMENTATION)
   mn_ip_check_frag_timers();
#endif

#if (MN_NUM_INTERFACES > 1)
   packet_type = 0;

   /* Loop through the interfaces once or until we get a packet at one. */
   for (i=0;i<MN_NUM_INTERFACES;i++)
      {
      if (interface_info[current_interface_no].interface_status & IF_OPEN)
         packet_type = mn_ip_get_pkt(current_interface_no);
      current_interface_no++;
      if (current_interface_no >= MN_NUM_INTERFACES)
         current_interface_no = 0;
      if (packet_type)
         break;
      }
#else
   packet_type = mn_ip_get_pkt(0);
#endif      /* (MN_NUM_INTERFACES > 1) */

   if (packet_type & ICMP_TYPE)        /* ICMP ? */
      {
      c = (byte)(mn_recv_escaped_byte(TRUE));   /* Get the ICMP type. */
      if (c == ICMP_ECHO_REPLY)
         {
         /* Mark that we got a reply to a PING we sent. */
         packet_type = packet_type | ICMP_REPLY_TYPE;
         }
      else
         {
#if (MN_PING_REPLY)
         if (get_ping_pkt(c) >= 0)        /* copy the packet into the buffer */
            {
            /* Signal the ping reply task or send the reply directly. */
#if (RTOS_USED != RTOS_NONE)
            MN_SIGNAL_POST(SIGNAL_PING_REPLY);
#else
            send_ping_reply();
#endif      /* (RTOS_USED != RTOS_NONE) */
            }
#else
         /* PING not enabled */
         mn_ip_discard_packet();       /* get rid of it. */
#endif      /* (MN_PING_REPLY) */
         }
      }

   else if (packet_type & IGMP_TYPE)        /* IGMP ? */
      {
#if (MN_IGMP)
      mn_igmp_process_packet();
#endif      /* (MN_IGMP) */
      mn_ip_discard_packet();       /* get rid of it. */
      }

   return (packet_type);
}

/* Does the bulk of the IP level receiving chores. */
byte mn_ip_get_pkt(SCHAR interface_no)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;
   byte packet_type;
#if MN_PPP
   SCHAR ppp_ok;
#endif      /* MN_PPP */
#if MN_ETHERNET
#if (USE_RECV_BUFF)
   word16 eth_len;
   byte *tmp_ptr;
#endif      /* (USE_RECV_BUFF) */
#endif      /* MN_ETHERNET */

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (0);

#if (RTOS_USED != RTOS_NONE)
   if (MN_MUTEX_WAIT(MUTEX_IP_RECEIVE,(MN_SOCKET_WAIT_TICKS)) != MUTEX_SUCCESS)
      return (0);
#endif      /* (RTOS_USED != RTOS_NONE) */

   recv_interface_no = interface_no;         /* save current interface */
   interface_ptr = MK_INTERFACE_PTR(interface_no);

   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
   interface_ptr->ip_recv_len = 0;
   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

#if (MN_ETHERNET && MN_POLLED_ETHERNET)
   if (interface_ptr->interface_type == IF_ETHER)
      {
      /* if ethernet is in polled mode call ethernet receive routine */
      if (ETHER_POLL_RECV <= 0)
         {
         MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
         return 0;
         }
      }
#if (MN_PPP || MN_SLIP)
   else
      {
      if (!mn_recv_byte_present(interface_no))  /* data present? */
         return 0;
      }
#endif      /* (MN_PPP || MN_SLIP) */
#else
   if (!mn_recv_byte_present(interface_no))  /* data present? */
      {
      MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
      return 0;
      }
#endif      /* (MN_ETHERNET && MN_POLLED_ETHERNET) */

#if MN_ETHERNET
   if (interface_ptr->interface_type == IF_ETHER)
      {
#if (USE_RECV_BUFF)
      /* read the length and calculate next_out pointer. We need this pointer
         for mn_ip_discard_packet.
      */
      eth_len = LSHIFT8(mn_recv_byte());
      eth_len = eth_len + (word16)(mn_recv_byte());

      /* if eth_len is > ETHER_SIZE something is really screwed up, so we
         reset the receive pointers and count, then exit.
      */
      if ((eth_len > ETHER_SIZE) || (eth_len == 0))
         {
         reset_recv(interface_no);
         MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
         return 0;
         }

      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      tmp_ptr = interface_ptr->recv_out_ptr + eth_len;
      if ( tmp_ptr <= interface_ptr->recv_buff_end_ptr )
         interface_ptr->next_out_ptr = tmp_ptr;
      else
         {
         interface_ptr->next_out_ptr = interface_ptr->recv_buff_ptr + \
            (eth_len - (interface_ptr->recv_buff_end_ptr - \
            interface_ptr->recv_out_ptr + 1));
         }
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (USE_RECV_BUFF) */

      /* ether_recv_header() will take care of ARP. It will return 1 if there
         is an IP packet to be processed, 0 if we processed a valid arp packet,
         otherwise returns -1.
      */
      switch (ether_recv_header())
         {
         case 1:
            /* IP packet, handle it below. */
            break;
         case 0:
            /* arp packet, we already finished with it. */
            return (ARP_TYPE);
         default:
            /* error */
            return 0;
         }
      }
#endif      /* MN_ETHERNET */

#if MN_PPP
   if (interface_ptr->interface_type == IF_PPP)
      {
#if (RTOS_USED != RTOS_NONE)
      if (MN_MUTEX_WAIT(MUTEX_PPP,(MN_SOCKET_WAIT_TICKS)) != MUTEX_SUCCESS)
         {
         MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
         return 0;
         }
#endif      /* (RTOS_USED != RTOS_NONE) */
      ppp_ok = (SCHAR)ppp_status.up;

      if (ppp_ok)
         ppp_ok = mn_ppp_state_machine(interface_no, FALSE);   /* process PPP header. */
      if (ppp_ok <= 0)
         {
         MN_MUTEX_RELEASE(MUTEX_PPP);
         MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
         return 0;
         }
      }
#elif MN_SLIP
   if (interface_ptr->interface_type == IF_SLIP)
      {
      /* Check for SLIP_END character */
      if (SLIP_END != (byte)(mn_recv_escaped_byte(TRUE)))
         {
         mn_ip_discard_packet();
         return 0;
         }
      }
#endif

   packet_type = ip_recv_header(interface_no);    /* process IP header. */

   if (!packet_type || (packet_type & UNKNOWN_TYPE))
      mn_ip_discard_packet();

   /* need to toss tcp packets if tcp not included and toss udp packets if
      udp not included.
   */
#if (!MN_TCP)
   else if (packet_type & TCP_TYPE)
      {
      mn_ip_discard_packet();
      packet_type = 0;
      }
#endif

#if (!MN_UDP)
   else if (packet_type & UDP_TYPE)
      {
      mn_ip_discard_packet();
      packet_type = 0;
      }
#endif

#if (MN_ETHERNET && MN_ARP && MN_ARP_AUTO_UPDATE)
   else if ((packet_type & TCP_TYPE) || (packet_type & UDP_TYPE) || \
            (packet_type & ICMP_TYPE))
      {
      /* update ARP cache with received IP & hw addresses */
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
      mn_arp_update(interface_ptr->recv_src_addr,eth_dest_hw_addr);
      MN_MUTEX_RELEASE(MUTEX_MN_INFO);
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      }
#endif      /* (MN_ETHERNET && MN_ARP) */

   return (packet_type);
}

/* ----------------------------------------------------------------------- */
/* Compute the IP checksum then send the IP header. Returns 1 if able to
   send packet, otherwise returns a negative number.
*/
SCHAR mn_ip_send_header(PSOCKET_INFO socket_ptr,byte ip_proto,word16 ip_len)
cmx_reentrant {
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   word16 csum;
#endif
   IP_HEADER_T ip_header;
   static word16 ip_id = 0;
   int memcmp_retval;
   word16 i;
   byte * data_ptr;
   PINTERFACE_INFO interface_ptr;
   SCHAR retval;

   if (socket_ptr == PTR_NULL)
      return (BAD_SOCKET_DATA);

   if ((socket_ptr->interface_no < 0) || (socket_ptr->interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);

#if (MN_DHCP)
   /* Don't allow packets to be sent when in DHCP renewing or rebinding mode
      other than DHCP packets.
   */
   if ((bootpMode != BOOTP_INACTIVE) && \
         (socket_ptr->src_port != DHCP_CLIENT_PORT))
      return (DHCP_LEASE_RENEWING);
#endif      /* (MN_DHCP) */

#if (MN_BOOTP || MN_DHCP)
   if (bootpMode != BOOTP_ACTIVE)
#endif      /* (MN_BOOTP || MN_DHCP) */
      {
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
      memcmp_retval = memcmp((void *)interface_ptr->ip_src_addr,(void *)null_addr,IP_ADDR_LEN);
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
      if (memcmp_retval == 0)
         return (BAD_SOCKET_DATA);
      }

   ip_len = ip_len + IP_HEADER_LEN;
   ip_header.len_hb = HIGHBYTE(ip_len);
   ip_header.len_lb = LOWBYTE(ip_len);

   MN_MUTEX_WAIT(MUTEX_IP_SEND,INFINITE_WAIT);
   ++ip_id;
   ip_header.id_hb = HIGHBYTE(ip_id);
   ip_header.id_lb = LOWBYTE(ip_id);
   MN_MUTEX_RELEASE(MUTEX_IP_SEND);

   if (socket_ptr->socket_type & MULTICAST_TYPE)
      ip_header.ttl = MN_MULTICAST_TTL;
   else
      ip_header.ttl = MN_TIME_TO_LIVE;

   ip_header.csum_hb = 0;
   ip_header.csum_lb = 0;
   ip_header.frag_hb = 0;
   ip_header.frag_lb = 0;
   ip_header.tos = ROUTINE_SERVICE;
   ip_header.ver_ihl = IP_VER_IHL;
   ip_header.proto = ip_proto;

#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
   (void)memcpy((void *)ip_header.srcaddr,(void *)interface_ptr->ip_src_addr,IP_ADDR_LEN);
   (void)memcpy((void *)ip_header.dstaddr,(void *)socket_ptr->ip_dest_addr,IP_ADDR_LEN);
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type != IF_ETHER)
#endif
      {
      csum = ~mn_header_checksum((void *)&ip_header, sizeof(IP_HEADER_T));

      ip_header.csum_hb = HIGHBYTE(csum);
      ip_header.csum_lb = LOWBYTE(csum);
      }
#endif

   /* send the interface header */
   retval = mn_ip_start_packet(interface_ptr, socket_ptr, ip_proto);
   if (retval < 1)
      return (retval);

#if (MN_IP_FRAGMENTATION)
   mn_ip_copy_send_header(socket_ptr->socket_no, &ip_header);
#endif

   data_ptr = (byte *)&ip_header;
   for (i=0; i<sizeof(IP_HEADER_T); i++)
      mn_send_escaped_byte(*data_ptr++, TRUE);

   return (1);
}

/* Send appropriate interface header to start a packet. */
SCHAR mn_ip_start_packet(PINTERFACE_INFO interface_ptr, \
   PSOCKET_INFO socket_ptr, byte ip_proto)
cmx_reentrant{
#if MN_ETHERNET
   byte isBcast;
   SCHAR retval;
#else
   socket_ptr=socket_ptr;
#endif

#if (MN_PPP || MN_SLIP || !(MN_BOOTP || MN_DHCP))
   ip_proto=ip_proto;
#endif

#if MN_ETHERNET
/* bww Check for broadcast flag, tell function to send to 255.255.255.255 if
   broadcast is on, otherwise do not.  Previous version sent a FALSE.
*/

   if (interface_ptr->interface_type == IF_ETHER)
      {
#if (MN_BOOTP || MN_DHCP)
      if (ip_proto == PROTO_ICMP)
         isBcast = FALSE;
      else
         isBcast = (byte)(bootpMode == BOOTP_ACTIVE);
#else
      isBcast = FALSE;
#endif      /* (MN_BOOTP || MN_DHCP) */

      retval = mn_ether_start_packet(socket_ptr, ETH_IP_TYPE, isBcast);
      if (retval != 1)
         return (retval);
      }
#endif      /* MN_ETHERNET */

#if MN_PPP
   if (interface_ptr->interface_type == IF_PPP)
      {
      if (!MN_PPP_STATUS_UP)
         return (PPP_LINK_DOWN);

      if (!mn_ppp_start_ip_packet())
         return (UNABLE_TO_SEND);
      }
#elif MN_SLIP
   if (interface_ptr->interface_type == IF_SLIP)
      {
      if (!mn_transmit_ready(socket_ptr->interface_no))
         return (UNABLE_TO_SEND);
      mn_send_byte(SLIP_END);
      }
#endif      /* MN_PPP */

   return (1);
}

/* eat rest of packet */
void mn_ip_discard_packet(void)
cmx_reentrant {
#if (MN_PPP)
   byte lastChar;
#endif
#if (MN_PPP || MN_SLIP)
   byte thisChar;
   int status;
#endif
#if (MN_ETHERNET && USE_RECV_BUFF)
   RECV_COUNT_T num_skip;
#endif

   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

#if MN_ETHERNET
#if (USE_RECV_BUFF)
   if (interface_ptr->interface_type == IF_ETHER)
      {
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      /* anthing to do? */
      if (interface_ptr->recv_out_ptr != interface_ptr->next_out_ptr)
         {
         if (interface_ptr->recv_out_ptr < interface_ptr->next_out_ptr)
            {
            /* calculate bytes we are skipping */
            num_skip = interface_ptr->next_out_ptr - \
               interface_ptr->recv_out_ptr;
            }
         else                                   /* handle wrap */
            {
            num_skip = interface_ptr->recv_buff_size - \
               (interface_ptr->recv_out_ptr - interface_ptr->next_out_ptr);
            }

         if (num_skip > interface_ptr->recv_count)
            {
            /* if num_skip is greater than recv_count we have a serious
               problem so just reset the receive pointers.
            */
            reset_recv(recv_interface_no);
            }
         else
            {
            DISABLE_INTERRUPTS;
            interface_ptr->recv_count = interface_ptr->recv_count - num_skip;
            interface_ptr->recv_out_ptr = interface_ptr->next_out_ptr;
            ENABLE_INTERRUPTS;
            }
         }

      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
      return;
      }
#else
   if (interface_ptr->interface_type == IF_ETHER)
      {
      eth_mn_ip_discard_packet();
      return;
      }
#endif      /* (USE_RECV_BUFF) */
#endif      /* MN_ETHERNET */

#if (MN_PPP)
   if (interface_ptr->interface_type == IF_PPP)
      {
      thisChar = 0;

      while(1)
         {
         lastChar = thisChar;
         status = mn_recv_byte();
         if (status == RECV_TIMED_OUT)
            break;
         else
            thisChar = (byte)status;
         if (thisChar != PPP_FRAME)
            continue;
         if (lastChar != PPP_ESC)
            break;
         }
      mn_ppp_recv_init();
      MN_MUTEX_RELEASE(MUTEX_PPP);
      }
#elif MN_SLIP
   if (interface_ptr->interface_type == IF_SLIP)
      {
      thisChar = 0;

      while(thisChar != SLIP_END)
         {
         status = mn_recv_byte();
         if (status == RECV_TIMED_OUT)
            break;
         else
            thisChar = (byte)status;
         }
      }
#endif

   MN_MUTEX_RELEASE(MUTEX_IP_RECEIVE);
}

/* get the IP header, returns packet type or 0 if the header is not valid. */
static byte ip_recv_header(SCHAR interface_no)
cmx_reentrant {
   IP_HEADER_T ip_header;
   byte ip_hdr_len;
   byte packet_type;
   byte opt_len;
   word16 i;
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   word16 csum, temp_data;
   word32 csum32;
   byte csum_hb, csum_lb;
#endif
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(interface_no);
   packet_type = 0;

   MEMSET(&ip_header, 0, sizeof(IP_HEADER_T));

   ip_header.ver_ihl = (byte)(mn_recv_escaped_byte(TRUE));
   ip_hdr_len = (ip_header.ver_ihl & 0x0f);
   if ((((ip_header.ver_ihl >> 4) & 0x0f) == IP_VER) && (ip_hdr_len >= MIN_IHL))
      {
      ip_hdr_len <<= 2;       /* convert ip_hdr_len to number of bytes */
      /* Get number of option bytes. */
      opt_len = (byte)(ip_hdr_len - IP_HEADER_LEN);

#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
      /* Eat tos. */
      (void)mn_recv_escaped_byte(TRUE);
#else
      ip_header.tos = (byte)(mn_recv_escaped_byte(TRUE));
#endif

      /* Get length */
      ip_header.len_hb = mn_recv_escaped_byte(TRUE);
      ip_header.len_lb = mn_recv_escaped_byte(TRUE);

      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      interface_ptr->ip_recv_len = MK_WORD16(ip_header.len_hb,ip_header.len_lb);
      if ((interface_ptr->ip_recv_len < IP_HEADER_LEN) || (opt_len & 1))
         {
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
         return (0);
         }

      ip_header.id_hb = mn_recv_escaped_byte(TRUE);
      ip_header.id_lb = mn_recv_escaped_byte(TRUE);

      ip_header.frag_hb = mn_recv_escaped_byte(TRUE);
      ip_header.frag_lb = mn_recv_escaped_byte(TRUE);
      if ((ip_header.frag_hb & (byte)(~MORE_FRAGMENTS_MASK)) || \
            ip_header.frag_lb)
         packet_type |= FRAG_TYPE;

#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
      /* Eat ttl. */
      (void)mn_recv_escaped_byte(TRUE);
#else
      ip_header.ttl = (byte)(mn_recv_escaped_byte(TRUE));
#endif

      ip_header.proto = (byte)(mn_recv_escaped_byte(TRUE));
      switch (ip_header.proto)
         {
         case PROTO_TCP:
            packet_type |= TCP_TYPE;
            break;
         case PROTO_UDP:
            packet_type |= UDP_TYPE;
            break;
         case PROTO_ICMP:
            packet_type |= ICMP_TYPE;
            break;
         case PROTO_IGMP:
            packet_type |= IGMP_TYPE;
            break;
         default:
            packet_type |= UNKNOWN_TYPE;
            break;
         }

#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
      (void)mn_recv_escaped_byte(TRUE);
      (void)mn_recv_escaped_byte(TRUE);
#else
      csum_hb = (byte)(mn_recv_escaped_byte(TRUE));
      csum_lb = (byte)(mn_recv_escaped_byte(TRUE));
#endif

      /* recv_src_addr is used by TCP and UDP to create a new socket */
      interface_ptr->recv_src_addr[0] = (byte)(mn_recv_escaped_byte(TRUE));
      interface_ptr->recv_src_addr[1] = (byte)(mn_recv_escaped_byte(TRUE));
      interface_ptr->recv_src_addr[2] = (byte)(mn_recv_escaped_byte(TRUE));
      interface_ptr->recv_src_addr[3] = (byte)(mn_recv_escaped_byte(TRUE));
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
      (void)memcpy((void *)ip_header.srcaddr, \
         (void *)interface_ptr->recv_src_addr, IP_ADDR_LEN);
#endif

      /* Discard packet if the source IP address is a multicast address. */
      if ((interface_ptr->recv_src_addr[0] >= 224) && \
            (interface_ptr->recv_src_addr[0] <= 239))
         {
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
         return (0);
         }

      interface_ptr->recv_dest_addr[0] = (byte)(mn_recv_escaped_byte(TRUE));
      interface_ptr->recv_dest_addr[1] = (byte)(mn_recv_escaped_byte(TRUE));
      interface_ptr->recv_dest_addr[2] = (byte)(mn_recv_escaped_byte(TRUE));
      interface_ptr->recv_dest_addr[3] = (byte)(mn_recv_escaped_byte(TRUE));
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
      (void)memcpy((void *)ip_header.dstaddr, \
         (void *)interface_ptr->recv_dest_addr, IP_ADDR_LEN);
#endif

#if (MN_PING_GLEANING)
      /* we should only get here if the MAC address of the incoming packet
         matched our MAC address. So, if the MAC addresses match assume the
         incoming IP address is ours and set it. To only have the incoming IP
         address used when we don't know our IP address, comment out the
         second line and uncomment the first line below.
      */
/*    if (interface_ptr->ip_src_addr[0] == 0 && packet_type & ICMP_TYPE) */
      if (packet_type & ICMP_TYPE)
         (void)memcpy(interface_ptr->ip_src_addr, \
            (void *)interface_ptr->recv_dest_addr, IP_ADDR_LEN);
#endif

#if (MN_ALLOW_BROADCAST || MN_BOOTP || MN_DHCP)
      if ( (!is_multicast(interface_no)) && \
            (((interface_ptr->recv_dest_addr[0] != interface_ptr->ip_src_addr[0]) && (interface_ptr->recv_dest_addr[0] != 255)) || \
            ((interface_ptr->recv_dest_addr[1] != interface_ptr->ip_src_addr[1]) && (interface_ptr->recv_dest_addr[1] != 255)) || \
            ((interface_ptr->recv_dest_addr[2] != interface_ptr->ip_src_addr[2]) && (interface_ptr->recv_dest_addr[2] != 255)) || \
            ((interface_ptr->recv_dest_addr[3] != interface_ptr->ip_src_addr[3]) && (interface_ptr->recv_dest_addr[3] != 255))))
#else
      if ( (!is_multicast(interface_no)) && \
            ((interface_ptr->recv_dest_addr[0] != interface_ptr->ip_src_addr[0]) || \
            (interface_ptr->recv_dest_addr[1] != interface_ptr->ip_src_addr[1]) || \
            (interface_ptr->recv_dest_addr[2] != interface_ptr->ip_src_addr[2]) || \
            (interface_ptr->recv_dest_addr[3] != interface_ptr->ip_src_addr[3])))
#endif      /* (MN_ALLOW_BROADCAST) */
         {
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
         return 0;
         }

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
      if (interface_ptr->interface_type != IF_ETHER)
#endif
         csum32 = (word32)mn_header_checksum((void *)&ip_header, \
            sizeof(IP_HEADER_T));
#endif

      /* Handle option bytes, if any. */
      if (opt_len)
         {
         opt_len = opt_len >> 1;
#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
         for (i=0; i<opt_len; i++)
            {
            /* Throw away options. */
            (void)mn_recv_escaped_byte(TRUE);
            (void)mn_recv_escaped_byte(TRUE);
            }
#else
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
         if (interface_ptr->interface_type == IF_ETHER)
            {
            for (i=0; i<opt_len; i++)
               {
               /* Throw away options. */
               (void)mn_recv_escaped_byte(TRUE);
               (void)mn_recv_escaped_byte(TRUE);
               }
            }
         else
#endif
            {
            for (i=0; i<opt_len; i++)
               {
               /* Throw away options, but get their checksum value. */
               temp_data = LSHIFT8(mn_recv_escaped_byte(TRUE));
               temp_data = temp_data + (word16)(mn_recv_escaped_byte(TRUE));
               csum32 = csum32 + temp_data;
               }

            csum32 = (csum32 & 0xFFFF) + (csum32 >> 16);
            }
#endif
         }

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
      if (interface_ptr->interface_type != IF_ETHER)
#endif
         {
         csum = (word16)(~csum32);

         /* validate checksum */
         if ((csum_hb != HIGHBYTE(csum)) || \
               (csum_lb != LOWBYTE(csum)))
            {
            MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
            return(0);
            }

         ip_header.csum_hb = csum_hb;
         ip_header.csum_lb = csum_lb;
         }
#endif

#if (MN_IP_FRAGMENTATION)
      if ((packet_type & FRAG_TYPE) && (packet_type & (TCP_TYPE|UDP_TYPE)))
         {
         packet_type = mn_ip_recv_frag_header(interface_ptr, &ip_header, \
            packet_type);
         }
      else
         {
         interface_ptr->pfrag_info = PTR_NULL;
#endif
         interface_ptr->ip_recv_len = interface_ptr->ip_recv_len - ip_hdr_len;
#if (MN_IP_FRAGMENTATION)
         }
#endif

      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      return(packet_type);       /* ok */
      }
   else
      {
      return(0);     /* error */
      }
}

/* Returns true if the received destination IP address is a valid multicast
   destination address. If multicasting is allowed the all-hosts group is
   always allowed even if IGMP is not enabled.
*/
static byte is_multicast(SCHAR interface_no)
cmx_reentrant {
   byte retval;
#if (MN_IGMP)
   PIGMP_INFO pigmp;
#endif      /* (MN_IGMP) */
#if (MN_ALLOW_MULTICAST)
   PINTERFACE_INFO interface_ptr;
#else

   interface_no = interface_no;
#endif      /* (MN_ALLOW_MULTICAST) */

   retval = FALSE;

#if (MN_ALLOW_MULTICAST)
   interface_ptr = MK_INTERFACE_PTR(interface_no);

   if (memcmp((void *)interface_ptr->recv_dest_addr,(void *)all_hosts_addr,IP_ADDR_LEN) == 0)
      retval = TRUE;
#if (MN_IGMP)
   if (!retval)
      {
      /* Search the igmp list for a valid matching entry. */
      MN_MUTEX_WAIT(MUTEX_IGMP,INFINITE_WAIT);
      pigmp = mn_igmp_search_entry(interface_ptr->recv_dest_addr,interface_no);
      if ((pigmp != PTR_NULL) && pigmp->ref_count && \
            ((pigmp->igmp_state == IGMP_DELAY_MEMBER) || \
            (pigmp->igmp_state == IGMP_IDLE_MEMBER)))
         retval = TRUE;
      MN_MUTEX_RELEASE(MUTEX_IGMP);
      }
#endif      /* (MN_IGMP) */
#endif      /* (MN_ALLOW_MULTICAST) */

   return (retval);
}

#if (MN_PING_REPLY)
#if (RTOS_USED != RTOS_NONE)
void mn_ping_reply_task(void)
cmx_reentrant {
   while (1)
      {
      /* Wait for a signal from mn_receive_task. */
      if (MN_SIGNAL_WAIT(SIGNAL_PING_REPLY,INFINITE_WAIT) == SIGNAL_SUCCESS)
         send_ping_reply();
      }
}
#endif      /* (RTOS_USED != RTOS_NONE) */

/* Reads rest of ping request packet. Returns socket number if ok to send a
   reply, otherwise returns -1.
*/
static SCHAR get_ping_pkt(byte c)
cmx_reentrant {
   SCHAR retval;
   byte csum_hb;
   byte csum_lb;
   word16 ip_len;
   byte *ping_reply_ptr;
#if (MN_ETHERNET && USE_RECV_BUFF)
   word16 bytes2end;
#endif      /* (MN_ETHERNET && USE_RECV_BUFF) */
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

   retval = -1;
   network.icmp_rx_bytes++; //Ajay

   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
   /* Is it an echo request? Is it small enough to fit in ping_reply_buff? */
   if ( (c == ICMP_ECHO) && \
         (interface_ptr->ip_recv_len <= (sizeof(ping_reply_buff)-1)) && \
         (interface_ptr->ip_recv_len >= 4) )
      {
      /* open a socket to send the ping reply */
      retval = mn_open_socket(interface_ptr->recv_src_addr,PING_PORT,\
         PING_PORT,PROTO_ICMP,STD_TYPE,0,0,recv_interface_no);
      if (retval >= 0)
         {
         c = (byte)(mn_recv_escaped_byte(TRUE));  /* skip the code */

         ip_len = interface_ptr->ip_recv_len - 2;
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

         MN_MUTEX_WAIT(MUTEX_PING_BUFF,INFINITE_WAIT);
         /* add socket number and length to ping_reply_buff */
         ping_reply_buff[0] = (byte)retval;
         ping_reply_buff[1] = HIGHBYTE(ip_len);
         ping_reply_buff[2] = LOWBYTE(ip_len);
         ip_len = ip_len - 2;

         /* read and modify the checksum */
         csum_hb = (byte)(mn_recv_escaped_byte(TRUE));
         csum_lb = (byte)(mn_recv_escaped_byte(TRUE));
         if ((csum_hb = csum_hb + 8) < 0x8)
            csum_lb++;
         ping_reply_buff[3] = csum_hb;
         ping_reply_buff[4] = csum_lb;
         ping_reply_ptr = &ping_reply_buff[5];

         /* Copy everything from the receive buffer to the ping reply buffer.
         */
#if (MN_ETHERNET && USE_RECV_BUFF)
         if (interface_ptr->interface_type == IF_ETHER)
            {
            MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
            /* calculate the number of bytes until the end of recv_buff */
            bytes2end = interface_ptr->recv_buff_end_ptr - \
               interface_ptr->recv_out_ptr + 1;

         /* check if we have a wrap in the recv_buff */
            if (bytes2end >= ip_len)                     /* no wrap */
               (void)memcpy(ping_reply_ptr, \
                  (void *)interface_ptr->recv_out_ptr,ip_len);
            else                                         /* handle wrap */
               {
               (void)memcpy(ping_reply_ptr, \
                  (void *)interface_ptr->recv_out_ptr,bytes2end);
               ip_len = ip_len - bytes2end;
               (void)memcpy((ping_reply_ptr+bytes2end), \
                  (void *)interface_ptr->recv_buff_ptr,ip_len);
               }
            MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
            }
         else
#endif
            {
            while (ip_len)
               {
               ip_len--;
               *ping_reply_ptr++ = (byte)(mn_recv_escaped_byte(TRUE));
               }
            }

         MN_MUTEX_RELEASE(MUTEX_PING_BUFF);
         }
      else
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      }
   else
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

   mn_ip_discard_packet();
   return (retval);
}

static void send_ping_reply(void)
cmx_reentrant {
   word16 data_len;
   SCHAR socket_no;
   PSOCKET_INFO socket_ptr;
#if (MN_PPP || MN_SLIP)
   byte *ping_reply_ptr;
#endif      /* (MN_PPP || MN_SLIP) */
   PINTERFACE_INFO interface_ptr;
   
   network.icmp_tx_bytes++;	//Ajay

   MN_MUTEX_WAIT(MUTEX_PING_BUFF,INFINITE_WAIT);

   /* The socket number is the first byte in the ping reply buffer. */
   socket_no = (SCHAR)ping_reply_buff[0];

   if ((socket_no >= 0) && SOCKET_ACTIVE(socket_no))
      {
      socket_ptr = MK_SOCKET_PTR(socket_no);  /* get pointer to the socket */
      interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);
      if (socket_ptr->ip_proto == PROTO_ICMP)
         {
         /* The next two bytes in the ping reply buffer are the length. */
         data_len = LSHIFT8(ping_reply_buff[1]);
         data_len = data_len + (word16)(ping_reply_buff[2]);

         if (data_len <= (sizeof(ping_reply_buff) - 3))
            {
#if (MN_ETHERNET)
            if (interface_ptr->interface_type == IF_ETHER)
               {
               socket_ptr->send_ptr = &ping_reply_buff[3];
               socket_ptr->send_len = data_len;

               /* start sending the reply */
               if (mn_ip_send_header(socket_ptr,PROTO_ICMP,(data_len+2)) == 1)
                  {
                  mn_send_escaped_byte(ICMP_ECHO_REPLY,TRUE);
                  mn_send_escaped_byte(0,TRUE);             /* code */
                  MN_TASK_LOCK;
                  (void)mn_close_packet(socket_ptr, data_len);
                  MN_TASK_UNLOCK;
                  }
               }
#endif      /* (MN_ETHERNET) */

#if (MN_PPP || MN_SLIP)
            if ((interface_ptr->interface_type == IF_PPP) || \
                  (interface_ptr->interface_type == IF_SLIP))
               {
               /* start sending the reply */
               if (mn_ip_send_header(socket_ptr,PROTO_ICMP,(data_len+2)) == 1)
                  {
                  mn_send_escaped_byte(ICMP_ECHO_REPLY,TRUE);
                  mn_send_escaped_byte(0,TRUE);             /* code */
                  ping_reply_ptr = &ping_reply_buff[3];
                  while (data_len)
                     {
                     data_len--;
                     mn_send_escaped_byte(*ping_reply_ptr,TRUE);
                     ping_reply_ptr++;
                     }
                  MN_TASK_LOCK;
                  (void)mn_close_packet(socket_ptr, 0);
                  MN_TASK_UNLOCK;
                  }
               }
#endif      /* (MN_PPP || MN_SLIP) */
            }
         (void)mn_abort(socket_no);
         }
      }

   MN_MUTEX_RELEASE(MUTEX_PING_BUFF);
}
#endif      /* (MN_PING_REPLY) */

#if (MN_ETHERNET)
#if (USE_RECV_BUFF)
static void reset_recv(SCHAR interface_no)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(interface_no);

   DISABLE_INTERRUPTS;
   init_recv(interface_no);
   interface_ptr->next_out_ptr = interface_ptr->recv_buff_ptr;
   ENABLE_INTERRUPTS;
}
#endif      /* (USE_RECV_BUFF) */
#endif      /* (MN_ETHERNET) */

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
word16 mn_header_checksum(void *buff_ptr, word16 num_bytes)
cmx_reentrant {
   word32 csum;
   word16 ip_csum;
#if (MN_DSP)
   byte *data_ptr;
#else
   word16 *data_ptr;
#endif

   csum = 0;

#if (MN_DSP)
   data_ptr = (byte *)buff_ptr;

   /* Assume header sizes are an even number. */
   num_bytes = num_bytes >> 1;
   for (; num_bytes; num_bytes--)
      {
      csum = csum + CSUM_WORD16(*data_ptr, *(data_ptr + 1));
      data_ptr = data_ptr + 2;
      }

   while (csum >> 16)
      csum = (csum & 0xFFFF) + (csum >> 16);
   ip_csum = (word16)csum;

#else

   data_ptr = (word16 *)buff_ptr;

   /* Assume header sizes are an even number and at least 16 bytes. */
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;
   csum = csum + *data_ptr++;

   num_bytes = (num_bytes-16) >> 1;    /* convert remaining bytes to words */

   for ( ; num_bytes; num_bytes--)
      csum = csum + *data_ptr++;

   while (csum >> 16)
      csum = (csum & 0xFFFF) + (csum >> 16);

   ip_csum = (word16)csum;
#if MN_LITTLE_ENDIAN
   ip_csum = hs2net(ip_csum);
#endif

#endif

   return (ip_csum);
}
#endif

#if (MN_IP_FRAGMENTATION || MN_UDP)
void mn_copy_recv_buff(PINTERFACE_INFO interface_ptr, byte *buff_ptr, word16 data_len)
cmx_reentrant {
#if (MN_ETHERNET && (USE_RECV_BUFF || defined(POLHC12NE64) || \
      defined(CMXHC12NE64)))
   word16 temp_data, bytes2end;
#endif

   if ((interface_ptr == PTR_NULL) || (buff_ptr == PTR_NULL))
      return;

#if (MN_ETHERNET && (USE_RECV_BUFF || defined(POLHC12NE64) || \
   defined(CMXHC12NE64)))
#if (MN_NUM_INTERFACES > 1)
   if (interface_ptr->interface_type == IF_ETHER)
#endif
#if (MN_ETHERNET)
      {
      if ((interface_ptr->recv_out_ptr + data_len - 1) <= interface_ptr->recv_buff_end_ptr)
         {
         (void)memcpy((void *)buff_ptr, (void *)(interface_ptr->recv_out_ptr), \
            data_len);
         interface_ptr->recv_out_ptr = interface_ptr->recv_out_ptr + data_len;
         if (interface_ptr->recv_out_ptr > interface_ptr->recv_buff_end_ptr)
            interface_ptr->recv_out_ptr = interface_ptr->recv_buff_ptr;
         }
      else
         {
         bytes2end = interface_ptr->recv_buff_end_ptr - \
            interface_ptr->recv_out_ptr + 1;
         temp_data = data_len - bytes2end;
         (void)memcpy((void *)buff_ptr, (void *)(interface_ptr->recv_out_ptr), \
            bytes2end);
         (void)memcpy((void *)(buff_ptr+bytes2end), (void *)(interface_ptr->recv_buff_ptr), \
            temp_data);
         interface_ptr->recv_out_ptr = interface_ptr->recv_buff_ptr + temp_data;
         }
      DISABLE_INTERRUPTS;
      interface_ptr->recv_count = interface_ptr->recv_count - data_len;
      ENABLE_INTERRUPTS;
      }
#endif
#if (MN_NUM_INTERFACES > 1)
   else
#endif
#endif
#if ((MN_PPP || MN_SLIP) || \
      (MN_ETHERNET && !(USE_RECV_BUFF || defined(POLHC12NE64) || \
         defined(CMXHC12NE64))))
      {
      for (; data_len; data_len--)
         *buff_ptr++ = (byte)(mn_recv_escaped_byte(TRUE));
      }
#endif
}
#endif      /* (MN_IP_FRAGMENTATION || MN_UDP) */

/* Get a ICMP echo reply packet. Returns number of data bytes received or
   negative number on error. psocket_ptr will point to the socket that
   received the data. mn_ip_recv() MUST be called before calling this
   function.
*/

/* Definitions for ICMP header. */
#define CSUM_HB_POS  2
#define CSUM_LB_POS  3

long mn_icmp_recv_reply(PSOCKET_INFO * psocket_ptr)
cmx_reentrant {
   long retval;
   PSOCKET_INFO socket_ptr;
   PINTERFACE_INFO interface_ptr;
   byte * data_ptr;
   word16 data_len, ping_csum, in_csum;
   word32 csum;
   byte in_csum_hb, in_csum_lb;

   retval = -1;

   if ((recv_interface_no < 0) || (recv_interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

   socket_ptr = mn_find_socket(PING_PORT, PING_PORT, \
      interface_ptr->recv_src_addr, PROTO_ICMP, recv_interface_no);

   *psocket_ptr = socket_ptr;
   if (socket_ptr != PTR_NULL)
      {
#if (RTOS_USED != RTOS_NONE)
      /* Exit if we have a packet for the socket that hasn't been taken
         care of yet.
      */
      if (mn_socket_signal_count(socket_ptr))
         {
         mn_ip_discard_packet();       /* get rid of the packet. */
         return (retval);
         }
#endif      /* (RTOS_USED != RTOS_NONE) */

      /* Make sure there is enough room in the socket's buffer. */
      if (socket_ptr->recv_ptr && \
            (mn_socket_recv_buff_space(socket_ptr) >= interface_ptr->ip_recv_len))
         {
         /* Copy the ICMP type byte (we already read this byte in mn_ip_recv)
            and the rest of the ICMP packet into the socket's buffer.
         */
         data_ptr = socket_ptr->recv_ptr;
         *data_ptr++ = ICMP_ECHO_REPLY;
         data_len = interface_ptr->ip_recv_len - 1;
         mn_copy_recv_buff(interface_ptr, data_ptr, data_len);

         /* Verify the ICMP checksum. */
         data_ptr = socket_ptr->recv_ptr;
         data_len = data_len + 1;

         /* Get the ICMP checksum in the packet and temporarily replace it
            with 0.
         */
         in_csum_hb = *(data_ptr+CSUM_HB_POS);
         in_csum_lb = *(data_ptr+CSUM_LB_POS);
         in_csum = MK_WORD16(in_csum_hb, in_csum_lb);
         *(data_ptr+CSUM_HB_POS) = 0;
         *(data_ptr+CSUM_LB_POS) = 0;

         csum = 0;
         csum = mn_data_send_checksum(csum, data_ptr, data_len);
         ping_csum = mn_udp_tcp_end_checksum(csum);

         if (ping_csum == in_csum)
            {
            /* Restore the ICMP checksum. */
            *(data_ptr+CSUM_HB_POS) = in_csum_hb;
            *(data_ptr+CSUM_LB_POS) = in_csum_lb;

            retval = data_len;
            }
         else
            {
            socket_ptr->recv_len = 0;
            }
         }
      else
         {
         retval = SOCKET_BUFFER_TOO_SMALL;
         }
      }

   mn_ip_discard_packet();       /* get rid of the packet. */
   return (retval);
}

