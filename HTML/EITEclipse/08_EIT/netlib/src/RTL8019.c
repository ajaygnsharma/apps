/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
/* driver for RealTek 8019AS chip  */

#include "micronet.h"

#if (MN_ETHERNET)

/*
   Porting Notes:

   1. Make sure that mnconfig.h is configured properly for either ISR
      driven ethernet or MN_POLLED_ETHERNET as required.

   2. Make sure the ETHER_POLL_RECV, ETHER_INIT and ETHER_SEND(p,n) in
      ethernet.h are defined properly.

      #define ETHER_POLL_RECV       rtl8019_recv()
      #define ETHER_INIT            rtl8019_init()
      #define ETHER_SEND(p,n)       rtl8019_send((p),(n))

   3. Set the variable base below to point to the base address for the chip
      if memory mapped mode is used. This variable is not used in IO mode.
      The AVR port is set for a base address of 0x8300, but this may be
      different for other hardware.

   4. Setup the nic_read and nic_write macros in rtlregs.h to use the base
      defined in step 3.

   5. Make sure the NIC_STOP_PAGE, NIC_TX_PAGES and NIC_TX_BUFFERS macros
      below are defined properly for your hardware.

   6. Add code at the start of rtl8019_init to do a hardware reset.

   7. If ISR driven ethernet is used, add code to the end of the rtl8019_init
      function to enable the ISR.

   8. Change the function declaration for the ISR function EtherInt to
      the declaration required for your hardware and C compiler. Every
      compiler does this differently so consult your compiler manual if
      necessary. This step is not needed if using MN_POLLED_ETHERNET.

*/

#include "rtlregs.h"

#if (!(USE_SEND_BUFF))
#error USE_SEND_BUFF must be set to 1 for this driver.
#endif      /* (!(USE_SEND_BUFF)) */

#if (!(USE_RECV_BUFF) && (!MN_POLLED_ETHERNET))
#error USE_RECV_BUFF must be set to 1 for this driver.
#endif      /* (!(USE_RECV_BUFF)) */

#if (MN_USE_HW_CHKSUM)
#error MN_USE_HW_CHKSUM not supported by this driver.
#endif

#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))
#if (!MN_POLLED_ETHERNET)
#error Only polled ethernet is supported for the PIC version of this driver.
#endif      /* (!MN_POLLED_ETHERNET) */
#define DO_DEBUG        0
#endif      /* (defined(POLPIC18E) || defined(CMXPIC18E)) */

#if (defined(POLAVRR) || defined(CMXAVRR))
#define DO_DEBUG        0     /* set to 1 to send debug info to UART */
#if DO_DEBUG
#include <pgmspace.h>
#include "support.h"
#endif      /* DO_DEBUG */
#endif      /* (defined(POLAVRR) || defined(CMXAVRR)) */

#if (defined(POLST7E) || defined(CMXST7E))
static void delay(word32 dly);
#define DO_DEBUG        0     /* set to 1 to print debug info */
#endif      /* (defined(POLST7E) || defined(CMXST7E)) */

#if (defined(POLEZ8E) || defined(CMXEZ8E))
#define DO_DEBUG        0

#if (!MN_POLLED_ETHERNET)
void EtherInt(void);
#endif      /* (!MN_POLLED_ETHERNET) */

#endif      /* (defined(POLEZ8E) || defined(CMXEZ8E)) */

#if (defined(POLDSPICE) || defined(CMXDSPICE))
#define DO_DEBUG        0
#endif      /* (defined(POLDSPICE) || defined(CMXDSPICE)) */

#if DO_DEBUG
#include <stdio.h>            
char Str[25];
void DiagSend(char *Str);
static cmx_const char str0[] = "In Init\r\n";
static cmx_const char str1[] = "Init Ok\r\n";
static cmx_const char str2[] = "Write Ok\r\n";
static cmx_const char str3[] = "No Recv\r\n";
static cmx_const char str4[] = "No Room\r\n";
static cmx_const char str5[] = "Recv OK\r\n";
#if (!MN_POLLED_ETHERNET)
static cmx_const char str6[] = "Write Done\r\n";
#else
static cmx_const char str7[] = "No Pkt\r\n";
#endif      /* (!MN_POLLED_ETHERNET) */
static cmx_const char str8[] = "Write err\r\n";
static cmx_const char str9[] = "No data\r\n";
static cmx_const char str10[] = "dma err\r\n";
static cmx_const char str11[] = "In Recv\r\n";
static cmx_const char str12[] = "In Send\r\n";
#if (!MN_POLLED_ETHERNET)
static cmx_const char str13[] = "In ISR\r\n";
#endif      /* (!MN_POLLED_ETHERNET) */
#endif      /* DO_DEBUG */

/* Set the base address of the chip here. In rtlregs.h set up the nic_read
   and nic_write macros to use base.
*/
#if (defined(POLAVRR) || defined(CMXAVRR))
static byte *base = (byte *)0x8300;
#elif (defined(POLDSPICE) || defined(CMXDSPICE))
/* base not used, see rtlregs.h for nic_read and nic_write macros. */
#elif (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLST7E) || \
   defined(CMXST7E) || defined(POLEZ8E) || defined(CMXEZ8E))
/* base not used, see rtlregs.h for nic_read and nic_write macros. */
#else
#error need to set base address of NIC
#endif

#define MIN_PKT_SIZE    60

/* enable xmit, xmit error and receive interrupts */
#define NIC_ISRS        (NIC_IMR_PRXE | NIC_IMR_PTXE | NIC_IMR_TXEE)

/*
 * Size of a single ring buffer page.
 */
#define NIC_PAGE_SIZE   256

/*
 * First ring buffer page address.
 */
#define NIC_START_PAGE  0x40

/*
 * Last ring buffer page address plus 1.
 */
#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))
#define NIC_STOP_PAGE   0x5F
#else
#define NIC_STOP_PAGE   0x80
#endif

/*
 * Number of pages in a single transmit buffer.
 *
 * This should be at least the MTU size.
 */
#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))
#define NIC_TX_PAGES    4
#else
#define NIC_TX_PAGES    6
#endif

/*
 * Number of transmit buffers.
 */
#if 1    /* (defined(POLPIC18E) || defined(CMXPIC18E)) */
#define NIC_TX_BUFFERS  1
#else
#define NIC_TX_BUFFERS  2
#endif

/*
 * Standard sizing information
 */
#define TX_PAGES           (NIC_TX_BUFFERS * NIC_TX_PAGES)

#define NIC_FIRST_TX_PAGE  (NIC_START_PAGE)
#define NIC_FIRST_RX_PAGE  (NIC_FIRST_TX_PAGE + TX_PAGES)

/*
 * Realtek packet header.
 */
typedef struct nic_pkt_header_s {
   byte ph_status;      /* Status, contents of RSR register */
   byte ph_nextpg;      /* Page for next packet */
   word16 ph_size;      /* Size of header and packet in octets */
} NIC_PKT_HEADER_T;

/*
 * Controller memory layout:
 *
 * 0x4000 - 0x47ff  2k bytes unused?
 * 0x4800 - 0x4bff  1k bytes transmit buffer
 * 0x4c00 - 0x7fff  13k bytes receive buffer
 */
static byte send_start_pg;  /* 0x40 */
static byte ring_start_pg;  /* 0x4c */
static byte ring_stop_pg;   /* 0x80 */

static void dma_complete(void) cmx_reentrant;
static int recv_pkt(void) cmx_reentrant;

#if (MN_ALLOW_MULTICAST)
#define RCR_SETTING     (NIC_RCR_AB|NIC_RCR_PRO)
#else
#define RCR_SETTING     (NIC_RCR_AB)
#endif

#define USE_RECV_MACRO  1  /* set to 1 to use inline code for put_recv_byte */

#if (USE_RECV_MACRO)
#define PUT_RECV_BYTE(A)         { *(interface_ptr->recv_in_ptr) = (A); \
   interface_ptr->recv_in_ptr++; \
   if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr) \
      interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr; }
#else
static void put_recv_byte(byte c);
#define PUT_RECV_BYTE(A)         put_recv_byte(A)

static void put_recv_byte(byte c)
{
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);
   *(interface_ptr->recv_in_ptr) = c;
   interface_ptr->recv_in_ptr++;
   if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr)
      interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr;
}
#endif      /* (USE_RECV_MACRO) */

/* ----------------------------------------------------------------------- */

/* initialize rtl8019 */
int rtl8019_init(void)
cmx_reentrant {
    byte i;

   /* first do a hardware reset of the chip */
#if (defined(POLAVRR) || defined(CMXAVRR))

    /*
     * The hardware reset pin of the nic is connected
     * to bit 4 on port e. Make this pin an output pin,
     * set it to high for 200 msecs and delay 1 sec.
     */
   DDRE |= 0x10;
   PORTE |= 0x10;
   MN_TASK_WAIT((MN_ONE_SECOND)/5);
   PORTE &= (~0x10);
   MN_TASK_WAIT(MN_ONE_SECOND);

#if (DO_DEBUG)
   UCR = 0x18;                      /* Set up Diagnostic Port */
   UBRR = 5;                        /* 5 = (38400 Baud) at 3.6864 Mhz */

   strcpy_P(Str,str0);     /* In Init */
   DiagSend(Str);
#endif      /* (DO_DEBUG) */
#endif      /* (defined(POLAVRR) || defined(CMXAVRR)) */

#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))
   /*
   * Setup for PORTA.RA0 as analog input while rests
   * as digital i/o lines.
   */ 
/*   ADCON1  = 0b10001110;  */     /* RA0 as analog input, Right justified */
   ADCON1  = 0x8E;       /* RA0 as analog input, Right justified */
   TRISA   = 0x03;
         
   /*
    * LCD is enabled using RA5.
    */ 
   PORTA_RA5 = 0;          /* Disable LCD. */
            
   /*
    * Turn off the LED's.
    */ 
   LATA2 = 1;
   LATA3 = 1;
                  
   /*
    * External data EEPROM needs pull-ups, so enable internal
    * pull-ups.
    */ 
   INTCON2_RBPU = 0;

   SET_NIC_READ();
   WRITE_NIC_ADDR(0);
   INTCON2_RBPU = 0;
   NIC_IOW_IO = 1;
   NIC_IOR_IO = 1;
   NIC_RESET_IO = 1;
   NIC_CTRL_TRIS = 0x00;
   NIC_RESET_IO = 0;
   nic_write(NIC_RESET, nic_read(NIC_RESET));
   MN_TASK_WAIT(2);
#endif      /* (defined(POLPIC18E) || defined(CMXPIC18E)) */

#if (defined(POLST7E) || defined(CMXST7E))
   FS.DR_state = (byte)(FS.DR_state | 0x04);   /* Force RSTDRV high */
   PFDR = FS.DR_state;
/*   MN_TASK_WAIT(1); */
   delay(1);
   FS.DR_state = (byte)(FS.DR_state & 0xfb);   /* Force RSTDRV low */
   PFDR = FS.DR_state;
/*   MN_TASK_WAIT(2); */
   delay(50U);    

#if (DO_DEBUG)
   SCICR2 = 0;                /* disable uart */
   SCICR1 = 0x00;             /* 1 start bit, 8 data bits, 1 stop bit */
   SCIBRR = 0xD2;             /* 9600 */
   SCIETPR = 0;               /* extended prescalers not used */
   SCIERPR = 0;
   SCICR2 = 0x0c;             /* TE (TDO enable) and RE (RDI enable) */

   mn_strcpy_cb(Str,str0);     /* In Init */
   DiagSend(Str);
#endif      /* (DO_DEBUG) */
#endif      /* (defined(POLST7E) || defined(CMXST7E)) */

#if (defined(POLEZ8E) || defined(CMXEZ8E))
   PGOUT = 0xFF;

   /* set address port direction to output */
   PGADDR = DATA_DIR;
   PGCTL  = 0x80;       /* PG0->PG4=A0->A4, PG5=/IORB, PG6=/IOWB */

   /* disable alternate function for port G */
   PGADDR = ALT_FUN;
   PGCTL  = 0x00;

   /* set output for port G */
   PGADDR = OUT_CTL;
   PGCTL  = 0x00;

   /* enable high-drive for port G */
   PGADDR = HDR_EN;
   PGCTL  = 0xFF;

   /* disable stop mode recovery for port G */
   PGADDR = SMRS_EN;
   PGCTL  = 0x00;

#endif      /* (defined(POLEZ8E) || defined(CMXEZ8E)) */

#if (defined(POLDSPICE) || defined(CMXDSPICE))
   unsigned int delay;
   delay = 0x0FFF;

/* initialize dsPIC30F pin state/direction */

/* next 6 instructions to ensure SRAM is deselected */
   ADPCFGbits.PCFG9 = 1;
   ADPCFGbits.PCFG10 = 1;
   LATBbits.LATB9 = 1;
   LATBbits.LATB10 = 1;
   TRISBbits.TRISB9 = 0;
   TRISBbits.TRISB10 = 0;
      
/* set control pins as digital */
   ADPCFGbits.PCFG15 = 1;            /* ensure AN15/RB15 is digital */
   ADPCFGbits.PCFG14 = 1;            /* ensure AN14/RB14 is digital */
   ADPCFGbits.PCFG13 = 1;            /* ensure AN13/RB13 is digital */
   ADPCFGbits.PCFG12 = 1;            /* ensure AN12/RB12 is digital */

/* set initial control pin drive states       */
   LATBbits.LATB15 = 1;             /* assert RSTDRV signal  */
   LATBbits.LATB14 = 1;             /* ensure *IOWR power-on state is high */
   LATBbits.LATB13 = 1;             /* ensure *IORD power-on state is high */
   LATBbits.LATB12 = 1;             /* ensure *ALE power-on state is high  */
                        /* (ALE signal used for '573 LE and AEN on 8019AS) */
      
/* set pin direction set here */
   TRISAbits.TRISA15 = 1;        /* ensure *MAC_INT pin is input on dsPIC */
   TRISBbits.TRISB15 = 0;           /* set pin (RSTDRV) as output */
   TRISBbits.TRISB14 = 0;           /* set pin (IOWR) as output */
   TRISBbits.TRISB13 = 0;           /* set pin (IORD) as output */
   TRISBbits.TRISB12 = 0;           /* set pin (ALE) as output */

   LATD = 0x0000;                   /* set initial Port state */
   TRISD = 0x0000;                  /* set PortD as outputs */

#if (!MN_POLLED_ETHERNET)
   /* configure the INT4 pin ( for *MAC_INT output from Realtek ) */
   INTCON2bits.INT4EP = 0;          /* set to interrupt on positive edge */
   IFS2bits.INT4IF = 0;             /* clear interrupt flag */
   IPC9bits.INT4IP = 4;             /* set interrupt priority to 4 */
   IEC2bits.INT4IE = 1;             /* enable interrupt */
#endif          /* (!MN_POLLED_ETHERNET) */

   LATBbits.LATB15 = 0;             /* de-assert RSTDRV reset signal  */
   while ( delay -- );              /* 552uS @ 7.3728MIPS */
#endif      /* (defined(POLDSPICE) || defined(CMXDSPICE)) */

   i = 0;
    /*
     * If the hardware reset didn't set the nic in reset
     * state, perform an additional software reset and
     * wait until the controller enters the reset state.
     */
   if ( (nic_read(NIC_PG0_ISR) & NIC_ISR_RST) == 0)
      {
      nic_write(NIC_RESET, nic_read(NIC_RESET));
      MN_TASK_WAIT((MN_ONE_SECOND)/5);
      for(i = 0; i < 255; i++)
         if (nic_read(NIC_PG0_ISR) & NIC_ISR_RST)
            break;
      }

   if ( i == 255)
      return ETHER_INIT_ERROR;

    /*
     * Mask all interrupts and clear any interrupt
     * status flag to set the INT pin back to low.
     */
    nic_write(NIC_PG0_IMR, 0);
    nic_write(NIC_PG0_ISR, 0xff);

#if (!(defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E)))
    /*
     * During reset the nic loaded its initial configuration
     * from an external eeprom. On the ethernut board we
     * do not have any configuration eeprom, but simply tied
     * the eeprom data line to high level. So we have to clear
     * some bits in the configuration register. Switch to
     * register page 3.
     */
   nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1);

    /*
     * The nic configuration registers are write protected
     * unless both EEM bits are set to 1.
     */
    nic_write(NIC_PG3_EECR, NIC_EECR_EEM0 | NIC_EECR_EEM1);

    /*
     * Set network media auto detect. We don't force
     * 10BaseT, because this would disable the link
     * test too.
     */
    nic_write(NIC_PG3_CONFIG2, NIC_CONFIG2_BSELB);

    /*
     * Set full duplex mode. Something doesn't work here,
     * because the switch connected to the Ethernut board
     * still indicates a half duplex link.
     */
    nic_write(NIC_PG3_CONFIG3, NIC_CONFIG3_FUDUP);

    /*
     * Reenable write protection of the nic
     * configuration registers.
     */
    nic_write(NIC_PG3_EECR, 0);

#endif      /* (!(defined(POLPIC18E) || defined(CMXPIC18E))) */

    /*
     * Switch to register page 0. Keep nic in
     * stop mode.
     */
   nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2);

     /*
     * Set data configuration register:
     * Byte-wide DMA transfers.
     * Normal operation (no loopback).
     * Send command not executed.
     * 8 byte fifo threshold.
     */
    nic_write(NIC_PG0_DCR, NIC_DCR_LS | NIC_DCR_FT1);
 
    /*
     * Clear remote dma byte count register.
     */
    nic_write(NIC_PG0_RBCR0, 0);
    nic_write(NIC_PG0_RBCR1, 0);

    /*
     * Temporarily set receiver to monitor mode and
     * transmitter to internal loopback mode. Incoming
     * packets will not be stored in the nic ring buffer
     * and no data will be send to the network.
     */
    nic_write(NIC_PG0_RCR, NIC_RCR_MON);
    nic_write(NIC_PG0_TCR, NIC_TCR_LB0);

#if (USE_16BIT)
   /* set to word mode */
   nic_write(NIC_PG0_DCR, NIC_DCR_LS | NIC_DCR_FT1 | NIC_DCR_WTS);
#endif      /* (USE_16BIT) */

    /*
     * Define ring buffer pages:
     * 'send_start_pg': First transmit buffer page.
     * 'ring_start_pg': First receive buffer page.
     * 'ring_stop_pg': Last receive buffer page + 1.
     */
    send_start_pg = NIC_FIRST_TX_PAGE;
    ring_start_pg = NIC_FIRST_RX_PAGE;
    ring_stop_pg = NIC_STOP_PAGE;

    /*
     * Configure the nic's ring buffer page layout.
     */
    nic_write(NIC_PG0_TPSR, NIC_FIRST_TX_PAGE);
    nic_write(NIC_PG0_PSTART, NIC_FIRST_RX_PAGE);
    nic_write(NIC_PG0_PSTOP, NIC_STOP_PAGE);
    nic_write(NIC_PG0_BNRY, NIC_STOP_PAGE - 1);

    /*
     * Once again clear interrupt mask and interrupt
     * status register.
     */
    nic_write(NIC_PG0_ISR, 0xff);
    nic_write(NIC_PG0_IMR, 0);

    /*
     * Switch to register page 1 and copy our MAC address
     * into the nic. We are still in stop mode.
     */
   nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0);
   for(i = 0; i < 6; i++) {
      nic_write((byte)(NIC_PG1_PAR0 + i),eth_src_hw_addr[i]);
    }

    /*
     * Clear multicast filter bits to disable all packets.
     */
   for(i = 0; i < 8; i++)
      nic_write((byte)(NIC_PG1_MAR0 + i),0);

    /*
     * Set current page pointer to one page after the
     * boundary pointer.
     */
    nic_write(NIC_PG1_CURR, NIC_START_PAGE + TX_PAGES);

    /*
     * Switch back to register page 0, remaining
     * in stop mode.
     */
    nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2);

    /*
     * Take receiver out of monitor mode and enable it
     * for accepting broadcasts.
     */
    nic_write(NIC_PG0_RCR, RCR_SETTING);

    nic_write(NIC_PG0_TCR, 0);

    /*
     * Clear all interrupt status flags and mask
     * all interrupts except 'packet received'
     * and 'packet transmitted'.
     */
    nic_write(NIC_PG0_ISR, 0xff);
#if (!MN_POLLED_ETHERNET)
    nic_write(NIC_PG0_IMR, NIC_ISRS);
#endif          /* (!MN_POLLED_ETHERNET) */

    /*
     * Fire up the nic by clearing the stop bit and
     * setting the start bit. To activate the local
     * receive dma we must also take the nic out of
     * the local loopback mode.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

#if (!MN_POLLED_ETHERNET)
#if (defined(POLAVRR) || defined(CMXAVRR))
   EIMSK |= 0x20;    /* enable nic interrupts */
#elif (defined(POLST7E) || defined(CMXST7E))
   DISABLE_INTERRUPTS;

   /* TLI enabled and set as rising edge sensitive */
   EICR |= (EICR_tlis | EICR_tlie);

   ENABLE_INTERRUPTS;
#elif (defined(POLDSPICE) || defined(CMXDSPICE))
   /* Interrupt set above. */
#elif (defined(POLEZ8E) || defined(CMXEZ8E))
   DISABLE_INTERRUPTS;
   SET_VECTOR(P3AD, EtherInt);
   IRQPS |= 0x08;       /* select PD3 for interrupt source */
   IRQES |= 0x08;       /* interrupt on rising edge for PD3 */
   IRQ1ENH |= 0x08;     /* enable PD3 interrupts, high priority */
   IRQ1ENL |= 0x08;
   ENABLE_INTERRUPTS;
#else
#error need to enable NIC interrupts
#endif
#endif          /* (!MN_POLLED_ETHERNET) */

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
   strcpy_P(Str,str1);        /* Init Ok */
   DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
   mn_strcpy_cb(Str,str1);    /* Init Ok */
   DiagSend(Str);
#endif

   return 1;
}

#if (MN_POLLED_ETHERNET)
int rtl8019_recv(void)
cmx_reentrant {
/* receives a frame in polled mode. returns number of bytes received if
   successful or negative number on error.
   Copies entire frame including ethernet header into the receive buffer.
*/
#if (MN_ETHER_WAIT_TICKS)
   TIMER_INFO_T wait_timer;
#endif
   int retval;

   retval = 0;

#if (MN_ETHER_WAIT_TICKS)
   mn_reset_timer(&wait_timer,(MN_ETHER_WAIT_TICKS));
   while (!mn_timer_expired(&wait_timer))
      {
      /* read ISR status */
      if (nic_read(NIC_PG0_ISR) & NIC_ISR_PRX)
         {
         retval = 1;
         break;
         }
#if (RTOS_USED != RTOS_NONE)
      MN_TASK_WAIT(1);
#endif      /* (RTOS_USED != RTOS_NONE) */
      }
#else
   if (nic_read(NIC_PG0_ISR) & NIC_ISR_PRX)
      retval = 1;
#endif

   if (!retval)
      {
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
      strcpy_P(Str,str7);        /* No Pkt */
      DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
      mn_strcpy_cb(Str,str7);    /* No Pkt */
      DiagSend(Str);
#endif
      return (SOCKET_TIMED_OUT);
      }

   return (recv_pkt());
}
#else
/*
 * Ethernet controller interrupt entry.
 *
 */
#if (defined(POLAVRR) || defined(CMXAVRR))
#if (defined(__IAR_SYSTEMS_ICC))
interrupt [INT5_vect] void EtherInt(void)
#else
#pragma vector=INT5_vect
__interrupt void EtherInt(void)
#endif
#elif (defined(POLST7E) || defined(CMXST7E))
@interrupt void tli_isr(void)
#elif (defined(POLDSPICE) || defined(CMXDSPICE))
void __attribute__((__interrupt__)) _INT4Interrupt(void)
#elif (defined(POLEZ8E) || defined(CMXEZ8E))
#pragma interrupt
void EtherInt(void)
#else
#error need to declare EtherInt interrupt function
#endif      /* (defined(POLAVRR) || defined(CMXAVRR)) */
{
#if ( defined(CMXAVRR) && (RTOS_USED != RTOS_NONE) )
   static byte isr;
   static volatile byte junk;

   K_OS_Intrp_Entry();
#else
   byte isr;
   volatile byte junk;
#endif

#if (defined(POLDSPICE) || defined(CMXDSPICE))
   IFS2bits.INT4IF = 0;             /* clear interrupt flag */
#endif      /* (defined(POLDSPICE) || defined(CMXDSPICE)) */

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
   strcpy_P(Str,str13);
   DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
   mn_strcpy_cb(Str,str13);
   DiagSend(Str);
#endif

   /* make sure we are in page 0 */
   nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

   /* disable nic ISRs */
   nic_write(NIC_PG0_IMR, 0);

   /* read ISR status, loop until no more interrupts */
   while ( (isr = nic_read(NIC_PG0_ISR)) )
      {
      /* Packet received. */
      if (isr & NIC_ISR_PRX)
         {
         nic_write(NIC_PG0_ISR, NIC_ISR_PRX);
         while (recv_pkt() > 0)
            {
            nic_write(NIC_PG0_ISR, NIC_ISR_PRX);
            /* tell upper layers we got a packet */
            MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE);
            }
         }

      /* Packet send complete. */
      if (isr & NIC_ISR_PTX)
         {
         nic_write(NIC_PG0_ISR, NIC_ISR_PTX);
         MN_XMIT_BUSY_CLEAR;    /* allow the next packet transmission */
         MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
         strcpy_P(Str,str6);        /* Write Done */
         DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
         mn_strcpy_cb(Str,str6);    /* Write Done */
         DiagSend(Str);
#endif      /* (defined(POLAVRR) || defined(CMXAVRR)) */
         }

      /* clear any errors */
      if (isr & NIC_ISR_OVW)
         nic_write(NIC_PG0_ISR, NIC_ISR_OVW);

      if (isr & NIC_ISR_RXE)
         nic_write(NIC_PG0_ISR, NIC_ISR_RXE);

      if (isr & NIC_ISR_TXE)
         {
         nic_write(NIC_PG0_ISR, NIC_ISR_TXE);
         MN_XMIT_BUSY_CLEAR;    /* allow the next packet transmission */
         MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
         }

      if (isr & NIC_ISR_CNT)
         {
         nic_write(NIC_PG0_ISR, NIC_ISR_CNT);
         junk = nic_read(NIC_PG0_CNTR0);
         junk = nic_read(NIC_PG0_CNTR1);
         junk = nic_read(NIC_PG0_CNTR2);
         }

      /* make sure we are in page 0 */
      nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);
      }

   /* enable nic ISRs */
   nic_write(NIC_PG0_IMR, NIC_ISRS);
}
#endif          /* (MN_POLLED_ETHERNET) */

/*
 * Remote DMA completion.
 */
static void dma_complete(void)
cmx_reentrant {
    byte i;

   /*
    * Complete remote dma.
    */
   nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

   /*
    * Check that we have a DMA complete flag.
    */
/*   for(i = 0; i <= 40; i++) */
   for(i = 0; i < 250; i++)
       if(nic_read(NIC_PG0_ISR) & NIC_ISR_RDC)
           break;

   if (i >= 250)
      {
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
      strcpy_P(Str,str10);
      DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
      mn_strcpy_cb(Str,str10);
      DiagSend(Str);
#endif
      }

   /*
    * Reset remote dma complete flag.
    */
   nic_write(NIC_PG0_ISR, NIC_ISR_RDC);
}

/* Send a packet. The ethernet header must in the frame to be sent.
   Returns the number of bytes sent or negative number on error.
   The ethernet header and other header info is in the xmit_buff. The TCP
   or UDP data is pointed to by socket_ptr->send_ptr. xmit_sock_len is the
   number of data bytes to be sent in the current packet.
*/
int rtl8019_send(PSOCKET_INFO socket_ptr, word16 xmit_sock_len)
cmx_reentrant {
   word16 i;
   word16 pkt_len;
   word16 tot_len;
   word16 xmit_buff_len;
   byte padding;
   byte *MsgSendPointer;
#if (MN_POLLED_ETHERNET)
   byte xmit_status;
#endif          /* (MN_POLLED_ETHERNET) */
#if (USE_16BIT)
   byte oddLength;
   word16 tdata;
#endif      /* (USE_16BIT) */
   PINTERFACE_INFO interface_ptr;

   MsgSendPointer = BYTE_PTR_NULL;
   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
   strcpy_P(Str,str12);
   DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
   mn_strcpy_cb(Str,str12);
   DiagSend(Str);
#endif

   /* make sure there is something to send */
   if (interface_ptr->send_out_ptr == interface_ptr->send_in_ptr)
      {
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
      strcpy_P(Str,str9);
      DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
      mn_strcpy_cb(Str,str9);
      DiagSend(Str);
#endif
      return (0);
      }

   /* calculate the number of bytes to send */
   xmit_buff_len = interface_ptr->send_in_ptr - interface_ptr->send_out_ptr;
   if (xmit_sock_len && (socket_ptr->send_ptr != PTR_NULL))
      MsgSendPointer = socket_ptr->send_ptr;
#if (USE_16BIT)
   else if (xmit_buff_len & 0x0001)
      {
      /* xmit_buff_len should be an even #, otherwise force it to be even. */
      mn_send_byte(0x00);
      xmit_buff_len++;
      }
#endif      /* (USE_16BIT) */

   pkt_len = xmit_sock_len + xmit_buff_len;

#if (USE_16BIT)
   /* ensure even number of bytes for 16-bit mode */
   if (pkt_len & 0x01)
      pkt_len++;
#endif      /* (USE_16BIT) */


   /* make sure bytes to send is in the correct range */
   if (pkt_len > ETHER_SIZE)
      {
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
      strcpy_P(Str,str8);
      DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
      mn_strcpy_cb(Str,str8);
      DiagSend(Str);
#endif
      return (ETHER_SEND_ERROR);
      }
   else if (pkt_len < MIN_PKT_SIZE)
      {
      padding = (byte)(MIN_PKT_SIZE - pkt_len);
      tot_len = MIN_PKT_SIZE;
      }
   else
      {
      padding = 0;
      tot_len = pkt_len;
      }

   /* make sure we are in page 0 */
   nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

#if (MN_POLLED_ETHERNET)
   /* reset interrupt flags */
   nic_write(NIC_PG0_ISR, NIC_ISR_PTX | NIC_ISR_TXE);
#else
   /* disable nic ISRs */
   nic_write(NIC_PG0_IMR, 0);
#endif          /* (!MN_POLLED_ETHERNET) */

    /*
     * Set remote dma byte count
     * and start address.
     */
   nic_write(NIC_PG0_RBCR0, LOWBYTE(tot_len));
   nic_write(NIC_PG0_RBCR1, HIGHBYTE(tot_len));
   nic_write(NIC_PG0_RSAR0, 0);
   nic_write(NIC_PG0_RSAR1, send_start_pg);

    /*
     * Peform the write.
     */
   nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD1);

#if (USE_16BIT)
   xmit_buff_len = xmit_buff_len >> 1;
   for (i=0; i<xmit_buff_len; i++)
      {
      tdata = *interface_ptr->send_out_ptr++;
      tdata += (word16)((word16)(*interface_ptr->send_out_ptr++)<<8);
      /* The dsPIC does not need the following line, others may need it. */
/*      tdata = hs2net(tdata); */
      nic_write16(NIC_IOPORT, tdata);
      }

   if (xmit_sock_len & 0x01)
      oddLength = TRUE;
   else
      oddLength = FALSE;

   xmit_sock_len = xmit_sock_len >> 1;
   for (i=0;i<xmit_sock_len;i++)
      {
      tdata = *MsgSendPointer++;
      tdata += (word16)((word16)(*MsgSendPointer++)<<8);
      /* The dsPIC does not need the following line, others may need it. */
/*      tdata = hs2net(tdata); */
      nic_write16(NIC_IOPORT, tdata);
      }

   if (oddLength)
      {
      tdata = *MsgSendPointer;
      /* The dsPIC does not need the following line, others may need it. */
/*      tdata = hs2net(tdata); */
      nic_write16(NIC_IOPORT, tdata);
      }

    /* Add pad bytes. */
   padding = padding >> 1;
   for (i=0;i<padding;i++)
      nic_write(NIC_IOPORT, 0);
#else
   for (i=0;i<xmit_buff_len;i++)
      nic_write(NIC_IOPORT, *interface_ptr->send_out_ptr++);

   for (i=0;i<xmit_sock_len;i++)
      nic_write(NIC_IOPORT, *MsgSendPointer++);

    /* Add pad bytes. */
   for (i=0;i<padding;i++)
      nic_write(NIC_IOPORT, 0);
#endif      /* (USE_16BIT) */

    /*
     * Complete remote dma.
     */
   dma_complete(); 
      
    /*
     * Number of bytes to be transmitted.
     */
    nic_write(NIC_PG0_TBCR0, LOWBYTE(tot_len));
    nic_write(NIC_PG0_TBCR1, HIGHBYTE(tot_len));

    /*
     * First page of packet to be transmitted.
     */
    nic_write(NIC_PG0_TPSR, send_start_pg);

    /*
     * Start transmission.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_TXP | NIC_CR_RD2);

#if (MN_POLLED_ETHERNET)
   while (1)
      {
      xmit_status = nic_read(NIC_PG0_TSR);
      if (xmit_status & (NIC_TSR_ABT | NIC_TSR_FU))
         {
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
         strcpy_P(Str,str8);
         DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
         mn_strcpy_cb(Str,str8);
         DiagSend(Str);
#endif
         return (ETHER_SEND_ERROR);
         }
      else if (xmit_status & NIC_TSR_PTX)
         break;
      }
#else
   /* enable nic ISRs */
   nic_write(NIC_PG0_IMR, NIC_ISRS);
#endif          /* (MN_POLLED_ETHERNET) */

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
   strcpy_P(Str,str2);
   DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
   mn_strcpy_cb(Str,str2);
   DiagSend(Str);
#endif
   return ((int)tot_len);
}

#if (!(USE_RECV_BUFF))
static NIC_PKT_HEADER_T hdr;

#if (USE_16BIT)
   static byte read_high_byte;
#endif
#endif

/* Called to receive a packet. Returns number of bytes received if
   successful or negative number on error.
   Note: the way we use the structure here may not work for DSPs.
*/
static int recv_pkt(void)
cmx_reentrant {
   byte bnry;
   byte curr;
#if (USE_RECV_BUFF || (!(USE_16BIT) && MN_LITTLE_ENDIAN))
   word16 i;
#endif
   word16 recv_len;
   byte *buf;
   byte len_hb, len_lb;
#if (USE_16BIT)
   word16 tdata;
#endif      /* (USE_16BIT) */
#if (USE_RECV_BUFF)
   byte nextpg;
   NIC_PKT_HEADER_T hdr;
#if (USE_16BIT)
   byte oddLength;
#endif      /* (USE_16BIT) */
#endif
   int retval;
#if 0
   word16 bytes2end;
#endif
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);
   retval = 0;

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
   strcpy_P(Str,str11);
   DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
   mn_strcpy_cb(Str,str11);
   DiagSend(Str);
#endif

   buf = (byte *)&hdr;

    /*
     * Get the last page we read plus 1.
     */
    bnry = (byte)(nic_read(NIC_PG0_BNRY) + 1);
    if(bnry >= ring_stop_pg)
        bnry = ring_start_pg;

    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2 | NIC_CR_PS0);
    curr = nic_read(NIC_PG1_CURR);
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);
    if(bnry == curr) {
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
      strcpy_P(Str,str3);         /* No Recv */
      DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
      mn_strcpy_cb(Str,str3);     /* No Recv */
      DiagSend(Str);
#endif
        return 0;
    }

    /*
     * Set remote dma byte count and start address.
     */
    nic_write(NIC_PG0_RBCR0, sizeof(NIC_PKT_HEADER_T));
    nic_write(NIC_PG0_RBCR1, 0);
    nic_write(NIC_PG0_RSAR0, 0);
    nic_write(NIC_PG0_RSAR1, bnry);

    /*
     * Perform the read of the packet header.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD0);

#if (USE_16BIT)
   tdata = nic_read16(NIC_IOPORT);
#if (MN_LITTLE_ENDIAN)
/*   hdr.ph_status = LOWBYTE(tdata); */
   hdr.ph_nextpg = HIGHBYTE(tdata);
#else
/*   hdr.ph_status = HIGHBYTE(tdata); */
   hdr.ph_nextpg = LOWBYTE(tdata);
#endif      /* (MN_LITTLE_ENDIAN) */
   hdr.ph_size = nic_read16(NIC_IOPORT);
#else
#if (MN_LITTLE_ENDIAN)
    for(i = 0; i < sizeof(NIC_PKT_HEADER_T); i++)
        *buf++ = nic_read(NIC_IOPORT);
#else
   hdr.ph_status = nic_read(NIC_IOPORT);
   hdr.ph_nextpg = nic_read(NIC_IOPORT);
   hdr.ph_size = nic_read(NIC_IOPORT);
   hdr.ph_size += (word16)nic_read(NIC_IOPORT) << 8;
#endif      /* (MN_LITTLE_ENDIAN) */
#endif      /* (USE_16BIT) */

    /*
     * Complete remote dma.
     */
    dma_complete();

    recv_len = hdr.ph_size - sizeof(NIC_PKT_HEADER_T);

    /*
     * Sanity check the packet size - if it's not sane then purge the
     * receive buffers and try to continue.
     */
   if ( (hdr.ph_nextpg < ring_start_pg) || (hdr.ph_nextpg >= ring_stop_pg) || \
            (recv_len < MIN_PKT_SIZE) || \
            ((interface_ptr->recv_buff_size - interface_ptr->recv_count - 2 ) < recv_len) )
   {

        /*
         * Select register page 1 and get
         * the dma page used for the next
         * received packet.
         */
        nic_write(NIC_CR, NIC_CR_RD2 | NIC_CR_PS0);
        curr = nic_read(NIC_PG1_CURR);
        nic_write(NIC_CR, NIC_CR_RD2);

        bnry = (byte)(curr - 1);
        if(bnry < ring_start_pg)
            bnry = (byte)(ring_stop_pg - 1);

        /*
         * Set the last page we read.
         */
        nic_write(NIC_PG0_BNRY, bnry);
#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
        strcpy_P(Str,str4);         /* No Room */
        DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
        mn_strcpy_cb(Str,str4);     /* No Room */
        DiagSend(Str);
#endif
        return (ETHER_RECV_ERROR);
   }


   /* if we get to here we have a valid packet to receive */
   retval = recv_len;

   len_hb = HIGHBYTE(recv_len);
   len_lb = LOWBYTE(recv_len);

#if (MN_POLLED_ETHERNET)
   MN_TASK_LOCK;
#endif      /* (MN_POLLED_ETHERNET) */

#if (USE_RECV_BUFF)
   /* put recv_len into buffer so all packets start with the length.
      Do NOT remove the next two lines.
   */
   PUT_RECV_BYTE(len_hb);
   PUT_RECV_BYTE(len_lb);
#endif

   /*
    * Set remote dma byte count and
    * start address. Don't read the
    * header again.
    */
   nic_write(NIC_PG0_RBCR0, len_lb);
   nic_write(NIC_PG0_RBCR1, len_hb);
   nic_write(NIC_PG0_RSAR0, sizeof(NIC_PKT_HEADER_T));
   nic_write(NIC_PG0_RSAR1, bnry);

   /* Perform the read. */
   nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD0);

#if (USE_RECV_BUFF)
#if (USE_16BIT)
   if (recv_len & 0x01)
      oddLength = TRUE;
   else
      oddLength = FALSE;

   recv_len = recv_len >> 1;
   for(i = 0; i < recv_len; i++)
      {
      tdata = nic_read16(NIC_IOPORT);
#if (MN_LITTLE_ENDIAN)
      PUT_RECV_BYTE(LOWBYTE(tdata));
      PUT_RECV_BYTE(HIGHBYTE(tdata));
#else
      PUT_RECV_BYTE(HIGHBYTE(tdata));
      PUT_RECV_BYTE(LOWBYTE(tdata));
#endif
      }

   if (oddLength)
      PUT_RECV_BYTE( nic_read(NIC_IOPORT) );
#else
    for(i = 0; i < recv_len; i++)
      PUT_RECV_BYTE( nic_read(NIC_IOPORT) );
#endif      /* (USE_16BIT) */

    interface_ptr->recv_count += retval + 2;

    /*
     * Complete remote dma.
     */
    dma_complete();

    /*
     * Update boundary, contains the last
     * page we read.
     */
    nextpg = (byte)(hdr.ph_nextpg - 1);
    if(nextpg < ring_start_pg)
        nextpg = (byte)(ring_stop_pg - 1);
    nic_write(NIC_PG0_BNRY, nextpg);

#else
   /* USE_RECV_BUFF == 0 */
   interface_ptr->recv_count = retval;
#if (USE_16BIT)
   read_high_byte = TRUE;
#endif
#endif

#if (MN_POLLED_ETHERNET)
   MN_TASK_UNLOCK;
#endif      /* (MN_POLLED_ETHERNET) */

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG )
      strcpy_P(Str,str5);         /* Recv OK */
      DiagSend(Str);
#elif ( (defined(POLST7E) || defined(CMXST7E)) && DO_DEBUG )
      mn_strcpy_cb(Str,str5);     /* Recv OK */
      DiagSend(Str);
#endif
   return (retval);
}

#if ( (defined(POLAVRR) || defined(CMXAVRR)) && DO_DEBUG)
void DiagSend(char *Str)
   {
   char i;

   i = 0; 
   
   while(Str[i] != 0)
      {   
      while(!(USR & 0x20));
      UDR = Str[i++];
      }  

  }

#if 0
/* trap spurious interrupts */
interrupt [INT0_vect] void i0(void)
{
   while(!(USR & 0x20));
   UDR = 'A';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [INT1_vect] void i1(void)
{
   while(!(USR & 0x20));
   UDR = 'B';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [INT2_vect] void i2(void)
{
   while(!(USR & 0x20));
   UDR = 'C';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [INT3_vect] void i3(void)
{
   while(!(USR & 0x20));
   UDR = 'D';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [INT4_vect] void i4(void)
{
   while(!(USR & 0x20));
   UDR = 'E';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [INT6_vect] void i5(void)
{
   while(!(USR & 0x20));
   UDR = 'F';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [INT7_vect] void i6(void)
{
   while(!(USR & 0x20));
   UDR = 'G';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER2_COMP_vect] void i7(void)
{
   while(!(USR & 0x20));
   UDR = 'H';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER2_OVF_vect] void i8(void)
{
   while(!(USR & 0x20));
   UDR = 'I';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER1_CAPT_vect] void i9(void)
{
   while(!(USR & 0x20));
   UDR = 'J';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER1_COMPA_vect] void i10(void)
{
   while(!(USR & 0x20));
   UDR = 'K';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER1_COMPB_vect] void i11(void)
{
   while(!(USR & 0x20));
   UDR = 'L';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER1_OVF_vect] void i12(void)
{
   while(!(USR & 0x20));
   UDR = 'M';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [TIMER0_OVF_vect] void i13(void)
{
   while(!(USR & 0x20));
   UDR = 'N';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [SPI_STC_vect] void i14(void)
{
   while(!(USR & 0x20));
   UDR = 'O';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [UART_RX_vect] void i15(void)
{
   while(!(USR & 0x20));
   UDR = 'P';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [UART_UDRE_vect] void i16(void)
{
   while(!(USR & 0x20));
   UDR = 'Q';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [UART_TX_vect] void i17(void)
{
   while(!(USR & 0x20));
   UDR = 'R';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [ADC_vect] void i18(void)
{
   while(!(USR & 0x20));
   UDR = 'S';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [EE_RDY_vect] void i19(void)
{
   while(!(USR & 0x20));
   UDR = 'T';
   DISABLE_INTERRUPTS;
   while (1);
}
interrupt [ANA_COMP_vect] void i20(void)
{
   while(!(USR & 0x20));
   UDR = 'U';
   DISABLE_INTERRUPTS;
   while (1);
}
#endif      /* 0 */
#endif      /* (defined(POLAVRR) || defined(CMXAVRR)) */

#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))
byte readByte(byte reg)
{
   byte dat;

   SET_NIC_READ();
   WRITE_NIC_ADDR(reg);
   NIC_IOR_IO = 0;
   dat = NIC_DATA_IO;
   NIC_IOR_IO = 1;

   return(dat);
}

void writeByte(byte reg, byte dat)
{
   WRITE_NIC_ADDR(reg);
   NIC_DATA_IO = dat;
   SET_NIC_WRITE();
   NIC_IOW_IO = 0;
   NOP;
   NIC_IOW_IO = 1;
   SET_NIC_READ();
}
#endif      /* (defined(POLPIC18E) || defined(CMXPIC18E)) */

#if (defined(POLST7E) || defined(CMXST7E))
#if DO_DEBUG
void DiagSend(char *Str)
{
   char i;

   i = 0; 
   
   while (Str[i] != 0)
      {   
      while ((SCISR & 0x80) == 0);
      SCIDR = Str[i++];
      }  

}
#endif      /* DO_DEBUG */

byte readByte(byte regAddr)
{
   byte regVal;
   
   /* Set Port to Input float */
   PGDDR = 0x00;
   PGOR = 0x00;
   
   /* AEN low + IO address */
   PHDR = (byte)((regAddr * 8) + 0x03); 
   NOP;

   /* IORB low */
   PHDR = (byte)((regAddr * 8) + 0x02);
   NOP;

   /* read data */
   regVal = PGDR;
/*   NOP; */

   /* IORB high */
   PHDR = (byte)((regAddr * 8) + 0x03);
/*   NOP; */
   
   return regVal;
}

void writeByte(byte regAddr, byte regData)
{
   /* AEN low + IO address */
   PHDR = (byte)((regAddr * 8) + 0x03); 

   /* Set Port to Drive and output data */
   PGDDR = 0xff;
   PGOR = 0xff;
   PGDR = regData;
/*   NOP; */

   /* IOWB low */
   PHDR = (byte)((regAddr * 8) + 0x01);
/*   NOP; */

   /* IOWB high */
   PHDR = (byte)((regAddr * 8) + 0x03);
   NOP;

   /* Set Port to Input float */
/*   PGDDR = 0x00; */
/*   PGOR = 0x00; */
}

static void delay(word32 dly)
{
   word32 i;
   word32 factor;
   
   factor = dly * 20UL;   
   i=0; 
   while (i <= factor)
     i++;   
}

#endif      /* (defined(POLST7E) || defined(CMXST7E)) */

#if (defined(POLEZ8E) || defined(CMXEZ8E))
byte readByte(byte regAddr)
{
   byte regVal;
   byte temp;

   /* write address with IORB high and IOWB high */
   temp   = (regAddr | 0x60);
   PGOUT  = temp;

   /* set data port direction to input */
   PEADDR = DATA_DIR;
   PECTL  = 0xFF;

   /* write address with IORB low and IOWB high */
   temp   = (regAddr | 0x40);
   PGOUT  = temp;
   NOP;

   /* read from the data port */
   regVal = PEIN;

   /* write address with IORB high and IOWB high */
   temp   = (regAddr | 0x60);
   PGOUT  = temp;

   return regVal;
}

void writeByte(byte regAddr, byte regData)
{
   byte temp;

   /* write address with IORB high and IOWB high */
   temp   = (regAddr | 0x60);
   PGOUT  = temp;
   
   /* write the data to the data port */
   PEOUT  = regData;

   /* set data port direction to output */
   PEADDR = DATA_DIR;
   PECTL  = 0x00;

   /* write address with IORB high and IOWB low */
   temp   = (regAddr | 0x20);
   PGOUT  = temp;
   
   /* write address with IORB high and IOWB high */
   temp   = (regAddr | 0x60);
   PGOUT  = temp;
}
#endif      /* (defined(POLEZ8E) || defined(CMXEZ8E)) */

#if (defined(POLDSPICE) || defined(CMXDSPICE))
byte readByte(byte regAddr)
{
   byte regVal;
/* control register selection */
   TRISD = 0x0000;               /* set PortD as outputs */
   LATBbits.LATB12 = 1;          /* set ALE */
   LATD = regAddr;               /* SA0-SA5 = address  */
   Nop();
   LATBbits.LATB12 = 0;          /* ALE/AEN low, SA0-SA5 latched via 573  */

/* control register data read */
   TRISD = 0xFFFF;               /* set PortD to input */
   LATBbits.LATB13 = 0;          /* assert *IORD  */
   /* nop required for 30MIPs operation */
   Nop(); 
   regVal = PORTD;               /* read NIC internal control reg data  */
   LATBbits.LATB13 = 1;          /* negate *IORD */
   LATBbits.LATB12 = 1;          /* set ALE */

   return regVal;
}

void writeByte(byte regAddr, byte regData)
{
/* control register selection */
   TRISD = 0x0000;               /* set PortD as outputs */
   LATBbits.LATB12 = 1;          /* set ALE */
   LATD = regAddr;               /* SA0-SA5 = address */
   Nop();
   LATBbits.LATB12 = 0;          /* ALE/AEN low, SA0-SA5 latched via 573  */
       
/* control register data write */
   LATBbits.LATB14 = 0;          /* assert *IOWR  */
   LATD = regData;
   Nop();
   LATBbits.LATB14 = 1;          /* negate *IOWR */
   LATBbits.LATB12 = 1;          /* set ALE */
}

#if (USE_16BIT)
word16 readByte16(byte regAddr)
{
   word16 regVal;
/* control register selection */
   TRISD = 0x0000;               /* set PortD as outputs */
   LATBbits.LATB12 = 1;          /* set ALE */
   LATD = regAddr;               /* SA0-SA5 = address  */
   Nop();
   LATBbits.LATB12 = 0;          /* ALE/AEN low, SA0-SA5 latched via 573  */

/* control register data read */
   TRISD = 0xFFFF;               /* set PortD to input */
   LATBbits.LATB13 = 0;          /* assert *IORD  */
   /* nop required for 30MIPs operation */
   Nop();
   regVal = PORTD;               /* read NIC internal control reg data  */
   LATBbits.LATB13 = 1;          /* negate *IORD */
   LATBbits.LATB12 = 1;          /* set ALE */

   return regVal;
}

void writeByte16(byte regAddr, word16 regData)
{
/* control register selection */
   TRISD = 0x0000;               /* set PortD as outputs */
   LATBbits.LATB12 = 1;          /* set ALE */
   LATD = regAddr;               /* SA0-SA5 = address */
   Nop();
   LATBbits.LATB12 = 0;          /* ALE/AEN low, SA0-SA5 latched via 573  */
       
/* control register data write */
   LATBbits.LATB14 = 0;          /* assert *IOWR  */
   LATD = regData;
   Nop();
   LATBbits.LATB14 = 1;          /* negate *IOWR */
   LATBbits.LATB12 = 1;          /* set ALE */
}
#endif      /* (USE_16BIT) */
#endif      /* (defined(POLDSPICE) || defined(CMXDSPICE)) */

/* Functions needed by the rest of the stack if USE_RECV_BUFF is set to 0. */
#if (!(USE_RECV_BUFF))
void eth_init_recv(SCHAR interface_no)
cmx_reentrant {
}

int eth_mn_recv_byte(void)
cmx_reentrant {
   byte c;
   PINTERFACE_INFO interface_ptr;
#if (USE_16BIT)
   static word16 tdata;
#endif      /* (USE_16BIT) */

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);
   c = 0;

   if (interface_ptr->recv_count)
      {
#if (USE_16BIT)
      if (read_high_byte)
         {
         tdata = nic_read16(NIC_IOPORT);
#if (MN_LITTLE_ENDIAN)
         c = LOWBYTE(tdata);
#else
         c = HIGHBYTE(tdata);
#endif
         }
      else
         {
#if (MN_LITTLE_ENDIAN)
         c = HIGHBYTE(tdata);
#else
         c = HIGHBYTE(tdata);
#endif
         }

      read_high_byte = (byte)(!read_high_byte);
#else
      c = nic_read(NIC_IOPORT);
#endif

      interface_ptr->recv_count--;
      }

   return ((int)c);
}

/* This function not used in MN_POLLED_ETHERNET mode. */
byte eth_mn_recv_byte_present(SCHAR interface_no)
cmx_reentrant {
#if (!MN_POLLED_ETHERNET)
   /* Add code here if support for interrupt-driven ethernet is added. */
#endif
   return (0);
}

void eth_mn_ip_discard_packet(void)
cmx_reentrant {
   byte nextpg;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

    /*
     * Complete remote dma.
     */
    dma_complete();

    /*
     * Update boundary, contains the last
     * page we read.
     */
    nextpg = (byte)(hdr.ph_nextpg - 1);
    if(nextpg < ring_start_pg)
        nextpg = (byte)(ring_stop_pg - 1);
    nic_write(NIC_PG0_BNRY, nextpg);

   interface_ptr->recv_count = 0;
}
#endif      /* (!(USE_RECV_BUFF)) */

#endif      /* (MN_ETHERNET) */
