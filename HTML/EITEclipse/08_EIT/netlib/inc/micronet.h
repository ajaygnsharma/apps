/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
**********************************************************
   Added define _POST_FILES to activate data post
   support in the TCP/IP stack.
   4.9.2008 - M.J.Butcher                                */

#ifndef MICRONET_H_INC
#define MICRONET_H_INC  1

#define _POST_FILES                                                      // activate file post support in HTTP

/*********************************************************/
/* do not change anything in this section */
/*********************************************************/
#include "mnconfig.h"      /* must be #included first */

#include <stdlib.h>
#include <string.h>

#include "mn_os.h"         /* must be #included before mn_defs.h */
#include "mn_defs.h"
#include "mn_env.h"
#include "mn_tasks.h"      /* must be #included after mn_defs.h */

#if (RTOS_USED != RTOS_NONE && MN_ETHERNET && MN_ARP)
/* workaround for ARP and RTOS - do not change */
#undef MN_ARP_AUTO_UPDATE
#define MN_ARP_AUTO_UPDATE  1
#endif      /* (RTOS_USED != RTOS_NONE && MN_ETHERNET && MN_ARP) */

#if ((MN_HTTP || MN_FTP_SERVER || MN_FTP_CLIENT) && !MN_VIRTUAL_FILE)
#error HTTP and FTP require MN_VIRTUAL_FILE
#endif
#if (MN_HTTP && !MN_TCP)
#error HTTP requires TCP
#endif
#if ((MN_FTP_SERVER || MN_FTP_CLIENT) && !MN_TCP)
#error FTP requires TCP
#endif
#if (MN_SMTP && !MN_TCP)
#error SMTP requires TCP
#endif
#if (MN_POP3 && !MN_TCP)
#error POP3 requires TCP
#endif
#if (!MN_TCP && !MN_UDP)
#error TCP or UDP must be selected
#endif
#if (MN_TCP_DELAYED_ACKS && !MN_TCP)
#undef MN_TCP_DELAYED_ACKS
#define MN_TCP_DELAYED_ACKS   0
#endif
#if (MN_TCP_DELAYED_ACKS && MN_IP_FRAGMENTATION)
#error TCP delayed ACKs not set up to work with IP Fragmentation
#endif
#if (MN_NUM_INTERFACES < 1)
#error MN_NUM_INTERFACES too small
#endif
#if ((MN_ETHERNET && (MN_PPP || MN_SLIP)) && MN_NUM_INTERFACES < 2)
#error MN_NUM_INTERFACES too small
#endif
#if (MN_PPP && MN_SLIP)
#error Select PPP or SLIP not both
#endif
#if (!MN_ETHERNET && !MN_PPP && !MN_SLIP)
#error Ethernet or PPP or SLIP must be selected
#endif
#if (MN_DHCP && MN_BOOTP)
#error Select DHCP or BOOTP not both
#endif
#if (MN_DHCP && !MN_UDP)
#error DHCP requires UDP
#endif
#if (MN_BOOTP && !MN_UDP)
#error BOOTP requires UDP
#endif
#if (MN_TFTP && !MN_UDP)
#error TFTP requires UDP
#endif
#if (MN_SNMP && !MN_UDP)
#error SNMP requires UDP
#endif
#if (MN_MODEM && !(MN_PPP || MN_SLIP))
#error Modem requires PPP or SLIP
#endif
#if (!defined(MN_DNS))
#define MN_DNS    0
#endif
#if (!defined(MN_USE_PPP_DNS))
#define MN_USE_PPP_DNS    0
#endif
#if (MN_DNS && !MN_UDP)
#error DNS requires UDP
#endif
#if (MN_USE_PPP_DNS && !MN_DNS)
#error USE_PPP_DNS requires DNS
#endif
#if (MN_ARP && !MN_ETHERNET)
#error ARP requires ETHERNET
#endif
#if (!defined(MN_SNTP))
#define MN_SNTP   0
#endif
#if (MN_SNTP && !MN_UDP)
#error SNTP requires UDP
#endif

#if (MN_USE_HW_CHKSUM && !MN_ETHERNET)
#undef MN_USE_HW_CHKSUM
#define MN_USE_HW_CHKSUM   0
#endif

#if (MN_UDP_CHKSUM && (MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
#undef MN_UDP_CHKSUM
#define MN_UDP_CHKSUM   0
#endif

#if (MN_NUM_SOCKETS < 1 || MN_NUM_SOCKETS > 127)
#error Number of sockets must be between 1 and 127
#endif
#if (MN_NUM_SOCKETS < 2 && (MN_PING_REPLY || MN_ARP))
#error At least two sockets are required when using ARP or PING_REPLY
#endif

#if (MN_ETHERNET)
#if (USE_RECV_BUFF)
#if (MN_ETH_RECV_BUFF_SIZE < 64)
#error MN_ETH_RECV_BUFF_SIZE is too small!
#endif
#endif      /* (USE_RECV_BUFF) */
#if (USE_SEND_BUFF)
#if (MN_ETH_XMIT_BUFF_SIZE < 64)
#error MN_ETH_XMIT_BUFF_SIZE is too small!
#endif
#endif      /* (USE_SEND_BUFF) */
#endif      /* (MN_ETHERNET) */

#if (MN_ETHERNET && MN_ARP)
#if (MN_ARP_CACHE_SIZE < 1 || MN_ARP_CACHE_SIZE > 255)
#error Invalid MN_ARP_CACHE_SIZE
#endif      /* (MN_ARP_CACHE_SIZE < 1 || MN_ARP_CACHE_SIZE > 255) */
#endif      /* (MN_ETHERNET && MN_ARP) */

#if (MN_TCP)
#if (MN_ETHERNET && USE_RECV_BUFF)
#if (MN_ETH_TCP_WINDOW > MN_ETH_RECV_BUFF_SIZE)
#error MN_ETH_TCP_WINDOW too large for buffers
#endif
#endif      /* (MN_ETHERNET && USE_RECV_BUFF) */
#if ((MN_PPP || MN_SLIP) && (MN_UART_TCP_WINDOW > MN_UART_RECV_BUFF_SIZE))
#error MN_UART_TCP_WINDOW too large for buffers
#endif

#if ( ((MN_SOCKET_WAIT_TICKS)*MN_TCP_RESEND_TRYS) >= 65536 )
#error The product of MN_SOCKET_WAIT_TICKS and MN_TCP_RESEND_TRYS must be less than 65536
#endif
#endif      /* MN_TCP */

#if (MN_IGMP && !MN_ALLOW_MULTICAST)
#error IGMP requires MN_ALLOW_MULTICAST
#endif      /* (MN_IGMP && !MN_ALLOW_MULTICAST) */

#if (MN_ALLOW_MULTICAST && !MN_UDP)
#error Multicasting only works with UDP
#endif      /* (MN_ALLOW_MULTICAST && !MN_UDP) */

#if (MN_IP_FRAGMENTATION)
#if ((MN_IP_REASSEMBLY_TIME <= 0) || (MN_IP_REASSEMBLY_TIME > 65535))
#error Invalid MN_IP_REASSEMBLY_TIME value
#endif
#if ((MN_NUM_REASSEMBLIES <= 0) || (MN_NUM_REASSEMBLIES > 127))
#error Invalid MN_NUM_REASSEMBLIES value
#endif
#if ((MN_REASSEMBLY_BUFF_SIZE < 68) || (MN_REASSEMBLY_BUFF_SIZE > 65535))
#error Invalid MN_REASSEMBLY_BUFF_SIZE value
#endif
#endif

#if (MN_FTP_SERVER)
#if MN_FTP_MAX_PARAM < 23
#error MN_FTP_MAX_PARAM must be at least 23
#endif
#if MN_FTP_NUM_USERS > 127
#error MN_FTP_NUM_USERS must be less than 128
#endif
#if (MN_ETHERNET && MN_FTP_BUFFER_LEN < MN_ETH_TCP_WINDOW)
#error MN_FTP_BUFFER_LEN is too small
#endif      /* (MN_FTP_BUFFER_LEN < TCP_WINDOW) */
#if ((MN_PPP || MN_SLIP) && MN_FTP_BUFFER_LEN < MN_UART_TCP_WINDOW)
#error MN_FTP_BUFFER_LEN is too small
#endif      /* (MN_FTP_BUFFER_LEN < TCP_WINDOW) */
#endif      /* (MN_FTP_SERVER) */

#if (MN_SMTP)
#if (MN_SMTP_BUFFER_LEN < 46)
#error MN_SMTP_BUFFER_LEN is too small
#endif
#endif      /* MN_SMTP */

#if (MN_HTTP && MN_SERVER_SIDE_INCLUDES && !MN_NUM_GET_FUNCS)
#error Use of SERVER_SIDE_INCLUDES requires MN_NUM_GET_FUNCS greater than 0
#endif      /* MN_HTTP && MN_SERVER_SIDE_INCLUDES && !MN_NUM_GET_FUNCS */

#if (MN_HTTP && MN_SERVER_SIDE_INCLUDES)
#if (MN_ETHERNET && MN_HTTP_BUFFER_LEN < MN_ETH_TCP_WINDOW)
#error MN_HTTP_BUFFER_LEN is too small
#endif      /* (MN_HTTP_BUFFER_LEN < TCP_WINDOW) */
#if ((MN_PPP || MN_SLIP) && MN_HTTP_BUFFER_LEN < MN_UART_TCP_WINDOW)
#error MN_HTTP_BUFFER_LEN is too small
#endif      /* (MN_HTTP_BUFFER_LEN < TCP_WINDOW) */
#endif      /* MN_HTTP && MN_SERVER_SIDE_INCLUDES */

#if (!MN_VIRTUAL_FILE && (MN_USE_EFFS || MN_USE_EFFSM || MN_USE_EFFS_FAT || MN_USE_EFFS_THIN))
#error MN_VIRTUAL_FILE is required for CMX-FFS products
#endif

#if ((MN_USE_EFFS && MN_USE_EFFSM) || (MN_USE_EFFS && MN_USE_EFFS_FAT) || \
   (MN_USE_EFFS_FAT && MN_USE_EFFS_THIN) || (MN_USE_EFFSM && MN_USE_EFFS_FAT) || \
   (MN_USE_EFFSM && MN_USE_EFFS_THIN) || (MN_USE_EFFS_FAT && MN_USE_EFFS_THIN))
#error Only one CMX-FFS product may be selected at a time
#endif

/*********************************************************/

#include "mn_port.h"
#include "mn_error.h"

#include "mn_timer.h"
#include "socket.h"
#if MN_ETHERNET
#include "arp.h"
#include "ethernet.h"
#endif   /* MN_ETHERNET */
#if MN_IP_FRAGMENTATION
#include "ip_frag.h"
#endif
#include "intrface.h"
#include "ip.h"
#include "mn_csum.h"
#include "callback.h"
#include "physical.h"
#include "support.h"
#include "mnstring.h"
#include "mnserver.h"

#if MN_TCP
#include "tcp.h"
#endif

#if MN_UDP
#include "udp.h"
#endif

#if MN_IGMP
#include "igmp.h"
#endif

#if MN_SLIP
#include "slip.h"
#endif

#if MN_PPP
#include "ppp.h"
#endif

#if MN_MODEM
#include "modem.h"
#endif

#if MN_VIRTUAL_FILE
#if (MN_USE_EFFS || MN_USE_EFFSM)
/* Set the following to 1 if using a version of CMX-FFS_STD earlier than
   2.00.
*/
#define OLD_FFS_STD  0

#include "fsf.h"

#if (OLD_FFS_STD)
#ifndef F_NO_ERROR
#define F_NO_ERROR   FS_NOERR
#endif
#ifndef F_ERR_NOTFOUND
#define F_ERR_NOTFOUND  FS_NOTFOUND
#endif
#define F_SEEK_SET   FS_SEEK_SET
#define F_FILE       FS_FILE
#else
/* defsold.h removed in v2.18 */
/* #include "defsold.h"*/
#include "defs.h"
#endif

#endif

#if (MN_USE_EFFS)
#if (OLD_FFS_STD)
#define f_open(filename,mode)                fs_open(filename,mode)
#define f_close(filehandle)                  fs_close(filehandle)
#define f_read(buf,size,_size_t,filehandle)  fs_read(buf,size,_size_t,filehandle)
#define f_write(buf,size,_size_t,filehandle) fs_write(buf,size,_size_t,filehandle)
#define f_seek(filehandle,offset,whence)     fs_seek(filehandle,offset,whence)
#define f_tell(filehandle)                   fs_tell(filehandle)
#define f_delete(filename)                   fs_delete(filename)
#endif
#elif (MN_USE_EFFSM)
#if (OLD_FFS_STD)
#include "fsmf.h"
#define f_open(filename,mode)                fsm_open(filename,mode)
#define f_close(filehandle)                  fsm_close(filehandle)
#define f_read(buf,size,_size_t,filehandle)  fsm_read(buf,size,_size_t,filehandle)
#define f_write(buf,size,_size_t,filehandle) fsm_write(buf,size,_size_t,filehandle)
#define f_seek(filehandle,offset,whence)     fsm_seek(filehandle,offset,whence)
#define f_tell(filehandle)                   fsm_tell(filehandle)
#define f_delete(filename)                   fsm_delete(filename)
#endif
#elif (MN_USE_EFFS_FAT)
#include "fat.h"
#elif (MN_USE_EFFS_THIN)
#include "thin_usr.h"
#endif

#if (MN_USE_EFFS || MN_USE_EFFSM || MN_USE_EFFS_FAT || MN_USE_EFFS_THIN)
#define CMX_FFS_USED 1

/* CMX-MicroNet assumes that all the files will be on the same drive.
   Set the drive number used below.
*/
#define MN_FFS_DRIVENUM 0

/* Older versions of CMX-FFS-FAT and CMX-FFS-Std, and all versions of 
   CMX-FFS-Thin through v1.71, returned the number of bytes for f_read and
   f_write. Newer versions of CMX-FFS-FAT, CMX-FFS-Std and CMX-FFS-Thin
   return the number of items.
   Set the define below according to what your version of FFS does.
*/
#define OLD_STYLE_READWRITE   0

#if OLD_STYLE_READWRITE
#define F_READ_LEN(A,B,C,D)      (f_read((A),(B),(C),(D)))
#define F_WRITE_LEN(A,B,C,D)     (f_write((void *)(A),(B),(C),(D)))
#else
#define F_READ_LEN(A,B,C,D)      (f_read((A),(C),(B),(D)))
#define F_WRITE_LEN(A,B,C,D)     (f_write((void *)(A),(C),(B),(D)))
#endif

/* CMX-FFS_FAT v3.31 and later and CMX-FFS-Safe (CMX-FFS-Std) v2.26 and
   later require function f_enterFS to be called by all tasks before
   the task can access the FFS. If you have an earlier version define 
   macro F_ENTERFS as empty. CMX-FFS-THIN does not have an f_enterFS call.
*/
#if (MN_USE_EFFS || MN_USE_EFFSM)
#if (OLD_FFS_STD)
#define F_ENTERFS
#else
#define F_ENTERFS    f_enterFS()
#endif
#elif (MN_USE_EFFS_FAT)
#define F_ENTERFS    f_enterFS()
#elif (MN_USE_EFFS_THIN)
#define F_ENTERFS
#endif

#else
#define CMX_FFS_USED 0
#endif
#include "vfile.h"
#endif

#if MN_HTTP
#include "http.h"
#endif

#if MN_FTP_SERVER
#include "ftpservr.h"
#endif
#if MN_FTP_CLIENT
#include "ftpcli.h"
#endif
#if (MN_FTP_SERVER || MN_FTP_CLIENT)
#include "mn_mem.h"
#endif

#if MN_DHCP
#include "dhcp.h"
#if (MN_ETHERNET && (MN_ETH_RECV_BUFF_SIZE < \
   (236 + DHCP_OPT_LEN + ETH_HEADER_LEN + IP_HEADER_LEN + UDP_HEADER_LEN + 2)))
/* Note: 236 is the size of the DHCP_INFO_T structure - DHCP_OPT_LEN */
#error MN_ETH_RECV_BUFF_SIZE is too small for DHCP!
#endif
#endif      /* MN_DHCP */

/* bww */
#if MN_BOOTP
#include "bootp.h"
#endif      /* MN_BOOTP */

#if MN_TFTP
#include "tftp.h"
#endif

#if MN_SMTP
#include "smtp.h"
#endif

#if MN_POP3
#include "pop3.h"
#endif

#if MN_SNMP
#include "snmp.h"
#if (MN_SNMP_IN_BUFF_SIZE < 10)
#error invalid setting for MN_SNMP_IN_BUFF_SIZE
#endif
#if (MN_SNMP_OUT_BUFF_SIZE < 10)
#error invalid setting for MN_SNMP_OUT_BUFF_SIZE
#endif
#endif

#if (MN_DNS)
#include "mn_dns.h"
#endif      /* (MN_DNS) */

#if (MN_SNTP)
#include "mn_sntp.h"
#include "mn_date.h"
#endif      /* (MN_SNTP) */

#include "mnextern.h"

#endif   /* #ifndef MICRONET_H_INC */
