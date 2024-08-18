/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef INTERFACE_H_INC
#define INTERFACE_H_INC 1

#define KEEP_ERROR_COUNT   0     /* set to 1 to count errors for UARTs */

typedef struct interface_info_s {
   byte interface_status;
   byte interface_type;
   byte ip_dest_addr[IP_ADDR_LEN];
   byte ip_src_addr[IP_ADDR_LEN];
   byte recv_src_addr[IP_ADDR_LEN];
   byte recv_dest_addr[IP_ADDR_LEN];
#if (MN_DNS)
   byte ip_dns_addr[IP_ADDR_LEN];
#endif      /* MN_DNS */
   word16 mtu;
   word16 ip_recv_len;
#if (MN_IP_FRAGMENTATION)
   PFRAG_INFO pfrag_info;
#endif
   volatile RECV_COUNT_T recv_count;
   RECV_COUNT_T recv_buff_size;
   byte *recv_buff_ptr;
   byte *recv_buff_end_ptr;
   byte *volatile recv_in_ptr;
   byte *recv_out_ptr;
   byte *send_buff_ptr;
#if (MN_PPP || MN_SLIP || MN_PING_REPLY)
   byte *send_buff_end_ptr;
#endif      /* (MN_PPP || MN_SLIP || MN_PING_REPLY) */
   byte *send_in_ptr;
   byte *volatile send_out_ptr;
#if (MN_ETHERNET)
   byte *next_out_ptr;        /* holds position where next packet starts */
#endif      /* (MN_ETHERNET) */
#if (RTOS_USED != RTOS_NONE)
   MUTEX_NUM_T mutex_interface;
#endif      /* (RTOS_USED != RTOS_NONE) */
} INTERFACE_INFO_T;

typedef INTERFACE_INFO_T * PINTERFACE_INFO;
#define MK_INTERFACE_PTR(s)   &interface_info[(s)]

/* Interface status */
#define IF_OPEN   1
#define IF_CLOSED 0

/* Interface types */
/* If adding your own interface types do not use a value of 0 */
#define IF_ETHER        1
#define IF_PPP          2
#define IF_SLIP         3

#define IF_ANY         -1

/* Maximum Transfer Units */
#define DEFAULT_ETHERNET_MTU  1500
#define DEFAULT_PPP_MTU       1500
#define DEFAULT_SLIP_MTU      1006

SCHAR mn_interface_init(byte, byte *, RECV_COUNT_T, byte *, SEND_COUNT_T) cmx_reentrant;
SCHAR mn_assign_interface(SCHAR, byte *,byte *, byte *) cmx_reentrant;
SCHAR mn_open_interface(SCHAR interface_no) cmx_reentrant;
void mn_close_interface(SCHAR) cmx_reentrant;
SCHAR mn_find_interface(byte interface_type) cmx_reentrant;

#endif   /* #ifndef INTERFACE_H_INC */

