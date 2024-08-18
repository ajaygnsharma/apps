/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
**********************************************************
   Added HTTP_DATA_POST state.
   4.9.2008 - M.J.Butcher                                */

#ifndef HTTP_H_INC
#define HTTP_H_INC   1

int mn_http_server_recv(PSOCKET_INFO *) cmx_reentrant;
void mn_http_server_set_flag(PSOCKET_INFO) cmx_reentrant;
void mn_http_server_send(PSOCKET_INFO *, int) cmx_reentrant;
void mn_http_init(void) cmx_reentrant;
void mn_http_init_recv(PSOCKET_INFO) cmx_reentrant;
void mn_http_recv_byte(byte, PSOCKET_INFO) cmx_reentrant;
SCHAR mn_http_process_packet(PSOCKET_INFO) cmx_reentrant;
void mn_http_set_message(PSOCKET_INFO, PCONST_BYTE, word16) cmx_reentrant;
byte mn_http_set_file(PSOCKET_INFO, VF_PTR) cmx_reentrant;
int mn_http_find_value(byte *, byte *, byte *) cmx_reentrant;
void mn_http_decodeString(byte *, byte *) cmx_reentrant;
#if MN_SERVER_SIDE_INCLUDES
word16 mn_http_process_includes(PSOCKET_INFO, word16) cmx_reentrant;
#endif
void check_http_parse(PSOCKET_INFO *) cmx_reentrant;
void mn_http_close_file(SCHAR) cmx_reentrant;

#define SOCKET_NONE     0x7f

#define HTTP_START      0
#define HTTP_URI        1
#define HTTP_VER        2
#define HTTP_HEADER     3
#define HTTP_BODY       4
#define HTTP_LASTCHAR   5
#define HTTP_END        6
#define HTTP_DATA_POST  7

/* if the STATUS messages in http.c are changed, be sure to change
   the lengths below.
*/
#define STATUS_204_LEN  27
#define STATUS_400_LEN  39
#define STATUS_404_LEN  35
#define STATUS_501_LEN  47
#define STATUS_503_LEN  55

#endif   /* #ifndef HTTP_H_INC */


