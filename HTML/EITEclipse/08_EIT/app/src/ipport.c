#include "micronet.h"
#include "np_includes.h"

byte find_ip=FALSE;
byte wait_ip=FALSE;
byte reset_on=FALSE;

TIMER_INFO_T ipport_wait_timer;
//TIMER_INFO_T reset_wait_timer;
byte set_ip_buff[4]={0,0,0,0};
//
//void set_ip(PSOCKET_INFO *psocket_ptr)
//reentrant{
//	byte i;
//	PSOCKET_INFO socket_ptr = PTR_NULL;
//	*psocket_ptr=*psocket_ptr;
//	if(wait_ip==TRUE)
//	{
//		if (mn_timer_expired(&ipport_wait_timer))
//		{
//			// if timeout happens, close the socket and restore the previous IP.
//			find_ip=TRUE;
//			wait_ip=FALSE;
//			for (i=0;i<NUM_SOCKETS;i++)
//			{
//				if (SOCKET_ACTIVE(i) && sock_info[i].src_port == IPPORT&& sock_info[i].dest_port!=0)
//				{
//					socket_ptr = &sock_info[i];
//					mn_tcp_close(socket_ptr); 				
//				}
//			}
//			for (i=0;i<IP_ADDR_LEN;i++)
//			{
//				ip_src_addr[i]=set_ip_buff[i];
//			}
//			find_ip=FALSE;
//		}
//	}
//}
//
//void ip_backup(void)
//reentrant{
////	byte i;
// /*	for (i=0;i<IP_ADDR_LEN;i++)
//	{
//		set_ip_buff[i]= ip_src_addr[i];
//        ip_src_addr[i]=recv_dest_addr[i];
//	}*/
//}

void port_check(word16 port)
cmx_reentrant
{
  PINTERFACE_INFO interface_ptr;
  byte i;
	
  interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

  if(port == IPPORT && wait_ip == FALSE)
  {
    for (i=0;i<IP_ADDR_LEN;i++)
    {
      set_ip_buff[i]                = interface_ptr->ip_src_addr[i];
      interface_ptr->ip_src_addr[i] = interface_ptr->recv_dest_addr[i];
    }
    if(wait_ip==FALSE)
    {
      mn_reset_timer(&ipport_wait_timer,1000); // 10 second
    }
    wait_ip=TRUE;
  }
}

//void reset_server(void)
//{
//    TA = 0xAA;      // Open timed-access window by writing 'AA' followed by
//    TA = 0x55;      //     '55' to the TA register to access WDCON register
//    EWT = 1;        // Reset watchdog timer to prevent MCU reset (WDCON.0)
//    TA = 0xAA;      // Open timed-access window by writing 'AA' followed by
//    TA = 0x55;      //     '55' to the TA register to access WDCON register
//
//	EXIT(0);
//}
//
//void start_reset(void)
//{
//	if(reset_on==FALSE)
//	{
//		mn_reset_timer(&reset_wait_timer,500); // 2 second
//		reset_on=TRUE;
//	}
//}
//
//void check_reset(void)
//{
//	if(reset_on==TRUE)
//	{
//		if (mn_timer_expired(&reset_wait_timer))	
//		{
//			reset_server();
//		}
//	}
//}
