/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_TASKS_H_INC
#define MN_TASKS_H_INC 1

byte mn_os_init(void) cmx_reentrant;

#if (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP)
byte mn_alloc_mutex(MUTEX_NUM_T *) cmx_reentrant;
byte mn_alloc_signal(SIGNAL_NUM_T *, word16) cmx_reentrant;
byte K_Semaphore_Get_Count(byte, SIGNAL_COUNT_T *) cmx_reentrant; 
#endif      /* (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP) */

#if (RTOS_USED == RTOS_CMX_TINYP)
byte mn_get_rom_id(CMX_FP) cmx_reentrant;
#endif      /* (RTOS_USED == RTOS_CMX_TINYP) */

#if (RTOS_USED != RTOS_NONE)
void wait_for_mn_init(void) cmx_reentrant;
void mn_receive_task(void) cmx_reentrant;
#if (MN_PING_REPLY)
void mn_ping_reply_task(void) cmx_reentrant;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
void mn_igmp_timer_task(void) cmx_reentrant;
#endif      /* (MN_IGMP) */
#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
void mn_timer_update_task(void) cmx_reentrant;
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */
#if (MN_DHCP)
void mn_dhcp_lease_task(void) cmx_reentrant;
#endif      /* (MN_DHCP) */
#if (MN_FTP_SERVER)
void mn_ftp_server_task(void) cmx_reentrant;
#endif      /* (MN_FTP_SERVER) */
#if (MN_HTTP)
void mn_http_server_task(void) cmx_reentrant;
#endif      /* (MN_HTTP) */
#if (MN_TCP && MN_TCP_DELAYED_ACKS)
void mn_tcp_resend_task(void) cmx_reentrant;
#endif
#endif      /* (RTOS_USED != RTOS_NONE) */
#endif      /* #ifndef MN_TASKS_H_INC */
