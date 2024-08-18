/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_TIMER_H_INC
#define MN_TIMER_H_INC  1

typedef struct timer_info_s {
   TIMER_TICK_T timer_start;
   TIMER_TICK_T timer_end;
   byte timer_wrap;
} TIMER_INFO_T;

typedef TIMER_INFO_T * PTIMER_INFO;

void mn_reset_timer(PTIMER_INFO, TIMER_TICK_T) cmx_reentrant;
byte mn_timer_expired(PTIMER_INFO) cmx_reentrant;
#if (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP)
TIMER_TICK_T mn_get_timer_tick(void) cmx_reentrant;
#endif      /* (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP) */
void mn_wait_ticks(TIMER_TICK_T num_ticks) cmx_reentrant;

#endif   /* #ifndef MN_TIMER_H_INC */
