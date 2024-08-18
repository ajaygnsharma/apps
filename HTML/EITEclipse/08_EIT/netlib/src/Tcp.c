/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"
#include "np_includes.h"

#if MN_TCP
static void update_unacked_bytes(PSOCKET_INFO);
static SCHAR tcp_send_reset(PSOCKET_INFO) cmx_reentrant;
static SCHAR tcp_send_ack(PSOCKET_INFO) cmx_reentrant;
static SCHAR tcp_send_syn_close(PSOCKET_INFO,byte) cmx_reentrant;
static void tcp_update_ack(word16, PSOCKET_INFO) cmx_reentrant;
static SCHAR tcp_send_header(PSOCKET_INFO, byte **) cmx_reentrant;
static SCHAR tcp_send_nodata_hdr(PSOCKET_INFO,byte) cmx_reentrant;
static SCHAR tcp_send_nodata_pkt(PSOCKET_INFO,byte) cmx_reentrant;
static byte tcp_recv_header(word32 *, word16 *,PSOCKET_INFO *, byte **) cmx_reentrant;
static void tcp_recv_send(PSOCKET_INFO) cmx_reentrant;
static PSOCKET_INFO find_tcp_socket(word16,byte *,SCHAR) cmx_reentrant;
static void set_tcp_window(PSOCKET_INFO,PINTERFACE_INFO) cmx_reentrant;

#define tcp_send_fin_ack(p)   tcp_send_nodata_pkt((p),(TCP_FIN_ACK))
#define tcp_send_syn(p)       tcp_send_syn_close((p),(TCP_SYN))
#define tcp_send_syn_ack(p)   tcp_send_syn_close((p),(TCP_SYN_ACK))

/* ----------------------------------------------------------------------- */

/* initialize tcp settings for a socket */
void mn_tcp_init(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   if (socket_ptr != PTR_NULL)
      {
      interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);

      socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;
      set_tcp_window(socket_ptr, interface_ptr);
      socket_ptr->SND_UNA.NUML = ((word32)(socket_ptr->socket_no)<<16) + (MN_GET_TICK) + 1;
      socket_ptr->tcp_state = TCP_CLOSED;
      socket_ptr->tcp_flag = socket_ptr->recv_tcp_flag = 0;
      socket_ptr->tcp_unacked_bytes = 0;
      socket_ptr->data_offset = 0;
      socket_ptr->RCV_NXT.NUML = 0;
      socket_ptr->SEG_SEQ.NUML = 0;
      socket_ptr->SEG_ACK.NUML = 0;
      socket_ptr->tcp_timer.timer_start = 0;
      socket_ptr->tcp_timer.timer_end = 0;
      socket_ptr->tcp_timer.timer_wrap = FALSE;
#if MN_TCP_DELAYED_ACKS
      socket_ptr->packet_count = 0;
      socket_ptr->last_unacked_bytes = 0;
#endif
      }
}

/* open a tcp connection in active or passive mode, returns tcp_state.
   times out after MN_SOCKET_WAIT_TICKS*MN_TCP_RESEND_TRYS ticks if doing an
   ACTIVE_OPEN.
*/
SCHAR mn_tcp_open(byte open_type,PSOCKET_INFO socket_ptr)
cmx_reentrant {
   TIMER_INFO_T wait_timer;
#if (MN_PPP)
   PINTERFACE_INFO interface_ptr;
#endif      /* (MN_PPP) */

   if ((socket_ptr == PTR_NULL) || !SOCKET_ACTIVE(socket_ptr->socket_no))
      return (SOCKET_NOT_FOUND);

#if (MN_PPP)
   interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);
#endif      /* (MN_PPP) */

   switch (open_type)
      {
      case PASSIVE_OPEN:
         socket_ptr->tcp_state = TCP_LISTEN;
         break;
      case ACTIVE_OPEN:
         socket_ptr->tcp_state = TCP_SYN_SENT;
         if (tcp_send_syn(socket_ptr) > 0)
            mn_reset_timer(&wait_timer,((MN_SOCKET_WAIT_TICKS)*(MN_TCP_RESEND_TRYS)));
         else
            socket_ptr->tcp_state = TCP_CLOSED;
         break;
      case NO_OPEN:
         socket_ptr->tcp_state = TCP_LISTEN;
         return (TCP_LISTEN);
      default:
         return (TCP_CLOSED);
      }

   while ((socket_ptr->tcp_state != TCP_ESTABLISHED) && (socket_ptr->tcp_state != TCP_CLOSED))
      {
      if ((open_type == ACTIVE_OPEN) && mn_timer_expired(&wait_timer))
         {
         mn_tcp_abort(socket_ptr);  /* sets tcp_state to TCP_CLOSED */
         break;
         }
#if MN_PPP
      if (interface_ptr->interface_type == IF_PPP)
         {
         if (!MN_PPP_STATUS_UP)
            break;
         }
#endif
      tcp_recv_send(socket_ptr);
      }

   return ((SCHAR)socket_ptr->tcp_state);
}

/* Get a TCP packet. Returns number of data bytes received  if ok, negative
   number not equal to NEED_TO_LISTEN if error, 0 if no data received, or
   NEED_TO_LISTEN if we need to listen again. psocket_ptr will point to the
   socket that received the data.
   mn_ip_recv() MUST be called before calling this function.
*/
long mn_tcp_recv(PSOCKET_INFO *psocket_ptr)
cmx_reentrant {
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   byte odd_byte;
#endif
#if ((MN_PPP || MN_SLIP) || \
      (MN_ETHERNET && !(USE_RECV_BUFF || defined(POLHC12NE64) || \
         defined(CMXHC12NE64))))
   byte c;
#endif
#if (MN_ETHERNET)
#if (!MN_USE_HW_CHKSUM)
   byte * temp_ptr;
#endif
   word16 bytes2end;
#endif
   byte use_recv_callback;
   byte * buff_ptr;
   word16 temp_data;
   word16 data_len;
   word16 TCPcsum;
   word32 csum;
   PSOCKET_INFO socket_ptr;
   word32 temp_num;
   SCHAR retval;
   PINTERFACE_INFO interface_ptr;
#if (RTOS_USED != RTOS_NONE)
   SIGNAL_COUNT_T signal_count;
   word16 start_recv_len;
#endif      /* (RTOS_USED != RTOS_NONE) */
#if (MN_IP_FRAGMENTATION)
   byte fragmented_pkt;
   byte * reassembly_buff_ptr;
#endif

   TCPcsum = 0;
   csum = 0;

   if ((recv_interface_no < 0) || (recv_interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

#if (MN_IP_FRAGMENTATION)
   if (interface_ptr->pfrag_info == PTR_NULL)
      fragmented_pkt = FALSE;
   else
      fragmented_pkt = TRUE;

   if (fragmented_pkt)
      reassembly_buff_ptr = interface_ptr->pfrag_info->reassembly_buff;
   else
      reassembly_buff_ptr = PTR_NULL;

   if (!tcp_recv_header(&csum,&TCPcsum,psocket_ptr,&reassembly_buff_ptr))
#else
   if (!tcp_recv_header(&csum,&TCPcsum,psocket_ptr,PTR_NULL))
#endif
      {
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         ip_clear_frag_info(interface_ptr);
#endif
      mn_ip_discard_packet();
      return (TCP_BAD_HEADER);
      }

    /* shouldn't get here if *psocket_ptr == PTR_NULL */
   socket_ptr = *psocket_ptr;

   data_len = interface_ptr->ip_recv_len;

#if (RTOS_USED == RTOS_NONE)
   socket_ptr->recv_len = 0;
#endif

   use_recv_callback = FALSE;

   if (data_len)
      mn_app_init_recv(data_len,socket_ptr);

   /* Make sure there is room for the data in the socket's buffer. */
#if (MN_HTTP || MN_FTP_SERVER)
   /* Any protocol which processes the data in the callback functions instead
      of in the application layer should be here.
   */
   if ((socket_ptr->socket_type & HTTP_TYPE) || \
         (socket_ptr->src_port == FTP_CONTROL_PORT))
      {
      use_recv_callback = TRUE;
#if (RTOS_USED != RTOS_NONE)
      signal_count = 0;
      socket_ptr->recv_len = 0;
#endif      /* (RTOS_USED != RTOS_NONE) */
      }
   else
#endif
      {
#if (RTOS_USED != RTOS_NONE)
      signal_count = mn_socket_signal_count(socket_ptr);
      if (!signal_count)
         socket_ptr->recv_len = 0;
#endif      /* (RTOS_USED != RTOS_NONE) */
#if (MN_IP_FRAGMENTATION && !MN_COPY_FRAG_DATA)
      if (!(fragmented_pkt))
#endif
         {
         if (data_len)
            {
            if (data_len > mn_socket_recv_buff_space(socket_ptr))
               {
#if (MN_IP_FRAGMENTATION && MN_COPY_FRAG_DATA)
               if (fragmented_pkt)
                  ip_clear_frag_info(interface_ptr);
#endif
               mn_ip_discard_packet();
               return (SOCKET_BUFFER_TOO_SMALL);
               }
            }
         }
      }

#if (RTOS_USED != RTOS_NONE)
   if (signal_count)
      start_recv_len = socket_ptr->recv_len;
   else
      start_recv_len = 0;
#endif      /* (RTOS_USED != RTOS_NONE) */

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type != IF_ETHER)
#endif
      {
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
      csum += mn_udp_tcp_start_checksum(PROTO_TCP,(data_len+socket_ptr->data_offset),\
         socket_ptr->src_port, socket_ptr->dest_port, interface_ptr->ip_src_addr, \
         socket_ptr->ip_dest_addr);
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
      }
#endif

   if (data_len)
      {
#if (RTOS_USED != RTOS_NONE)
      buff_ptr = socket_ptr->recv_ptr + start_recv_len;
#else
      buff_ptr = socket_ptr->recv_ptr;
#endif

#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         {
#if (!MN_USE_HW_CHKSUM)
         csum = mn_data_send_checksum(csum, reassembly_buff_ptr, data_len);
#endif

         if (!use_recv_callback)
            {
#if (MN_COPY_FRAG_DATA)
            (void)memcpy((void *)buff_ptr, (void *)(reassembly_buff_ptr), \
               data_len);
            socket_ptr->recv_len += data_len;
#else
            mn_ip_set_socket_frag_data(interface_ptr, socket_ptr, \
               reassembly_buff_ptr, data_len);
#endif
            }
         else
            {
            for ( ; interface_ptr->ip_recv_len; interface_ptr->ip_recv_len--)
               mn_app_recv_byte(*reassembly_buff_ptr++,socket_ptr);
            }

#if (!(MN_COPY_FRAG_DATA))
         if (use_recv_callback)
#endif
            ip_clear_frag_info(interface_ptr);
         }
      else
#endif
         {
#if (MN_ETHERNET && (USE_RECV_BUFF || defined(POLHC12NE64) || \
   defined(CMXHC12NE64)))
#if (MN_NUM_INTERFACES > 1)
         if (interface_ptr->interface_type == IF_ETHER)
#endif
#if (MN_ETHERNET)
            {
            MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
            if ((interface_ptr->recv_out_ptr + data_len - 1) <= interface_ptr->recv_buff_end_ptr)
               {
               /* We can calculate the checksum in one shot. */
#if (!MN_USE_HW_CHKSUM)
               csum = mn_data_send_checksum(csum, interface_ptr->recv_out_ptr, \
                  data_len);
#endif
               if (!use_recv_callback)
                  {
                  (void)memcpy((void *)buff_ptr, (void *)(interface_ptr->recv_out_ptr), \
                     data_len);
                  interface_ptr->recv_out_ptr += data_len;
                  if (interface_ptr->recv_out_ptr > interface_ptr->recv_buff_end_ptr)
                     interface_ptr->recv_out_ptr = interface_ptr->recv_buff_ptr;
                  }
               }
            else
               {
               /* Handle a wrap in the receive buffer when getting csum. */
               bytes2end = interface_ptr->recv_buff_end_ptr - \
                  interface_ptr->recv_out_ptr + 1;
#if (!MN_USE_HW_CHKSUM)
               temp_ptr = interface_ptr->recv_out_ptr;
#endif
               if (!use_recv_callback)
                  {
                  temp_data = data_len - bytes2end;
                  (void)memcpy((void *)buff_ptr, (void *)(interface_ptr->recv_out_ptr), bytes2end);
                  (void)memcpy((void *)(buff_ptr+bytes2end), (void *)(interface_ptr->recv_buff_ptr), temp_data);
                  interface_ptr->recv_out_ptr = interface_ptr->recv_buff_ptr + temp_data;
                  }
#if (!MN_USE_HW_CHKSUM)
               odd_byte = (byte)(bytes2end & 0x01);
               bytes2end = bytes2end & 0xFFFE;     /* make even number */
               csum = mn_data_send_checksum(csum, temp_ptr, bytes2end);
               if (odd_byte)
                  {
                  /* Not an even number of bytes to the end of the buffer. */
                  temp_data = LSHIFT8(*interface_ptr->recv_buff_end_ptr) | \
                     (word16)(*interface_ptr->recv_buff_ptr);
                  csum += temp_data;
                  bytes2end = data_len - bytes2end - 2;
                  buff_ptr = interface_ptr->recv_buff_ptr + 1;
                  }
               else
                  {
                  bytes2end = data_len - bytes2end;
                  buff_ptr = interface_ptr->recv_buff_ptr;
                  }
               csum = mn_data_send_checksum(csum, buff_ptr, bytes2end);
#endif
               }
            MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

            if (use_recv_callback)
               {
               for ( ; interface_ptr->ip_recv_len; interface_ptr->ip_recv_len--)
                  mn_app_recv_byte((byte)(mn_recv_escaped_byte(TRUE)),socket_ptr);
               }
            else
               {
               socket_ptr->recv_len += data_len;
               DISABLE_INTERRUPTS;
               interface_ptr->recv_count -= data_len;
               ENABLE_INTERRUPTS;
               }
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
            temp_data = 0;
            odd_byte = 0;

            for ( ; interface_ptr->ip_recv_len; interface_ptr->ip_recv_len--)
               {
               c = (byte)(mn_recv_escaped_byte(TRUE));
               if (!odd_byte)
                  {
                  temp_data = LSHIFT8(c);
                  odd_byte = 1;
                  }
               else
                  {
                  temp_data = temp_data + (word16)(c);
                  csum += temp_data;
                  odd_byte = 0;
                  }

               if (use_recv_callback)
                  mn_app_recv_byte(c,socket_ptr);
               else
                 *buff_ptr++ = c;
               }

            if (odd_byte)           /* make sure we get all the csum data */
               csum += temp_data;

            if (!use_recv_callback)
               socket_ptr->recv_len += data_len;
            }
#endif
         }
      }
   else
      {
#if (RTOS_USED == RTOS_NONE)
      socket_ptr->recv_len = 0;
#else
      if (signal_count == 0)
         socket_ptr->recv_len = 0;
#endif      /* (RTOS_USED == RTOS_NONE) */
      }

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type != IF_ETHER)
#endif
      {
/*      if (TCPcsum != mn_udp_tcp_end_checksum(csum)) */
      csum = csum + TCPcsum;
      while (csum >> 16)
         csum = (csum & 0xFFFF) + (csum >> 16);
      if (csum != 0xFFFF)
         {
         /* checksum NG, so ignore packet */
         /* should sent ICMP Parameter Problem Message */
#if (RTOS_USED == RTOS_NONE)
         socket_ptr->recv_len = 0;
#else
         socket_ptr->recv_len = start_recv_len;
#endif      /* (RTOS_USED == RTOS_NONE) */
         mn_ip_discard_packet();
         return (TCP_BAD_CSUM);
         }
      }
#endif

#if MN_PPP
   if (interface_ptr->interface_type == IF_PPP)
      {
      if (data_len)
         {
         /* make sure FCS is okay */
         if(!mn_recv_fcs_ok())
            {
#if (RTOS_USED == RTOS_NONE)
            socket_ptr->recv_len = 0;
#else
            socket_ptr->recv_len = start_recv_len;
#endif      /* (RTOS_USED == RTOS_NONE) */
            mn_ip_discard_packet();
            return (TCP_BAD_FCS);
            }
         }
      }
#endif      /* MN_PPP */

   switch(socket_ptr->tcp_state)
      {
      case TCP_ESTABLISHED:
         break;
      case TCP_CLOSED:
         mn_ip_discard_packet();
         if (!(socket_ptr->tcp_flag & TCP_RST))
            {
            (void)tcp_send_reset(socket_ptr);
            if (socket_ptr->socket_type == AUTO_TYPE)
               {
               (void)mn_abort(socket_ptr->socket_no);
               *psocket_ptr = PTR_NULL;
               }
            }

         return (TCP_NO_CONNECT);
      case TCP_LISTEN:
         mn_ip_discard_packet();
         if (!(socket_ptr->tcp_flag & TCP_RST))
            {
            if (socket_ptr->tcp_flag & (TCP_ACK|TCP_FIN))
               {
               retval = tcp_send_reset(socket_ptr);
               /* Reset socket to previous listening state. */
               if (socket_ptr->recv_tcp_flag & PORT_WAS_ZERO)
                  socket_ptr->dest_port = 0;
               if (socket_ptr->recv_tcp_flag & ADDR_WAS_ZERO)
                  {
                  socket_ptr->ip_dest_addr[0] = 0;
                  socket_ptr->ip_dest_addr[1] = 0;
                  socket_ptr->ip_dest_addr[2] = 0;
                  socket_ptr->ip_dest_addr[3] = 0;
                  }
               socket_ptr->recv_tcp_flag = 0;
               if (retval > 0)
                  return 0;
               else
                  return ((long)retval);
               }
            else
               {
               socket_ptr->RCV_NXT.NUML = socket_ptr->SEG_SEQ.NUML;
               retval = tcp_send_syn_ack(socket_ptr);
               if (retval > 0)
                  socket_ptr->tcp_state = TCP_SYN_RECEIVED;
               else
                  return ((long)retval);
               }
            }
         else
            {
            (void)mn_abort(socket_ptr->socket_no);
            *psocket_ptr = PTR_NULL;
            return TCP_ERROR;
            }

         return NEED_TO_LISTEN;
      case TCP_SYN_RECEIVED:
         /* if we received a valid packet take care of it below. */
         if ( (socket_ptr->tcp_flag & TCP_ACK) && \
               !( socket_ptr->tcp_flag & (TCP_RST|TCP_SYN) ) )
            break;

         /* we received a packet we weren't expecting */
         mn_ip_discard_packet();
         if (socket_ptr->tcp_flag & TCP_RST)
            {
/*            mn_tcp_init(socket_ptr);
            socket_ptr->tcp_state = TCP_LISTEN;
*/
            (void)mn_abort(socket_ptr->socket_no);
            *psocket_ptr = PTR_NULL;
            return TCP_ERROR;
            }
         else if ((socket_ptr->tcp_flag & TCP_SYN) && \
               ((socket_ptr->RCV_NXT.NUML-1) == socket_ptr->SEG_SEQ.NUML))
            {
            /* if we received a SYN which is the same as the one that
               put us into SYN_RECEIVED state then our SYN_ACK must of
               got lost, send it again. For any other SYN send a reset.
            */
            (void)tcp_send_syn_ack(socket_ptr);
            }
         else
            tcp_send_reset(socket_ptr);
         return NEED_TO_LISTEN;
/*         break; */
      case TCP_SYN_SENT:
         mn_ip_discard_packet();
         if (socket_ptr->tcp_flag & TCP_RST)
            {
/*            mn_tcp_abort(socket_ptr); */
            (void)mn_abort(socket_ptr->socket_no);
            *psocket_ptr = PTR_NULL;
            return TCP_ERROR;
            }
         else if (!(socket_ptr->tcp_flag & TCP_ACK) || !(socket_ptr->tcp_flag & TCP_SYN))
            {
            retval = tcp_send_reset(socket_ptr);
            if (retval > 0)
               {
               socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;
               return NEED_TO_LISTEN;
               }
            else
               return ((long)retval);
            }
         socket_ptr->tcp_state = TCP_ESTABLISHED;
         tcp_update_ack(1,socket_ptr);
         retval = tcp_send_ack(socket_ptr);
         if (retval > 0)
            {
            socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;
            return 0;
            }
         else
            return ((long)retval);
      case TCP_FIN_WAIT_1:
         /* There are two possible closing sequences.
            Normal:
            A sends FIN,ACK
            B sends ACK
            B sends FIN,ACK
            A sends ACK

            Simultaneous:
            A sends FIN,ACK
            B sends FIN,ACK
            A sends ACK
            B sends ACK

            In TCP_FIN_WAIT_1 and TCP_FIN_WAIT_2 we are A. In TCP_LAST_ACK
            and CloseIt we are A or B. We always use the normal close when we
            are B.
        */
         mn_ip_discard_packet();
         if (!( socket_ptr->recv_tcp_flag & TCP_FIN ))
            {
            socket_ptr->tcp_state = TCP_FIN_WAIT_2;
            return NEED_TO_LISTEN;
            }
         else
            break;
      case TCP_FIN_WAIT_2:
         if ( socket_ptr->recv_tcp_flag & TCP_FIN )
            {
            /* update seq, ack first for normal close */
            tcp_update_ack(1,socket_ptr);
            }
         mn_ip_discard_packet();
         retval = tcp_send_ack(socket_ptr);
         if (retval > 0)
            {
            socket_ptr->tcp_state = TCP_CLOSED;
            return (TCP_NO_CONNECT);
            }
         else
            return ((long)retval);
      case TCP_LAST_ACK:
         socket_ptr->tcp_state = TCP_CLOSED;
         mn_ip_discard_packet();
         return (TCP_NO_CONNECT);
      default:
         if (socket_ptr->tcp_state > TCP_TIME_WAIT)
            {
            mn_tcp_abort(socket_ptr);
            return TCP_ERROR;
            }
         break;
         }

   temp_num = socket_ptr->RCV_NXT.NUML;
   if (socket_ptr->SEG_SEQ.NUML != temp_num) /* correct sequence # ? */
      {
#if (RTOS_USED == RTOS_NONE)
      socket_ptr->recv_len = 0;
#else
     socket_ptr->recv_len = start_recv_len;
#endif      /* (RTOS_USED == RTOS_NONE) */

      /* Even though it has an incorrect sequence number it still could
         have a valid acknowledgement number, so check it.
      */
      update_unacked_bytes(socket_ptr);

      mn_ip_discard_packet();
      if (!(socket_ptr->tcp_flag & TCP_RST))
         {
         retval = tcp_send_ack(socket_ptr);        /* No, just ACK it. */
         if (retval <= 0)
            return ((long)retval);
         }

      return NEED_TO_LISTEN;
      }

   if (socket_ptr->tcp_flag & TCP_RST)
      {
/*      mn_tcp_abort(socket_ptr); */
      mn_ip_discard_packet();
      (void)mn_abort(socket_ptr->socket_no);
      *psocket_ptr = PTR_NULL;
      return (TCP_ERROR);
      }
   else if (socket_ptr->tcp_flag & TCP_SYN)
      {
      mn_ip_discard_packet();
      retval = tcp_send_reset(socket_ptr);
      if (retval > 0)
         return (NEED_TO_LISTEN);
      else
         return ((long)retval);
      }
   else if (!(socket_ptr->tcp_flag & TCP_ACK))
      {
      mn_ip_discard_packet();
      return NEED_TO_LISTEN;
      }

   if (socket_ptr->tcp_state != TCP_ESTABLISHED)
      socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;

   if (socket_ptr->tcp_state < TCP_FIN_WAIT_1)
      socket_ptr->tcp_state = TCP_ESTABLISHED;

   update_unacked_bytes(socket_ptr);

   if (!data_len)
      {
      mn_ip_discard_packet();
      socket_ptr->RCV_NXT.NUML = socket_ptr->SEG_SEQ.NUML;
      if (!(socket_ptr->recv_tcp_flag & TCP_FIN))
         return 0;
      else
         goto CloseIt;
      }

   retval = mn_app_process_packet(socket_ptr);
   mn_ip_discard_packet();

   /* When mn_app_process_packet returns NEED_IGNORE_PACKET we don't ack the
      packet. We just pretend we never received it. This feature is used
      by the HTTP server to ignore a request it can't handle at the moment.
      The other side will resend the packet later at which point hopefully
      we can handle it.
   */
   if (retval == NEED_IGNORE_PACKET)
      return (0);

   socket_ptr->RCV_NXT.NUML = socket_ptr->SEG_SEQ.NUML + data_len;
   if (!(socket_ptr->recv_tcp_flag & TCP_FIN))
      {
      if ( ( (socket_ptr->socket_type & HTTP_TYPE) && \
            (socket_ptr->send_len == 0) ) ||
            !(socket_ptr->socket_type & HTTP_TYPE) )
         {
         retval = tcp_send_ack(socket_ptr);
         if (retval <= 0)
            return ((long)retval);
         }
#if (RTOS_USED != RTOS_NONE)
      return ((long)(data_len + start_recv_len));
#else
      return ((long)data_len);
#endif
      }

CloseIt:
   if (socket_ptr->tcp_state == TCP_FIN_WAIT_1)
      socket_ptr->SND_UNA.NUML = socket_ptr->SEG_ACK.NUML;
   socket_ptr->RCV_NXT.NUML++;
   retval = tcp_send_ack(socket_ptr);
   if (retval > 0)
      {
      if (socket_ptr->tcp_state == TCP_FIN_WAIT_1)
         {
         socket_ptr->tcp_state = TCP_CLOSED;
         return (TCP_NO_CONNECT);
         }
      else
         {
         RESET_TCP_TIMER(socket_ptr);
         retval = tcp_send_fin_ack(socket_ptr);
         socket_ptr->tcp_state = TCP_LAST_ACK;
         }
      if (retval > 0)
         return (NEED_TO_LISTEN);
      else
         {
         (void)mn_abort(socket_ptr->socket_no);
         *psocket_ptr = PTR_NULL;
         return (TCP_ERROR);
         }
     }
  else
     return ((long)retval);
}

#if 0 /*(MN_TCP_DELAYED_ACKS)*/
word16 num_errs;
#endif

/* Check for valid received ACK and update SND_UNA if needed. */
static void update_unacked_bytes(PSOCKET_INFO socket_ptr)
{
   word16 tcp_unacked_bytes;
   word32 temp_num;
#if (MN_TCP_DELAYED_ACKS)
   RI_INFO_T ri_info;
   byte ri_first, ri_2nd;
#endif

   tcp_unacked_bytes = socket_ptr->tcp_unacked_bytes;
   if (tcp_unacked_bytes)
      {
#if (MN_TCP_DELAYED_ACKS)
      /* The ACK received could be for both packets if we sent two or
         just for one packet. If we sent more than one packet then look
         for the resend_info entry with the lowest num, that
         was the first packet sent on that socket.
      */
      mn_get_ri_info(&ri_info, socket_ptr);

      if (ri_info.ri_cnt == 0)
         {
/*         num_errs++; */
         return;           /* This should not happen... */
         }

      if (ri_info.ri_cnt == 1)
         {
         temp_num = resend_info[ri_info.ri1].SND_UNA.NUML;
         ri_first = ri_info.ri1;
         }
      else
         {
         /* Get the lowest SND_UNA because tcp_unacked_bytes has the
            total bytes sent for both packets.
         */
         if (resend_info[ri_info.ri1].num < resend_info[ri_info.ri2].num)
            {
            temp_num = resend_info[ri_info.ri1].SND_UNA.NUML;
            ri_first = ri_info.ri1;
            ri_2nd = ri_info.ri2;
            }
         else
            {
            temp_num = resend_info[ri_info.ri2].SND_UNA.NUML;
            ri_first = ri_info.ri2;
            ri_2nd = ri_info.ri1;
            }
         }
#else
      temp_num = socket_ptr->SND_UNA.NUML;
#endif

      if ( socket_ptr->SEG_ACK.NUML == (temp_num + tcp_unacked_bytes) )
         {
#if (!MN_TCP_DELAYED_ACKS)
         mn_app_send_complete(tcp_unacked_bytes,socket_ptr);
#endif
         socket_ptr->tcp_unacked_bytes = 0;
         socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;
         socket_ptr->SND_UNA.NUML = socket_ptr->SEG_ACK.NUML;

#if (MN_TCP_DELAYED_ACKS)
         /* We got an ACK for all the outstanding packets. */
         resend_info[ri_info.ri1].socket_no = -1;
         if (ri_info.ri_cnt == 2)
            resend_info[ri_info.ri2].socket_no = -1;
         socket_ptr->last_unacked_bytes = 0;
         socket_ptr->packet_count = 0;
#endif
         }
#if (MN_TCP_DELAYED_ACKS)
      else if ((socket_ptr->packet_count > 1) && \
            ( socket_ptr->SEG_ACK.NUML == (temp_num + socket_ptr->last_unacked_bytes) ))
         {
         /* We got an ACK for the first outstanding packet only. */
         socket_ptr->tcp_unacked_bytes = socket_ptr->tcp_unacked_bytes - \
            socket_ptr->last_unacked_bytes;
         socket_ptr->last_unacked_bytes = 0;
         socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;
         socket_ptr->packet_count--;
         resend_info[ri_first].socket_no = -1;
         if (ri_info.ri_cnt == 2)
            resend_info[ri_2nd].num--;
         }
/*      else
         num_errs++;
*/
#endif
      }
}

/* Send a packet if we are not still waiting on an ack from the last one */
/* we sent. Returns number of data bytes sent, returns negative number if */
/* error or if the packet is too large to send */
long mn_tcp_send(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   word16 send_size, tcp_pkt_len;
   SCHAR retval;
   PINTERFACE_INFO interface_ptr;
#if (MN_PPP || MN_SLIP)
   byte *MsgSendPointer;
#if (MN_SEND_ROM_PKT)
   PCONST_BYTE RomMsgSendPointer;
#endif
#endif      /* (!MN_ETHERNET) */
#if (MN_IP_FRAGMENTATION)
   byte * start_ptr;
#endif
   long ret;
#if (MN_TCP_DELAYED_ACKS)
   int ri_index;
   PRESEND_INFO resend_ptr;
   word16 temp_len;
#endif
   word16 max_len;

   ret = 0;

   if ((socket_ptr == PTR_NULL) || (socket_ptr->src_port == 0) || \
         (socket_ptr->dest_port == 0))
      return (BAD_SOCKET_DATA);

   if ((socket_ptr->interface_no < 0) || (socket_ptr->interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);

   if (!socket_ptr->tcp_resends || (socket_ptr->tcp_state > TCP_TIME_WAIT))
      {
      mn_tcp_abort(socket_ptr);
      return TCP_ERROR;
      }

   if (socket_ptr->tcp_state < TCP_SYN_SENT) /* valid state for send ? */
      return (TCP_NO_CONNECT);
   else if (socket_ptr->tcp_state == TCP_SYN_SENT)
      {
      if (TCP_TIMER_EXPIRED(socket_ptr))
         {
         retval = tcp_send_syn(socket_ptr);
         if (retval <= 0)
            return ((long)retval);
         }
      return 0;
      }

   if (socket_ptr->tcp_unacked_bytes)
      {
#if (MN_TCP_DELAYED_ACKS)
      /* Can only send two packets without getting an ACK. */
      if (socket_ptr->packet_count >= 2)
         return (UNABLE_TO_TCP_SEND);
#else
      if (!TCP_TIMER_EXPIRED(socket_ptr))
         return 0;
#endif
      RESET_TCP_TIMER(socket_ptr);
      }
   else if (socket_ptr->tcp_state >= TCP_FIN_WAIT_1)
      {
      if (TCP_TIMER_EXPIRED(socket_ptr))
         {
         RESET_TCP_TIMER(socket_ptr);
         (void)tcp_send_fin_ack(socket_ptr);
         return 0;
         }
      }

   send_size = mn_app_get_send_size(socket_ptr);
#if (!MN_IP_FRAGMENTATION)
#if (MN_TCP_DELAYED_ACKS)
   /* make sure total send_size <= recv_tcp_window */
   if (send_size > socket_ptr->recv_tcp_window - socket_ptr->tcp_unacked_bytes)
      send_size = 0;
#else
   /* make sure send_size <= recv_tcp_window */
   if (send_size > socket_ptr->recv_tcp_window)
      send_size = socket_ptr->recv_tcp_window;
#endif
   /* make sure send_size <= MSS */
   if (send_size > socket_ptr->mss)
      send_size = socket_ptr->mss;
   /* make sure send_size <= TCP_WINDOW */
   if (send_size > socket_ptr->send_tcp_window)
      send_size = socket_ptr->send_tcp_window;
#endif
   if (!send_size)
      return 0;

   tcp_pkt_len = (send_size + TCP_HEADER_LEN);
#if (MN_IP_FRAGMENTATION)
   if (socket_ptr->recv_tcp_window == 0)
      return 0;

   /* Calculate the maximum IP data len based upon the minimum of the
      TCP_WINDOWS and mss.
   */
   max_len = interface_ptr->mtu - IP_HEADER_LEN - TCP_HEADER_LEN;
   if (max_len > socket_ptr->recv_tcp_window)
      max_len = socket_ptr->recv_tcp_window;
   if (max_len > socket_ptr->mss)
      max_len = socket_ptr->mss;
   if (max_len > socket_ptr->send_tcp_window)
      max_len = socket_ptr->send_tcp_window;
   max_len = max_len + TCP_HEADER_LEN;
#else
   max_len = interface_ptr->mtu - IP_HEADER_LEN;
   if (tcp_pkt_len > max_len)
      return (TCP_TOO_LONG);
#endif

#if (MN_TCP_DELAYED_ACKS)
   /* Get a resend_info entry to hold the data needed to resend the
      packet if necessary.
   */
   ri_index = mn_get_next_resend_info();
   if (ri_index < 0)
      return (UNABLE_TO_TCP_SEND);

   resend_ptr = MK_RESEND_PTR(ri_index);

   socket_ptr->last_unacked_bytes = socket_ptr->tcp_unacked_bytes;
   socket_ptr->tcp_unacked_bytes += send_size;
#else
   socket_ptr->tcp_unacked_bytes = send_size;
#endif

   /* Start a TCP packet.   */
   socket_ptr->tcp_flag = TCP_PSH|TCP_ACK;
#if (MN_IP_FRAGMENTATION)
   retval = tcp_send_header(socket_ptr,&start_ptr);
#else
   retval = tcp_send_header(socket_ptr,PTR_NULL);
#endif
   if (retval != 1)
      {
#if (MN_TCP_DELAYED_ACKS)
      socket_ptr->tcp_unacked_bytes = socket_ptr->last_unacked_bytes;
      socket_ptr->last_unacked_bytes = 0;
#else
      socket_ptr->tcp_unacked_bytes = 0;
#endif
      return ((long)retval);
      }

#if (MN_IP_FRAGMENTATION)
   if (tcp_pkt_len > max_len)
      {
      ret = mn_ip_send_fragments(socket_ptr,tcp_pkt_len,max_len,start_ptr);
      RESET_TCP_TIMER(socket_ptr);
      return (ret);
      }
   else
#endif
      {
#if (MN_PPP || MN_SLIP)
      if ((interface_ptr->interface_type == IF_PPP) || \
            (interface_ptr->interface_type == IF_SLIP))
         {
         MsgSendPointer = socket_ptr->send_ptr;
#if (MN_SEND_ROM_PKT)
         RomMsgSendPointer = socket_ptr->rom_send_ptr;
#endif
         if (MsgSendPointer != PTR_NULL)
            {
            do {  /* Send the packet data. */
               mn_send_escaped_byte(*MsgSendPointer,TRUE);
               MsgSendPointer++;
               --send_size;
               }
            while (send_size);
            }
#if (MN_SEND_ROM_PKT)
         else
            {
            do {  /* Send the packet data. */
               mn_send_escaped_byte(*RomMsgSendPointer,TRUE);
               RomMsgSendPointer++;
               --send_size;
               }
            while (send_size);
            }
#endif
         }
#endif      /* (MN_PPP || MN_SLIP) */

#if (MN_TCP_DELAYED_ACKS)
      socket_ptr->packet_count++;
      resend_ptr->num = socket_ptr->packet_count;
      resend_ptr->socket_no = socket_ptr->socket_no;

      /* Save the resend info. */
      resend_ptr->SND_UNA.NUML = socket_ptr->SND_UNA.NUML;

      /* Copy the packet headers */
      temp_len = interface_ptr->send_in_ptr - interface_ptr->send_out_ptr;
      (void)memcpy((void *)resend_ptr->packet_buff, \
         (void *)interface_ptr->send_buff_ptr, temp_len);

      /* Copy the data */
      if (send_size)
         {
         (void)memcpy((void *)(resend_ptr->packet_buff + temp_len), \
            (void *)socket_ptr->send_ptr, send_size);
         }

      resend_ptr->packet_size = temp_len + send_size;
#endif

      /* At this point for PPP and SLIP send_size will always be zero. This is
         ok because mn_close_packet only uses it for ethernet packets.
      */
      MN_TASK_LOCK;
      retval = mn_close_packet(socket_ptr, send_size);
      if (retval <= 0)
         {
         ret = (long)retval;
         MN_TASK_UNLOCK;
         }
      else
         {
         RESET_TCP_TIMER(socket_ptr);
#if (MN_TCP_DELAYED_ACKS)
         temp_len = socket_ptr->tcp_unacked_bytes - socket_ptr->last_unacked_bytes;
         socket_ptr->SND_UNA.NUML = socket_ptr->SND_UNA.NUML + temp_len;
         mn_app_send_complete(temp_len,socket_ptr);
         ret = (long)temp_len;
#else
         ret = (long)socket_ptr->tcp_unacked_bytes;
#endif
         MN_TASK_UNLOCK;
         }
      }

   return (ret);
}

/* abort the tcp connection */
void mn_tcp_abort(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   if (socket_ptr != PTR_NULL)
      {
      socket_ptr->tcp_state = TCP_CLOSED;
      socket_ptr->send_ptr = BYTE_PTR_NULL;
#if (MN_SEND_ROM_PKT)
      socket_ptr->rom_send_ptr = PTR_NULL;
#endif
      socket_ptr->send_len = 0;
      socket_ptr->tcp_unacked_bytes = 0;

#if (MN_TCP_DELAYED_ACKS)
      mn_free_resend_info(socket_ptr->socket_no);
#endif
      }
}

/* Initiate a close. */
void mn_tcp_close(PSOCKET_INFO socket_ptr)
cmx_reentrant {

   if (socket_ptr != PTR_NULL)
      {
      socket_ptr->tcp_unacked_bytes = 0;
      socket_ptr->send_ptr = BYTE_PTR_NULL;
#if (MN_SEND_ROM_PKT)
      socket_ptr->rom_send_ptr = PTR_NULL;
#endif
      socket_ptr->send_len = 0;
/*      if (socket_ptr->tcp_state < TCP_FIN_WAIT_1) */
      if ((socket_ptr->tcp_state == TCP_SYN_RECEIVED) \
          || (socket_ptr->tcp_state == TCP_ESTABLISHED) \
          || (socket_ptr->tcp_state == TCP_CLOSE_WAIT))
         {
         RESET_TCP_TIMER(socket_ptr);
         socket_ptr->tcp_state = TCP_FIN_WAIT_1;
         (void)tcp_send_fin_ack(socket_ptr);
         }
      }
}

/* shutdown the tcp connection */
void mn_tcp_shutdown(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   TIMER_INFO_T wait_timer;
   word16 timeout;

#if (((MN_TCP_RESEND_TICKS)*(MN_TCP_RESEND_TRYS)) > 65535)
   timeout = 65535;
#else
   timeout = (MN_TCP_RESEND_TICKS)*(MN_TCP_RESEND_TRYS);
#endif

   if (socket_ptr != PTR_NULL)
      {
      mn_tcp_close(socket_ptr);
      mn_reset_timer(&wait_timer, timeout);
      while ((socket_ptr->tcp_state > TCP_CLOSED) && \
            (socket_ptr->tcp_state <= TCP_TIME_WAIT))
         {
#if MN_PPP
         if ((!MN_PPP_STATUS_UP) || (mn_timer_expired(&wait_timer)))
#else
         if (mn_timer_expired(&wait_timer))
#endif
            {
            mn_tcp_abort(socket_ptr);
            break;
            }
         else
            tcp_recv_send(socket_ptr);
         }

#if (MN_TCP_DELAYED_ACKS)
      mn_free_resend_info(socket_ptr->socket_no);
#endif
      }
}

/* ----------------------------------------------------------------------- */
/* does a recv then a send, called from mn_tcp_open and mn_tcp_shutdown */
static void tcp_recv_send(PSOCKET_INFO socket_ptr)
cmx_reentrant {
#if (RTOS_USED == RTOS_NONE)
   byte packet_type;
   long recvd;

   recvd = 0;

   packet_type = mn_ip_recv();
   if (packet_type & TCP_TYPE)
      recvd = mn_tcp_recv(&socket_ptr);
#if MN_UDP
   else if (packet_type & UDP_TYPE)
      {
      (void)mn_udp_recv(&socket_ptr);
      return;
      }
#endif

   if ((recvd != NEED_TO_LISTEN) && (socket_ptr != PTR_NULL))
      (void)mn_tcp_send(socket_ptr);
#else
   long retval;

   retval = 0;

/*   if (MN_SIGNAL_WAIT(signal_socket[socket_ptr->socket_no],(MN_SOCKET_WAIT_TICKS)) == \ */
   if (MN_SIGNAL_WAIT(signal_socket[socket_ptr->socket_no], 1) == \
            SIGNAL_SUCCESS)
      {
      MN_TASK_LOCK;
      retval = socket_ptr->last_return_value;
      MN_TASK_UNLOCK;
      }

   if (retval != NEED_TO_LISTEN)
      (void)mn_tcp_send(socket_ptr);
#endif      /* (RTOS_USED == RTOS_NONE) */
}

/* Get a TCP header. return 1 if ok, 0 on error */
static byte tcp_recv_header(word32 *csum_ptr,word16 *TCPcsum_ptr,PSOCKET_INFO *psocket_ptr,byte **pptr)
cmx_reentrant {
   word16 src_port, dest_port, recv_tcp_window, mss;
   byte opt_len, recv_tcp_flag, data_offset;
   SEQNUM_U SEG_SEQ;
   SEQNUM_U SEG_ACK;
   PSOCKET_INFO socket_ptr;
   SCHAR socket_no;
   byte socket_type, c1, c2, get_mss;
   byte socket_status;
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   word32 csum;
#endif
   PINTERFACE_INFO interface_ptr;
#if (MN_IP_FRAGMENTATION)
   byte fragmented_pkt;
   byte * reassembly_buff_ptr;

   reassembly_buff_ptr = PTR_NULL;
#else
   pptr=pptr;
#endif

#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
   csum_ptr=csum_ptr;
   TCPcsum_ptr=TCPcsum_ptr;
#endif

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);
   socket_status = SAME_SOCKET;

#if (MN_IP_FRAGMENTATION)
   if ((pptr == PTR_NULL) || (*pptr == PTR_NULL))
      fragmented_pkt = FALSE;
   else
      {
      fragmented_pkt = TRUE;
      reassembly_buff_ptr = *pptr;
      }

   if (fragmented_pkt)
      {
      dest_port = LSHIFT8(*reassembly_buff_ptr++);
      dest_port += (word16)(*reassembly_buff_ptr++);
      src_port = LSHIFT8(*reassembly_buff_ptr++);
      src_port += (word16)(*reassembly_buff_ptr++);

#if (CHAR_BIT == 8)
#if (MN_LITTLE_ENDIAN)
#if 1
      SEG_SEQ.NUMC[3] = *reassembly_buff_ptr++;
      SEG_SEQ.NUMC[2] = *reassembly_buff_ptr++;
      SEG_SEQ.NUMC[1] = *reassembly_buff_ptr++;
      SEG_SEQ.NUMC[0] = *reassembly_buff_ptr++;

      SEG_ACK.NUMC[3] = *reassembly_buff_ptr++;
      SEG_ACK.NUMC[2] = *reassembly_buff_ptr++;
      SEG_ACK.NUMC[1] = *reassembly_buff_ptr++;
      SEG_ACK.NUMC[0] = *reassembly_buff_ptr++;
#else
      /* Some older compilers may not break down the above code properly.
         If this is the case then use the code below instead.
      */
      SEG_SEQ.NUMW[1] = LSHIFT8(*reassembly_buff_ptr++);
      SEG_SEQ.NUMW[1] += (word16)(*reassembly_buff_ptr++);
      SEG_SEQ.NUMW[0] = LSHIFT8(*reassembly_buff_ptr++);
      SEG_SEQ.NUMW[0] += (word16)(*reassembly_buff_ptr++);

      SEG_ACK.NUMW[1] = LSHIFT8(*reassembly_buff_ptr++);
      SEG_ACK.NUMW[1] += (word16)(*reassembly_buff_ptr++);
      SEG_ACK.NUMW[0] = LSHIFT8(*reassembly_buff_ptr++);
      SEG_ACK.NUMW[0] += (word16)(*reassembly_buff_ptr++);
#endif      /* 1 */
#else       /* big-endian */
      SEG_SEQ.NUMC[0] = *reassembly_buff_ptr++;
      SEG_SEQ.NUMC[1] = *reassembly_buff_ptr++;
      SEG_SEQ.NUMC[2] = *reassembly_buff_ptr++;
      SEG_SEQ.NUMC[3] = *reassembly_buff_ptr++;

      SEG_ACK.NUMC[0] = *reassembly_buff_ptr++;
      SEG_ACK.NUMC[1] = *reassembly_buff_ptr++;
      SEG_ACK.NUMC[2] = *reassembly_buff_ptr++;
      SEG_ACK.NUMC[3] = *reassembly_buff_ptr++;
#endif      /* (MN_LITTLE_ENDIAN) */
#else
      /* special handling for processors with > 8 bit chars */
      SEG_SEQ.NUML  = WORD32_LSHIFT24(*reassembly_buff_ptr++);
      SEG_SEQ.NUML += WORD32_LSHIFT16(*reassembly_buff_ptr++);
      SEG_SEQ.NUML += WORD32_LSHIFT8(*reassembly_buff_ptr++);
      SEG_SEQ.NUML += *reassembly_buff_ptr++;

      SEG_ACK.NUML  = WORD32_LSHIFT24(*reassembly_buff_ptr++);
      SEG_ACK.NUML += WORD32_LSHIFT16(*reassembly_buff_ptr++);
      SEG_ACK.NUML += WORD32_LSHIFT8(*reassembly_buff_ptr++);
      SEG_ACK.NUML += *reassembly_buff_ptr++;
#endif      /* (CHAR_BIT == 8) */

      data_offset = *reassembly_buff_ptr++;
      recv_tcp_flag = *reassembly_buff_ptr++;
      recv_tcp_window = LSHIFT8(*reassembly_buff_ptr++);
      recv_tcp_window += (word16)(*reassembly_buff_ptr++);
      }
   else
#endif /* MN_IP_FRAGMENTATION */
      {
      /* other side's source port is our dest port and vice versa */
      dest_port = LSHIFT8(mn_recv_escaped_byte(TRUE));
      dest_port += (word16)(mn_recv_escaped_byte(TRUE));
      src_port = LSHIFT8(mn_recv_escaped_byte(TRUE));
      src_port += (word16)(mn_recv_escaped_byte(TRUE));

      //port_check(src_port); // Ajay

#if (CHAR_BIT == 8)
#if (MN_LITTLE_ENDIAN)
#if 1
      SEG_SEQ.NUMC[3] = mn_recv_escaped_byte(TRUE);
      SEG_SEQ.NUMC[2] = mn_recv_escaped_byte(TRUE);
      SEG_SEQ.NUMC[1] = mn_recv_escaped_byte(TRUE);
      SEG_SEQ.NUMC[0] = mn_recv_escaped_byte(TRUE);

      SEG_ACK.NUMC[3] = mn_recv_escaped_byte(TRUE);
      SEG_ACK.NUMC[2] = mn_recv_escaped_byte(TRUE);
      SEG_ACK.NUMC[1] = mn_recv_escaped_byte(TRUE);
      SEG_ACK.NUMC[0] = mn_recv_escaped_byte(TRUE);
#else
      /* Some older compilers may not break down the above code properly.
         If this is the case then use the code below instead.
      */
      SEG_SEQ.NUMW[1] = LSHIFT8(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUMW[1] += (word16)(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUMW[0] = LSHIFT8(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUMW[0] += (word16)(mn_recv_escaped_byte(TRUE));

      SEG_ACK.NUMW[1] = LSHIFT8(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUMW[1] += (word16)(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUMW[0] = LSHIFT8(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUMW[0] += (word16)(mn_recv_escaped_byte(TRUE));
#endif      /* 1 */
#else       /* big-endian */
      SEG_SEQ.NUMC[0] = (byte)(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUMC[1] = (byte)(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUMC[2] = (byte)(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUMC[3] = (byte)(mn_recv_escaped_byte(TRUE));

      SEG_ACK.NUMC[0] = (byte)(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUMC[1] = (byte)(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUMC[2] = (byte)(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUMC[3] = (byte)(mn_recv_escaped_byte(TRUE));
#endif      /* (MN_LITTLE_ENDIAN) */
#else
      /* special handling for processors with > 8 bit chars */
      SEG_SEQ.NUML  = WORD32_LSHIFT24(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUML += WORD32_LSHIFT16(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUML += WORD32_LSHIFT8(mn_recv_escaped_byte(TRUE));
      SEG_SEQ.NUML += mn_recv_escaped_byte(TRUE);

      SEG_ACK.NUML  = WORD32_LSHIFT24(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUML += WORD32_LSHIFT16(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUML += WORD32_LSHIFT8(mn_recv_escaped_byte(TRUE));
      SEG_ACK.NUML += mn_recv_escaped_byte(TRUE);
#endif      /* (CHAR_BIT == 8) */

      data_offset = (byte)(mn_recv_escaped_byte(TRUE));
      recv_tcp_flag = (byte)(mn_recv_escaped_byte(TRUE));
      recv_tcp_window = LSHIFT8(mn_recv_escaped_byte(TRUE));
      recv_tcp_window += (word16)(mn_recv_escaped_byte(TRUE));
      }

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type == IF_ETHER)
      csum = 0;
   else
#endif
      {
#if (CHAR_BIT <= 16)
      csum =  SEG_SEQ.NUMW[0];
      csum += SEG_SEQ.NUMW[1];
      csum += SEG_ACK.NUMW[0];
      csum += SEG_ACK.NUMW[1];
#else
      csum  = LOWWORD(SEG_SEQ.NUML);
      csum += HIGHWORD(SEG_SEQ.NUML);
      csum += LOWWORD(SEG_ACK.NUML);
      csum += HIGHWORD(SEG_ACK.NUML);
#endif
      }
#endif

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type != IF_ETHER)
#endif
      csum += MK_WORD16(data_offset,recv_tcp_flag);
#endif

   data_offset &= 0xF0;                      /* get just the offset */
   data_offset = (byte)(data_offset >> 2);   /* calculate number of bytes. */

   if ((interface_ptr->ip_recv_len < data_offset) || (data_offset < (TCP_HEADER_LEN - 2)))
      {
      *psocket_ptr = PTR_NULL;
      return (0);
      }

   /* subtract it from total length */
   interface_ptr->ip_recv_len = interface_ptr->ip_recv_len - (word16)data_offset;

#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
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
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type == IF_ETHER)
      {
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         reassembly_buff_ptr = reassembly_buff_ptr + 2;
      else
#endif
         {
         (void)mn_recv_escaped_byte(TRUE);
         (void)mn_recv_escaped_byte(TRUE);
         }
      }
   else
#endif
      {
      csum += recv_tcp_window;
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         {
         *TCPcsum_ptr = LSHIFT8(*reassembly_buff_ptr++);
         *TCPcsum_ptr += (word16)(*reassembly_buff_ptr++);
         }
      else
#endif
         {
         *TCPcsum_ptr = LSHIFT8(mn_recv_escaped_byte(TRUE));
         *TCPcsum_ptr += (word16)(mn_recv_escaped_byte(TRUE));
         }
      }
#endif

   /* Skip over urgent pointer and options if not a SYN packet.
      For SYN packets look for MSS option and get the MSS.
      opt_len should be even number.
   */
   mss = 0;
   get_mss = FALSE;
   opt_len = (byte)(data_offset - (TCP_HEADER_LEN - 2));

   while (opt_len)
      {
      opt_len -= 2;
#if (MN_IP_FRAGMENTATION)
      if (fragmented_pkt)
         {
         c1 = *reassembly_buff_ptr++;
         c2 = *reassembly_buff_ptr++;
         }
      else
#endif
         {
         c1 = mn_recv_escaped_byte(TRUE);
         c2 = mn_recv_escaped_byte(TRUE);
         }
      if (get_mss)
         {
         get_mss = FALSE;
         mss = LSHIFT8(c1) | (word16)(c2);
         }
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type != IF_ETHER)
#endif
      {
      csum += LSHIFT8(c1);
      csum += (word16)(c2);
      }
#endif
      if ((recv_tcp_flag & TCP_SYN) && \
            (c1 == TCP_OPT_KIND_MSS) && (c2 == TCP_OPT_LEN_MSS))
         get_mss = TRUE;
      }

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   *csum_ptr = csum;
#endif

#if (MN_IP_FRAGMENTATION)
   if (fragmented_pkt)
      *pptr = reassembly_buff_ptr;
#endif

   /* check for exact match for ports and IP address. */
   socket_ptr = mn_find_socket(src_port, dest_port, \
      interface_ptr->recv_src_addr, PROTO_TCP, recv_interface_no);
   if (socket_ptr == PTR_NULL)
      {
      socket_ptr = mn_find_socket(src_port, dest_port, \
         interface_ptr->recv_src_addr, PROTO_TCP, IF_ANY);
      if (socket_ptr != PTR_NULL)
         {
         socket_ptr->interface_no = recv_interface_no;
#if (MN_NUM_INTERFACES > 1)
         set_tcp_window(socket_ptr, interface_ptr);
#endif
         }
      }

   if (socket_ptr == PTR_NULL)
      {
      /* check for passive open matches. */
      socket_ptr = find_tcp_socket(src_port, interface_ptr->recv_src_addr, \
         recv_interface_no);
      if (socket_ptr == PTR_NULL)
         {
         socket_ptr = find_tcp_socket(src_port, interface_ptr->recv_src_addr, \
            IF_ANY);
         if (socket_ptr != PTR_NULL)
            {
            socket_ptr->interface_no = recv_interface_no;
#if (MN_NUM_INTERFACES > 1)
            set_tcp_window(socket_ptr, interface_ptr);
#endif
            }
         }

      if ((socket_ptr != PTR_NULL) && (recv_tcp_flag & TCP_SYN))
         {
         /* socket found */
         socket_ptr->dest_port = dest_port;
         socket_status = NEW_SOCKET;
         }
      else if ((socket_ptr != PTR_NULL) && \
            ((socket_ptr->dest_port == 0) || (socket_ptr->ip_dest_addr[0] == 0)))
         {
         /* Error, got a match on a listening socket but didn't get a SYN. */
         if (socket_ptr->dest_port == 0)
            {
            socket_ptr->dest_port = dest_port;
            recv_tcp_flag |= PORT_WAS_ZERO;
            }
         if (socket_ptr->ip_dest_addr[0] == 0)
            recv_tcp_flag |= ADDR_WAS_ZERO;
         }
      else if (socket_ptr == PTR_NULL)
         {
         /* socket not found, open a new one with the same type as the
            incoming one.
         */
         if (recv_tcp_flag & TCP_RST)
            {
            /* we got a reset for a socket that wasn't open, so just toss it.
            */
            *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
            return (0);
            }
         socket_type = mn_get_socket_type(src_port);
         socket_no = mn_open_socket(interface_ptr->recv_src_addr,src_port, \
            dest_port,PROTO_TCP,socket_type,0,0,recv_interface_no);
         if (socket_no < 0)
            {
            /* couldn't open a socket, so just toss it.
            */
            *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
            return (0);
            }
         else
            {
            socket_ptr = MK_SOCKET_PTR(socket_no);
#if (MN_HTTP && RTOS_USED == RTOS_NONE)
            /* only allow automatic open of sockets for HTTP */
            if ((recv_tcp_flag & TCP_SYN) && (src_port == HTTP_PORT))
               {
               /* can only open a new HTTP socket if we are done parsing
                  the HTTP header of the previous packet.
               */
               if (http_parse == HTTP_START)
                  socket_status = NEW_SOCKET;
               else
                  {
                  (void)mn_abort(socket_no);
                  *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
                  return (0);
                  }
               }
            else
#endif
#if (MN_FTP_SERVER && RTOS_USED == RTOS_NONE)
            if ((recv_tcp_flag & TCP_SYN) && (src_port == FTP_CONTROL_PORT))
               socket_status = NEW_SOCKET;
            else
#endif      /* (MN_FTP_SERVER) */
               {
               /* set up so mn_tcp_recv will send a reset */
               socket_ptr->tcp_state = TCP_CLOSED;
               if (interface_ptr->ip_recv_len)
                  socket_ptr->RCV_NXT.NUML = SEG_SEQ.NUML + interface_ptr->ip_recv_len;
               else
                  socket_ptr->RCV_NXT.NUML = SEG_SEQ.NUML + 1;
               socket_ptr->socket_type = AUTO_TYPE;
               }
            }
         }
      }

   *psocket_ptr = socket_ptr;

#if (MN_SOCKET_INACTIVITY_TIME)
   /* We got a packet for the socket so reset the inactivity timer if
      required.
   */
   mn_reset_inactivity_timer(socket_ptr);
#endif      /* (MN_SOCKET_INACTIVITY_TIME) */

   socket_ptr->recv_tcp_flag = recv_tcp_flag;
   socket_ptr->tcp_flag = recv_tcp_flag & 0x3F;
   socket_ptr->recv_tcp_window = recv_tcp_window;
   socket_ptr->data_offset = data_offset;
   if (mss)
      socket_ptr->mss = mss;
   socket_ptr->SEG_SEQ.NUML = SEG_SEQ.NUML;
   socket_ptr->SEG_ACK.NUML = SEG_ACK.NUML;
   if (socket_status == NEW_SOCKET)
      socket_ptr->tcp_state = TCP_LISTEN;

#if (MN_ETHERNET && !MN_ARP)
   if (interface_ptr->interface_type == IF_ETHER)
      {
      MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
      (void)memcpy((void *)socket_ptr->eth_dest_hw_addr,(void *)eth_dest_hw_addr,\
         ETH_ADDR_LEN);
      MN_MUTEX_RELEASE(MUTEX_MN_INFO);
      }
#endif

   if (socket_ptr->ip_dest_addr[0] == 0)
      {
      socket_ptr->ip_dest_addr[0] = interface_ptr->recv_src_addr[0];
      socket_ptr->ip_dest_addr[1] = interface_ptr->recv_src_addr[1];
      socket_ptr->ip_dest_addr[2] = interface_ptr->recv_src_addr[2];
      socket_ptr->ip_dest_addr[3] = interface_ptr->recv_src_addr[3];
      }

   return(1);
}

/* Compute checksum and send a TCP Header. Returns 1 if able to
   send packet, otherwise returns a negative number.
*/
static SCHAR tcp_send_header(PSOCKET_INFO socket_ptr, byte **pstart_ptr)
cmx_reentrant {
#if (CHAR_BIT == 8)
   SCHAR i;           /* must be a signed char */
#endif      /* (CHAR_BIT == 8) */
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
   word32 csum;
   word16 tcp_csum;
#endif
#if ((!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)) || MN_IP_FRAGMENTATION)
   PINTERFACE_INFO interface_ptr;
#endif
   byte offset;
   word16 tcp_pkt_len, data_len;
   byte tcp_flag;
   SCHAR retval;

#if (!MN_IP_FRAGMENTATION)
   pstart_ptr=pstart_ptr;
#endif

#if ((!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)) || MN_IP_FRAGMENTATION)
   interface_ptr = MK_INTERFACE_PTR(socket_ptr->interface_no);
#endif

#if (MN_TCP_DELAYED_ACKS)
   data_len = socket_ptr->tcp_unacked_bytes - socket_ptr->last_unacked_bytes;
#else
   data_len = socket_ptr->tcp_unacked_bytes;
#endif
   tcp_pkt_len = data_len;
   tcp_flag = socket_ptr->tcp_flag;
   if (tcp_flag & TCP_SYN)
      {
      /* add MSS option to header if a SYN packet */
      tcp_pkt_len += TCP_OPT_HEADER_LEN;
      offset = TCP_OPT_OFFSET;
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
      if (interface_ptr->interface_type == IF_ETHER)
         csum = 0;
      else
#endif
         {
         csum = CSUM_WORD16(TCP_OPT_KIND_MSS,TCP_OPT_LEN_MSS);
         csum += socket_ptr->send_tcp_window;  /* set MSS to TCP_WINDOW */
         }
#endif
      }
   else
      {
      tcp_pkt_len += TCP_HEADER_LEN;
      offset = TCP_STD_OFFSET;
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
      csum = 0;
#endif
      }

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#if (MN_USE_HW_CHKSUM && MN_ETHERNET)
   if (interface_ptr->interface_type == IF_ETHER)
      tcp_csum = 0;
   else
#endif
      {
/* bww new call parameters */
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
      csum += mn_udp_tcp_start_checksum(PROTO_TCP,tcp_pkt_len, \
            socket_ptr->src_port, socket_ptr->dest_port, interface_ptr->ip_src_addr, \
            socket_ptr->ip_dest_addr);
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */

   /* finish checksum on header */
#if (CHAR_BIT == 8)
      csum += socket_ptr->SND_UNA.NUMW[0];
      csum += socket_ptr->SND_UNA.NUMW[1];
      csum += socket_ptr->RCV_NXT.NUMW[0];
      csum += socket_ptr->RCV_NXT.NUMW[1];
#else
      /* special handling for processors with > 8 bit chars */
      csum += LOWWORD(socket_ptr->SND_UNA.NUML);
      csum += HIGHWORD(socket_ptr->SND_UNA.NUML);
      csum += LOWWORD(socket_ptr->RCV_NXT.NUML);
      csum += HIGHWORD(socket_ptr->RCV_NXT.NUML);
#endif      /* (CHAR_BIT == 8) */

      csum += MK_WORD16(offset,tcp_flag);
      csum += socket_ptr->send_tcp_window;

      if (socket_ptr->send_ptr != PTR_NULL)
         {
         /* checksum on data */
         csum = mn_data_send_checksum(csum, socket_ptr->send_ptr, data_len);
         }
#if (MN_SEND_ROM_PKT)
      else
         {
         /* checksum on data */
         csum = mn_rom_data_send_checksum(csum, socket_ptr->rom_send_ptr, data_len);
         }
#endif

      /* finish checksum */
      tcp_csum = mn_udp_tcp_end_checksum(csum);
      }
#endif

   /* Send the IP header. */
   retval = mn_ip_send_header(socket_ptr,PROTO_TCP,tcp_pkt_len);
   if (retval != 1)
      return (retval);

#if (MN_IP_FRAGMENTATION)
   /* Save start of TCP header in transmit buffer. */
   if (pstart_ptr != PTR_NULL)
      *pstart_ptr = interface_ptr->send_in_ptr;
#endif

   mn_send_escaped_byte(HIGHBYTE(socket_ptr->src_port),TRUE);
   mn_send_escaped_byte(LOWBYTE(socket_ptr->src_port),TRUE);
   mn_send_escaped_byte(HIGHBYTE(socket_ptr->dest_port),TRUE);
   mn_send_escaped_byte(LOWBYTE(socket_ptr->dest_port),TRUE);
#if (CHAR_BIT == 8)
#if MN_LITTLE_ENDIAN
   for (i=3;i>=0;i--)
#else
   for (i=0;i<4;i++)
#endif
      mn_send_escaped_byte(socket_ptr->SND_UNA.NUMC[i],TRUE);
#if MN_LITTLE_ENDIAN
   for (i=3;i>=0;i--)
#else
   for (i=0;i<4;i++)
#endif
      mn_send_escaped_byte(socket_ptr->RCV_NXT.NUMC[i],TRUE);
#else
   /* special handling for processors with > 8 bit chars */
   mn_send_escaped_byte(WORD32_BYTE3(socket_ptr->SND_UNA.NUML),TRUE);
   mn_send_escaped_byte(WORD32_BYTE2(socket_ptr->SND_UNA.NUML),TRUE);
   mn_send_escaped_byte(WORD32_BYTE1(socket_ptr->SND_UNA.NUML),TRUE);
   mn_send_escaped_byte(WORD32_BYTE0(socket_ptr->SND_UNA.NUML),TRUE);

   mn_send_escaped_byte(WORD32_BYTE3(socket_ptr->RCV_NXT.NUML),TRUE);
   mn_send_escaped_byte(WORD32_BYTE2(socket_ptr->RCV_NXT.NUML),TRUE);
   mn_send_escaped_byte(WORD32_BYTE1(socket_ptr->RCV_NXT.NUML),TRUE);
   mn_send_escaped_byte(WORD32_BYTE0(socket_ptr->RCV_NXT.NUML),TRUE);
#endif      /* (CHAR_BIT == 8) */

   mn_send_escaped_byte(offset,TRUE);
   mn_send_escaped_byte(tcp_flag,TRUE);
   mn_send_escaped_byte(HIGHBYTE(socket_ptr->send_tcp_window),TRUE);
   mn_send_escaped_byte(LOWBYTE(socket_ptr->send_tcp_window),TRUE);
#if (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)
   mn_send_escaped_byte(0,TRUE);
   mn_send_escaped_byte(0,TRUE);
#else
   mn_send_escaped_byte(HIGHBYTE(tcp_csum),TRUE);
   mn_send_escaped_byte(LOWBYTE(tcp_csum),TRUE);
#endif
   mn_send_escaped_byte(0,TRUE);    /* urgent pointer high byte */
   mn_send_escaped_byte(0,TRUE);    /* urgent pointer low byte */

   if (tcp_flag & TCP_SYN)
      {
      /* add MSS option to header if a SYN packet */
      mn_send_escaped_byte(TCP_OPT_KIND_MSS,TRUE);
      mn_send_escaped_byte(TCP_OPT_LEN_MSS,TRUE);
      mn_send_escaped_byte(HIGHBYTE(socket_ptr->send_tcp_window),TRUE);
      mn_send_escaped_byte(LOWBYTE(socket_ptr->send_tcp_window),TRUE);
      }

   return (1);
}

/* update sequence and acknowledgement numbers */
static void tcp_update_ack(word16 data_len, PSOCKET_INFO socket_ptr)
cmx_reentrant {
   socket_ptr->SND_UNA.NUML = socket_ptr->SEG_ACK.NUML;
   /* Add the length of the received packet to the ACK. */
   socket_ptr->RCV_NXT.NUML = socket_ptr->SEG_SEQ.NUML + data_len;
}

static SCHAR tcp_send_nodata_hdr(PSOCKET_INFO socket_ptr, byte tcp_flag)
cmx_reentrant {
   socket_ptr->tcp_flag = tcp_flag;
   socket_ptr->send_ptr = PTR_NULL;
#if (MN_SEND_ROM_PKT)
   socket_ptr->rom_send_ptr = PTR_NULL;
#endif
   socket_ptr->send_len = 0;
   socket_ptr->tcp_unacked_bytes = 0;
#if (MN_TCP_DELAYED_ACKS)
   socket_ptr->last_unacked_bytes = 0;
#endif

   return (tcp_send_header(socket_ptr, PTR_NULL));
}

/* Send a TCP packet without any data. returns positive number if able to
   send the packet otherwise returns a negative number.
*/
static SCHAR tcp_send_nodata_pkt(PSOCKET_INFO socket_ptr, byte tcp_flag)
cmx_reentrant {
   SCHAR retval;

   retval = tcp_send_nodata_hdr(socket_ptr,tcp_flag);
   if (retval == 1)
      {
      MN_TASK_LOCK;
      retval = mn_close_packet(socket_ptr, 0);
      MN_TASK_UNLOCK;
      }

   return (retval);
}

/* called from tcp_send_syn_ack & tcp_send_syn to finish sending packet.
   returns positive number if able to send the packet otherwise returns
   a negative number.
*/
static SCHAR tcp_send_syn_close(PSOCKET_INFO socket_ptr, byte tcp_flag)
cmx_reentrant {
   byte *send_ptr;
#if (MN_SEND_ROM_PKT)
   PCONST_BYTE rom_send_ptr;
#endif
   SEND_LEN_T send_len;
   SCHAR retval;

   /* save original values and restore afterwards */
   send_ptr = socket_ptr->send_ptr;
#if (MN_SEND_ROM_PKT)
   rom_send_ptr = socket_ptr->rom_send_ptr;
#endif
   send_len = socket_ptr->send_len;

#if 0
   /* Set the above to 1 if running against a Cisco router. */
   if ((socket_ptr->tcp_resends == MN_TCP_RESEND_TRYS) && (tcp_flag == TCP_SYN_ACK))
#else
   if ((socket_ptr->tcp_resends == MN_TCP_RESEND_TRYS) && (tcp_flag == TCP_SYN))
      {
      if (socket_ptr->SND_UNA.NUML == 0)
         socket_ptr->SND_UNA.NUML = 1;
      }

   if (socket_ptr->tcp_resends == MN_TCP_RESEND_TRYS)
#endif
      socket_ptr->RCV_NXT.NUML++;

   retval = tcp_send_nodata_hdr(socket_ptr,tcp_flag);

   MN_TASK_LOCK;

   if (retval == 1)
      {
      retval = mn_close_packet(socket_ptr, 0);
      if (retval > 0)
         {
         RESET_TCP_TIMER(socket_ptr);
         socket_ptr->SND_UNA.NUML++;
         }
      }

   socket_ptr->send_ptr = send_ptr;
#if (MN_SEND_ROM_PKT)
   socket_ptr->rom_send_ptr = rom_send_ptr;
#endif
   socket_ptr->send_len = send_len;
   MN_TASK_UNLOCK;

   return (retval);
}

/* Send an ACK packet */
static SCHAR tcp_send_ack(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   byte *send_ptr;
#if (MN_SEND_ROM_PKT)
   PCONST_BYTE rom_send_ptr;
#endif
   SEND_LEN_T send_len;
   word16 tcp_unacked_bytes;
#if (MN_TCP_DELAYED_ACKS)
   word16 last_unacked_bytes;
#endif
   SCHAR retval;

   /* save original values and restore afterwards */
   send_ptr = socket_ptr->send_ptr;
#if (MN_SEND_ROM_PKT)
   rom_send_ptr = socket_ptr->rom_send_ptr;
#endif
   send_len = socket_ptr->send_len;
   tcp_unacked_bytes = socket_ptr->tcp_unacked_bytes;
#if (MN_TCP_DELAYED_ACKS)
   last_unacked_bytes = socket_ptr->last_unacked_bytes;
#endif

   retval = tcp_send_nodata_hdr(socket_ptr,TCP_ACK);
   MN_TASK_LOCK;
   if (retval == 1)
      retval = mn_close_packet(socket_ptr, 0);
   socket_ptr->tcp_resends = MN_TCP_RESEND_TRYS;
   socket_ptr->send_ptr = send_ptr;
#if (MN_SEND_ROM_PKT)
   socket_ptr->rom_send_ptr = rom_send_ptr;
#endif
   socket_ptr->send_len = send_len;
   socket_ptr->tcp_unacked_bytes = tcp_unacked_bytes;
#if (MN_TCP_DELAYED_ACKS)
   socket_ptr->last_unacked_bytes = last_unacked_bytes;
#endif
   MN_TASK_UNLOCK;

   return (retval);
}

/* Send a reset packet */
static SCHAR tcp_send_reset(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   byte tcp_flag;

   socket_ptr->SND_UNA.NUML = socket_ptr->SEG_ACK.NUML;

/*   tcp_flag = socket_ptr->recv_tcp_flag == TCP_SYN ? (TCP_RST|TCP_ACK) : TCP_RST; */
   if (socket_ptr->recv_tcp_flag == TCP_SYN)
      tcp_flag = (byte)(TCP_RST|TCP_ACK);
   else
      tcp_flag = (byte)(TCP_RST);
   return (tcp_send_nodata_pkt(socket_ptr,tcp_flag));
}

static PSOCKET_INFO find_tcp_socket(word16 src_port, byte *dstaddr, \
   SCHAR interface_no)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;

   /* socket not found, check for passive open (dest_port == 0) */
   socket_ptr = mn_find_socket(src_port, 0, dstaddr, PROTO_TCP, \
      interface_no);
   if (socket_ptr == PTR_NULL)
      socket_ptr = mn_find_socket(src_port, 0, null_addr, PROTO_TCP, \
         interface_no);

   return (socket_ptr);
}

static void set_tcp_window(PSOCKET_INFO socket_ptr, PINTERFACE_INFO interface_ptr)
cmx_reentrant {
   socket_ptr->mss = interface_ptr->mtu - (IP_HEADER_LEN + TCP_HEADER_LEN);
   socket_ptr->recv_tcp_window = DEFAULT_TCP_WINDOW;

#if (MN_ETHERNET && (MN_PPP || MN_SLIP))
   if (interface_ptr->interface_type == IF_ETHER)
#endif
#if (MN_ETHERNET)
      socket_ptr->send_tcp_window = MN_ETH_TCP_WINDOW;
#endif
#if (MN_ETHERNET && (MN_PPP || MN_SLIP))
   else
#endif
#if (MN_PPP || MN_SLIP)
      socket_ptr->send_tcp_window = MN_UART_TCP_WINDOW;
#endif
}

#endif   /* MN_TCP */
