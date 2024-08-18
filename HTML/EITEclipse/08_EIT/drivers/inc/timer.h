/*****************************************************************************
 *   timer.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H


typedef unsigned char byte;
/* depending on the CCLK and PCLK setting, e.g. CCLK = 60Mhz, 
PCLK = 1/4 CCLK, then, 10mSec = 150.000-1 counts */
//#define TIME_INTERVAL	149999            AJAY
//#define TIME_INTERVAL	14999           //1 ms interval if timer set with this value 

/*-------------------------------------------------------------------------------------------------
 TIME INTERVAL 

 This is the time interval for getting 1 ms resolution. Although it says that 14999 would mean
 1 ms for 60MHz clock but we actually need to be 11999 which is for 48MHz but thats the way it is.
 Actually looks like the Clock Speed is 48MHz as it works fine.
-------------------------------------------------------------------------------------------------*/
//#define TIME_INTERVAL	11999           //1 ms interval if timer set with this value 
#define TIME_INTERVAL	17999           //1 ms interval if timer set with this value 
	
//extern volatile DWORD timer1_counter;
//extern volatile DWORD flag_done;
//#define TIME_INTERVAL	(Fpclk/100 - 1)

extern void delayMs(byte timer_num, unsigned long delayInMs);
extern void enable_timer(byte timer_num );
extern void disable_timer(byte timer_num );
extern void reset_timer(byte timer_num );
//extern DWORD init_timer( BYTE timer_num, DWORD timerInterval );
extern  unsigned long init_timer(byte timer_num);
static int clk_cntr;
extern unsigned char clock_500ms;

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
