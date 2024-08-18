/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_PORT_H_INC
#define MN_PORT_H_INC   1

void init_recv(SCHAR) cmx_reentrant;
byte mn_transmit_ready(SCHAR) cmx_reentrant;
void mn_uart_init(void) cmx_reentrant;
void mn_timer_init(void) cmx_reentrant;

#endif      /* #ifndef MN_PORT_H_INC */
