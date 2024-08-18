/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
#include "micronet.h"

#if MN_SNMP

#if (RTOS_USED == RTOS_NONE)
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif      /* defined(__C51__) */
/* Update the system uptime. Called from tcp_timer ISR. */
#if ( defined(POLECOG1) || defined(POLECOG1E) )
void __irq_code mn_snmp_tick(void)
#elif (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_snmp_tick(void)
#else
void mn_snmp_tick(void)
#endif
#else    /* An RTOS used. */
void mn_snmp_tick(void)
#endif      /* (RTOS_USED == RTOS_NONE) */
cmx_reentrant {
#if (MN_ONE_SECOND == 100)
   byte * time_ptr;
   byte i;

   time_ptr = &ucMeasureData[sysUpTime0] + sysUpTime0_Length - 1;
   *time_ptr += 1;               /* least significant at highest location*/
   for (i=0; i<(sysUpTime0_Length-1); i++)
      {
      if (*time_ptr == 0)     /* check for roll over*/
         {
         time_ptr--;
         *time_ptr += 1;         /* increment next digit*/
         }
      else
         break;               /* quit on no roll over*/
      }

#elif (MN_ONE_SECOND < 100)
   byte * time_ptr;
   word32 upTime;

   time_ptr = &ucMeasureData[sysUpTime0];
   upTime = MK_WORD32(*time_ptr,*(time_ptr+1),*(time_ptr+2),*(time_ptr+3));
   upTime += 100 / MN_ONE_SECOND;
   *time_ptr     = WORD32_BYTE3(upTime);
   *(time_ptr+1) = WORD32_BYTE2(upTime);
   *(time_ptr+2) = WORD32_BYTE1(upTime);
   *(time_ptr+3) = WORD32_BYTE0(upTime);
#else    /* (MN_ONE_SECOND > 100) */
   byte * time_ptr;
   byte i;
   static byte count = MN_ONE_SECOND / 100;

   count--;
   if (count == 0)
      {
      count = MN_ONE_SECOND/100;
      time_ptr = &ucMeasureData[sysUpTime0] + sysUpTime0_Length - 1;
      *time_ptr += 1;            /* least significant at highest location*/
      for (i=0; i<(sysUpTime0_Length-1); i++)
         {
         if (*time_ptr == 0)     /* check for roll over*/
            {
            time_ptr--;
            *time_ptr += 1;      /* increment next digit*/
            }
         else
            break;               /* quit on no roll over*/
         }
      }
#endif
}
#if (RTOS_USED == RTOS_NONE)
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif      /* defined(__C51__) */
#endif      /* (RTOS_USED == RTOS_NONE) */

#if (RTOS_USED != RTOS_NONE)
/* Update the system uptime.
   Signalled from tcp_timer ISR.
*/
void mn_snmp_timer_task(void)
cmx_reentrant {

   while (1)
      {
      if (MN_SIGNAL_WAIT(SIGNAL_SNMP,INFINITE_WAIT) == SIGNAL_SUCCESS)
         {
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
         mn_snmp_tick();
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         }
      }
}
#endif      /* (RTOS_USED != RTOS_NONE) */

#endif      /* MN_SNMP */

