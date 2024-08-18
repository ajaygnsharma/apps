/********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

/**************************************************************
 This file contains processor specific timer and uart routines.
***************************************************************/

#include "micronet.h"
#include "np_includes.h"

/* global variables */
#if (MN_PPP || MN_SLIP)
#if (defined(MCHP_C18))
#pragma udata mn_port1
#endif
byte recv_buff[MN_UART_RECV_BUFF_SIZE];
#if (defined(MCHP_C18))
#pragma udata mn_port2
#endif
byte send_buff[MN_UART_XMIT_BUFF_SIZE];
#if (defined(MCHP_C18))
#pragma udata
#endif
#endif      /* (MN_PPP || MN_SLIP) */

#if (MN_ETHERNET)
#if (USE_RECV_BUFF)
#if (defined(MCHP_C18))
#pragma udata mn_port3
#endif
byte eth_recv_buff[MN_ETH_RECV_BUFF_SIZE];
#endif      /* (USE_RECV_BUFF) */
#if (USE_SEND_BUFF)
#if (defined(MCHP_C18))
#pragma udata mn_port4
#endif
#if (defined(POLARM9) || defined(CMXARM9))
#if (defined(__IAR_SYSTEMS_ICC__))
#pragma data_alignment=8
__no_init byte eth_send_buff[MN_ETH_XMIT_BUFF_SIZE] @ "EMAC_RAM";;
#elif (defined(__CC_ARM))
__align(8) byte eth_send_buff[MN_ETH_XMIT_BUFF_SIZE] __attribute__ ((section ("ETH_RAM")));
#endif
#else
#if (defined(POLARMSTR9) || defined(CMXARMSTR9) || \
   defined(POLSAM7SE512) || defined(CMXSAM7SE512) || \
   defined(POLSAM9263) || defined(CMXSAM9263) || \
   defined(POLSAM9260) || defined(CMXSAM9260))
#if (defined(__IAR_SYSTEMS_ICC__))
/* eth_send_buff must be aligned on a word boundary for STR91x and SAM7SE */
#pragma data_alignment=4
#endif
#endif
#ifdef __ghs__
#pragma alignvar(4)
#endif
#if ((defined(POLARMSTR9) || defined(CMXARMSTR9) || \
   defined(POLARMSTR9R) || defined(CMXARMSTR9R) || \
   defined(POLSAM7SE512) || defined(CMXSAM7SE512)) && \
   (defined(__GNUC__)  || defined(__CC_ARM)))
byte eth_send_buff[MN_ETH_XMIT_BUFF_SIZE+ALIGNMENT_BYTES];
#else
byte eth_send_buff[MN_ETH_XMIT_BUFF_SIZE];
#endif
#endif
#endif      /* (USE_SEND_BUFF) */
#if (defined(MCHP_C18))
#pragma udata
#endif
#endif      /* (!(MN_ETHERNET)) */

#if (RTOS_USED == RTOS_NONE || defined(CMXPIC18) || defined(CMXPIC18E) || \
   defined(CMXP18F97J60))
volatile byte xmit_busy;
#endif      /* (RTOS_USED == RTOS_NONE || defined(CMXPIC18) || defined(CMXPIC18E)) */

#if (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP)
#if 0 /* (defined(POLPIC18) || defined(CMXPIC18)) */
/* some PIC compilers can't handle 16-bit math in ISRs, workaround by
   using a union
*/
volatile BYTE2WORD_U timer;
#define  timer_tick timer.w
#else
volatile TIMER_TICK_T timer_tick;   /* Restart timer */
#endif
#endif      /* (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP) */

#if (KEEP_ERROR_COUNT)           /* Defined in intrface.h */
volatile word16 uart_errors;
#endif

/* Warning: enabling LOG_OUTPUT may cause problems with the UART transmit ISR
   on some processors.
*/
#define LOG_OUTPUT   0
void Nputchr(byte cc) cmx_reentrant;
#if ( LOG_OUTPUT && !(defined(POLPIC18) || defined(CMXPIC18) || \
   defined(POLPIC18E) || defined(CMXPIC18E)) )
#define OBUF_SIZE 250
char obuf[OBUF_SIZE];
static char *obuf_ptr = obuf;
static char * const obuf_max = &obuf[OBUF_SIZE-1];

#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif
#if (defined(POLECOG1) || defined(CMXECOG1) || defined(POLECOG1E) || defined(CMXECOG1E))
void __irq_code Nputchr(byte cc)
#else
void Nputchr(byte cc)
#endif
cmx_reentrant {
   if (obuf_ptr > obuf_max)
      obuf_ptr = obuf;

   *obuf_ptr++ = cc;
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif
#endif      /* LOG_OUTPUT */

#if (MN_PPP || MN_SLIP)
#if (RTOS_USED != RTOS_NONE)
   /* If we have a frame character send a signal to the PPP module
      if the PPP layer is not up, otherwise send a signal to the IP layer.
   */
#if (MN_PPP)
#define SEND_RECEIVE_SIGNAL(C) { \
   if ((C) == PPP_FRAME) { \
      if (!ppp_status.up) { MN_ISR_SIGNAL_POST(SIGNAL_PPP); } \
      else { MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE); } } \
}

#define SEND_BUFF_FULL_SIGNAL { \
   if (ppp_status.up) { MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE); } \
}

#elif (MN_SLIP)
#define SEND_RECEIVE_SIGNAL(C) { \
   if ((C) == SLIP_END) { MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE); } \
}

#define SEND_BUFF_FULL_SIGNAL    { MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE); }
#endif
#endif      /* (RTOS_USED != RTOS_NONE) */
#endif      /* (MN_PPP || MN_SLIP) */

/* ----------------------------------------------------------------------- */

#if (MN_PPP || MN_SLIP)
#if (!(defined(POLAVR) || defined(CMXAVR) || defined(POLAVRE) || \
      defined(CMXAVRE) || defined(POLAVRR) || defined(CMXAVRR) || \
      defined(POLLM3Sx9xx) || defined(CMXLM3Sx9xx) || \
      defined(POLARMSTR7) || defined(CMXARMSTR7)))
static byte mn_uart_get_xmit_byte(byte *) cmx_reentrant;
#endif      /* (!(defined(POLAVR) ... */
static void mn_uart_put_recv_byte(byte) cmx_reentrant;
#endif      /* (MN_PPP || MN_SLIP) */

/* Initialize receive buffer pointers. */
void init_recv(SCHAR interface_no)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;
   interface_ptr = MK_INTERFACE_PTR(interface_no);

#if (!(USE_RECV_BUFF))
   if (interface_ptr->interface_type == IF_ETHER)
      {
      eth_init_recv(interface_no);
      return;
      }
#endif      /* (!(USE_RECV_BUFF)) */

   interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr;
   interface_ptr->recv_out_ptr = interface_ptr->recv_buff_ptr;
   interface_ptr->recv_count = 0;
}

/* waits up to MN_SOCKET_WAIT_TICKS for the previous transmit to be done,
   i.e. xmit_busy = 0. returns 1 if it is ok to start the next packet or
   0 if not ok. Resets send_in_ptr and send_out_ptr if ok.

   If MN_SOCKET_WAIT_TICKS is set at the default 6 seconds then returning 0
   is almost certainly a fatal error. Even the slowest serial link should
   be able to transmit a packet in less than 6 seconds. If MN_SOCKET_WAIT_TICKS
   is set for a smaller value than the default then returning 0 may only mean
   that the packet is not done sending yet.
*/
byte mn_transmit_ready(SCHAR interface_no)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;
#if (RTOS_USED == RTOS_NONE)
   byte xmit_busy_val;
   TIMER_INFO_T wait_timer;
#endif

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES) || \
         !(interface_info[interface_no].interface_status & IF_OPEN))
      return (0);

#if (RTOS_USED == RTOS_NONE)
#if 1
   /* This loop can be removed if you don't want to block */
   if (xmit_busy)
      {
      mn_reset_timer(&wait_timer,(MN_SOCKET_WAIT_TICKS));
      while ( !mn_timer_expired(&wait_timer) )
         {
         if (!xmit_busy)
            break;
         }
      }
#endif

   DISABLE_INTERRUPTS;
   xmit_busy_val = xmit_busy;
   ENABLE_INTERRUPTS;

   if (!xmit_busy_val)
      {
      send_interface_no = interface_no;
      interface_ptr = MK_INTERFACE_PTR(interface_no);
      interface_ptr->send_in_ptr = interface_ptr->send_buff_ptr;
      interface_ptr->send_out_ptr = interface_ptr->send_buff_ptr;
      }

   return (byte)(!xmit_busy_val);
#else
#if 1
   /* wait for signal */
   if (MN_SIGNAL_WAIT(SIGNAL_TRANSMIT,(MN_SOCKET_WAIT_TICKS)) == SIGNAL_SUCCESS)
#else
   /* check for signal */
   if (MN_SIGNAL_GET(SIGNAL_TRANSMIT) == SIGNAL_SUCCESS)
#endif
      {
      send_interface_no = interface_no;
      interface_ptr = MK_INTERFACE_PTR(interface_no);
      interface_ptr->send_in_ptr = interface_ptr->send_buff_ptr;
      interface_ptr->send_out_ptr = interface_ptr->send_buff_ptr;
      return (1);
      }
   return (0);
#endif      /* (RTOS_USED == RTOS_NONE) */

}

#if (MN_PPP || MN_SLIP)
#if defined(__C51__)    /* Keil 8051 */
#pragma NOAREGS
#endif
#if (!(defined(POLAVR) || defined(CMXAVR) || defined(POLAVRE) || \
      defined(CMXAVRE) || defined(POLAVRR) || defined(CMXAVRR) || \
      defined(POLLM3Sx9xx) || defined(CMXLM3Sx9xx) || \
      defined(POLARMSTR7) || defined(CMXARMSTR7)))
/* Gets a byte to transmit from the xmit buffer. Returns 1 if there was
   a byte, returns 0 otherwise.
*/
#if (defined(POLECOG1) || defined(CMXECOG1) || defined(POLECOG1E) || defined(CMXECOG1E))
static byte __irq_code mn_uart_get_xmit_byte(byte *out_ptr)
#else
static byte mn_uart_get_xmit_byte(byte *out_ptr)
#endif
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;
   byte retval;

   retval = 0;

   interface_ptr = MK_INTERFACE_PTR(uart_interface_no);

   if (interface_ptr->send_out_ptr < interface_ptr->send_in_ptr)
      {
      *out_ptr = *(interface_ptr->send_out_ptr);
      interface_ptr->send_out_ptr++;
#if (LOG_OUTPUT)
      Nputchr(*out_ptr);
#endif
      retval = 1;
      }
   else
      {
      MN_XMIT_BUSY_CLEAR;
      MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
      }

   return (retval);
}
#endif      /* (!(defined(POLAVR) ... */

/* puts a byte into the recv buffer. */
#if (defined(POLECOG1) || defined(CMXECOG1) || defined(POLECOG1E) || defined(CMXECOG1E))
static void __irq_code mn_uart_put_recv_byte(byte c)
#else
static void mn_uart_put_recv_byte(byte c)
#endif
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(uart_interface_no);

   if (interface_ptr->recv_count < (interface_ptr->recv_buff_size-1))
      {
      *(interface_ptr->recv_in_ptr) = c;
      interface_ptr->recv_in_ptr++;
      if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr)
         interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr;
      interface_ptr->recv_count++;
#if (RTOS_USED != RTOS_NONE)
      SEND_RECEIVE_SIGNAL(c);
#endif      /* (RTOS_USED != RTOS_NONE) */
      }
   else
      {
#if (KEEP_ERROR_COUNT)
      uart_errors++;
#endif
#if (RTOS_USED != RTOS_NONE)
      SEND_BUFF_FULL_SIGNAL;
#endif      /* (RTOS_USED != RTOS_NONE) */
      }
}
#if defined(__C51__)    /* Keil 8051 */
#pragma AREGS
#endif
#endif      /* (MN_PPP || MN_SLIP) */


/*=================================================================================================
 ----------------------------------------------------------------------- */

#if (defined(POLARM2368) || defined(CMXARM2368) || \
   defined(POLARM2378) || defined(CMXARM2378) || \
   defined(POLARM2468) || defined(CMXARM2468))
#if (defined(__CC_ARM) || defined(__GNUC__) || defined(__IAR_SYSTEMS_ICC__))
void timer_isr(void) cmx_reentrant;

#if (MN_PPP || MN_SLIP)
static byte fifo_size;

void uart_handler(void) cmx_reentrant;
void start_xmit(void);
static void xmit(void) cmx_reentrant;

void mn_uart_init(void)
cmx_reentrant {
   volatile byte junk;

   DISABLE_INTERRUPTS;
#if (defined(POLARM2468) || defined(CMXARM2468))
   PINSEL7 |= 0x0000000F;	            /* P3.16 TXD1, P3.17 RXD1 */
#else
   PINSEL0 |= 0x40000000;              /* Enable TxD1                       */
   PINSEL1 |= 0x00000001;              /* Enable RxD1                       */
#endif
   U1IER = 0;                          /* Disable UART1 interrupts */
   U1FDR    = 0;                       /* Fractional divider not used       */
   U1LCR = 0x83;              /* 8 bits, no Parity, 1 Stop bit, DLAB = 1 */
   /* Baud Rates @ 12.0MHz VPB Clock: 38400 = 19, 19200 = 39, 9600 = 78 */
   U1DLL = 19;
   U1DLM = 0;
   U1LCR = 0x03;              /* DLAB = 0 */
   /* Clear existing interrupts, for UART1 may need to read U1MSR also. */
   junk = U1LSR;
   junk = U1RBR;
   /* Enable FIFO with receive interrupt after 8 char */
   U1FCR = 0x87;
   fifo_size = (U1IIR & 0xF0) == 0xC0 ? 16 : 1;

   VICVectAddr7 = (word32)uart_handler; /* set interrupt vector in slot 7 */
   VICVectCntl7 = 7;                   /* use it for UART1 Interrupt */
   VICIntEnable = 0x00000080;          /* Enable UART1 Interrupt */

   junk = U1IIR;
   U1IER = 0x05;              /* Enable receive and line status interrupts */
   U1MCR = 0x03;              /* Set modem outputs */

   ENABLE_INTERRUPTS;
}

void start_xmit(void)
{
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(uart_interface_no);

   DISABLE_INTERRUPTS;

   U1THR = *(interface_ptr->send_out_ptr);  /* send first char */

#if (LOG_OUTPUT)
   Nputchr(*(interface_ptr->send_out_ptr));
#endif
   interface_ptr->send_out_ptr++;

   U1IER |= 0x02;
   ENABLE_INTERRUPTS;
}

void uart_handler(void)
cmx_reentrant {
   volatile byte inter;
   volatile byte status;
   byte c;
#if 0
   byte need_xmit;

   need_xmit = FALSE;
#endif

   while ( !( (inter = U1IIR) & 0x01) )
      {
      switch (inter & 0x0F)
         {
         case 0x0C:                 /* character timeout */
         case 0x04:                 /* recv */
            status = U1LSR;
            if (status & 0x01)      /* data ready */
               {
               c = U1RBR;
               mn_uart_put_recv_byte(c);
               }
#if 0
            if (status & 0x20)      /* THR empty */
               need_xmit = TRUE;
#endif
            break;

         case 0x02:                 /* xmit */
            xmit();
            break;

         case 0x06:                 /* line status */
            status = U1LSR;
#if (KEEP_ERROR_COUNT)
            if (status & 0x0e)
               uart_errors++;
#endif
            break;

         case 0x00:                 /* modem status */
            status = U1MSR;
            /* Add code here to handle modem. */
            break;

         default:
            break;
         }
      }

#if 0
   if (need_xmit)    /* we got a recv and xmit interrupt at the same time */
      xmit();
#endif
}

static void xmit(void)
cmx_reentrant {
   byte num;
   byte c;

   if (U1LSR & 0x20)    /* THR empty */
      {
      num = fifo_size;
      while (num)
         {
         num--;
         if (mn_uart_get_xmit_byte(&c))
            {
            U1THR = c;
            }
         else
            {
            U1IER &= (~0x02);
            break;
            }
         }
      }
}
#endif      /* (MN_PPP || MN_SLIP) */

/**************************************************************************************************
 timer_isr(void)
 description: isr for the timer and happens once every 10 m sec
 input: none
 output: none
**************************************************************************************************/
void timer_isr(void)
cmx_reentrant 
{
   byte ip;
   U32 i;

   ip = 0;

   MN_TICK_UPDATE;

#if (RTOS_USED == RTOS_NONE)
#if MN_DHCP
   mn_dhcp_update_timer();
#endif      /* MN_DHCP */
#if (MN_ARP && MN_ARP_TIMEOUT)
   mn_arp_update_timer();
#endif      /* (MN_ARP && MN_ARP_TIMEOUT) */
#if (MN_IGMP)
   mn_igmp_update_timers();
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
   mn_snmp_tick();
#endif      /* (MN_SNMP) */
#if (MN_SOCKET_INACTIVITY_TIME)
   mn_update_inactivity_timers();
#endif      /* (MN_SOCKET_INACTIVITY_TIME) */
#else    /* RTOS used */
#if (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME)
   MN_ISR_SIGNAL_POST(SIGNAL_TIMER_UPDATE);
#endif      /* (MN_DHCP || (MN_ARP && MN_ARP_TIMEOUT) || MN_SOCKET_INACTIVITY_TIME) */
#if (MN_IGMP)
   MN_ISR_SIGNAL_POST(SIGNAL_IGMP);
#endif      /* (MN_IGMP) */
#if (MN_SNMP)
   MN_ISR_SIGNAL_POST(SIGNAL_SNMP);
#endif      /* (MN_SNMP) */
#endif      /* (RTOS_USED == RTOS_NONE) */

  if(flag.cio == ENABLED)
  {
    /* Only 3 GPIO are available and the first two are used */
    if((iserver.model == EIT_D_4) || (iserver.model == EIT_PCB_2F) || (iserver.model == EIT_PCB_T))
	{
	  io_dir(2, 6, INPUT);
  	  io_dir(2,3, OUTPUT);
	  
	  ip = test_bit(2,6);
	  
	  if(gpio.if_condition[0] == gpio.then_condition[0])
      {
        if(ip)
	    {
      	  io_set(2,3, LOGIC_HIGH);
	    }
	    else
        {
		  io_set(2,3, LOGIC_LOW);
		}
      }
	}
	else if(iserver.model == EIT_W_4)
    {
	  io_dir(2, 6, INPUT);
  	  io_dir(2, 3, INPUT);
  	  io_dir(2, 5, OUTPUT);
  	  io_dir(2, 4, OUTPUT);
  	  
	  if(gpio.if_condition[0] == gpio.then_condition[0])
	  {
	    ip = test_bit(2,6);
	  	if(ip)
	    {
      	  io_set(2,5, LOGIC_HIGH);
	    }
	    else
        {
		  io_set(2,5, LOGIC_LOW);
		}
	  }

	  if(gpio.if_condition[1] == gpio.then_condition[1])
	  {
	    ip = test_bit(2,3);
	  	if(ip)
	    {
      	  io_set(2,4, LOGIC_HIGH);
	    }
	    else
        {
		  io_set(2,4, LOGIC_LOW);
		}
	  }
	}
	else if(iserver.model == EIT_PCB_4)
	{
	  io_dir(2, 6, INPUT);
  	  io_dir(2, 3, INPUT);
  	  io_dir(2, 5, INPUT);
  	  
	  io_dir(2, 4, OUTPUT);
  	  io_dir(2, 2, OUTPUT);
  	  io_dir(2, 7, OUTPUT);
  	  
	  if(gpio.if_condition[0] == gpio.then_condition[0])
	  {
	    ip = test_bit(2,6);
	  	if(ip)
	    {
      	  io_set(2,4, LOGIC_HIGH);
	    }
	    else
        {
		  io_set(2,4, LOGIC_LOW);
		}
	  }

	  if(gpio.if_condition[1] == gpio.then_condition[1])
	  {
	    ip = test_bit(2,3);
	  	if(ip)
	    {
      	  io_set(2,2, LOGIC_HIGH);
	    }
	    else
        {
		  io_set(2,2, LOGIC_LOW);
		}
	  }
	  
	  if(gpio.if_condition[2] == gpio.then_condition[2])
	  {
	    ip = test_bit(2,5);
	  	if(ip)
	    {
      	  io_set(2,7, LOGIC_HIGH);
	    }
	    else
        {
		  io_set(2,7, LOGIC_LOW);
		}
	  }
	}
  }

  if(uib_count)
  {
    FIO1PIN3  = 0x7f;  // turn the LED ON
  }
  else
  {
    FIO1PIN3  |= 0x80;
  }

  for(i = 0; i < 5; i++)
  {
    if(alarm[i].enable == TRUE)
    {
	  alarm[i].reminder_interval_counter++;
	  if(alarm[i].reminder_interval_counter == (alarm[i].reminder_interval * 100))
	  {
	    flag.isvr_disconnect = TRUE;
	   flag.isvr_condition  = TRUE;
	  }     
    }
  }

  T0IR = 1;                      /* Clear interrupt flag */
}

void mn_timer_init(void)
cmx_reentrant {
   DISABLE_INTERRUPTS;
   /* The following assumes a 48 Mhz clock. */
   //T0MR0 = 119999;                     /* 10mSec = 120,000-1 counts */
   T0MR0 = 179999;                     /* 10mSec = 180,000-1 counts */ //Ajay
   T0MCR = 3;                          /* Interrupt and Reset on MR0 */
   T0TCR = 1;                          /* Timer0 Enable */
   VICVectAddr4 = (word32)timer_isr;   /* set interrupt vector in 4 */
   VICVectCntl4 = 4;                   /* use it for Timer 0 Interrupt */
   VICIntEnable = 0x00000010;          /* Enable Timer0 Interrupt */

#if (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP)
   timer_tick = 0;
#endif      /* (RTOS_USED == RTOS_NONE || RTOS_USED == RTOS_CMX_TINYP) */
   ENABLE_INTERRUPTS;
}
#endif      /* (defined(__CA__) || defined(__GNUC__) || defined(__IAR_SYSTEMS_ICC__)) */
#endif      /* (defined(POLARM2368) || defined(CMXARM2368)) */

