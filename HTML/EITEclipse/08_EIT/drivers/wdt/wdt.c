/*****************************************************************************
 *   wdt.c:  Watchdog C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "wdt.h"
#define TRUE 1

volatile  unsigned long wdt_counter;

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**				watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the VIC table
**				is full and WDT interrupt handler can be
**				installed.
** 
*****************************************************************************/
unsigned long WDTInit( void )
{
  wdt_counter = 0;

  WDTC = WDT_FEED_VALUE;	/* once WDEN is set, the WDT will start after feeding */
  WDMOD = WDEN | WDRESET;
  //WDMOD = WDEN;

  WDFEED = 0xAA;		/* Feeding sequence */
  WDFEED = 0x55;    
  return( TRUE );
}

/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTFeed( void )
{
  WDFEED = 0xAA;		/* Feeding sequence */
  WDFEED = 0x55;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
