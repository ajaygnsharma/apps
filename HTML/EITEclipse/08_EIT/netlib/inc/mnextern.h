/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
**********************************************************
   Added additional global variables to support posting
   data files.
   See the conditional define _POST_FILES.
   4.9.2008 - M.J.Butcher                                */

#ifndef MNEXTERN_H_INC
#define MNEXTERN_H_INC  1

/* interface.c */
extern INTERFACE_INFO_T interface_info[MN_NUM_INTERFACES];
extern SCHAR send_interface_no;
extern SCHAR recv_interface_no;
#if (MN_ETHERNET)
extern SCHAR eth_interface_no;
#endif      /* (MN_ETHERNET) */
#if (MN_PPP || MN_SLIP)
extern SCHAR uart_interface_no;
#endif      /* (MN_PPP || MN_SLIP) */

/* mn_os.c */
#if (RTOS_USED == RTOS_CMX_RTX)
extern TASKID_T mn_receive_id;
#if (MN_PING_REPLY)
extern TASKID_T mn_ping_reply_id;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
extern TASKID_T mn_igmp_timer_id;
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
extern TASKID_T mn_snmp_timer_id;
extern TASKID_T mn_snmp_reply_id;
#endif      /* (MN_SNMP) */
#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
extern TASKID_T mn_timer_update_id;
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */
#if (MN_DHCP)
extern TASKID_T mn_dhcp_lease_id;
#endif      /* (MN_DHCP) */
#if (MN_FTP_SERVER)
extern TASKID_T mn_ftp_server_id;
#endif      /* (MN_FTP_SERVER) */
#if (MN_HTTP)
extern TASKID_T mn_http_server_id;
#endif      /* (MN_HTTP) */
#endif      /* (RTOS_USED == RTOS_CMX_RTX) */
#if (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP)
extern MUTEX_NUM_T mutex_ip_receive;
extern MUTEX_NUM_T mutex_ip_send;
#if (MN_ETHERNET)
extern MUTEX_NUM_T mutex_mn_info;
#endif      /* (MN_ETHERNET) */
#if (MN_PING_REPLY)
extern MUTEX_NUM_T mutex_ping_buff;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
extern MUTEX_NUM_T mutex_igmp;
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
extern MUTEX_NUM_T mutex_snmp;
#endif      /* (MN_SNMP) */
#if (MN_PPP)
extern MUTEX_NUM_T mutex_ppp;
#endif      /* (MN_PPP) */
#if (MN_ETHERNET && MN_ARP)
extern MUTEX_NUM_T mutex_arp_cache;
#endif      /* (MN_ETHERNET && MN_ARP) */
#if (MN_DHCP || MN_BOOTP)
extern MUTEX_NUM_T mutex_bootp_dhcp;
#endif      /* (MN_DHCP || MN_BOOTP) */
#if (MN_TFTP)
extern MUTEX_NUM_T mutex_tftp;
#endif      /* (MN_TFTP) */
#if (MN_SMTP)
extern MUTEX_NUM_T mutex_smtp;
#endif      /* (MN_SMTP) */
#if (MN_POP3)
extern MUTEX_NUM_T mutex_pop3;
#endif      /* (MN_POP3) */

#if (MN_VIRTUAL_FILE)
extern MUTEX_NUM_T mutex_vfs_dir;
#if (MN_HTTP)
#if (MN_NUM_POST_FUNCS)
extern MUTEX_NUM_T mutex_vfs_pf;
#endif      /* (MN_NUM_POST_FUNCS) */
#if (MN_SERVER_SIDE_INCLUDES)
extern MUTEX_NUM_T mutex_vfs_gf;
#endif      /* (MN_SERVER_SIDE_INCLUDES) */
#endif      /* (MN_HTTP) */
#if (MN_USE_EFFSM)
extern MUTEX_NUM_T mutex_effsm;
#endif      /* (MN_USE_EFFSM) */
#endif      /* (MN_VIRTUAL_FILE) */

#if (MN_FTP_SERVER)
extern MUTEX_NUM_T mutex_ftp_server;
#endif      /* (MN_FTP_SERVER) */
#if (MN_FTP_CLIENT)
extern MUTEX_NUM_T mutex_ftp_client;
#endif      /* (MN_FTP_CLIENT) */

#if (MN_HTTP)
extern MUTEX_NUM_T mutex_http_server;
#endif      /* (MN_HTTP) */

#if (MN_DNS)
extern MUTEX_NUM_T mutex_dns;
#endif      /* (MN_DNS) */

#if (MN_SNTP)
extern MUTEX_NUM_T mutex_sntp;
#endif      /* (MN_SNTP) */

extern SIGNAL_NUM_T signal_transmit;
extern SIGNAL_NUM_T signal_receive;
extern SIGNAL_NUM_T signal_socket[MN_NUM_SOCKETS];
#if (MN_PING_REPLY)
extern SIGNAL_NUM_T signal_ping_reply;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
extern SIGNAL_NUM_T signal_igmp;
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
extern SIGNAL_NUM_T signal_snmp;
#endif      /* (MN_SNMP) */
#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
extern SIGNAL_NUM_T signal_timer_update;
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */
#if (MN_PPP)
extern SIGNAL_NUM_T signal_ppp;
#endif      /* (MN_PPP) */
#endif      /* (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP) */
#if (RTOS_USED != RTOS_NONE)
extern byte mn_init_done;
#endif      /* (RTOS_USED != RTOS_NONE) */

/* mn_port.c */
#if (MN_PPP || MN_SLIP)
extern byte recv_buff[MN_UART_RECV_BUFF_SIZE];
#endif      /* (MN_PPP || MN_SLIP) */
#if (MN_ETHERNET)
#if (USE_RECV_BUFF)
extern byte eth_recv_buff[MN_ETH_RECV_BUFF_SIZE];
#endif      /* (USE_RECV_BUFF) */
#endif      /* (MN_ETHERNET) */
#if (MN_PPP || MN_SLIP)
extern byte send_buff[MN_UART_XMIT_BUFF_SIZE];
#endif      /* (MN_PPP || MN_SLIP) */
#if (MN_ETHERNET)
#if (USE_SEND_BUFF)
#ifdef __ghs__
#pragma alignvar(4)
#endif
#if ((defined(POLARMSTR9) || defined(CMXARMSTR9) || \
   defined(POLARMSTR9R) || defined(CMXARMSTR9R) || \
   defined(POLSAM7SE512) || defined(CMXSAM7SE512)) && \
   (defined(__GNUC__)  || defined(__CC_ARM)))
extern byte eth_send_buff[MN_ETH_XMIT_BUFF_SIZE+ALIGNMENT_BYTES];
#else
extern byte eth_send_buff[MN_ETH_XMIT_BUFF_SIZE];
#endif
#endif      /* (USE_SEND_BUFF) */
#endif      /* (MN_ETHERNET) */
#if (RTOS_USED == RTOS_NONE || defined(CMXPIC18) || defined(CMXPIC18E) || \
   defined(CMXP18F97J60))
extern volatile byte xmit_busy;
#endif      /* (RTOS_USED == RTOS_NONE || defined(CMXPIC18) || defined(CMXPIC18E)) */
extern volatile word16 uart_errors;
#if (RTOS_USED != RTOS_NONE)
#endif      /* (RTOS_USED != RTOS_NONE) */

/* callback.c */
extern byte ip_dest_addr[IP_ADDR_LEN];
extern byte ip_src_addr[IP_ADDR_LEN];
#if (MN_SMTP)
extern byte ip_smtp_addr[IP_ADDR_LEN];
#endif      /* (MN_SMTP) */
#if MN_ETHERNET
extern byte eth_src_hw_addr[ETH_ADDR_LEN];
extern byte eth_dest_hw_addr[ETH_ADDR_LEN];
extern byte gateway_ip_addr[IP_ADDR_LEN];
extern byte subnet_mask[IP_ADDR_LEN];
#endif

/* ip.c */
extern byte null_addr[IP_ADDR_LEN];
#if (MN_DHCP || MN_BOOTP || MN_ALLOW_BROADCAST)
extern byte broadcast_addr[IP_ADDR_LEN];
#endif      /* (MN_DHCP || MN_BOOTP || MN_ALLOW_BROADCAST) */
#if (MN_ALLOW_MULTICAST)
extern byte all_hosts_addr[IP_ADDR_LEN];
#endif      /* (MN_ALLOW_MULTICAST) */

/* socket.c */
extern SOCKET_INFO_T sock_info[MN_NUM_SOCKETS];
#if (MN_TCP && MN_TCP_DELAYED_ACKS)
extern PACKET_RESEND_INFO_T resend_info[MN_TCP_RESEND_BUFFS];
#endif

#if (MN_IGMP)
/* igmp.c */
extern IGMP_INFO_T igmp_info[MN_IGMP_LIST_SIZE];
#endif      /* (MN_IGMP) */

#if MN_DHCP
extern DHCP_INFO_T dhcp_info;
extern DHCP_LEASE_T dhcp_lease;
#endif      /* MN_DHCP */

#if (MN_BOOTP || MN_DHCP)
extern SCHAR bootpMode;
#endif      /* (MN_BOOTP || MN_DHCP) */

#if (MN_MODEM)
extern MODEM_SCRIPT_T dial_script[];
extern MODEM_SCRIPT_T answer_script[];
extern MODEM_INFO_T modem_info;
#endif      /* (MN_MODEM) */

#if MN_PPP
extern PPPSTATUS_T ppp_status;
extern word32 ppp_accm;
#if MN_USE_PAP
extern PAP_USER_T pap_user[MN_PAP_NUM_USERS];
#endif      /* MN_USE_PAP */
#endif      /* MN_PPP */

#if MN_HTTP
extern byte *URIptr;
extern byte *BODYptr;
extern byte http_parse;
extern cmx_const byte HTTPStatus204[];
extern cmx_const byte HTTPStatus400[];
extern cmx_const byte HTTPStatus404[];
extern cmx_const byte HTTPStatus501[];
extern cmx_const byte HTTPStatus503[];
  #ifdef _POST_FILES
    extern word16 PostBODYlength;
    extern int posting_data;
    extern unsigned long PostFileLen; 
  #endif
#endif

#if (MN_FTP_SERVER)
extern byte ftp_transfer_mode;
#endif      /* (MN_FTP_SERVER) */

#if ((MN_FTP_SERVER || MN_FTP_CLIENT) && MN_NEED_MEM_POOL)
extern byte mem_pool[MN_MEM_POOL_SIZE];
#endif      /* ((MN_FTP_SERVER || MN_FTP_CLIENT) && MN_NEED_MEM_POOL) */

#endif   /* #ifndef MNEXTERN_H_INC */
