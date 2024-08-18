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

byte fifo_size;

/**************************************************************************************************
  Function: rs232_full_init_uart(void)
  Description: Initialize the UART 1
  Input: None
  Output: None
**************************************************************************************************/
void rs232_full_init_uart(void)
{
  volatile byte junk; 
  
  /*-----------------------------------------------------------------------------------------------
  Enable TXD1(1:0), RXD1(3:2),   CTS1(5:4),  DCD1(7:6), 
         DSR1(9:8), DTR1(11:10), RI1(13:12), RTS1(15:14) on Port 2.
  -----------------------------------------------------------------------------------------------*/
  PINSEL4 = 0x0000AAAA;

  /* P0.0 is used to Turn ON/OFF SP3243 1 - ON, 0 - OFF   P0.0 High   */
  FIO0DIR0 = 0x20;                     /* P0.0 Output */
  FIO0CLR0 = 0x20;                     /* P0.0 Low  */
  FIO0MASK0= 0xdf;                     /* P0.0 Mask */
  FIO0SET0 = 0x20;                     /* P0.0 High */
    
  U1IER = 0;           		           /* Disable UART2 interrupts    */
    
  U1LCR = ((U1LCR & 0x7f) | 0x10);     /*  DLAB = 1:To write baudrate */
  U1LCR = 0x83;					       /* DLAB = 1 */

  switch(serial.baudrate)
  {
    case 1: U1FDR = 0x20; U1DLL = 0xA6; U1DLM = 0x0E; break;/* 300 bd */
    case 2: U1FDR = 0x20; U1DLL = 0x53; U1DLM = 0x07; break;/* 600 bd */
    case 3: U1FDR = 0x21; U1DLL = 0x71; U1DLM = 0x02; break;/*1200 bd */
    case 4: U1FDR = 0x41; U1DLL = 0x77; U1DLM = 0x01; break;/*2400 bd */
	case 5: U1FDR = 0x87; U1DLL = 0x7D; U1DLM = 0x00; break;/*4800 bd */
    case 6: U1FDR = 0xC7; U1DLL = 0x4A; U1DLM = 0x00; break;/*9600 bd */
    case 7: U1FDR = 0xC7; U1DLL = 0x25; U1DLM = 0x00; break;/*19200 bd*/
    case 8: U1FDR = 0xB3; U1DLL = 0x17; U1DLM = 0x00; break;/*38400 bd*/
    case 9: U1FDR = 0x97; U1DLL = 0x0B; U1DLM = 0x00; break;/*57600 bd*/
    case 10:U1FDR = 0x92; U1DLL = 0x08; U1DLM = 0x00; break;/*115200bd*/
    case 11:U1FDR = 0x92; U1DLL = 0x04; U1DLM = 0x00; break;/*230400bd*/
    case 12:U1FDR = 0x92; U1DLL = 0x02; U1DLM = 0x00; break;/*460800bd*/	  
	default:U1FDR = 0xC7; U1DLL = 0x4A; U1DLM = 0x00; break;/*9600 bd */
  }
        
  switch(serial.databits)    
  { 
    case 1: U1LCR = ((U1LCR & 0xfc) | 0x00); break; /* 5 data bits	*/
	case 2: U1LCR = ((U1LCR & 0xfc) | 0x01); break; /* 6 data bits	*/
	case 3: U1LCR = ((U1LCR & 0xfc) | 0x02); break; /* 7 data bits	*/
	case 4: U1LCR = ((U1LCR & 0xfc) | 0x03); break; /* 8 data bits    */
	default:U1LCR = ((U1LCR & 0xfc) | 0x03); break; /* 8 data bits	*/
  }
     
  switch(serial.parity)
  {
    case 1: U1LCR = ((U1LCR & 0xc7) | 0x00); break; /* No Parity      */
    case 2: U1LCR = ((U1LCR & 0xc7) | 0x18); break; /* Even Parity    */
    case 3: U1LCR = ((U1LCR & 0xc7) | 0x08); break; /* Odd Parity	    */
	default:U1LCR = ((U1LCR & 0xc7) | 0x00); break; /* No Parity      */  
  }
    
  switch(serial.stopbits)
  {
    case 1: U1LCR = ((U1LCR & 0xfb) | 0x00); break; /* 1 stop bit     */
    case 2: U1LCR = ((U1LCR & 0xfb) | 0x04); break; /* 2 stop bit	    */
    default:U1LCR = ((U1LCR & 0xfb) | 0x00); break; /* 1 stop bit 	*/
  }
    
  U1LCR = ((U1LCR & 0x7f) | 0x00);    /*  DLAB = 1:To write baudrate  */
	
  if(serial.flowcontrol == HARDWARE_FLOW_CONTROL)
  {
    //U1MCR = 0xC3;     /* this does not work */
	/* Set modem outputs also enables HW Flow Control */
	/* This value is tried and tested and it works, please dont change this */
	U1MCR = 0xC2;     
  }
  else
  {
    U1MCR = 0x03;  /* Set modem DTR and RTS High */
  }

  junk = U1LSR;
  junk = U1RBR;
       
  U1FCR = 0x87; /* Enable FIFO with receive interrupt after 8 char */
  serial.fifo_size = (U1IIR & 0xF0) == 0xC0 ? 16 : 1;
     
  VICVectAddr7 = (word32)uart1_handler; /* interrupt vector in slot7  */
  VICVectCntl7 = 7;                     /* use it for UART1 Interrupt */
  VICIntEnable = 0x00000080;            /* Enable UART1 Interrupt     */

  junk = U1IIR;
  U1IER = 0x05;          /* Enable receive and line status interrupts,
  Dont enable Transmit Buffer empty interrupt and also modem signal interrupts */
}

/* Write character to Serial Port    */
int rs232_full_sendchar (int ch)
{
  while (!(U1LSR & 0x20));
    //FIO1CLR3 = 0x40;
  return (U1THR = ch);

}
