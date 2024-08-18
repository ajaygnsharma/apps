/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if MN_UDP

static PSOCKET_INFO find_udp_socket(word16,word16,byte *,SCHAR) cmx_reentrant;

/* ----------------------------------------------------------------------- */

/* Get a UDP packet. Returns number of data bytes received on the socket,
   negative number on error, or 0 if no data received or the destination
   socket does not exist. psocket_ptr will point to the socket that received
   the data. mn_ip_recv() MUST be called before calling this function.
*/
long mn_udp_recv(PSOCKET_INFO *psocket_ptr)
cmx_reentrant {
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   word32 csum;
   word16 udp_csum, computed_csum;
#endif
   word16 src_port, dest_port;
   word16 udp_pkt_len, data_len;
   PSOCKET_INFO socket_ptr;
   byte *check_addr;
   byte *buff_ptr;
   PINTERFACE_INFO interface_ptr;
#if (RTOS_USED != RTOS_NONE)
   word16 start_recv_len;
#endif      /* (RTOS_USED != RTOS_NONE) */
#if (MN_IP_FRAGMENTATION)
   byte fragmented_pkt;
   byte * reassembly_buff_ptr;

   reassembly_buff_ptr = PTR_NULL;
#endif

   if ((recv_interface_no < 0) || (recv_interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

#if (MN_IP_FRAGMENTATION)
   if (interface_ptr->pfrag_info == PTR_NULL)
      fragmented_pkt = FALSE;
   else
      fragmented_pkt = TRUE;

   if (fragmented_pkt)
      {
      reassembly_buff_ptr = interface_ptr->pfrag_info->reassembly_buff;
      dest_port = LSHIFT8(*reassembly_buff_ptr++);
      dest_port += (word16)(*reassembly_buff_ptr++);
      src_port = LSHIFT8(*reassembly_buff_ptr++);
      src_port += (word16)(*reassembly_buff_ptr++);
      }
   else
#endif
      {
      dest_port = LSHIFT8(mn_recv_escaped_byte(TRUE));
      dest_port += (word16)(mn_recv_escaped_byte(TRUE));
      src_port = LSHIFT8(mn_recv_escaped_byte(TRUE));
      src_port += (word16)(mn_recv_escaped_byte(TRUE));
      }

#if (MN_ALLOW_MULTICAST)
   if ((interface_ptr->recv_dest_addr[0] >= 224) && \
         (interface_ptr->recv_dest_addr[0] <= 239))
      check_addr = interface_ptr->recv_dest_addr;
   else
#endif      /* (MN_ALLOW_MULTICAST) */
#if (MN_ALLOW_BROADCAST)
   if (interface_ptr->recv_dest_addr[3] == 255)
      check_addr = interface_ptr->recv_dest_addr;
   else
#endif
      check_addr = interface_ptr->recv_src_addr;

   /* check what socket the packet goes to on current interface. */
   socket_ptr = find_udp_socket(src_port, dest_port, check_addr, \
      recv_interface_no);
   if (socket_ptr == PTR_NULL)
      {
      /* check what socket the packet goes to on any interface. */
      socket_ptr = find_udp_socket(src_port, dest_port, check_addr, IF_ANY);
      if (socket_ptr != PTR_NULL)
         socket_ptr->interface_no = recv_interface_no;
      }
   if (socket_ptr != PTR_NULL)
      {
      /* socket found */
      socket_ptr->dest_port = dest_port;
      }
   else
      {
#if (MN_TFTP)
      /* Look for a tftp bound message, specifically the first message
         from a server when the server changes its own port number.
      */
      socket_ptr = mn_find_socket(src_port, TFTP_SERVER_PORT, \
         interface_ptr->recv_src_addr, PROTO_UDP, recv_interface_no);
      if (socket_ptr != PTR_NULL)
         {
         /* socket found */
         socket_ptr->dest_port = dest_port;
         }
      else
#endif      /* (MN_TFTP) */
         {
         *psocket_ptr = PTR_NULL;
#if (MN_IP_FRAGMENTATION)
         if (fragmented_pkt)
            ip_clear_frag_info(interface_ptr);
#endif
         mn_ip_discard_packet();
         return (0);
         }
      }

   *psocket_ptr = socket_ptr;

#if (MN_SOCKET_INACTIVITY_TIME)
   /* We got a packet for the socket so reset the inactivity timer if
      required.
   */
   mn_reset_inactivity_timer(socket_ptr);
#endif      /* (MN_SOCKET_INACTIVITY_TIME) */

#if (MN_ETHERNET && !MN_ARP)
   if (interface_ptr->interface_type == IF_ETHER)
      {
      MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
      (void)memcpy((void *)socket_ptr->eth_dest_hw_addr, \
         (void *)eth_dest_hw_addr, ETH_ADDR_LEN);
      MN_MUTEX_RELEASE(MUTEX_MN_INFO);
      }
#endif

   if (socket_ptr->ip_dest_addr[0] == 0)
      {
#if (MN_ALLOW_MULTICAST)
      if (socket_ptr->socket_type & MULTICAST_TYPE)
         {
         socket_ptr->ip_dest_addr[0] = interface_ptr->recv_dest_addr[0];
         socket_ptr->ip_dest_addr[1] = interface_ptr->recv_dest_addr[1];
         socket_ptr->ip_dest_addr[2] = interface_ptr->recv_dest_addr[2];
         socket_ptr->ip_dest_addr[3] = interface_ptr->recv_dest_addr[3];
         }
      else
#endif      /* (MN_ALLOW_MULTICAST) */
         {
         socket_ptr->ip_dest_addr[0] = interface_ptr->recv_src_addr[0];
         socket_ptr->ip_dest_addr[1] = interface_ptr->recv_src_addr[1];
         socket_ptr->ip_dest_addr[2] = interface_ptr->recv_src_addr[2];
         socket_ptr->ip_dest_addr[3] = interface_ptr->recv_src_addr[3];
         }
      }

#if (MN_IP_FRAGMENTATION)
   if (fragmented_pkt)
      {
      udp_pkt_len = LSHIFT8(*reassembly_buff_ptr++);
      udp_pkt_len += (word16)(*reassembly_buff_ptr++);
      }
   else
#endif
      {
      udp_pkt_len = LSHIFT8(mn_recv_escaped_byte(TRUE));
      udp_pkt_len += (word16)(mn_recv_escaped_byte(TRUE));
      }

   if (udp_pkt_len < UDP_HEADER_LEN)
      {
      *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         ip_clear_frag_info(interface_ptr);
#endif
      mn_ip_discard_packet();
      return (UDP_ERROR);
      }

#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
   /* Eat checksum bytes. */
#if (MN_IP_FRAGMENTATION)
   if (fragmented_pkt)
      reassembly_buff_ptr = reassembly_buff_ptr + 2;
   else
#endif
      {
      (void)mn_recv_escaped_byte(TRUE);
      (void)mn_recv_escaped_byte(TRUE);
      }
#else
   csum = 0;

#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type == IF_ETHER)
      {
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         reassembly_buff_ptr = reassembly_buff_ptr + 2;
      else
#endif
         {
         /* Eat checksum bytes. */
         (void)mn_recv_escaped_byte(TRUE);
         (void)mn_recv_escaped_byte(TRUE);
         }
      udp_csum = 0;
      }
   else
#endif
      {
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         {
         udp_csum = LSHIFT8(*reassembly_buff_ptr++);
         udp_csum += (word16)(*reassembly_buff_ptr++);
         }
      else
#endif
         {
         udp_csum = LSHIFT8(mn_recv_escaped_byte(TRUE));
         udp_csum += (word16)(mn_recv_escaped_byte(TRUE));
         }

      if (udp_csum)           /* if checksum is zero ignore it */
         {
         /* bww new parameters */
#if (MN_BOOTP || MN_DHCP || MN_ALLOW_BROADCAST || MN_ALLOW_MULTICAST)
         csum = mn_udp_tcp_start_checksum(PROTO_UDP, udp_pkt_len,\
            src_port, dest_port, interface_ptr->recv_dest_addr, \
            interface_ptr->recv_src_addr);

#else
#if (MN_PPP || MN_PING_GLEANING || !(MN_ARP))
         MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_PING_GLEANING || !(MN_ARP)) */
         csum = mn_udp_tcp_start_checksum(PROTO_UDP, udp_pkt_len,\
            src_port, dest_port, interface_ptr->ip_src_addr, \
            socket_ptr->ip_dest_addr);
#if (MN_PPP || MN_PING_GLEANING || !(MN_ARP))
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_PING_GLEANING || !(MN_ARP)) */
#endif      /* (MN_BOOTP || MN_DHCP || MN_ALLOW_BROADCAST || MN_ALLOW_MULTICAST) */
         }
      }
#endif

#if (RTOS_USED != RTOS_NONE)
   if (mn_socket_signal_count(socket_ptr))
      start_recv_len = socket_ptr->recv_len;
   else
      {
      start_recv_len = 0;
      socket_ptr->recv_len = 0;
      }
#else
   socket_ptr->recv_len = 0;
#endif      /* (RTOS_USED != RTOS_NONE) */

   udp_pkt_len -= UDP_HEADER_LEN;

#if (MN_IP_FRAGMENTATION && !MN_COPY_FRAG_DATA)
   if (!(fragmented_pkt))
#endif
      {
      /* Make sure there is room for the new data in the socket buffer. */
      if (udp_pkt_len > mn_socket_recv_buff_space(socket_ptr))
         {
         *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
#if (MN_IP_FRAGMENTATION)
         if (fragmented_pkt)
            ip_clear_frag_info(interface_ptr);
#endif
         mn_ip_discard_packet();
         return (SOCKET_BUFFER_TOO_SMALL);
         }
      }

   buff_ptr = socket_ptr->recv_ptr + socket_ptr->recv_len;
   data_len = udp_pkt_len;

#if (MN_IP_FRAGMENTATION)
   if (fragmented_pkt)
      {
#if (MN_COPY_FRAG_DATA)
      /* Copy data into user's buffer. */
      (void)memcpy((void *)buff_ptr, (void *)(reassembly_buff_ptr), \
         data_len);
      ip_clear_frag_info(interface_ptr);
#else
      mn_ip_set_socket_frag_data(interface_ptr, socket_ptr, \
         reassembly_buff_ptr, data_len);
#endif
      }
   else
#endif
      {
      /* Copy data from the receive buffer to the socket's buffer */
#if (RTOS_USED != RTOS_NONE)
#if (MN_NUM_INTERFACES > 1)
      if (interface_ptr->interface_type == IF_ETHER)
#endif
#if (MN_ETHERNET)
         MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif
#endif

      mn_copy_recv_buff(interface_ptr, buff_ptr, data_len);

#if (RTOS_USED != RTOS_NONE)
#if (MN_NUM_INTERFACES > 1)
      if (interface_ptr->interface_type == IF_ETHER)
#endif
#if (MN_ETHERNET)
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif
#endif
      }

#if (MN_IP_FRAGMENTATION && !MN_COPY_FRAG_DATA)
   if (!(fragmented_pkt))
#endif
      socket_ptr->recv_len += data_len;

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   if (udp_csum)
      {
#if (RTOS_USED == RTOS_NONE)
      buff_ptr = socket_ptr->recv_ptr;
#else
      buff_ptr = socket_ptr->recv_ptr + start_recv_len;
#endif      /* (RTOS_USED != RTOS_NONE) */

      /* checksum on data */
      csum = mn_data_send_checksum(csum, buff_ptr, data_len);

      /* finish checksum */
      computed_csum = mn_udp_tcp_end_checksum(csum);

      /* Note: if the checksum is computed to be zero it is sent as 0xFFFF. */
      if (udp_csum != computed_csum)
         {
         if (!((computed_csum == 0) && (udp_csum == 0xFFFF)))
            {
            /* checksum NG, so ignore packet */
#if (RTOS_USED == RTOS_NONE)
            socket_ptr->recv_len = 0;
#else
            socket_ptr->recv_len = start_recv_len;
#endif      /* (RTOS_USED == RTOS_NONE) */
            mn_ip_discard_packet();
            return (UDP_BAD_CSUM);
            }
         }
      }
#endif

#if MN_PPP
   if (interface_ptr->interface_type == IF_PPP)
      {
      if(!mn_recv_fcs_ok())
         {
#if (RTOS_USED == RTOS_NONE)
         socket_ptr->recv_len = 0;
#else
         socket_ptr->recv_len = start_recv_len;
#endif      /* (RTOS_USED == RTOS_NONE) */
         mn_ip_discard_packet();
         return (UDP_BAD_FCS);
         }
      }
#endif      /* MN_PPP */

/*   (void)mn_app_process_packet(socket_ptr); */

   mn_ip_discard_packet();

   return ((long)socket_ptr->recv_len);
}

/* Send IP header, UDP header and data. */
/* Returns number of data bytes sent or negative number on error. */
long mn_udp_send(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   word16 data_len;
   word16 udp_pkt_len;
   byte *MsgSendPointer;
#if (MN_SEND_ROM_PKT)
   PCONST_BYTE RomMsgSendPointer;
#endif
#if (MN_UDP_CHKSUM)
   word32 csum;
   word16 udp_csum;
#endif
#if (MN_IP_FRAGMENTATION)
   byte * start_ptr;
#endif
   word16 max_len;
   SCHAR retval;
   PINTERFACE_INFO interface_ptr;

   if (socket_ptr == PTR_NULL)
      return (BAD_SOCKET_DATA);

   if ((socket_ptr->interface_no < 0) || (socket_ptr->interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);

   data_len = socket_ptr->send_len;
   if (data_len == 0)
      return (0);

   udp_pkt_len = data_len + UDP_HEADER_LEN;
   max_len = interface_ptr->mtu - IP_HEADER_LEN;
#if (!MN_IP_FRAGMENTATION)
   if (udp_pkt_len > max_len)
      return (UDP_TOO_LONG);
#endif

   MsgSendPointer = socket_ptr->send_ptr;
#if (MN_SEND_ROM_PKT)
   RomMsgSendPointer = socket_ptr->rom_send_ptr;
#endif

#if (MN_UDP_CHKSUM)
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type == IF_ETHER)
      udp_csum = 0;
   else
#endif
      {
      /* bww new parameters */
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
      csum = mn_udp_tcp_start_checksum(PROTO_UDP,udp_pkt_len,\
         socket_ptr->src_port, socket_ptr->dest_port, interface_ptr->ip_src_addr, \
         socket_ptr->ip_dest_addr);
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */

      /* checksum on data */
      if (MsgSendPointer != PTR_NULL)
         csum = mn_data_send_checksum(csum, MsgSendPointer, data_len);
#if (MN_SEND_ROM_PKT)
      else
         csum = mn_rom_data_send_checksum(csum, RomMsgSendPointer, data_len);
#endif

      /* finish checksum */
      udp_csum = mn_udp_tcp_end_checksum(csum);

      /* Note: if the checksum is computed to be zero it is sent as 0xFFFF. */
      if (udp_csum == 0)
         udp_csum = 0xFFFF;
      }
#endif      /* (MN_UDP_CHKSUM) */

   retval = mn_ip_send_header(socket_ptr,PROTO_UDP,udp_pkt_len);
   if (retval != 1)
      return ((long)retval);
#if (MN_IP_FRAGMENTATION)
   /* Save start of UDP header in transmit buffer. */
   start_ptr = interface_ptr->send_in_ptr;
#endif
   mn_send_escaped_byte(HIGHBYTE(socket_ptr->src_port),TRUE);
   mn_send_escaped_byte(LOWBYTE(socket_ptr->src_port),TRUE);
   mn_send_escaped_byte(HIGHBYTE(socket_ptr->dest_port),TRUE);
   mn_send_escaped_byte(LOWBYTE(socket_ptr->dest_port),TRUE);
   mn_send_escaped_byte(HIGHBYTE(udp_pkt_len),TRUE);
   mn_send_escaped_byte(LOWBYTE(udp_pkt_len),TRUE);
#if (MN_UDP_CHKSUM)
   mn_send_escaped_byte(HIGHBYTE(udp_csum),TRUE);
   mn_send_escaped_byte(LOWBYTE(udp_csum),TRUE);
#else    /* no checksum */
   mn_send_escaped_byte(0,TRUE);
   mn_send_escaped_byte(0,TRUE);
#endif      /* (MN_UDP_CHKSUM) */

#if (MN_IP_FRAGMENTATION)
   if (udp_pkt_len > max_len)
      return (mn_ip_send_fragments(socket_ptr,udp_pkt_len,max_len,start_ptr));
   else
#endif
      {
#if (MN_PPP || MN_SLIP)
      if ((interface_ptr->interface_type == IF_PPP) || \
            (interface_ptr->interface_type == IF_SLIP))
         {
         if (MsgSendPointer != PTR_NULL)
            {
            while (data_len)
               {
               mn_send_escaped_byte(*MsgSendPointer++,TRUE);
               data_len--;
               }
            }
#if (MN_SEND_ROM_PKT)
         else
            {
            while (data_len)
               {
               mn_send_escaped_byte(*RomMsgSendPointer++,TRUE);
               data_len--;
               }
            }
#endif
         }
#endif      /* (!MN_ETHERNET) */

      /* At this point for PPP and SLIP data_len will always be zero. This is
         ok because mn_close_packet only uses it for ethernet packets.
      */
      MN_TASK_LOCK;
      retval = mn_close_packet(socket_ptr, data_len);
      MN_TASK_UNLOCK;
      if (retval <= 0)
         return ((long)retval);

      return ((long)socket_ptr->send_len);
      }
}

static PSOCKET_INFO find_udp_socket(word16 src_port, word16 dest_port, \
   byte *dstaddr, SCHAR interface_no)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;

   /* check what socket the packet goes to. check active open first. */
   socket_ptr = mn_find_socket(src_port, dest_port, dstaddr, PROTO_UDP, \
      interface_no);
   if (socket_ptr == PTR_NULL)
      {
      /* socket not found, check for passive open (dest_port == 0) */
      socket_ptr = mn_find_socket(src_port, 0, dstaddr, PROTO_UDP, \
         interface_no);
      if (socket_ptr == PTR_NULL)
         socket_ptr = mn_find_socket(src_port, 0, null_addr, PROTO_UDP, \
            interface_no);
      }
   return (socket_ptr);
}
#endif   /* MN_UDP */

