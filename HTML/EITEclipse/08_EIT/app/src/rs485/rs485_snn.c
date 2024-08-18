/*------------------------------------------------------------------------
						NP_COMM.C

Description:	Perform the following action
				1. Configures external variables for Serial port
				2. Initialize the Serial port Buffers
				3. Initialize UART baudrate etc..
				4. Get Serial Data in
				5. Put Serial Date out
				6. Perform Flow Control

Author:			CMX Systems and Ajay Sharma

Rev:			0

History:		
	2007_11_25  1. serial_in() code merged into this file
				
				  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

int rs485_init_uart(void)
{
  volatile byte junk;

  PINSEL0 |= 0x00000050; /* Enable TXD0 and RXD0 on P0.2 and P0.3 lines */
      
  /*-----------------------------------------------------------------------------------------------
   P0.1 should always be low to enable the rs485 driver MAX4079     
     _RE is always Logic High to enable the chip ON 
  -----------------------------------------------------------------------------------------------*/
  //FIO0DIR0 = 0x02;                     /* P0.1 Output                 */
  //FIO0DIR0 = 0x13;                     /* P0.1 Output                 */
  //FIO0MASK0= 0xfd;                     /* P0.1 Mask                   */
  //FIO0CLR0 = 0x02;                     /* P0.1 Low                    */
  
  /*-----------------------------------------------------------------------------------------------
   P0.0(H/_F) decides full of half duplex of the MX4079 chip
  Half : P0.0 is High 
  full:  P0.0 is Low 

  Also : P0.4 decides the Data Enable line of MAX4079 chip.
  We need to make it:
  Half:  P0.4 is High
  Full:  P0.4 is Low
  -----------------------------------------------------------------------------------------------*/
  if(flag.rs485type == RS485_HDPLX)
  {
    FIO0DIR0 = 0x13;                     /* P0.0,P0.1,P0.4 Output                 */
    FIO0MASK0= 0xec;                     /* P0.0,P0.1,P0.4 Mask                   */
    if(flag.rs485txrx == 1) // MASTER
    {
      FIO0SET0 = 0x11;                     /* P0.0,P0.4 High                        */
      FIO0CLR0 = 0x02;                     /* P0.1 Low                              */
    }
    else if(flag.rs485txrx == 2) // Slave
    {
      FIO0SET0 = 0x01;                     /* P0.0,P0.4 High, RE = 0, H/F_ = 1, DE = 0 */
      FIO0CLR0 = 0x12;                     /* P0.1 RE_ = 0                              */
    }
    
  }
  else if(flag.rs485type == RS485_FDPLX)
  {
    FIO0DIR0 = 0x13;                     /* P0.0,P0.1,P0.4 Output                                */
    FIO0MASK0= 0xec;                     /* P0.0,P0.1,P0.4 Mask                                  */
    FIO0SET0 = 0x10;                     /* P0.4 High: DE = 1                            */
    FIO0CLR0 = 0x03;                     /* P0.0,P0.1 Low H/F_ = 0, RE_ = 0  */ 
  }
  
  U0IER = 0;           		       /* Disable UART2 interrupts      */
  U0LCR = ((U0LCR & 0x7f) | 0x10);   /*  DLAB = 1:To write baudrate */
  U0LCR = 0x83;					   // Cant tell why this is needed?

	switch(serial.baudrate)
    {
      case 1: U0FDR = 0x20; U0DLL = 0xA6; U0DLM = 0x0E; break;/* 300 bd */
      case 2: U0FDR = 0x20; U0DLL = 0x53; U0DLM = 0x07; break;/* 600 bd */
      case 3: U0FDR = 0x21; U0DLL = 0x71; U0DLM = 0x02; break;/*1200 bd */
      case 4: U0FDR = 0x41; U0DLL = 0x77; U0DLM = 0x01; break;/*2400 bd */
	  case 5: U0FDR = 0x87; U0DLL = 0x7D; U0DLM = 0x00; break;/*4800 bd */
      case 6: U0FDR = 0xC7; U0DLL = 0x4A; U0DLM = 0x00; break;/*9600 bd */
      case 7: U0FDR = 0xC7; U0DLL = 0x25; U0DLM = 0x00; break;/*19200 bd*/
      case 8: U0FDR = 0xB3; U0DLL = 0x17; U0DLM = 0x00; break;/*38400 bd*/
      case 9: U0FDR = 0x97; U0DLL = 0x0B; U0DLM = 0x00; break;/*57600 bd*/
      case 10:U0FDR = 0x92; U0DLL = 0x08; U0DLM = 0x00; break;/*115200bd*/
      case 11:U0FDR = 0x92; U0DLL = 0x04; U0DLM = 0x00; break;/*230400bd*/
      case 12:U0FDR = 0x92; U0DLL = 0x02; U0DLM = 0x00; break;/*460800bd*/	  
	  default:U0FDR = 0xC7; U0DLL = 0x4A; U0DLM = 0x00; break;/*9600 bd */
    }
        
    switch(serial.databits)    
    { 
	  case 1: U0LCR = ((U0LCR & 0xfc) | 0x00); break; /* 5 data bits	*/
	  case 2: U0LCR = ((U0LCR & 0xfc) | 0x01); break; /* 6 data bits	*/
	  case 3: U0LCR = ((U0LCR & 0xfc) | 0x02); break; /* 7 data bits	*/
	  case 4: U0LCR = ((U0LCR & 0xfc) | 0x03); break; /* 8 data bits    */
	  default:U0LCR = ((U0LCR & 0xfc) | 0x03); break; /* 8 data bits	*/
	}
     
	switch(serial.parity)
	{
	  case 1: U0LCR = ((U0LCR & 0xc7) | 0x00); break; /* No Parity      */
      case 2: U0LCR = ((U0LCR & 0xc7) | 0x18); break; /* Even Parity    */
      case 3: U0LCR = ((U0LCR & 0xc7) | 0x08); break; /* Odd Parity	    */
	  default:U0LCR = ((U0LCR & 0xc7) | 0x00); break; /* No Parity      */
	}
    
	switch(serial.stopbits)
	{
	  case 1: U0LCR = ((U0LCR & 0xfb) | 0x00); break; /* 1 stop bit     */
      case 2: U0LCR = ((U0LCR & 0xfb) | 0x04); break; /* 2 stop bit	    */
      default:U0LCR = ((U0LCR & 0xfb) | 0x00); break; /* 1 stop bit 	*/
	}
    
	U0LCR = ((U0LCR & 0x7f) | 0x00);    /*  DLAB = 1:To write baudrate  */
	junk = U0LSR;
    junk = U0RBR;
       
    /* Enable FIFO with receive interrupt after 8 char */
    U0FCR = 0x87;
    serial.fifo_size = (U0IIR & 0xF0) == 0xC0 ? 16 : 1;
    
    VICVectAddr6 = (word32)uart0_handler;/* Interrupt vector in slot28 */
    VICVectCntl6 = 6;                   /* use it for UART2 Interrupt */
    VICIntEnable = 0x00000040;            /* Enable UART2 Interrupt 	*/
    
    junk = U0IIR;
    U0IER = 0x05;         /* Enable receive and line status interrupts  */

  return 0;
}

