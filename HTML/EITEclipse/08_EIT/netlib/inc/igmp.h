/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef IGMP_H_INC
#define IGMP_H_INC 1

#define IGMP_LEN           8
#define IGMP_QUERY         0x11
#define IGMP_V1_REPORT     0x12
#define IGMP_MAX_RESP_TIME 0x00
#define IGMP_DELAY         (10 * (MN_ONE_SECOND))

/* IGMP states */
#define IGMP_NON_MEMBER    0
#define IGMP_DELAY_MEMBER  1
#define IGMP_IDLE_MEMBER   2

typedef struct igmp_info_s
{
   byte ip_group_addr[IP_ADDR_LEN];
   word16 igmp_timer;
   byte ref_count;
   byte igmp_state;
   SCHAR interface_no;
} IGMP_INFO_T;

typedef IGMP_INFO_T * PIGMP_INFO;

void mn_igmp_init(void) cmx_reentrant;
SCHAR mn_igmp_join(byte ip[], SCHAR interface_no) cmx_reentrant;
SCHAR mn_igmp_leave(byte ip[], SCHAR interface_no) cmx_reentrant;
void mn_igmp_process_packet(void) cmx_reentrant;
PIGMP_INFO mn_igmp_search_entry(byte ip[], SCHAR interface_no) cmx_reentrant;
#if (RTOS_USED == RTOS_NONE)
#if (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_igmp_update_timers(void);
#else
void mn_igmp_update_timers(void) cmx_reentrant;
#endif      /* (defined(_CC51)) */
void mn_igmp_check_timers(void) cmx_reentrant;
#endif      /* (RTOS_USED == RTOS_NONE) */

#endif   /* #ifndef IGMP_H_INC */

