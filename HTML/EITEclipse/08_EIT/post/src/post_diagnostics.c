/*------------------------------------------------------------------------ 
					post_system.c

Description:	Return SSIs for network.htm page
                6. Half Duplex Enabled/Disabled

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

#define PING_DATA_LEN 32
#define DATA_BUFF_LEN (PING_DATA_LEN + ECHO_HEADER_LEN)

byte data_buff[DATA_BUFF_LEN];
word32 ping_count;

#define CSUM_HB_POS 2
#define CSUM_LB_POS 3
#define ID_HB_POS   4
#define ID_LB_POS   5
#define SEQ_HB_POS  6
#define SEQ_LB_POS  7

/*------------------------------------------------------------------------
 Function:    post_defaults()
 Description: POST FUNCTION: Restore Defaults
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_rst_cnt(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte ping_dest_ip_addr[IP_ADDR_LEN] = {192,168,1,30} ;
  SCHAR socket_no;
  PSOCKET_INFO ping_socket_ptr;
  long  status;
  word16 id_num, seq_num, ping_csum, i;
  word16 in_id, in_seq, in_csum;
  word32 csum;
  byte *data_ptr;
  int web_ping_cnt = 0;

  if(mn_http_find_value(BODYptr,(byte *)"cnt_type",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1':                                           
        serial.tx_bytes = serial.rx_bytes = 0;
        mn_http_set_file(socket_ptr, mn_vf_get_entry("diagnostics.htm"));
        break;  
      
      case '2': 						                  
        network.tcp_rx_bytes = network.tcp_tx_bytes = 0; 
        mn_http_set_file(socket_ptr, mn_vf_get_entry("diagnostics.htm"));
        break;  
      
      case '3': 						                  
	    network.udp_rx_bytes = network.udp_tx_bytes = 0; 
        mn_http_set_file(socket_ptr, mn_vf_get_entry("diagnostics.htm"));
        break;  
	  
      case '4': 						                  
	    network.icmp_rx_bytes = network.icmp_tx_bytes = 0;
        mn_http_set_file(socket_ptr, mn_vf_get_entry("diagnostics.htm"));
        break;  
	  
      case '5':
        if(mn_http_find_value(BODYptr,(byte *)"ip",temp_buff))
        {
          if(s_ip_process(temp_buff,0))
          {
            memcpy(ping_dest_ip_addr, temp_buff, IP_ADDR_LEN);
            if(mn_http_find_value(BODYptr,(byte *)"cnt",temp_buff))
            {
              web_ping_cnt = np_atoi((char *)temp_buff);
            }

            socket_no = mn_open(ping_dest_ip_addr, PING_PORT, PING_PORT, NO_OPEN, PROTO_ICMP, STD_TYPE, data_buff, DATA_BUFF_LEN, 0);
            
            if(socket_no <= 0)
            {
              return;
            } 
            
            ping_socket_ptr = MK_SOCKET_PTR(socket_no);

            id_num     = 0x100;
            seq_num    = 0;
            ping_count = 0;

            while(ping_count < web_ping_cnt)
            {
              /* Prapare ICMP echo request */
              data_ptr = &data_buff[0];
              *data_ptr++ = ICMP_ECHO;
              *data_ptr++ = 0;
              *data_ptr++ = 0;
              *data_ptr++ = 0;
              *data_ptr++ = HIGHBYTE(id_num);
              *data_ptr++ = LOWBYTE(id_num);
              *data_ptr++ = HIGHBYTE(seq_num);
              *data_ptr++ = LOWBYTE(seq_num);

              for(i = 0; i < PING_DATA_LEN; i++)
              {
                /* PING DATA */
                *data_ptr++ = (byte)i;
              }

              csum = 0;
              csum = mn_data_send_checksum(csum, data_buff, DATA_BUFF_LEN);
              ping_csum = mn_udp_tcp_end_checksum(csum);

              data_buff[CSUM_HB_POS] = HIGHBYTE(ping_csum);
              data_buff[CSUM_LB_POS] = LOWBYTE(ping_csum);

              /* Send the Packet */
              if(mn_ip_send_header(ping_socket_ptr, PROTO_ICMP, DATA_BUFF_LEN) < 0)
              {
                network.icmp_tx_bytes++; //Ajay
				break;
              }

              data_ptr = &data_buff[0];

              for(i = 0; i < DATA_BUFF_LEN; i++)
              {
                mn_send_escaped_byte(*data_ptr++, TRUE);
              }
              
              
              MN_TASK_LOCK;
              (void)mn_close_packet(ping_socket_ptr, 0);
              MN_TASK_UNLOCK;
              
              status = mn_recv_wait(socket_no, 0, 0, MN_ONE_SECOND);

              if(status > 0)
              {
                ping_csum = ping_csum + 0x0800;

                in_csum = MK_WORD16(data_buff[CSUM_HB_POS], data_buff[CSUM_LB_POS]);
                in_id = MK_WORD16(data_buff[ID_HB_POS], data_buff[ID_LB_POS]);
                in_seq = MK_WORD16(data_buff[SEQ_HB_POS], data_buff[SEQ_LB_POS]);


                if((in_csum == ping_csum) && (in_id == id_num) && \
                   (in_seq == seq_num))
                {
                  ping_count++;
                }
                
                MN_TASK_WAIT(MN_ONE_SECOND);
              }
              seq_num = seq_num + PING_DATA_LEN;
            }
            (void)mn_abort(socket_no);
          }
        }
        if((ping_count == web_ping_cnt) && web_ping_cnt > 0)
        {
          mn_http_set_file(socket_ptr, mn_vf_get_entry("host_active.htm"));
        }
        else
        {
          mn_http_set_file(socket_ptr, mn_vf_get_entry("host_down.htm"));
        }
        
        break;

      default :                                           
        serial.tx_bytes = serial.rx_bytes = 0;
        mn_http_set_file(socket_ptr, mn_vf_get_entry("diagnostics.htm"));
        break;
    }
  }
  
  
}
