/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef PHYSICAL_H_INC
#define PHYSICAL_H_INC  1

void mn_send_escaped_byte(byte,byte) cmx_reentrant;
byte mn_recv_escaped_byte(byte) cmx_reentrant;
void mn_send_byte(byte) cmx_reentrant;
int mn_peek_recv(void) cmx_reentrant;
int mn_recv_byte(void) cmx_reentrant;
byte mn_recv_byte_present(SCHAR) cmx_reentrant;

#if (MN_ETHERNET)
byte mn_get_xmit_byte(void) cmx_reentrant;
void mn_put_recv_byte(byte) cmx_reentrant;
#endif      /* (MN_ETHERNET) */

#if (MN_ETHERNET && !USE_RECV_BUFF)
void eth_init_recv(SCHAR) cmx_reentrant;
int eth_mn_recv_byte(void) cmx_reentrant;
byte eth_mn_recv_byte_present(SCHAR) cmx_reentrant;
void eth_mn_ip_discard_packet(void) cmx_reentrant;
#endif      /* (MN_ETHERNET && !USE_RECV_BUFF) */

#endif   /* #ifndef PHYSICAL_H_INC */


