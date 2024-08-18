/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_ERROR_H_INC
#define MN_ERROR_H_INC  1

/* error numbers */
#define NOT_ENOUGH_SOCKETS       -128
#define SOCKET_ALREADY_EXISTS    -127
#define NOT_SUPPORTED            -126
#define PPP_OPEN_FAILED          -125
#define TCP_OPEN_FAILED          -124
#define BAD_SOCKET_DATA          -123
#define SOCKET_NOT_FOUND         -122
#define SOCKET_TIMED_OUT         -121
#define BAD_IP_HEADER            -120
#define NEED_TO_LISTEN           -119
#define RECV_TIMED_OUT           -118
#define ETHER_INIT_ERROR         -117
#define ETHER_SEND_ERROR         -116
#define ETHER_RECV_ERROR         -115
#define NEED_TO_SEND             -114
#define UNABLE_TO_SEND           -113
#define VFILE_ENTRY_IN_USE       -112
#define TFTP_FILE_NOT_FOUND      -111
#define TFTP_NO_FILE_SPECIFIED   -110
#define TFTP_FILE_TOO_BIG        -109
#define TFTP_FAILED              -108
#define SMTP_ALREADY_OPEN        -107
#define SMTP_OPEN_FAILED         -106
#define SMTP_NOT_OPEN            -105
#define SMTP_BAD_PARAM_ERR       -104
#define SMTP_ERROR               -103
#define NEED_TO_EXIT             -102
#define FTP_FILE_MAXOUT          -101
#define DHCP_ERROR               -100
#define DHCP_LEASE_EXPIRED       -99
#define PPP_LINK_DOWN            -98
#define GET_FUNC_ERROR           -97
#define FTP_SERVER_DOWN          -96
#define ARP_REQUEST_FAILED       -95
#define NEED_IGNORE_PACKET       -94
#define TASK_DID_NOT_START       -93
#define DHCP_LEASE_RENEWING      -92
#define IGMP_ERROR               -91
#define VFILE_OPEN_FAILED        -90
#define VFILE_READ_FAILED        -89
#define VFILE_WRITE_FAILED       -88
#define VFILE_DELETE_FAILED      -87
#define VFILE_SEEK_FAILED        -86
#define INVALID_VALUE            -85
#define FTP_OPEN_FAILED          -84
#define FTP_CMD_FAILED           -83
#define FTP_DUPLICATE_FILE       -82
#define FTP_FILE_NOT_FOUND       -81
#define FTP_NOT_LOGGED_IN        -80
#define INTERFACE_NOT_FOUND      -79
#define BAD_INTERFACE_DATA       -78
#define MN_EWOULDBLOCK           -77
#define MODEM_TIMED_OUT          -76
#define DNS_ID_ERROR             -75
#define DNS_OPCODE_ERROR         -74
#define DNS_RCODE_ERROR          -73
#define DNS_COUNT_ERROR          -72
#define DNS_TYPE_ERROR           -71
#define DNS_CLASS_ERROR          -70
#define DNS_NOT_FOUND            -69
#define SOCKET_BUFFER_TOO_SMALL  -68
#define POP3_OPEN_FAILED         -67
#define POP3_ALREADY_OPEN        -66
#define POP3_NOT_OPEN            -65
#define REASSEMBLY_ERROR         -64
#define REASSEMBLY_TIMED_OUT     -63
#define BAD_REASSEMBLY_DATA      -62
#define SNTP_ERROR               -61
#define UNABLE_TO_TCP_SEND       -60

/* TCP function error codes */
#define TCP_ERROR       -1
#define TCP_TOO_LONG    -2
#define TCP_BAD_HEADER  -3
#define TCP_BAD_CSUM    -4
#define TCP_BAD_FCS     -5
#define TCP_NO_CONNECT  -6

/* UDP error codes */
#define UDP_ERROR       -1
#define UDP_TOO_LONG    -2
#define UDP_BAD_CSUM    -4
#define UDP_BAD_FCS     -5

#endif   /* #ifndef MN_ERROR_H_INC */

