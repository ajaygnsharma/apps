/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

//#include <rt_misc.h>
#include <stdio.h>

#pragma import(__use_no_semihosting_swi)


extern int  sendchar(int ch);        /* in serial.c                     */
extern int getkey (void);            /* in serial.c                     */  

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;


int fputc(int ch, FILE *f) {
  return (sendchar(ch));
}

// simplified fgetc version that only redirects STDIN
int fgetc(FILE *f) 
{
  // redirect STDIN
  return (getkey());
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  sendchar(ch);
}


void _sys_exit(int return_code) 
{
	label:  goto label;  /* endless loop */
}
