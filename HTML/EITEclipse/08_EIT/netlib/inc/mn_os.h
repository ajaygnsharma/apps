/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

/* mnconfig.h must be #included before mn_os.h */

#ifndef MN_OS_H_INC
#define MN_OS_H_INC 1

#define RTOS_NONE       0
#define RTOS_CMX_RTX    1
#define RTOS_CMX_TINYP  2

#ifndef RTOS_USED
#define RTOS_USED RTOS_NONE
#endif

#if (RTOS_USED == RTOS_NONE)
/* Task definitions */
#define TASK_SUCCESS
#define TASK_STACK_SIZE
#define MN_TASK_CREATE(A,B,C,D)
#define MN_TASK_START(A)
#define MN_TASK_WAIT(A)             (mn_wait_ticks(A))
#define MN_TASK_LOCK
#define MN_TASK_UNLOCK
#define MN_TASK_STOP

/* Mutex definitions */
#define MUTEX_SUCCESS
#define MUTEX_IP_RECEIVE
#define MUTEX_IP_SEND
#define MUTEX_MN_INFO
#define MUTEX_PING_BUFF
#define MUTEX_IGMP
#define MUTEX_PPP
#define MUTEX_ARP_CACHE
#define MUTEX_BOOTP_DHCP
#define MUTEX_TFTP
#define MUTEX_SMTP
#define MUTEX_POP3
#define MUTEX_SNMP
#define MUTEX_SNTP
#define MUTEX_VFS_DIR
#define MUTEX_VFS_PF
#define MUTEX_VFS_GF
#define MUTEX_EFFSM
#define MUTEX_FTP_SERVER
#define MUTEX_FTP_CLIENT
#define MUTEX_HTTP_SERVER
#define MUTEX_DNS

#define MN_MUTEX_CREATE(A,B)
#define MN_MUTEX_GET(A)
#define MN_MUTEX_WAIT(A,B)
#define MN_MUTEX_RELEASE(A)

/* Signaling definitions */
#define SIGNAL_SUCCESS
#define SIGNAL_TRANSMIT
#define SIGNAL_RECEIVE
#define SIGNAL_PPP
#define SIGNAL_TIMER_UPDATE
#define SIGNAL_PING_REPLY
#define SIGNAL_IGMP
#define SIGNAL_SNMP

#define MN_SIGNAL_CREATE(A,B)
#define MN_SIGNAL_RESET(A)
#define MN_SIGNAL_GET(A)
#define MN_SIGNAL_WAIT(A,B)
#define MN_SIGNAL_POST(A)
#define MN_SIGNAL_COUNT(A,B)
#define MN_ISR_SIGNAL_POST(A)

/* other definitions */
#define MN_TICK_UPDATE              ++timer_tick
#define MN_GET_TICK                 (mn_get_timer_tick())
#define MN_OS_INIT
#define MN_OS_START
#define MN_OS_END
#define MN_PPP_STATUS_UP            (ppp_status.up)

#elif (RTOS_USED == RTOS_CMX_RTX)
/* include necessary header files for RTOS */
#if (defined(CMXX86))
/* PCProto-RTX */
#include "pxfuncs.h"
#include "pxextern.h"
/* #include "pxconfig.h" */
#else
#include "cxfuncs.h"
#include "cxextern.h"
/* #include "cxconfig.h" */
#endif      /* (defined(CMXX86)) */

/* Task definitions */
typedef byte TASKID_T;
typedef byte TASK_STATUS_T;
#define TASK_SUCCESS                K_OK
#if (defined(POLARM) || defined(CMXARM) || defined(POLARME) || \
   defined(CMXARME) || defined(POLARM2294) || defined(CMXARM2294) || \
   defined(POLARMSTR7) || defined(CMXARMSTR7) || \
   defined(POLARMSTR9) || defined(CMXARMSTR9) || \
   defined(POLARMSTR9R) || defined(CMXARMSTR9R) || \
   defined(POLARM9) || defined(CMXARM9) || \
   defined(POLARM2148) || defined(CMXARM2148) || \
   defined(POLARM2368) || defined(CMXARM2368) || \
   defined(POLARM2378) || defined(CMXARM2378) || \
   defined(POLARM2468) || defined(CMXARM2468) || \
   defined(POLSAM7X256) || defined(CMXSAM7X256) || \
   defined(POLSAM7SE512) || defined(CMXSAM7SE512) || \
   defined(POLMCF5282) || defined(CMXMCF5282) || \
   defined(POLMCF5223) || defined(CMXMCF5223) || \
   defined(POLMCF5235) || defined(CMXMCF5235) || \
   defined(POLMCF5208) || defined(CMXMCF5208) || \
   defined(POLLM3Sx9xx) || defined(CMXLM3Sx9xx) || \
   defined(POLSTM32) || defined(CMXSTM32) || \
   defined(POLSAM9263) || defined(CMXSAM9263) || \
   defined(POLSAM9260) || defined(CMXSAM9260) || \
   defined(POLPIC32) || defined(CMXPIC32) || \
   defined(POLMPC5566) || defined(CMXMPC5566))
#define TASK_STACK_SIZE             1536
#elif (defined(POLNECV850) || defined(CMXNECV850) || \
   defined(POLNECV850ES) || defined(CMXNECV850ES) || \
   defined(POLV850EPH3) || defined(CMXV850EPH3))
#define TASK_STACK_SIZE             1536
#elif (defined(POLH8S2472) || defined(CMXH8S2472))
#define TASK_STACK_SIZE             512
#elif (defined(_C166))                    /* Tasking C16x */
#define TASK_STACK_SIZE             512
#else
#define TASK_STACK_SIZE             256
#endif
#define MN_TASK_CREATE(A,B,C,D)     K_Task_Create((A),(B),(C),(D))
#define MN_TASK_START(A)            K_Task_Start(A)
#define MN_TASK_WAIT(A)             K_Task_Wait(A)
#define MN_TASK_LOCK                K_Task_Lock()
#define MN_TASK_UNLOCK              K_Task_Unlock()
#define MN_TASK_STOP                K_Task_End()

/* Mutex definitions */
typedef byte MUTEX_NUM_T;
typedef byte MUTEX_STATUS_T;
#define MUTEX_SUCCESS               K_OK
#define MUTEX_IP_RECEIVE            mutex_ip_receive
#define MUTEX_IP_SEND               mutex_ip_send
#define MUTEX_MN_INFO               mutex_mn_info
#define MUTEX_PING_BUFF             mutex_ping_buff
#define MUTEX_IGMP                  mutex_igmp
#define MUTEX_PPP                   mutex_ppp
#define MUTEX_ARP_CACHE             mutex_arp_cache
#define MUTEX_BOOTP_DHCP            mutex_bootp_dhcp
#define MUTEX_TFTP                  mutex_tftp
#define MUTEX_SMTP                  mutex_smtp
#define MUTEX_POP3                  mutex_pop3
#define MUTEX_SNMP                  mutex_snmp
#define MUTEX_VFS_DIR               mutex_vfs_dir
#define MUTEX_VFS_PF                mutex_vfs_pf
#define MUTEX_VFS_GF                mutex_vfs_gf
#define MUTEX_EFFSM                 mutex_effsm
#define MUTEX_FTP_SERVER            mutex_ftp_server
#define MUTEX_FTP_CLIENT            mutex_ftp_client
#define MUTEX_HTTP_SERVER           mutex_http_server
#define MUTEX_DNS                   mutex_dns
#define MUTEX_SNTP                  mutex_sntp

#define MN_MUTEX_CREATE(A)          mn_alloc_mutex(A)
#define MN_MUTEX_GET(A)             K_Resource_Get(A)
#define MN_MUTEX_WAIT(A,B)          K_Resource_Wait((A),(B))
#define MN_MUTEX_RELEASE(A)         K_Resource_Release(A)

/* Signaling definitions */
typedef byte SIGNAL_NUM_T;
typedef byte SIGNAL_STATUS_T;
typedef word16 SIGNAL_COUNT_T;
#define SIGNAL_SUCCESS              K_OK
#define SIGNAL_TRANSMIT             signal_transmit
#define SIGNAL_RECEIVE              signal_receive
#define SIGNAL_PPP                  signal_ppp
#define SIGNAL_TIMER_UPDATE         signal_timer_update
#define SIGNAL_PING_REPLY           signal_ping_reply
#define SIGNAL_IGMP                 signal_igmp
#define SIGNAL_SNMP                 signal_snmp

#define MN_SIGNAL_CREATE(A,B)       mn_alloc_signal( (A), (B) )
#define MN_SIGNAL_RESET(A)          K_Semaphore_Reset( (A), 1 );
#define MN_SIGNAL_GET(A)            K_Semaphore_Get(A)
#define MN_SIGNAL_WAIT(A,B)         K_Semaphore_Wait((A),(B))
#define MN_SIGNAL_POST(A)           K_Semaphore_Post(A)
#define MN_SIGNAL_COUNT(A,B)        K_Semaphore_Get_Count((A),(B))
#define MN_ISR_SIGNAL_POST(A)       K_Intrp_Semaphore_Post(A)

/* other definitions */
typedef word32 TIMER_TICK_T;
#define MN_TICK_UPDATE              K_OS_Tick_Update()
#define MN_GET_TICK                 K_OS_Tick_Get_Ctr()
#define MN_OS_INIT                  K_OS_Init()
#define MN_OS_START                 K_OS_Start()
#if (defined(CMXX86))
#define MN_OS_END                   cmx_end()
#else
#define MN_OS_END
#endif      /* (defined(CMXX86)) */

#elif (RTOS_USED == RTOS_CMX_TINYP)

/* include necessary header files for RTOS */
#include "cpfuncs.h"
#include "cpextern.h"
/* #include "cpconfig.h" */

/* Task definitions */
typedef byte TASKID_T;    /* for CMX-Tiny+ the taskid == rom_id */
typedef byte TASK_STATUS_T;
#define TASK_SUCCESS                K_OK
#define TASK_STACK_SIZE             60
#define MN_TASK_CREATE(A)           K_Task_Create((A))
#define MN_TASK_START(A)            K_Task_Start(A)
#define MN_TASK_WAIT(A)             K_Task_Wait(A)
#define MN_TASK_LOCK                K_Task_Lock()
#define MN_TASK_UNLOCK              K_Task_Unlock()
#define MN_TASK_STOP                K_Task_End()

/* Mutex definitions */
typedef byte MUTEX_NUM_T;
typedef byte MUTEX_STATUS_T;
#define MUTEX_SUCCESS               K_OK
#define MUTEX_IP_RECEIVE            mutex_ip_receive
#define MUTEX_IP_SEND               mutex_ip_send
#define MUTEX_MN_INFO               mutex_mn_info
#define MUTEX_PING_BUFF             mutex_ping_buff
#define MUTEX_IGMP                  mutex_igmp
#define MUTEX_PPP                   mutex_ppp
#define MUTEX_ARP_CACHE             mutex_arp_cache
#define MUTEX_BOOTP_DHCP            mutex_bootp_dhcp
#define MUTEX_TFTP                  mutex_tftp
#define MUTEX_SMTP                  mutex_smtp
#define MUTEX_POP3                  mutex_pop3
#define MUTEX_SNMP                  mutex_snmp
#define MUTEX_VFS_DIR               mutex_vfs_dir
#define MUTEX_VFS_PF                mutex_vfs_pf
#define MUTEX_VFS_GF                mutex_vfs_gf
#define MUTEX_EFFSM                 mutex_effsm
#define MUTEX_FTP_SERVER            mutex_ftp_server
#define MUTEX_FTP_CLIENT            mutex_ftp_client
#define MUTEX_HTTP_SERVER           mutex_http_server
#define MUTEX_DNS                   mutex_dns
#define MUTEX_SNTP                  mutex_sntp

#define MN_MUTEX_CREATE(A)          mn_alloc_mutex(A)
#define MN_MUTEX_GET(A)             K_Resource_Get(A)
#define MN_MUTEX_WAIT(A,B)          K_Resource_Wait((A),(B))
#define MN_MUTEX_RELEASE(A)         K_Resource_Release(A)

/* Signaling definitions */
typedef byte SIGNAL_NUM_T;
typedef byte SIGNAL_STATUS_T;
typedef word16 SIGNAL_COUNT_T;
#define SIGNAL_SUCCESS              K_OK
#define SIGNAL_TRANSMIT             signal_transmit
#define SIGNAL_RECEIVE              signal_receive
#define SIGNAL_PPP                  signal_ppp
#define SIGNAL_TIMER_UPDATE         signal_timer_update
#define SIGNAL_PING_REPLY           signal_ping_reply
#define SIGNAL_IGMP                 signal_igmp
#define SIGNAL_SNMP                 signal_snmp

#define MN_SIGNAL_CREATE(A,B)       mn_alloc_signal( (A), (B) )
#define MN_SIGNAL_RESET(A)          K_Semaphore_Reset( (A) );
#define MN_SIGNAL_GET(A)            K_Semaphore_Get(A)
#define MN_SIGNAL_WAIT(A,B)         K_Semaphore_Wait((A),(B))
#define MN_SIGNAL_POST(A)           K_Semaphore_Post(A)
#define MN_SIGNAL_COUNT(A,B)        K_Semaphore_Get_Count((A),(B))
#define MN_ISR_SIGNAL_POST(A)       K_Semaphore_Post(A)

/* other definitions */
/* CMX-Tiny+ doesn't keep track of timer ticks. We use the same timer tick
   as the NO_RTOS version.
*/
typedef word16 TIMER_TICK_T;
#define MN_TICK_UPDATE              { ++timer_tick; K_OS_Tick_Update(); }
#define MN_GET_TICK                 (mn_get_timer_tick())
#define MN_OS_INIT                  K_OS_Init()
#define MN_OS_START                 K_OS_Start()
#if (defined(CMXX86))
#define MN_OS_END                   cmx_end()
#else
#define MN_OS_END
#endif      /* (defined(CMXX86)) */

#else
#error Undefined RTOS!
#endif      /* (RTOS_USED == RTOS_NONE) */

/* xmit_busy is a transmit busy flag used in Non-RTOS versions and the PIC18.
   RTOS versions use a signal instead. It should not be necessary to change
   these defines.
*/
#if (RTOS_USED == RTOS_NONE || defined(CMXPIC18) || defined(CMXPIC18E) || \
   defined(CMXP18F97J60))
#define MN_XMIT_BUSY_SET            xmit_busy = 1
#define MN_XMIT_BUSY_CLEAR          xmit_busy = 0
#else
#define MN_XMIT_BUSY_SET
#define MN_XMIT_BUSY_CLEAR
#endif      /* (RTOS_USED == RTOS_NONE || defined(CMXPIC18) || defined(CMXPIC18E)) */

#if (RTOS_USED != RTOS_NONE)
/* Do not change the next #define */
#define MN_PPP_STATUS_UP            mn_get_ppp_status_up()

/* Priority definitions - In CMX RTOSes a lower number is a higher priority.
   These priorities may be changed.
*/
#define TCP_RESEND_TASK_PRIORITY    18
#define RECEIVE_TASK_PRIORITY       16 
#define APP_TASK_PRIORITY           14
#define MAIN_TASK_PRIORITY          12
#define PING_TASK_PRIORITY          10
#define IGMP_TASK_PRIORITY          10
#define SNMP_TASK_PRIORITY          10
#define DHCP_LEASE_TASK_PRIORITY     8
#define TIMER_TASK_PRIORITY          6
#endif      /* (RTOS_USED != RTOS_NONE) */

/* In CMX RTOSes 0 is used to mean wait without a timeout. */
#define INFINITE_WAIT   0

#endif      /* #ifndef MN_OS_H_INC */
