/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "micronet.h"
#include "u2e_http.h"
#include "np_includes.h"

/* Implementation of putchar (also used by printf function to output data)    */

int sendchar (int ch)  /* Write character to Serial Port    */
{                 
  if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
  {
    while (!(U1LSR & 0x20));
      return (U1THR = ch);
  }
  else
  {
    while (!(U0LSR & 0x20));
      return (U0THR = ch);
  }
}


int getkey (void)  	  /* Read character from Serial Port   */
{                     
  if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
  {   
    while (!(U1LSR & 0x01));
	  return (U1RBR);
  }
  else
  {  
    while (!(U0LSR & 0x01));
      return (U0RBR);
  }
}
