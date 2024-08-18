/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

INTERFACE_INFO_T interface_info[MN_NUM_INTERFACES];
SCHAR send_interface_no;
SCHAR recv_interface_no;

#if (MN_ETHERNET)
SCHAR eth_interface_no;
#endif      /* (MN_ETHERNET) */

#if (MN_PPP || MN_SLIP)
SCHAR uart_interface_no;
#endif      /* (MN_PPP || MN_SLIP) */


/* Initializes an interface of the passed type.
   Returns interface number if successful or negative number on error.
*/
SCHAR mn_interface_init(byte interface_type, byte * recv_buff_ptr, \
   RECV_COUNT_T recv_buff_size, byte * send_buff_ptr, \
   SEND_COUNT_T send_buff_size)
cmx_reentrant {
   SCHAR interface_no;
   PINTERFACE_INFO interface_ptr;

   if (!send_buff_ptr || !send_buff_size)
      return (BAD_INTERFACE_DATA);

#if (MN_PPP || MN_SLIP)
   if ((interface_type == IF_PPP) || (interface_type == IF_SLIP))
      {
      if (!recv_buff_ptr || !recv_buff_size)
         return (BAD_INTERFACE_DATA);
      }
#endif      /* (MN_PPP || MN_SLIP) */

   interface_no = mn_find_interface(0);   /* find next interface number */

   if (interface_no >= 0)
      {
      interface_ptr = MK_INTERFACE_PTR(interface_no);
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);

      interface_ptr->interface_type = interface_type;

#if (MN_PPP)
      if (interface_type == IF_PPP)
         interface_ptr->mtu = DEFAULT_PPP_MTU;
#endif      /* (MN_PPP) */

#if (MN_SLIP)
      if (interface_type == IF_SLIP)
         interface_ptr->mtu = DEFAULT_SLIP_MTU;
#endif      /* (MN_SLIP) */

#if (MN_ETHERNET)
      if (interface_type == IF_ETHER)
         {
         interface_ptr->mtu = DEFAULT_ETHERNET_MTU;
         eth_interface_no = interface_no;
         }
#endif      /* (MN_ETHERNET) */

#if (MN_PPP || MN_SLIP)
      if ((interface_type == IF_PPP) || (interface_type == IF_SLIP))
         uart_interface_no = interface_no;
#endif      /* (MN_PPP || MN_SLIP) */

#if (MN_IP_FRAGMENTATION)
      interface_ptr->pfrag_info = PTR_NULL;
#endif

      /* Initialize buffer pointers */
      interface_ptr->recv_buff_ptr = recv_buff_ptr;
      init_recv(interface_no);
#if (MN_ETHERNET)
      interface_ptr->next_out_ptr = recv_buff_ptr;
#endif      /* (MN_ETHERNET) */
      if ((recv_buff_ptr != PTR_NULL) && (recv_buff_size > 0))
         interface_ptr->recv_buff_end_ptr = &recv_buff_ptr[recv_buff_size-1];
      else
         interface_ptr->recv_buff_end_ptr = recv_buff_ptr;
      interface_ptr->recv_buff_size = recv_buff_size;

      interface_ptr->send_buff_ptr = send_buff_ptr;
      interface_ptr->send_in_ptr = interface_ptr->send_out_ptr = send_buff_ptr;
#if (MN_PPP || MN_SLIP || MN_PING_REPLY)
      if ((send_buff_ptr != PTR_NULL) && (send_buff_size > 0))
         interface_ptr->send_buff_end_ptr = &send_buff_ptr[send_buff_size-1];
      else
         interface_ptr->send_buff_end_ptr = send_buff_ptr;
#endif      /* (MN_PPP || MN_SLIP || MN_PING_REPLY) */
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      }

   return (interface_no);
}

/* Assign ip_src_addr and ip_dest_addr to an interface.
   Returns 1 if interface found otherwise returns a negative number.
*/
SCHAR mn_assign_interface(SCHAR interface_no, byte *ip_src_ptr, \
   byte *ip_dest_ptr, byte *ip_dns_ptr)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(interface_no);
   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);

   if (ip_src_ptr)
      {
      (void)memcpy((void *)interface_ptr->ip_src_addr, (void *)ip_src_ptr, \
         IP_ADDR_LEN);
      }

   if (ip_dest_ptr)
      {
      (void)memcpy((void *)interface_ptr->ip_dest_addr, (void *)ip_dest_ptr, \
         IP_ADDR_LEN);
      }

#if (MN_DNS)
   if (ip_dns_ptr)
      {
      (void)memcpy((void *)interface_ptr->ip_dns_addr, (void *)ip_dns_ptr, \
         IP_ADDR_LEN);
      }
#else
   ip_dns_ptr = ip_dns_ptr;
#endif      /* (MN_DNS) */

   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
   return (1);
}

/* Activates, or opens, an interface.
   Returns 1 if interface found otherwise returns a negative number.
*/
SCHAR mn_open_interface(SCHAR interface_no)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);
   
   interface_ptr = MK_INTERFACE_PTR(interface_no);
   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
   interface_ptr->interface_status |= IF_OPEN;
   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

   return (1);
}

/* Completely shutdown an interface. To reopen an interface you need to
   run mn_interface_init and mn_open_interface again. It will be necessary
   to restart the modem, PPP and DHCP where applicable. 
*/
void mn_close_interface(SCHAR interface_no)
cmx_reentrant {
   SCHAR i;
#if (MN_DHCP || MN_PPP || MN_MODEM || (RTOS_USED != RTOS_NONE))
   PINTERFACE_INFO interface_ptr;
#endif      /* (MN_DHCP || MN_PPP || MN_MODEM) */
#if (RTOS_USED != RTOS_NONE)
   MUTEX_NUM_T mutex_num;
#endif   

   if ((interface_no >= 0) && (interface_no < MN_NUM_INTERFACES))
      {
      /* close all sockets connected to this interface. */
      for (i=0;i<MN_NUM_SOCKETS;i++)
         {
         if (sock_info[i].interface_no == interface_no)
            mn_close(sock_info[i].socket_no);
         }

#if (MN_DHCP || MN_PPP || MN_MODEM || (RTOS_USED != RTOS_NONE))
      interface_ptr = MK_INTERFACE_PTR(interface_no);
#endif      /* (MN_DHCP || MN_PPP || MN_MODEM) */

#if (MN_DHCP)
      if (interface_ptr->interface_type == IF_ETHER)
         mn_dhcp_release();
#endif      /* (MN_DHCP) */

#if (MN_PPP)
      if (interface_ptr->interface_type == IF_PPP)
         mn_ppp_close();
#endif      /* (MN_PPP) */

#if (MN_MODEM)
      if ((interface_ptr->interface_type == IF_PPP) || \
            (interface_ptr->interface_type == IF_SLIP))
         mn_modem_disconnect();
#endif      /* (MN_MODEM) */

#if (RTOS_USED != RTOS_NONE)
      mutex_num = interface_ptr->mutex_interface;
      MN_MUTEX_WAIT(mutex_num,INFINITE_WAIT);
#endif
      MEMSET(&interface_info[interface_no], 0, sizeof(&interface_info[interface_no]));
#if (RTOS_USED != RTOS_NONE)
      interface_ptr->mutex_interface = mutex_num;
      MN_MUTEX_RELEASE(mutex_num);
#endif
      }
}

/* Returns an interface number of an interface with the passed type
   or negative number if not found.
*/
SCHAR mn_find_interface(byte interface_type)
cmx_reentrant {
   SCHAR i;
   SCHAR interface_no;

   interface_no = INTERFACE_NOT_FOUND;

   for (i=0;i<MN_NUM_INTERFACES;i++)
      {
      if (interface_info[i].interface_type == interface_type)
         {
         interface_no = i;
         break;
         }
      }

   return (interface_no);
}

