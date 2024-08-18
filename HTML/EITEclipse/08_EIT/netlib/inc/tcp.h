/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef TCP_H_INC
#define TCP_H_INC 1

void mn_tcp_init(PSOCKET_INFO) cmx_reentrant;
void mn_tcp_abort(PSOCKET_INFO) cmx_reentrant;
void mn_tcp_shutdown(PSOCKET_INFO) cmx_reentrant;
SCHAR mn_tcp_open(byte,PSOCKET_INFO) cmx_reentrant;
long mn_tcp_send(PSOCKET_INFO) cmx_reentrant;
long mn_tcp_recv(PSOCKET_INFO *) cmx_reentrant;
void mn_tcp_close(PSOCKET_INFO) cmx_reentrant;

/* TCP states */
#define TCP_CLOSED         0
#define TCP_LISTEN         1
#define TCP_SYN_SENT       2
#define TCP_SYN_RECEIVED   3
#define TCP_ESTABLISHED    4
#define TCP_FIN_WAIT_1     5
#define TCP_FIN_WAIT_2     6
#define TCP_CLOSE_WAIT     7
#define TCP_CLOSING        8
#define TCP_LAST_ACK       9
#define TCP_TIME_WAIT      10

/* TCP flag bits */
#define TCP_URG      0x20
#define TCP_ACK      0x10
#define TCP_PSH      0x08    
#define TCP_RST      0x04    
#define TCP_SYN      0x02    
#define TCP_FIN      0x01    
#define TCP_FIN_ACK  0x11    
#define TCP_SYN_ACK  0x12    
#define PORT_WAS_ZERO   0x40
#define ADDR_WAS_ZERO   0x80

#define TCP_STD_OFFSET   (5<<4)

/* defines for TCP MSS option */
#define TCP_OPT_KIND_MSS   0x02
#define TCP_OPT_LEN_MSS    0x04
#define TCP_OPT_OFFSET     (6<<4)
#define TCP_OPT_HEADER_LEN 24

#define DEFAULT_TCP_WINDOW 536

#define RESET_TCP_TIMER(p)    mn_reset_timer( &((p)->tcp_timer), (MN_TCP_RESEND_TICKS) )
#define TCP_TIMER_EXPIRED(p)  (mn_timer_expired( &((p)->tcp_timer) ))

#endif   /* #ifndef TCP_H_INC */

