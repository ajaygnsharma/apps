/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

#ifndef MNCONFIG_H_INC
#define MNCONFIG_H_INC 1

/* Protocols */
#define MN_NUM_INTERFACES 1
#define MN_TCP         1
#define MN_UDP         1
#define MN_UDP_CHKSUM  0 // was 1  Ajay Sharma
#define MN_ETHERNET    1 
#define MN_SLIP        0 
#define MN_PPP         0 
#define MN_PING_REPLY  1 
#define MN_IGMP        0 

/* Sockets */
#define MN_NUM_SOCKETS             10
#define MN_SOCKET_WAIT_TICKS       600
#define MN_SOCKET_INACTIVITY_TIME  7200 //60 //was 0

/* Send and Recv buffers */
#define MN_ETH_RECV_BUFF_SIZE      2048
#define MN_ETH_XMIT_BUFF_SIZE      1518
#define MN_UART_RECV_BUFF_SIZE     1600
#define MN_UART_XMIT_BUFF_SIZE     1600

/* IP options */
#define MN_TIME_TO_LIVE     64
#define MN_PING_GLEANING     0 
#define MN_PING_BUFF_SIZE   32
#define MN_ALLOW_BROADCAST   1 // was 0 Ajay Sharma 
#define MN_ALLOW_MULTICAST   1 // was 0  Ajay Sharma
#define MN_MULTICAST_TTL    1
#define MN_IGMP_LIST_SIZE   4
#define MN_IP_FRAGMENTATION   0 
#define MN_IP_REASSEMBLY_TIME 60
#define MN_NUM_REASSEMBLIES   1
#define MN_REASSEMBLY_BUFF_SIZE  3000
#define MN_COPY_FRAG_DATA     1 

/* TCP options */
#define MN_ETH_TCP_WINDOW   1460
#define MN_UART_TCP_WINDOW  1460
#define MN_TCP_RESEND_TICKS 600
#define MN_TCP_RESEND_TRYS  12
#define MN_TCP_DELAYED_ACKS 0 // was 1 Ajay Sharma
#define MN_TCP_RESEND_BUFFS 6

/* Ethernet */
#define MN_POLLED_ETHERNET   1
#define MN_ETHER_WAIT_TICKS  1
#define MN_USE_HW_CHKSUM  0 

/* ARP */
#define MN_ARP              1
#define MN_ARP_TIMEOUT      1 
#define MN_ARP_AUTO_UPDATE  1 // was 0 
#define MN_ARP_CACHE_SIZE   4
#define MN_ARP_KEEP_TICKS   6000
#define MN_ARP_RESEND_TRYS  6
#define MN_ARP_WAIT_TICKS   200

/* DHCP */
#define MN_DHCP                     1 
#define MN_DHCP_RESEND_TRYS         4
#define MN_DHCP_DEFAULT_LEASE_TIME  36000

/* BOOTP */
#define MN_BOOTP              0 
#define MN_BOOTP_RESEND_TRYS  6
#define MN_BOOTP_REQUEST_IP   1 

/* DNS options */
#define MN_DNS                  1 
#define MN_DNS_WAIT_TICKS      300
#define MN_DNS_SEND_TRYS       3
#define MN_USE_PPP_DNS          0 
#define MN_DNS_RECV_BUFF_SIZE  512
#define MN_DNS_XMIT_BUFF_SIZE  64

/* PPP options */
#define MN_USE_PAP             1 
#define MN_PAP_USER_LEN        10
#define MN_PAP_PASSWORD_LEN    10
#define MN_PAP_NUM_USERS       1
#define MN_PPP_RESEND_TICKS    300
#define MN_PPP_RESEND_TRYS     6
#define MN_PPP_TERMINATE_TRYS  2
#define MN_FAST_FCS            1 

/* Modem */
#define MN_MODEM           0 
#define MN_DIRECT_CONNECT  1 

/* HTTP */
#define MN_HTTP                  1
#define MN_SERVER_SIDE_INCLUDES  1
#define MN_INCLUDE_HEAD          0 
#define MN_URI_BUFFER_LEN        500 // was 500 // was 52
#ifdef _POST_FILES
    #define MN_BODY_BUFFER_LEN   1460 // was 252 - when posting data it is necessary to store a complete TCP data frame
#else
    #define MN_BODY_BUFFER_LEN   252 // was 52
#endif
#define MN_HTTP_BUFFER_LEN       1460

/* FTP */
#define MN_NEED_MEM_POOL     0 
#define MN_MEM_POOL_SIZE     4096
#define MN_FTP_SERVER        0
#define MN_FTP_MAX_PARAM     24
#define MN_FTP_BUFFER_LEN    2500
#define MN_FTP_USER_LEN      10
#define MN_FTP_PASSWORD_LEN  10
#define MN_FTP_NUM_USERS     1
#define MN_FTP_CLIENT         0 
#define MN_FTPC_USER_LEN      10
#define MN_FTPC_PASSWORD_LEN  10
#define MN_FTPC_ACCOUNT_LEN   0
#define MN_FTPC_CMD_BUFF_SIZE   64
#define MN_FTPC_REPLY_BUFF_SIZE 1460
#define MN_FTPC_FILE_BUFFER_LEN 2500
#define MN_FTPC_WAIT_TICKS      600

/* TFTP */
#define MN_TFTP              0 
#define MN_TFTP_RESEND_TRYS  3
#define MN_TFTP_USE_FLASH    0 

/* SMTP */
#define MN_SMTP             1 //was 1 
#define MN_SMTP_AUTH        0 // was 1 
#define MN_SMTP_BUFFER_LEN  1460

/* SNTP */
#define MN_SNTP             0 
#define MN_SNTP_WAIT_TICKS 300
#define MN_SNTP_SEND_TRYS  3

/* POP3 */
#define MN_POP3                  0 
#define MN_POP3_CMD_BUFFER_LEN   25
#define MN_POP3_REPLY_BUFFER_LEN 1460
#define MN_POP3_LINE_BUFFER_LEN  250
#define MN_POP3_WAIT_TICKS       600

/* SNMP */
#define MN_SNMP             1// was 1 
#define MN_SNMP_VERSION2C   1 
#define MN_SNMP_TRAP        1 // was 1 
#define MN_SNMP_IN_BUFF_SIZE 484
#define MN_SNMP_OUT_BUFF_SIZE 484

/* Virtual File System */
#define MN_VIRTUAL_FILE    1
#define MN_NUM_VF_PAGES    46 //50 // was 9
#define MN_VF_NAME_LEN     20
#define MN_FUNC_NAME_LEN   20
#define MN_NUM_POST_FUNCS  37 // was 2
#define MN_NUM_GET_FUNCS   230// was 2
#define MN_USE_LONG_FSIZE   0 
#define MN_USE_EFFS_THIN    0 
#define MN_USE_EFFS         0 
#define MN_USE_EFFSM        0 
#define MN_USE_EFFS_FAT     0 

#endif   /* ifndef MNCONFIG_H_INC */
