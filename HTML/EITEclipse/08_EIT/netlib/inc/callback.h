/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef CALLBACK_H_INC
#define CALLBACK_H_INC  1

#if MN_TCP
word16 mn_app_get_send_size(PSOCKET_INFO) cmx_reentrant;
void mn_app_init_recv(word16,PSOCKET_INFO) cmx_reentrant;
void mn_app_recv_byte(byte,PSOCKET_INFO) cmx_reentrant;
SCHAR mn_app_process_packet(PSOCKET_INFO) cmx_reentrant;
void mn_app_send_complete(word16,PSOCKET_INFO) cmx_reentrant;
#endif   /* #if MN_TCP */
SCHAR mn_app_server_idle(PSOCKET_INFO *) cmx_reentrant;
#if (RTOS_USED == RTOS_NONE)
SCHAR mn_app_recv_idle(void) cmx_reentrant;
SCHAR mn_app_server_process_packet(PSOCKET_INFO) cmx_reentrant;
#if (MN_FTP_CLIENT)
SCHAR mn_app_ftpc_idle(void) cmx_reentrant;
#endif      /* (MN_FTP_CLIENT) */
#endif      /* (RTOS_USED == RTOS_NONE) */

PSOCKET_INFO find_remote_socket(word16 src_port, word16 dest_port, byte *ip_dest_addr)
cmx_reentrant;
PSOCKET_INFO mh_tcp_state(unsigned int socket_no)
cmx_reentrant;

/* Look for a socket to send the UART data out of. */
static PSOCKET_INFO find_my_socket(void)cmx_reentrant;

byte s_mac_process(byte * mac_buff) cmx_reentrant;
void moddev_supplement(byte *buff,byte number) cmx_reentrant;
char mn_port1000_process_packet(PSOCKET_INFO socket_ptr) cmx_reentrant;
static PSOCKET_INFO find_UDP_socket(unsigned long local_port) cmx_reentrant;
static PSOCKET_INFO find_remote_UDP_socket(word16 src_port, unsigned long dest_port,byte *ip_dest_addr) cmx_reentrant;

#endif   /* #ifndef CALLBACK_H_INC */


