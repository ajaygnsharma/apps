/*********************************************************
Copyright (c) CMX Systems, Inc. 2004. All rights reserved
*********************************************************/

#ifndef SOCKET_H_INC
#define SOCKET_H_INC 1

typedef struct ip_header_s {
   byte ver_ihl;
   byte tos;
   byte len_hb;
   byte len_lb;
   byte id_hb;
   byte id_lb;
   byte frag_hb;
   byte frag_lb;
   byte ttl;
   byte proto;
   byte csum_hb;
   byte csum_lb;
   byte srcaddr[4];
   byte dstaddr[4];
} IP_HEADER_T;

typedef IP_HEADER_T * PIP_HEADER;

#if MN_TCP
/* SEQNUM_U is used by TCP to hold sequence and acknowledgement numbers. */
typedef union seqnum_u {
   byte NUMC[4];
   word16 NUMW[2];
   word32 NUML;
} SEQNUM_U;

#endif      /* MN_TCP */

typedef struct socket_info_s {
   word16 src_port;                     /* local port number */
   word16 dest_port;                    /* remote port number */
   byte ip_dest_addr[IP_ADDR_LEN];      /* remote destination address */
#if MN_ETHERNET
   byte eth_dest_hw_addr[ETH_ADDR_LEN]; /* remote MAC address */
#endif
   SCHAR interface_no;                  /* interface number used */
   byte *send_ptr;      /* pointer to a buffer containing data to send */
#if (MN_SEND_ROM_PKT)
   PCONST_BYTE rom_send_ptr;  /* pointer to rom data to send */
#endif
   SEND_LEN_T send_len; /* amount of data to send, can be word16 or word32 */
   byte *recv_ptr;      /* pointer to a buffer to receive the data */
   byte *recv_end;      /* pointer to end of receive buffer */
   word16 recv_len;     /* length of data currently in the receive buffer */
   byte ip_proto;       /* prototype, TCP or UDP */
   SCHAR socket_no;     /* index into sock_info array */
   byte socket_type;    /* flag for sockets that require special handling */
   byte socket_state;   /* 1 = active socket, 0 = inactive socket. Also */
                        /* used by HTTP & FTP servers to flag if they are */
                        /* in the middle of sending a file. */
   void * user_data;    /* May be used to hold socket-related data. */
#if (MN_SOCKET_INACTIVITY_TIME)
/* Number of seconds to wait before receiving a packet that a socket
   is considered inactive. Only used if MN_SOCKET_INACTIVITY_TIME is set to a
   non-zero value in mnconfig.h.
*/
   word16 inactivity_timer;
#endif      /* (MN_SOCKET_INACTIVITY_TIME) */
#if (RTOS_USED != RTOS_NONE)
/* Used by mn_receive_task to hold return value from mn_tcp_recv or mn_udp_recv */
   long last_return_value;
#endif      /* (RTOS_USED != RTOS_NONE) */
#if MN_TCP
   byte tcp_state;            /* Holds TCP state, see tcp.h for values */
   byte tcp_resends;          /* Number of resends left before giving up */
   byte tcp_flag;             /* flag byte of packet to be sent */
   byte recv_tcp_flag;        /* flag byte of packet just received */
   byte data_offset;          /* data offset byte of packet just received */
   word16 tcp_unacked_bytes;  /* Number of bytes we are waiting an ACK for. */
                              /* A new packet cannot be sent unless this is 0. */                              
   word16 recv_tcp_window;    /* TCP window in the packet just received. */
   word16 send_tcp_window;    /* TCP window to send. */
   word16 mss;                /* Maximum Segment Size for outgoing packets. */
#if MN_TCP_DELAYED_ACKS
   byte packet_count;         /* Number of packets sent waiting for ACK. */
   word16 last_unacked_bytes; /* Number of unacked bytes in 1st packet sent. */
#endif
   SEQNUM_U RCV_NXT;    /* Sequence number we are expecting from remote */
                        /* and Acknowledgement number we are sending. */
   SEQNUM_U SEG_SEQ;    /* Sequence number just received from remote. */
   SEQNUM_U SEG_ACK;    /* Acknowledgement number just received from remote. */
   SEQNUM_U SND_UNA;    /* Sequence number we are sending. */
   TIMER_INFO_T tcp_timer;    /* Resend timer structure. */
#endif      /* MN_TCP */
} SOCKET_INFO_T;

typedef SOCKET_INFO_T * PSOCKET_INFO;
#define MK_SOCKET_PTR(s)      &sock_info[(s)]
#define SOCKET_ACTIVATE(s)    sock_info[(s)].socket_state |= 0x01
#define SOCKET_DEACTIVATE(s)  sock_info[(s)].socket_state = 0
#define SOCKET_ACTIVE(s)      (sock_info[(s)].socket_state & 0x01)
#define CLEAR_SOCKET(s)       { SOCKET_DEACTIVATE(s); }

#if MN_TCP_DELAYED_ACKS
/* Use the largest of the ethernet and UART transmit buffer sizes as the
   size of the buffer to hold the packet to resend.
*/
#if (MN_NUM_INTERFACES == 1)
#if (MN_ETHERNET)
/* 54 is the size of the ethernet, IP and TCP headers combined. */
#define XMIT_BUFF_SIZE  (MN_ETH_TCP_WINDOW + 54)
#else
#define XMIT_BUFF_SIZE  MN_UART_XMIT_BUFF_SIZE
#endif      /* (MN_ETHERNET) */
#else
/* Multiple interfaces */
#if (MN_UART_XMIT_BUFF_SIZE >= (MN_ETH_TCP_WINDOW + 54))
#define XMIT_BUFF_SIZE  MN_UART_XMIT_BUFF_SIZE
#else
#define XMIT_BUFF_SIZE  (MN_ETH_TCP_WINDOW + 54)
#endif
#endif

/* Structure which holds the information needed to resend a packet. */
typedef struct packet_resend_s {
   SEQNUM_U SND_UNA;
   byte packet_buff[XMIT_BUFF_SIZE];
   SEND_LEN_T packet_size;
   SCHAR socket_no;
   byte num;
} PACKET_RESEND_INFO_T;

typedef struct ri_info_s {
   byte ri1;
   byte ri2;
   byte ri_cnt;   
} RI_INFO_T;

typedef RI_INFO_T * PRI_INFO;
typedef PACKET_RESEND_INFO_T * PRESEND_INFO;
#define MK_RESEND_PTR(s)       &resend_info[(s)]

int mn_get_next_resend_info(void) cmx_reentrant;
void mn_free_resend_info(SCHAR) cmx_reentrant;
void mn_get_ri_info(PRI_INFO, PSOCKET_INFO) cmx_reentrant;
void mn_resend_tcp(void) cmx_reentrant;
#endif

/* Ports */
#define DEFAULT_PORT       1500
#define ECHO_PORT          7
#define DEFAULT_HTTP_PORT          80
#define SMTP_PORT          25
#define POP3_PORT          110
#define FTP_DATA_PORT      20
#define FTP_CONTROL_PORT   21
#define DNS_PORT           53
#define BOOTP_SERVER_PORT  67
#define BOOTP_CLIENT_PORT  68
#define DHCP_SERVER_PORT   67
#define DHCP_CLIENT_PORT   68
#define TFTP_SERVER_PORT   69
#define SNMP_PORT          161
#define SNMP_TRAP_PORT     162
/* ARP doesn't use ports but we need a port number to get a socket */
#define ARP_PORT           0xFFFF
/* PING_REPLY doesn't use ports but we need a port number to get a socket */
#define PING_PORT          0

/* socket types */
#define  STD_TYPE       0x00
#define  HTTP_TYPE      0x01
#define  FTP_TYPE       0x02
#define  SMTP_TYPE      0x04
#define  TFTP_TYPE      0x08
#define  MULTICAST_TYPE 0x10
#define  S_ARP_TYPE     0x20
#define  AUTO_TYPE      0x40   /* for automatically created sockets */
#define  INACTIVITY_TIMER  0x80

/* open types */
#define PASSIVE_OPEN    0
#define ACTIVE_OPEN     1
#define NO_OPEN         2

/* used by mn_tcp_recv_header */
#define SAME_SOCKET     0
#define NEW_SOCKET      1

/* socket states */
#define ACTIVE_STATE          0x01
#define SENDING_STATE         0x02

/* Macros for HTTP and FTP servers */
#define SET_SENDING(s)        sock_info[(s)].socket_state |= (SENDING_STATE)
#define CLEAR_SENDING(s)      sock_info[(s)].socket_state &= ((byte)(~(SENDING_STATE)))
#define WAS_SENDING(s)        (sock_info[(s)].socket_state & (SENDING_STATE))


int mn_init(void) cmx_reentrant;
SCHAR mn_open(byte [], word16, word16, byte, byte, byte, byte *, word16, SCHAR) cmx_reentrant;
SCHAR mn_open_socket(byte [], word16, word16, byte, byte, byte *, word16, SCHAR) cmx_reentrant;
long mn_send(SCHAR, byte *, word16) cmx_reentrant;
#if (MN_SEND_ROM_PKT)
long mn_send_rom(SCHAR, PCONST_BYTE, word16) cmx_reentrant;
#endif
long mn_recv(SCHAR, byte *, word16) cmx_reentrant;
long mn_recv_wait(SCHAR, byte *, word16, word16) cmx_reentrant;
int mn_close(SCHAR) cmx_reentrant;
int mn_abort(SCHAR) cmx_reentrant;
byte mn_get_socket_type(word16 src_port) cmx_reentrant;
SCHAR mn_close_packet(PSOCKET_INFO, word16) cmx_reentrant;
PSOCKET_INFO mn_find_socket(word16,word16,byte *,byte, SCHAR) cmx_reentrant;
#if (MN_SOCKET_INACTIVITY_TIME)
#if (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_update_inactivity_timers(void);
#else
void mn_update_inactivity_timers(void) cmx_reentrant;
#endif      /* (defined(_CC51)) */
void mn_reset_inactivity_timer(PSOCKET_INFO) cmx_reentrant;
#endif      /* (MN_SOCKET_INACTIVITY_TIME) */
#if (RTOS_USED != RTOS_NONE)
SIGNAL_COUNT_T mn_socket_signal_count(PSOCKET_INFO) cmx_reentrant;
#endif      /* (RTOS_USED != RTOS_NONE) */
word16 mn_socket_recv_buff_space(PSOCKET_INFO) cmx_reentrant;

#endif   /* #ifndef SOCKET_H_INC */

