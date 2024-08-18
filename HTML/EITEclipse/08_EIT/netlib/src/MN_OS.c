/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if (RTOS_USED == RTOS_CMX_RTX)
TASKID_T mn_receive_id;
#if (MN_PING_REPLY)
TASKID_T mn_ping_reply_id;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
TASKID_T mn_igmp_timer_id;
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
TASKID_T mn_snmp_timer_id;
TASKID_T mn_snmp_reply_id;
#endif      /* (MN_SNMP) */
#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
TASKID_T mn_timer_update_id;
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */
#if (MN_DHCP)
TASKID_T mn_dhcp_lease_id;
#endif      /* (MN_DHCP) */
#if (MN_FTP_SERVER)
TASKID_T mn_ftp_server_id;
#endif      /* (MN_FTP_SERVER) */
#if (MN_HTTP)
TASKID_T mn_http_server_id;
#endif      /* (MN_HTTP) */
#if (MN_TCP && MN_TCP_DELAYED_ACKS)
TASKID_T mn_tcp_resend_id;
#endif      /* (MN_TCP && MN_TCP_DELAYED_ACKS) */
#endif      /* (RTOS_USED == RTOS_CMX_RTX) */

#if (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP)
MUTEX_NUM_T mutex_ip_receive;
MUTEX_NUM_T mutex_ip_send;
#if (MN_ETHERNET)
MUTEX_NUM_T mutex_mn_info;
#endif      /* (MN_ETHERNET) */
#if (MN_PING_REPLY)
MUTEX_NUM_T mutex_ping_buff;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
MUTEX_NUM_T mutex_igmp;
#endif      /* (MN_IGMP) */
#if (MN_PPP)
MUTEX_NUM_T mutex_ppp;
#endif      /* (MN_PPP) */
#if (MN_ETHERNET && MN_ARP)
MUTEX_NUM_T mutex_arp_cache;
#endif      /* (MN_ETHERNET && MN_ARP) */
#if (MN_DHCP || MN_BOOTP)
MUTEX_NUM_T mutex_bootp_dhcp;
#endif      /* (MN_DHCP || MN_BOOTP) */
#if (MN_TFTP)
MUTEX_NUM_T mutex_tftp;
#endif      /* (MN_TFTP) */
#if (MN_SMTP)
MUTEX_NUM_T mutex_smtp;
#endif      /* (MN_SMTP) */
#if (MN_POP3)
MUTEX_NUM_T mutex_pop3;
#endif      /* (MN_POP3) */
#if (MN_SNMP)
MUTEX_NUM_T mutex_snmp;
#endif      /* (MN_SNMP) */
#if (MN_VIRTUAL_FILE)
MUTEX_NUM_T mutex_vfs_dir;
#if (MN_HTTP)
#if (MN_NUM_POST_FUNCS)
MUTEX_NUM_T mutex_vfs_pf;
#endif      /* (MN_NUM_POST_FUNCS) */
#if (MN_SERVER_SIDE_INCLUDES)
MUTEX_NUM_T mutex_vfs_gf;
#endif      /* (MN_SERVER_SIDE_INCLUDES) */
#endif      /* (MN_HTTP) */
#if (MN_USE_EFFSM)
MUTEX_NUM_T mutex_effsm;
#endif      /* (MN_USE_EFFSM) */
#endif      /* (MN_VIRTUAL_FILE) */
#if (MN_FTP_SERVER)
MUTEX_NUM_T mutex_ftp_server;
#endif      /* (MN_FTP_SERVER) */
#if (MN_FTP_CLIENT)
MUTEX_NUM_T mutex_ftp_client;
#endif      /* (MN_FTP_CLIENT) */
#if (MN_HTTP)
MUTEX_NUM_T mutex_http_server;
#endif      /* (MN_HTTP) */
#if (MN_DNS)
MUTEX_NUM_T mutex_dns;
#endif      /* (MN_DNS) */
#if (MN_SNTP)
MUTEX_NUM_T mutex_sntp;
#endif      /* (MN_SNTP) */

static byte num_mutexes;

SIGNAL_NUM_T signal_transmit;
SIGNAL_NUM_T signal_receive;
SIGNAL_NUM_T signal_socket[MN_NUM_SOCKETS];
#if (MN_PING_REPLY)
SIGNAL_NUM_T signal_ping_reply;
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
SIGNAL_NUM_T signal_igmp;
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
SIGNAL_NUM_T signal_snmp;
#endif      /* (MN_SNMP) */
#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
SIGNAL_NUM_T signal_timer_update;
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */
#if (MN_PPP)
SIGNAL_NUM_T signal_ppp;
#endif      /* (MN_PPP) */

static byte num_signals;
#endif      /* (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP) */

#if (RTOS_USED == RTOS_CMX_TINYP)
static byte create_tinyp_task(CMX_FP) cmx_reentrant;
#endif      /* (RTOS_USED == RTOS_CMX_TINYP) */

#if (RTOS_USED != RTOS_NONE)
byte mn_init_done;
#endif      /* (RTOS_USED != RTOS_NONE) */

/* Performs OS related initialization. Must be done before mn_init.
   Returns 1 if successful, 0 if not.
*/
byte mn_os_init(void)
cmx_reentrant {
   byte status;
#if (RTOS_USED != RTOS_NONE)
   SIGNAL_STATUS_T sig_status;
   MUTEX_STATUS_T mut_status;
   TASK_STATUS_T task_status;
   byte i;
#endif      /* (RTOS_USED != RTOS_NONE) */

   status = 1;

#if (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP)
   num_mutexes = MAX_RESOURCES;
   num_signals = MAX_SEMAPHORES;
#endif      /* (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP) */

#if (RTOS_USED != RTOS_NONE)

   MN_OS_INIT;

   MEMSET(signal_socket, 0, sizeof(signal_socket));
   MEMSET(interface_info, 0, sizeof(interface_info));

   /* create the necessary semaphores (signals) */
   sig_status = MN_SIGNAL_CREATE(&signal_transmit,0);
   if (sig_status)
      sig_status = MN_SIGNAL_CREATE(&signal_receive,0);

   i = 0;
   while (sig_status && i < MN_NUM_SOCKETS)
      {
      sig_status = MN_SIGNAL_CREATE(&signal_socket[i],0);
      i++;
      }

#if (MN_PING_REPLY)
   if (sig_status)
      sig_status = MN_SIGNAL_CREATE(&signal_ping_reply,0);
#endif      /* (MN_PING_REPLY) */

#if (MN_IGMP)
   if (sig_status)
      sig_status = MN_SIGNAL_CREATE(&signal_igmp,0);
#endif      /* (MN_IGMP) */

#if (MN_SNMP)
   if (sig_status)
      sig_status = MN_SIGNAL_CREATE(&signal_snmp,0);
#endif      /* (MN_SNMP) */

#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
   if (sig_status)
      sig_status = MN_SIGNAL_CREATE(&signal_timer_update,0);
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */

#if (MN_PPP)
   if (sig_status)
      sig_status = MN_SIGNAL_CREATE(&signal_ppp,0);
#endif      /* (MN_PPP) */

   if (!sig_status)
      return (0);

   /* create the necessary mutexes (resources) */
   mut_status = MN_MUTEX_CREATE(&mutex_ip_receive);
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_ip_send);

   /* One mutex for each interface. */
   for (i=0; i<MN_NUM_INTERFACES; i++)
      {
      mut_status = MN_MUTEX_CREATE(&interface_info[i].mutex_interface);
      if (!mut_status)
         break;
      }

#if (MN_ETHERNET)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_mn_info);
#endif      /* (MN_ETHERNET) */
#if (MN_PING_REPLY)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_ping_buff);
#endif      /* (MN_PING_REPLY) */
#if (MN_IGMP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_igmp);
#endif      /* (MN_IGMP) */
#if (MN_PPP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_ppp);
#endif      /* (MN_PPP) */
#if (MN_ETHERNET && MN_ARP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_arp_cache);
#endif      /* (MN_ETHERNET && MN_ARP) */
#if (MN_DHCP || MN_BOOTP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_bootp_dhcp);
#endif      /* (MN_DHCP || MN_BOOTP) */
#if (MN_TFTP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_tftp);
#endif      /* (MN_TFTP) */
#if (MN_SMTP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_smtp);
#endif      /* (MN_SMTP) */
#if (MN_POP3)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_pop3);
#endif      /* (MN_POP3) */
#if (MN_SNMP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_snmp);
#endif      /* (MN_SNMP) */

#if (MN_VIRTUAL_FILE)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_vfs_dir);
#if (MN_HTTP)
#if (MN_NUM_POST_FUNCS)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_vfs_pf);
#endif      /* (MN_NUM_POST_FUNCS) */
#if (MN_SERVER_SIDE_INCLUDES)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_vfs_gf);
#endif      /* (MN_SERVER_SIDE_INCLUDES) */
#endif      /* (MN_HTTP) */
#if (MN_USE_EFFSM)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_effsm);
#endif      /* (MN_USE_EFFSM) */
#endif      /* (MN_VIRTUAL_FILE) */

#if (MN_FTP_SERVER)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_ftp_server);
#endif      /* (MN_FTP_SERVER) */

#if (MN_FTP_CLIENT)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_ftp_client);
#endif      /* (MN_FTP_CLIENT) */

#if (MN_HTTP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_http_server);
#endif      /* (MN_HTTP) */

#if (MN_DNS)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_dns);
#endif      /* (MN_DNS) */

#if (MN_SNTP)
   if (mut_status)
      mut_status = MN_MUTEX_CREATE(&mutex_sntp);
#endif      /* (MN_SNTP) */

   if (!mut_status)
      return (0);

   /* create and start the necessary tasks */
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(RECEIVE_TASK_PRIORITY, &mn_receive_id, \
      mn_receive_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_receive_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_receive_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);

#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(TIMER_TASK_PRIORITY, &mn_timer_update_id, \
      mn_timer_update_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_timer_update_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_timer_update_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */

#if (MN_DHCP)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(DHCP_LEASE_TASK_PRIORITY, &mn_dhcp_lease_id, \
      mn_dhcp_lease_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_dhcp_lease_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_dhcp_lease_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_DHCP) */

#if (MN_PING_REPLY)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(PING_TASK_PRIORITY, &mn_ping_reply_id, \
      mn_ping_reply_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_ping_reply_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_ping_reply_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_PING_REPLY) */

#if (MN_IGMP)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(IGMP_TASK_PRIORITY, &mn_igmp_timer_id, \
      mn_igmp_timer_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_igmp_timer_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_igmp_timer_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_IGMP) */

#if (MN_SNMP)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(SNMP_TASK_PRIORITY, &mn_snmp_timer_id, \
      mn_snmp_timer_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_snmp_timer_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_snmp_timer_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);

#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(SNMP_TASK_PRIORITY, &mn_snmp_reply_id, \
      mn_snmp_reply_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_snmp_reply_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_snmp_reply_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_SNMP) */

#if (MN_FTP_SERVER)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(APP_TASK_PRIORITY, &mn_ftp_server_id, \
      mn_ftp_server_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_ftp_server_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_ftp_server_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_FTP_SERVER) */

#if (MN_HTTP)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(APP_TASK_PRIORITY, &mn_http_server_id, \
      mn_http_server_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_http_server_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_http_server_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_HTTP) */

#if (MN_TCP && MN_TCP_DELAYED_ACKS)
#if (RTOS_USED == RTOS_CMX_RTX)
   task_status = MN_TASK_CREATE(TCP_RESEND_TASK_PRIORITY, &mn_tcp_resend_id, \
      mn_tcp_resend_task, TASK_STACK_SIZE);
   if (task_status == TASK_SUCCESS)
      task_status = MN_TASK_START(mn_tcp_resend_id);
#elif (RTOS_USED == RTOS_CMX_TINYP)
   task_status = create_tinyp_task(mn_tcp_resend_task);
#endif
   if (task_status != TASK_SUCCESS)
      return (0);
#endif      /* (MN_TCP && MN_TCP_DELAYED_ACKS) */
#endif      /* (RTOS_USED != RTOS_NONE) */

   return (status);
}

#if (RTOS_USED != RTOS_NONE)
void wait_for_mn_init(void)
cmx_reentrant {
   byte init_done;

   do
      {
      DISABLE_INTERRUPTS;
      init_done = mn_init_done;
      ENABLE_INTERRUPTS;

      if (!init_done)
         MN_TASK_WAIT(1);
      }
   while (!init_done);
}
#endif      /* (RTOS_USED != RTOS_NONE) */

#if (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP)

#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif      /* defined(__C51__) */
/* Wait on signal from tcp_timer ISR and update ARP and/or DHCP timers. */
void mn_timer_update_task(void)
cmx_reentrant {
   while (1)
      {
      if (MN_SIGNAL_WAIT(SIGNAL_TIMER_UPDATE,INFINITE_WAIT) == SIGNAL_SUCCESS)
         {
#if MN_DHCP
         mn_dhcp_update_timer();
#endif      /* MN_DHCP */
#if (MN_ARP && MN_ARP_TIMEOUT)
         mn_arp_update_timer();
#endif      /* (MN_ARP && MN_ARP_TIMEOUT) */
#if (MN_SOCKET_INACTIVITY_TIME)
         mn_update_inactivity_timers();
#endif      /* (MN_SOCKET_INACTIVITY_TIME) */
         }
      }
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif      /* defined(__C51__) */
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */

/* allocate a resource */
byte mn_alloc_mutex(MUTEX_NUM_T * mutex_ptr)
cmx_reentrant {
   if (!num_mutexes)
      return(0);
   else
      {
      *mutex_ptr = --num_mutexes;
      return(1);
      }
}

/* allocate a semaphore */
byte mn_alloc_signal(SIGNAL_NUM_T * signal_ptr, word16 num)
cmx_reentrant {
   SIGNAL_STATUS_T status;

   if (!num_signals)
      return(0);
   else
      {
      *signal_ptr = --num_signals;
#if (RTOS_USED == RTOS_CMX_RTX)
/*      return( K_Semaphore_Create(*signal_ptr, num) == K_OK ? 1 : 0 ); */
      status = K_Semaphore_Create(*signal_ptr, num);
      if (status == K_OK)
         return (1);
      else
         return (0);
#elif (RTOS_USED == RTOS_CMX_TINYP)
      return (1);
#endif
      }
}

/* Returns K_OK if a valid semaphore number is passed, K_ERROR if not.
   The count of the semaphore goes into count_ptr.
*/
byte K_Semaphore_Get_Count(byte sem_num, SIGNAL_COUNT_T *count_ptr)  
cmx_reentrant {
   if (sem_num >= MAX_SEMAPHORES)
      return(K_ERROR);  /* return error status to caller */

   K_I_Disable_Sched();

   *count_ptr = sem_array[sem_num].sem_count;

   K_I_Func_Return();
   return(K_OK);
}
#endif      /* (RTOS_USED == RTOS_CMX_RTX || RTOS_USED == RTOS_CMX_TINYP) */

/* Get ROM_ID from task's address. This function is needed because we don't
   always know the precise location of a task in the rom_task_list.
*/
#if (RTOS_USED == RTOS_CMX_TINYP)
byte mn_get_rom_id(CMX_FP task_ptr)
cmx_reentrant {
   byte i;
   byte retval;

   retval = K_ERROR;

   if (task_ptr != PTR_NULL)
      {
      MN_TASK_LOCK;
      for (i=0; i<ROM_TASKS; i++)
         {
         if (task_rom_list[i].task_addr == task_ptr)
            {
            retval = i;
            break;
            }
         }
      MN_TASK_UNLOCK;
      }

   return (retval);
}

static byte create_tinyp_task(CMX_FP task_ptr)
cmx_reentrant {
   TASK_STATUS_T task_status;
   byte task_id;

   task_status = mn_get_rom_id(task_ptr);
   if (task_status != K_ERROR)
      {
      task_id = task_status;
      task_status = MN_TASK_CREATE(task_id);
      if (task_status == TASK_SUCCESS)
         task_status = MN_TASK_START(task_rom_list[task_id].ram_tcb_index);
      }

   return (task_status);
}
#endif      /* (RTOS_USED == RTOS_CMX_TINYP) */
