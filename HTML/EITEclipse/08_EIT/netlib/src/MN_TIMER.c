/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP)
#if 0    /* (defined(POLPIC18) || defined(CMXPIC18)) */
/* some PIC compilers can't handle 16-bit math in ISRs, workaround by
   using a union
*/
extern volatile BYTE2WORD_U timer;
#define  timer_tick timer.w
#else
extern volatile TIMER_TICK_T timer_tick;
#endif
#endif      /* (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP) */

/* resets a timer */
void mn_reset_timer(PTIMER_INFO timer_ptr, TIMER_TICK_T num_ticks)
cmx_reentrant {
   timer_ptr->timer_start = MN_GET_TICK;
   timer_ptr->timer_end = timer_ptr->timer_start + num_ticks;

   /* check for wrap */
   if (timer_ptr->timer_start <= timer_ptr->timer_end)
      timer_ptr->timer_wrap = FALSE;
   else
      {
      timer_ptr->timer_end = num_ticks - \
         (0xffff - timer_ptr->timer_start + 1);
      timer_ptr->timer_wrap = TRUE;
      }
}

/* returns 1 if a timer has expired, otherwise returns 0 */
byte mn_timer_expired(PTIMER_INFO timer_ptr)
cmx_reentrant {
   TIMER_TICK_T curr_tick;

   curr_tick = MN_GET_TICK;

   if (timer_ptr->timer_wrap)
      {
      if ((curr_tick < timer_ptr->timer_start) && \
            (curr_tick > timer_ptr->timer_end))
         return TRUE;
      }
   else
      {
      if (( curr_tick > timer_ptr->timer_end ) || \
            (curr_tick < timer_ptr->timer_start))
         return TRUE;
      }

   return FALSE;
}

#if (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP)
TIMER_TICK_T mn_get_timer_tick(void)
cmx_reentrant {
   TIMER_TICK_T curr_tick;

#if (GET_TICK_ATOMIC == 0)
   DISABLE_INTERRUPTS;
#endif
   curr_tick = timer_tick;
#if (GET_TICK_ATOMIC == 0)
   ENABLE_INTERRUPTS;
#endif

   return (curr_tick);
}
#endif      /* (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP) */

/* waits for a given number of ticks. */
void mn_wait_ticks(TIMER_TICK_T num_ticks)
cmx_reentrant {
   TIMER_INFO_T wait_timer;

   mn_reset_timer(&wait_timer,num_ticks);
   while (!mn_timer_expired(&wait_timer));
}
