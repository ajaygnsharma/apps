/*****************************************************************************
 *   timer.c:  Timer C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"		/* LPC23xx/24xx Peripheral Registers	*/
#include "type.h"
#include "irq.h"
#include "timer.h"


volatile unsigned long timer0_counter = 0;
volatile unsigned long timer1_counter = 0;
unsigned char clock_500ms;

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 2 or 3
** Returned value:		None
** 
******************************************************************************/
void enable_timer( BYTE timer_num )
{
  if ( timer_num == 0 )
	T0TCR = 1;
  else if( timer_num == 1)
	T1TCR = 1;
  else if( timer_num == 2)
    T2TCR = 1;
  else if( timer_num == 3)
    T3TCR = 1;
  
  return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0/2/3
** Returned value:		None
** 
******************************************************************************/
void disable_timer( BYTE timer_num )
{
  if ( timer_num == 0 )
	T0TCR = 0;
  else if(timer_num == 1)
	T1TCR = 0;
  else if( timer_num == 2)
    T2TCR = 0;
  else if( timer_num == 3)
    T3TCR = 0;
  
  return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0/1/2/3
** Returned value:		None
** 
******************************************************************************/
void reset_timer( BYTE timer_num )
{
   unsigned long regVal;

  if ( timer_num == 0 )
  {
	regVal = T0TCR;
	regVal |= 0x02;
	T0TCR = regVal;
  }
  else if(timer_num == 1)
  {
	regVal = T1TCR;
	regVal |= 0x02;
	T1TCR = regVal;
  }
  else if( timer_num == 2)
  {
	regVal = T2TCR;
	regVal |= 0x02;
	T2TCR = regVal;
  }
  else if( timer_num == 3)
  {
	regVal = T3TCR;
	regVal |= 0x02;
	T3TCR = regVal;
  }
  
  return;
}


/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs( byte timer_num,  unsigned long delayInMs)
{
  if ( timer_num == 0 )
  {
	/*
	* setup timer #0 for delay
	*/
	T0TCR = 0x02;		/* reset timer */
	T0PR  = 0x00;		/* set prescaler to zero */
	T0MR0 = delayInMs * TIME_INTERVAL;
	T0IR  = 0xff;		/* reset all interrrupts */
	T0MCR = 0x04;		/* stop timer on match */
	T0TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (T0TCR & 0x01);
  }
  else if ( timer_num == 1 )
  {
	/*
	* setup timer #1 for delay
	*/
	T1TCR = 0x02;		/* reset timer */
	T1PR  = 0x00;		/* set prescaler to zero */
	T1MR0 = delayInMs * TIME_INTERVAL;
	T1IR  = 0xff;		/* reset all interrrupts */
	T1MCR = 0x04;		/* stop timer on match */
	T1TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (T1TCR & 0x01);
  }
  return;
}

/******************************************************************************
** Function name:		Timer1Handler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer3Handler (void) __irq 
{  
//  //static int clk_cntr;
//  DWORD regVal;
//
//  IENABLE;			/* handles nested interrupt */
//  
//  T3IR = 1;			/* clear interrupt flag */
//  VICVectAddr = 0;	/* Acknowledge Interrupt */  
//  regVal = T3TCR;
//  regVal |= 0x02;
//  T3TCR = regVal;
//  T3TCR = 0;
//  FIO1SET3 = 0xC0;
//  
//  IDISABLE;
  T3IR = 1;			/* clear interrupt flag */
  IENABLE;			/* handles nested interrupt */

  FIO1SET3 = 0xC0;

  IDISABLE;
  VICVectAddr = 0;	/* Acknowledge Interrupt */
  
}


/******************************************************************************
** Function name:		Timer2Handler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer2Handler (void) __irq 
{  
  T2IR = 1;			/* clear interrupt flag */
  IENABLE;			/* handles nested interrupt */

  /* Execute this ISR only if the Conditional GPIO is enabled */
  //if(flag.gpio_type == 2)
  //{
  //  if(gpio.if_condition[0] == gpio.then_condition[0])
  //    FIO1SET3 = 0xC0;
  //}

  IDISABLE;
  VICVectAddr = 0;	/* Acknowledge Interrupt */
  
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval(msec)
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
unsigned long init_timer (byte timer_num) 
{
  if(timer_num == 2)
  {
    T2MR0 = TIME_INTERVAL * 500; // TimerInterval is in Milliseconds
    T2MCR = 3;				/* Interrupt and Reset on MR1 */
  
    VICVectAddr26  = (unsigned long)Timer2Handler;/* Set Interrupt Vector        */
    VICVectPriority26  = 26;                        /* use it for Timer2 Interrupt */
    VICIntEnable   = 0x04000000;                   /* Enable Timer1 Interrupt     */
  }
  else if(timer_num == 3)
  {
    T3MR0 = TIME_INTERVAL * 500; // TimerInterval is in Milliseconds
    T3MCR = 3;				/* Interrupt and Reset on MR1 */
  
    VICVectAddr27  = (unsigned long)Timer3Handler;/* Set Interrupt Vector        */
    VICVectPriority27  = 27;                          /* use it for Timer1 Interrupt */
    VICIntEnable   = 0x08000000;                   /* Enable Timer1 Interrupt     */
  }

  return TRUE; 
}

/******************************************************************************
**                            End Of File
******************************************************************************/
