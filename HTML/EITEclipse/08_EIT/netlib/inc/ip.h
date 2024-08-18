/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef IP_H_INC
#define IP_H_INC  1

void mn_ip_init(void) cmx_reentrant;
#if (RTOS_USED != RTOS_NONE)
void mn_receive_task(void) cmx_reentrant;
#endif      /* (RTOS_USED != RTOS_NONE) */
byte mn_ip_recv(void) cmx_reentrant;
byte mn_ip_get_pkt(SCHAR) cmx_reentrant;
SCHAR mn_ip_send_header(PSOCKET_INFO,byte,word16) cmx_reentrant;
void mn_ip_discard_packet(void) cmx_reentrant;
SCHAR mn_ip_start_packet(PINTERFACE_INFO, PSOCKET_INFO, byte) cmx_reentrant;
#if (MN_IP_FRAGMENTATION || MN_UDP)
void mn_copy_recv_buff(PINTERFACE_INFO, byte *, word16) cmx_reentrant;
#endif
#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
word16 mn_header_checksum(void *, word16) cmx_reentrant;
#endif
#if (MN_IP_FRAGMENTATION)
void mn_ip_set_socket_frag_data(PINTERFACE_INFO,PSOCKET_INFO,byte *,word16) cmx_reentrant;
byte mn_ip_recv_frag_header(PINTERFACE_INFO, PIP_HEADER, byte) cmx_reentrant;
void ip_clear_frag_info(PINTERFACE_INFO) cmx_reentrant;
#endif

long mn_icmp_recv_reply(PSOCKET_INFO * psocket_ptr) cmx_reentrant;

#define IP_VER          4
#define MIN_IHL         5
#define IP_VER_IHL      0x45
#define IP_VER_IHL_TOS  0x4500
#define IP_HEADER_LEN   20
#define TCP_HEADER_LEN  20
#define UDP_HEADER_LEN  8
#define ROUTINE_SERVICE 0
#define PROTO_ICMP      1
#define PROTO_IGMP      2
#define PROTO_IP        4
#define PROTO_TCP       6
#define PROTO_UDP       17

#define ICMP_ECHO       8
#define ICMP_ECHO_REPLY 0
#define ECHO_HEADER_LEN 8

/* packet types */
#define TCP_TYPE     0x01
#define UDP_TYPE     0x02
#define ICMP_TYPE    0x04
#define UNKNOWN_TYPE 0x08
#define FRAG_TYPE    0x10
#define ARP_TYPE     0x20
#define IGMP_TYPE    0x40
#define ICMP_REPLY_TYPE 0x80

/* byte-oriented fragment definitions. */
#define DONT_FRAGMENT         0x40
#define MORE_FRAGMENTS        0x20
#define MORE_FRAGMENTS_MASK   0xC0
#define FRAGMENTS_MASK        0xE0

#define MIN_IP_LEN   68

#endif   /* #ifndef IP_H_INC */

