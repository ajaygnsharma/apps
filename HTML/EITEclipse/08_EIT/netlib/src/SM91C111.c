/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

/* driver for eCog1 board with LAN91C111 ethernet.
   Added support for Custom M16C/62P board with LAN91C113 ethernet and
   NEC V850ES/SG2+Net board with LAN91C113 ethernet and
   Phytec phyCORE-LPC2294 (assumes ethernet interrupt is connected to EINT0).

   Note: the phyCORE_LPC2294 mounted on the HD200 development board only works
   in 10Base-T mode.

   Assumes that the internal PHY is being used.

   For use with v3.05 and up only.
*/

#include "micronet.h"

/*
   Porting Notes:

   1. Make sure that mnconfig.h is configured properly for either ISR
      driven ethernet or MN_POLLED_ETHERNET as required.

   2. Make sure the ETHER_POLL_RECV, ETHER_INIT and ETHER_SEND(p,n) in
      ethernet.h are defined properly.

      #define ETHER_POLL_RECV       smsc91C111_recv()
      #define ETHER_INIT            smsc91C111_init()
      #define ETHER_SEND(p,n)       smsc91C111_send((p),(n))

   3. Write readSMSC and writeSMSC functions to read and write to the
      SMSC chip.

   4. If the chip's MAC address is stored in EEPROM set the define
      EEPROM_PRESENT below to 1, otherwise set it to 0.

   5. To have the SMSC chip negotiate its speed and whether full duplex
      or not set AUTO_NEGOTIATE to 1. Otherwise set the full_duplex and
      speed_100 variables to their desired values (see below). The other
      defines below may be changed if desired but it is probably not
      necessary.

   6. If ISR driven ethernet is used, add code to the end of the
      smsc91C111_init function to enable the ISR.

   7. Change the function declaration for the ISR function ether_isr to
      the declaration required for your hardware and C compiler. Every
      compiler does this differently so consult your compiler manual if
      necessary. This step is not needed if using MN_POLLED_ETHERNET.

*/

#if (MN_ETHERNET)

#if (!(USE_SEND_BUFF))
#error USE_SEND_BUFF must be set to 1 for this driver.
#endif      /* (!(USE_SEND_BUFF)) */

#if (!(USE_RECV_BUFF))
#error USE_RECV_BUFF must be set to 1 for this driver.
#endif      /* (!(USE_RECV_BUFF)) */

#if (MN_USE_HW_CHKSUM)
#error MN_USE_HW_CHKSUM not supported by this driver.
#endif

#include "sm91c111.h"

/* modifiable #defines */
#define EEPROM_PRESENT  0     /* set to 1 if EEPROM is being used */

/* set the following to 1 to have the chip auto-negotiate parameters, or set
   to 0 to use the full_duplex and speed_100 settings below.
*/
#if (defined(POLARM2294) || defined(CMXARM2294))
#define AUTO_NEGOTIATE     0
#else
#define AUTO_NEGOTIATE     1
#endif

#if (!AUTO_NEGOTIATE)
int full_duplex = 0;             /* set to 1 for full duplex */
int speed_100 = 0;               /* set to 1 for 100Base-T, 0 for 10Base-T */
#endif      /* AUTO_NEGOTIATE */

#define HARDWARE_PAD    1     /* set to 1 to enable hardware padding */
#define NUM_ALLOC_POLLS 256   /* times to poll if allocate completed */
#define NUM_TXSUC_POLLS 1500   /* times to poll for TXSUC */

#if (!MN_POLLED_ETHERNET)
/* if polling for the ALLOC interrupt fails, it is unlikely that enabling
   the ALLOC interrupt and waiting for it will work either, but you have
   the option to do so by setting the following #define to 1.
*/
#define USE_ALLOC_INT   0        /* set to 1 to use ALLOC interrupt */
#if (USE_ALLOC_INT)
volatile int wait_for_alloc;
#endif      /* (USE_ALLOC_INT) */

#else

#define USE_ALLOC_INT   0        /* do not change */

#endif      /* (!MN_POLLED_ETHERNET) */

/* local function prototypes */
static int allocpkt(word16 total_len) cmx_reentrant;
static void delay(void) cmx_reentrant;
static void reset_mmu(void) cmx_reentrant;
#if (MN_POLLED_ETHERNET)
static word16 polltxint(void) cmx_reentrant;
#endif      /* (MN_POLLED_ETHERNET) */

static word16 mii_read(byte) cmx_reentrant;
static void mii_write(byte, word16) cmx_reentrant;
static int phy_init(void) cmx_reentrant;

#if (defined(POLECOG1E) || defined(CMXECOG1E))
static void configuration_init( void ) cmx_reentrant;
static void emi_init( void ) cmx_reentrant;
static void mmu_init( void ) cmx_reentrant;
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */

/* macros */
#if (defined(POLECOG1E) || defined(CMXECOG1E))
static word16 base_addr= 0x3d80;
#define SMSC_WRITE(OFFSET,DATA)  *(word16 *)(base_addr + (OFFSET)) = (word16)(DATA)
#define SMSC_READ(OFFSET)        (*((word16 *)(base_addr + (OFFSET))))

#elif (defined(POLM16CP) || defined(CMXM16CP))
#define base_addr                0x10000
#define SMSC_WRITE(OFFSET,DATA)  *(far word16 *)(base_addr + (OFFSET)) = (word16)(DATA)
#define SMSC_READ(OFFSET)        (*((far word16 *)(base_addr + (OFFSET))))

#elif (defined(POLARM2294) || defined(CMXARM2294))
#define READ_32_BITS             1
#define WRITE_32_BITS            1
#define base_addr                0x82000300
#define SMSC_WRITE(OFFSET,DATA)  *(word16 *)(base_addr + (OFFSET)) = (word16)(DATA)
#define SMSC_WRITE32(OFFSET,DATA)  *(word32 *)(base_addr + (OFFSET)) = (word32)(DATA)
#define SMSC_READ(OFFSET)        (*((volatile word16 *)(base_addr + (OFFSET))))
#define SMSC_READ32(OFFSET)      (*((volatile word32 *)(base_addr + (OFFSET))))

#if (!MN_POLLED_ETHERNET)
#if (defined(__CA__))
extern void IRQ_Wrapper(void) __arm;
#else
extern void IRQ_Wrapper(void);
#endif   /* (defined(__CA__)) */
#endif      /* (MN_POLLED_ETHERNET) */

#elif (defined(POLNECV850ES) || defined(CMXNECV850ES))
#define base_addr                0x100600
#define SMSC_WRITE(OFFSET,DATA)  *(volatile word16 *)(base_addr + (OFFSET)) = (word16)(DATA)
#define SMSC_READ(OFFSET)        (*((volatile word16 *)(base_addr + (OFFSET))))

#else
#error need to define SMSC_WRITE and SMSC_READ macros.
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */
#define BANKSWITCH(B)            SMSC_WRITE((BANKSEL),(B))

#ifndef READ_32_BITS
#define READ_32_BITS 0
#endif
#ifndef WRITE_32_BITS
#define WRITE_32_BITS 0
#endif

#if (MN_ALLOW_MULTICAST)
#define RCR_SETTING     (STRIPCRC|RXEN|PRMS)
#else
#define RCR_SETTING     (STRIPCRC|RXEN)
#endif

/* file-wide variables */
static byte smsc_rev;

#define PUT_RECV_BYTE(A) { \
   *(interface_ptr->recv_in_ptr) = (A); \
   interface_ptr->recv_in_ptr++; \
   if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr) \
      interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr; \
}

/*********************************************************/

int smsc91C111_init(void)
cmx_reentrant {
   word16 reg_data;
#if (EEPROM_PRESENT)
   word16 mac10,mac32,mac54;
#endif      /* (EEPROM_PRESENT) */

#if (!MN_POLLED_ETHERNET)
#if (defined(POLECOG1E) || defined(CMXECOG1E))
   /* disable ethernet interrupt */
   reg.configuration.gpio_byte_0_config_h.bits.interrupt_config_3 = 0;
#elif (defined(POLM16CP) || defined(CMXM16CP))
   /* disable ethernet interrupt */
   DISABLE_INTERRUPTS;
   INT0IC = 0x00;
   ENABLE_INTERRUPTS;
#elif (defined(POLARM2294) || defined(CMXARM2294))
   byte vpbdiv_temp;

   /* disable eint0 interrupt */
   VICIntEnClr = (1 << 14);
#elif (defined(POLNECV850ES) || defined(CMXNECV850ES))
   PIC1.6 = 1;     /* disable INTP1 interrupt */
#else
   #error add code to disable ethernet interrupt
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */
#endif      /* (!MN_POLLED_ETHERNET) */

#if (defined(POLECOG1E) || defined(CMXECOG1E))
   configuration_init();
   emi_init() ;
   mmu_init() ;
   MN_TASK_WAIT(5);

   /* Set logical, physical and data parameters for cs1 - ethernet */
   reg.mmu.eram_cs1a_data_logical_address = 0x3c ;    /* at 0x3C00 */
   reg.mmu.eram_cs1a_data_physical_address = 0x0 ;    /* 0 */
   reg.mmu.eram_cs1a_data_size.bits.data_size = 0x03; /* 1k */

   reg.configuration.gpio_byte_0_control_h.bits.data_out_set_2 = 1 ;
   reg.configuration.gpio_byte_0_control_h.bits.data_out_clear_2 = 1 ;

   MN_TASK_WAIT(5);

#elif (defined(POLM16CP) || defined(CMXM16CP))
   pd10_6 = 1;              /* set ethernet reset port pin to an output */
   ETH_RESET = 0;          /* set ethernet chip reset low */
   ETH_RESET = 1;          /* reset ethernet chip */
   ETH_RESET = 1;
   ETH_RESET = 0;
   MN_TASK_WAIT((MN_ONE_SECOND)/10);

#elif (defined(POLNECV850ES) || defined(CMXNECV850ES))
   P5.1 = 0;
   PM5.1 = 0;
   MN_TASK_WAIT(MN_ONE_SECOND);
#endif

#if (defined(POLARM2294) || defined(CMXARM2294) || \
      defined(POLNECV850ES) || defined(CMXNECV850ES))
   /* Read bankselect register to see if we are reading the controller ok. */
   reg_data = SMSC_READ(BANKSEL);
   if ( (reg_data & 0xFF00) != 0x3300 )
      return (ETHER_INIT_ERROR);
#endif

   /* soft reset */
   BANKSWITCH(BANK0);
   SMSC_WRITE(RCR,SOFTRST);
   delay();

   /* make sure power is on */
   BANKSWITCH(BANK1);
   SMSC_WRITE(CONFIG,EPH_POWER_EN);
   delay();

   BANKSWITCH(BANK0);
   SMSC_WRITE(RCR,CLEAR);
   delay();
   SMSC_WRITE(TCR,CLEAR);

   BANKSWITCH(BANK1);
#if (!MN_POLLED_ETHERNET)
   /* Turn on Link error enable and xmit error enable.
      auto-release gets turned on later.
   */
   reg_data = SMSC_READ(CONTROL);
   SMSC_WRITE(CONTROL,reg_data | LEENABLE | TEENABLE);
#endif      /* (!MN_POLLED_ETHERNET) */

#if (EEPROM_PRESENT)
   /* read MAC address and write it to eth_src_hw_addr. This assumes the
      MAC address was stored in EEPROM.
   */
   mac54 = SMSC_READ(IA54);
   mac32 = SMSC_READ(IA32);
   mac10 = SMSC_READ(IA10);
   MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
   eth_src_hw_addr[0] = LOWBYTE(mac10);
   eth_src_hw_addr[1] = HIGHBYTE(mac10);
   eth_src_hw_addr[2] = LOWBYTE(mac32);
   eth_src_hw_addr[3] = HIGHBYTE(mac32);
   eth_src_hw_addr[4] = LOWBYTE(mac54);
   eth_src_hw_addr[5] = HIGHBYTE(mac54);
   MN_MUTEX_RELEASE(MUTEX_MN_INFO);
#else
   MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
   SMSC_WRITE(IA10,MK_WORD16(eth_src_hw_addr[1],eth_src_hw_addr[0]));
   SMSC_WRITE(IA32,MK_WORD16(eth_src_hw_addr[3],eth_src_hw_addr[2]));
   SMSC_WRITE(IA54,MK_WORD16(eth_src_hw_addr[5],eth_src_hw_addr[4]));
   MN_MUTEX_RELEASE(MUTEX_MN_INFO);
#endif      /* (EEPROM_PRESENT) */

   /* get the revision - the rev A parts (smsc_rev == 0) have a bug
      where the ODDFRM bit doesn't get set on odd length packets. We
      keep track of the rev for a workaround later.
   */
   BANKSWITCH(BANK3);
   reg_data = SMSC_READ(REVISION);
   smsc_rev = (byte)(reg_data & 0x000f);

   reset_mmu();

   reg_data = SMSC_READ(INTERRUPT);
   SMSC_WRITE(INTERRUPT,reg_data & 0x00FF);  /* disable interrupts */

   BANKSWITCH(BANK0);
   SMSC_WRITE(RCR,RCR_SETTING);      /* strip CRC from received packets */

#if (MN_POLLED_ETHERNET)
#if HARDWARE_PAD
   SMSC_WRITE(TCR,PADEN);                 /* turn on hardware padding */
#endif      /* HARDWARE_PAD */
#else          /* interrupt driven */
#if HARDWARE_PAD
   SMSC_WRITE(TCR,PADEN|TXENA);           /* turn on hardware padding & xmit */
#else
   SMSC_WRITE(TCR,TXENA);                 /* turn on xmit */
#endif      /* HARDWARE_PAD */
#endif      /* MN_POLLED_ETHERNET */

   BANKSWITCH(BANK2);

#if (!MN_POLLED_ETHERNET)
   /* setup interrupts */
   reg_data = SMSC_READ(INTERRUPT);
#if (defined(POLECOG1E) || defined(CMXECOG1E))
   reg_data |= RXINT_EN | TXINT_EN | RXOVRNINT_EN | EPHINT_EN;
#else
   reg_data |= RXINT_EN | TXINT_EN | RXOVRNINT_EN | EPHINT_EN | MDINT_EN;
#endif
   SMSC_WRITE(INTERRUPT,reg_data);
#endif      /* (!MN_POLLED_ETHERNET) */

   /* Initialize PHY */
   if (phy_init() < 0)
      return (ETHER_INIT_ERROR);

#if (defined(POLECOG1E) || defined(CMXECOG1E))
   /* allocate and release a packet - for some reason we have to do this
      before a receive if we want the following transmit to work.
   */
   if (allocpkt(64))
      {
      SMSC_WRITE(MMUCOM,RELEPNR);
      }
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */

   BANKSWITCH(BANK1);
   reg_data = SMSC_READ(CONTROL);
   SMSC_WRITE(CONTROL,reg_data | AUTORELEASE);  /* auto-release TX packets */
   BANKSWITCH(BANK2);

#if (!MN_POLLED_ETHERNET)
#if (defined(POLECOG1E) || defined(CMXECOG1E))
   /* enable gpio7 interrupt */
   reg.configuration.gpio_byte_0_config_h.bits.interrupt_config_3 = 5;
#elif (defined(POLM16CP) || defined(CMXM16CP))
   DISABLE_INTERRUPTS;
   IFSR0 &= 0xFE;             /* interrupt on one edge */
   INT0IC = 0x14;             /* set interrupt level to 4, rising edge */
   ENABLE_INTERRUPTS;
#elif (defined(POLARM2294) || defined(CMXARM2294))
   DISABLE_INTERRUPTS;
   PINSEL1 &= ~0x02;    /* Set pin function to eint0. */
   PINSEL1 |= 0x01;
   vpbdiv_temp = VPBDIV;
   vpbdiv_temp = VPBDIV;
   VPBDIV = 0x00;
   VPBDIV = 0x01;
   EXTMODE |= 0x01;     /* Set EINT0 interrupt to edge sensitive. */
   EXTPOLAR |= 0x01;    /* Set EINT0 interrupt to rising edge. */
   VPBDIV = vpbdiv_temp;
   EXTINT = 0x01;       /* Clear EINT0 interrupt. */
   VICVectAddr0 = (word32)IRQ_Wrapper; /* set interrupt vector in slot 0 */
   VICVectCntl0 = 0x20 | 14;           /* use it for EINT0 Interrupt */
   VICIntEnable = (1 << 14);           /* Enable EINT0 Interrupt */
   ENABLE_INTERRUPTS;
#elif (defined(POLNECV850ES) || defined(CMXNECV850ES))
   DISABLE_INTERRUPTS;
   PMC0.4  = 1;                        /* P04 is used for INTP1 */
   INTF0.4 = 0;                        /* interrupt on rising edge */
   INTR0.4 = 1;
   PIC1 = 0x02;                        /* enable INTP1 interrupt, level 2 */
   ENABLE_INTERRUPTS;
#else
   #error add code to enable ethernet interrupt
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */
#endif      /* (!MN_POLLED_ETHERNET) */

   return 1;
}

int smsc91C111_send(PSOCKET_INFO socket_ptr, word16 xmit_sock_len)
cmx_reentrant {
#if WRITE_32_BITS
   word32 tdat32;
#if (!HARDWARE_PAD)
   word16 i;
#endif
#else
   word16 i;
#endif
   word16 tdata;
   word16 xmit_buff_len;
   word16 total_len;
   int retval;
   byte oddLength;
   byte *MsgSendPointer;
#if (MN_POLLED_ETHERNET)
   int tcrdat;
#endif      /* (MN_POLLED_ETHERNET) */
#if (!HARDWARE_PAD)
   word16 pad_len;
#endif      /* (!HARDWARE_PAD) */
   PINTERFACE_INFO interface_ptr;

   retval = 0;
   oddLength = FALSE;
   MsgSendPointer = PTR_NULL;
#if (!HARDWARE_PAD)
   pad_len = 0;
#endif      /* (!HARDWARE_PAD) */
   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   /* make sure there is something to send */
   if (interface_ptr->send_out_ptr != interface_ptr->send_in_ptr)
      {
      /* calculate the number of bytes to send */
      xmit_buff_len = interface_ptr->send_in_ptr - interface_ptr->send_out_ptr;
      if (xmit_sock_len && (socket_ptr->send_ptr != PTR_NULL))
         {
         MsgSendPointer = socket_ptr->send_ptr;
         oddLength = (xmit_sock_len & 0x0001);
         }
      else if (xmit_buff_len & 0x0001)
         {
         /* xmit_buff_len should be an even # if socket_ptr->send_ptr != PTR_NULL,
            otherwise force it to be even.
         */
         mn_send_byte(0x00);
         xmit_buff_len++;
         }

      total_len = xmit_sock_len + xmit_buff_len;
      if (oddLength)    /* make sure total_len is even. we will add a pad byte. */
         total_len++;
      if (total_len > ETHER_SIZE)
         return (ETHER_SEND_ERROR);

      if (total_len < MIN_PACKET_SIZE)
         {
#if (!HARDWARE_PAD)
         pad_len = MIN_PACKET_SIZE - total_len;
#endif      /* (!HARDWARE_PAD) */
         total_len = MIN_PACKET_SIZE;
         }

      /* add 6 for status word, byte count and control word */
      if (allocpkt(total_len+6))
         {
         retval = total_len;

#if (MN_POLLED_ETHERNET)
         BANKSWITCH(BANK0);
         tcrdat = SMSC_READ(TCR);
         SMSC_WRITE(TCR,tcrdat|TXENA);    /* enable transmit */
         BANKSWITCH(BANK2);
#endif      /* (MN_POLLED_ETHERNET) */

         while (SMSC_READ(POINTER) & NOTEMPTY);
         SMSC_WRITE(POINTER,TXWR);
         SMSC_WRITE(DATA1,0x0000);        /* status word */

         /* add 6 for status word, byte count and control word */
         SMSC_WRITE(DATA1,total_len+6);

#if WRITE_32_BITS
         /* write the headers */
         while (xmit_buff_len >= 4)
            {
            tdat32 = *interface_ptr->send_out_ptr++;
            tdat32 += (word32)((word32)(*interface_ptr->send_out_ptr++)<<8);
            tdat32 += (word32)((word32)(*interface_ptr->send_out_ptr++)<<16);
            tdat32 += (word32)((word32)(*interface_ptr->send_out_ptr++)<<24);
            SMSC_WRITE32(DATA1,tdat32);
            xmit_buff_len -= 4;
//            if (xmit_buff_len < 4)
//               break;
            }

         /* At this point xmit_buff_len should be either 2 or 0. */
         if (xmit_buff_len > 0)
            {
            tdata = *interface_ptr->send_out_ptr++;
            tdata += (word16)((word16)(*interface_ptr->send_out_ptr++)<<8);
            SMSC_WRITE(DATA1,tdata);
            }

         /* send socket data if necessary */
         if (xmit_sock_len)
            {
            if (oddLength)
               xmit_sock_len--;     /* need even count for the loop */

            /* write the data */
            while (xmit_sock_len >= 4)
               {
               tdat32 = *MsgSendPointer++;
               tdat32 += (word32)((word32)(*MsgSendPointer++)<<8);
               tdat32 += (word32)((word32)(*MsgSendPointer++)<<16);
               tdat32 += (word32)((word32)(*MsgSendPointer++)<<24);
               SMSC_WRITE32(DATA1,tdat32);
               xmit_sock_len -= 4;
//               if (xmit_sock_len < 4)
//                  break;
               }

            if (xmit_sock_len > 1)
               {
               tdata = *MsgSendPointer++;
               tdata += (word16)((word16)(*MsgSendPointer++)<<8);
               SMSC_WRITE(DATA1,tdata);
               }

            if (oddLength)
               {
               tdata = *MsgSendPointer;
               SMSC_WRITE(DATA1,tdata);
               }
            }
#else
         /* write the headers */
         xmit_buff_len >>= 1;
         for (i=0; i<xmit_buff_len; i++)
            {
            tdata = *interface_ptr->send_out_ptr++;
            tdata += (word16)((word16)(*interface_ptr->send_out_ptr++)<<8);
            SMSC_WRITE(DATA1,tdata);
            }

         /* send socket data if necessary */
         if (xmit_sock_len)
            {
            if (oddLength)
               xmit_sock_len--;     /* need even count for the loop */

            /* write the data */
            xmit_sock_len >>= 1;
            for (i=0; i<xmit_sock_len; i++)
               {
               tdata = *MsgSendPointer++;
               tdata += (word16)((word16)(*MsgSendPointer++)<<8);
               SMSC_WRITE(DATA1,tdata);
               }

            if (oddLength)
               {
               tdata = *MsgSendPointer;
               SMSC_WRITE(DATA1,tdata);
               }
            }
#endif

#if (!HARDWARE_PAD)
         /* write the padding. pad_len should be even. */
         if (pad_len)
            {
            pad_len >>= 1;
            for (i=0;i<pad_len;i++)
               SMSC_WRITE(DATA1,0x0000);
            }
#endif      /* (!HARDWARE_PAD) */

         /* write the control word */
         SMSC_WRITE(DATA1,0x0000);

#if (!MN_POLLED_ETHERNET)
         DISABLE_INTERRUPTS;
         SMSC_WRITE(INTERRUPT,(SMSC_READ(INTERRUPT) | TXEMPTYINT_EN));
#endif      /* (!MN_POLLED_ETHERNET) */

         /* queue it up */
         SMSC_WRITE(MMUCOM,QUEUETX);

#if (!MN_POLLED_ETHERNET)
         ENABLE_INTERRUPTS;
#endif      /* (!MN_POLLED_ETHERNET) */

#if (MN_POLLED_ETHERNET)
         if (polltxint() != TXSUC)
            retval = ETHER_SEND_ERROR;

         BANKSWITCH(BANK0);
         SMSC_WRITE(TCR,tcrdat);
         BANKSWITCH(BANK2);
#endif      /* (MN_POLLED_ETHERNET) */
         }
      else
         {
         /* allocpkt failed. */
         retval = ETHER_SEND_ERROR;
         }
      }

   return (retval);
}

#if MN_POLLED_ETHERNET
/* receives a frame in polled mode. returns number of bytes received if
   successful or negative number on error.
   Copies entire frame including ethernet header into the receive buffer.
*/
int smsc91C111_recv(void)
cmx_reentrant {
   word16 recv_status;
   word16 recv_len, i;
#if (MN_ETHER_WAIT_TICKS)
   TIMER_INFO_T wait_timer;
#endif
   int retval;
   int oddLength;
   word16 intr_status;
   PINTERFACE_INFO interface_ptr;
#if READ_32_BITS
   word32 tdata;
#else
   word16 tdata;
#endif      /* READ_32_BITS */
   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   retval = 0;

#if (MN_ETHER_WAIT_TICKS)
   /* wait for RXINT */
   mn_reset_timer(&wait_timer,(MN_ETHER_WAIT_TICKS));
   while (!mn_timer_expired(&wait_timer))
      {
      intr_status = SMSC_READ(INTERRUPT);
      if ( intr_status & RXOVRNINT )
         {
         SMSC_WRITE(INTERRUPT,intr_status);
         BANKSWITCH(BANK0);
         SMSC_WRITE(RCR,RCR_SETTING);      /* enable receive */
         reset_mmu();
         MN_XMIT_BUSY_CLEAR;
         retval = ETHER_RECV_ERROR;
         break;
         }

      if ( intr_status & RXINT )
         {
         retval = 1;
         break;
         }
#if (RTOS_USED != RTOS_NONE)
      MN_TASK_WAIT(1);
#endif      /* (RTOS_USED != RTOS_NONE) */
      }
#else
   intr_status = SMSC_READ(INTERRUPT);
   if ( intr_status & RXOVRNINT )
      {
      SMSC_WRITE(INTERRUPT,intr_status);
      BANKSWITCH(BANK0);
      SMSC_WRITE(RCR,RCR_SETTING);      /* enable receive */
      reset_mmu();
      MN_XMIT_BUSY_CLEAR;
      retval = ETHER_RECV_ERROR;
      }
   else if ( intr_status & RXINT )
      retval = 1;
#endif

   if (retval == 1)
      {
      if ( (REMPTY & SMSC_READ(FIFOPORTS)) != REMPTY )
         {
         while (SMSC_READ(POINTER) & NOTEMPTY);
         SMSC_WRITE(POINTER,RXRD);
#if READ_32_BITS
         tdata = (SMSC_READ32(DATA1));
#if (MN_LITTLE_ENDIAN)
         recv_status = LOWWORD(tdata);
         recv_len = HIGHWORD(tdata) & 0x0fff;
#else
         recv_status = HIGHWORD(tdata);
         recv_len = LOWWORD(tdata) & 0x0fff;
#endif      /* MN_LITTLE_ENDIAN */
#else
         recv_status = (SMSC_READ(DATA1));
         recv_len = SMSC_READ(DATA1) & 0x0fff;
#endif      /* READ_32_BITS */

         /* make sure there were no errors in the packet */
         if (recv_status & (BADCRC|ALGNERR|TOOSHORT|TOOLONG) || recv_len < 6)
            retval = ETHER_RECV_ERROR;
         else
            {
            /* subtract for status, byte count, and control word. */
            recv_len -= 6;

            /* for rev A parts always assume odd length and let the
               upper layers take care of it.
            */
            oddLength = (smsc_rev != 1) || (recv_status & ODDFRM);
            /* adjust length for odd byte */
            if (oddLength)
               recv_len++;

            if ( (interface_ptr->recv_buff_size - interface_ptr->recv_count - 2) >= recv_len)
               {
               retval = recv_len;

               MN_TASK_LOCK;

               /* put recv_len into buffer so all packets start with the
                  length. Do NOT remove the next two lines.
               */
               PUT_RECV_BYTE(HIGHBYTE(recv_len));
               PUT_RECV_BYTE(LOWBYTE(recv_len));

               if (oddLength)
                  recv_len--;    /* need even count for the loop */

#if READ_32_BITS
               while (recv_len >= 4)
                  {
                  recv_len -= 4;
                  tdata = (SMSC_READ32(DATA1));
#if (MN_LITTLE_ENDIAN)
                  for (i=0;i<4;i++)
#else
                  for (i=3;i>=0;i--)
#endif      /* MN_LITTLE_ENDIAN */
                     PUT_RECV_BYTE((byte)(((word32)(tdata) >> (i<<3)) & 0x000000FF));
                  }

               /* now get control word and possibly data */
               tdata = (SMSC_READ32(DATA1));
               if (recv_len == 2)
                  {
                  /* we have some data */
#if (MN_LITTLE_ENDIAN)
                  PUT_RECV_BYTE(WORD32_BYTE0(tdata));
                  PUT_RECV_BYTE(WORD32_BYTE1(tdata));
#else
                  PUT_RECV_BYTE(WORD32_BYTE3(tdata));
                  PUT_RECV_BYTE(WORD32_BYTE2(tdata));
#endif      /* MN_LITTLE_ENDIAN */
                  tdata = (tdata >> 16);
                  }

               if (oddLength)    /* get the last byte */
                  PUT_RECV_BYTE(WORD32_BYTE0(tdata));

#else
               recv_len >>= 1;
               for (i=0; i<recv_len;i++)
                  {
                  tdata = SMSC_READ(DATA1);
#if ((defined(POLECOG1E) || defined(CMXECOG1E)) || MN_LITTLE_ENDIAN)
                  PUT_RECV_BYTE(LOWBYTE(tdata));
                  PUT_RECV_BYTE(HIGHBYTE(tdata));
#else
                  PUT_RECV_BYTE(HIGHBYTE(tdata));
                  PUT_RECV_BYTE(LOWBYTE(tdata));
#endif
                  }

               /* now get control word */
               tdata = SMSC_READ(DATA1);
               if (oddLength)    /* get the last byte */
                  {
#if ((defined(POLECOG1E) || defined(CMXECOG1E)) || MN_LITTLE_ENDIAN)
                  PUT_RECV_BYTE(LOWBYTE(tdata));
#else
                  PUT_RECV_BYTE(HIGHBYTE(tdata));
#endif
                  }
#endif      /* READ_32_BITS */

               interface_ptr->recv_count += retval + 2;
               MN_TASK_UNLOCK;
               }
            else           /* not enough room to hold this packet */
               retval = ETHER_RECV_ERROR;
            }
         BANKSWITCH(BANK2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RELERX);
         }
      else
         retval = 0;
      }
   else if (retval == 0)
      retval = SOCKET_TIMED_OUT;

   return (retval);
}
#else       /* interrupt driven */
#if (defined(POLECOG1E) || defined(CMXECOG1E))
void __irq_code ether_isr(void) cmx_reentrant;

void __irq_code ether_isr(void) cmx_reentrant

#else
void ether_isr(void);

#if (defined(NC30) || defined(NC308))
#if (RTOS_USED == RTOS_NONE)
#pragma INTERRUPT ether_isr
#endif      /* (RTOS_USED == RTOS_NONE) */
#endif      /* (defined(NC30) || defined(NC308)) */

#if (defined(POLNECV850ES) || defined(CMXNECV850ES))
#if (RTOS_USED == RTOS_NONE)
#pragma interrupt INTP1 ether_isr
__interrupt
#endif      /* (RTOS_USED == RTOS_NONE) */
#endif

void ether_isr(void)
#endif
{
   word16 oldbank, pntr, mask_status, pnr;
   word16 intr_status, reg_data;
   word16 recv_status;
   word16 recv_len, i, temp_len;
   volatile word16 eph_status;
   int oddLength;
   PINTERFACE_INFO interface_ptr;
#if READ_32_BITS
   word32 tdata;
#else
   word16 tdata;
#endif      /* READ_32_BITS */

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

#if (defined(NC30) || defined(NC308))
   DISABLE_INTERRUPTS;
#endif      /* (!(defined(POLECOG1E) || defined(CMXECOG1E))) */

   oldbank = SMSC_READ(BANKSEL) & 0x0007; /* get current bank */

   BANKSWITCH(BANK2);
   pntr = SMSC_READ(POINTER);

   /* read interrupt status & mask, status in low byte, mask in high byte */
   mask_status = SMSC_READ(INTERRUPT);
   SMSC_WRITE(INTERRUPT,0x0000);     /* clear mask */

   /* get the valid interrupts to process */
   intr_status = (mask_status & 0x00FF) & (mask_status >> 8);
   mask_status &= 0xFF00;

   while (intr_status)
      {
      if (intr_status & RXOVRNINT)    /* receive overrun */
         {
         SMSC_WRITE(INTERRUPT,RXOVRNINT);
         BANKSWITCH(BANK0);
         SMSC_WRITE(RCR,RCR_SETTING);      /* enable receive */
#if (defined(POLECOG1E) || defined(CMXECOG1E))
         BANKSWITCH(BANK2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RESETMMU);        /* reset MMU */
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
#else
         reset_mmu();
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */
         intr_status = SMSC_READ(INTERRUPT) & (mask_status >> 8);
#if (USE_ALLOC_INT)
         intr_status |= ALLOCINT;            /* force reset of alloc int */
#endif      /* (USE_ALLOC_INT) */
         intr_status |= TXEMPTYINT;          /* force reset of txempty int */
         }

      /* receiver interrupt */
      if (intr_status & RXINT)
         {
         if ( (REMPTY & SMSC_READ(FIFOPORTS)) != REMPTY )
            {
            SMSC_WRITE(POINTER,RXRD);
#if READ_32_BITS
            tdata = (SMSC_READ32(DATA1));
#if (MN_LITTLE_ENDIAN)
            recv_status = LOWWORD(tdata);
            recv_len = HIGHWORD(tdata) & 0x0fff;
#else
            recv_status = HIGHWORD(tdata);
            recv_len = LOWWORD(tdata) & 0x0fff;
#endif      /* MN_LITTLE_ENDIAN */
#else
            recv_status = (SMSC_READ(DATA1));
            recv_len = SMSC_READ(DATA1) & 0x0fff;
#endif      /* READ_32_BITS */

            /* make sure there were no errors in the packet */
            if (!(recv_status & (BADCRC|ALGNERR|TOOSHORT|TOOLONG)) && \
                  recv_len >= 6)
               {
               /* subtract for status, byte count, and control word. */
               recv_len -= 6;

               /* for rev A parts always assume odd length and let the
                  upper layers take care of it.
               */
               oddLength = (smsc_rev != 1) || (recv_status & ODDFRM);
               /* adjust length for odd byte */
               if (oddLength)
                  recv_len++;

               if ( (interface_ptr->recv_buff_size - interface_ptr->recv_count - 2) >= recv_len)
                  {
                  temp_len = recv_len;

                  /* put recv_len into buffer so all packets start with the
                     length. Do NOT remove the next two lines.
                  */
                  PUT_RECV_BYTE(HIGHBYTE(recv_len));
                  PUT_RECV_BYTE(LOWBYTE(recv_len));

                  if (oddLength)
                     recv_len--;    /* need even count for the loop */

#if READ_32_BITS
                  while (recv_len >= 4)
                     {
                     recv_len -= 4;
                     tdata = (SMSC_READ32(DATA1));
#if (MN_LITTLE_ENDIAN)
                  for (i=0;i<4;i++)
#else
                  for (i=3;i>=0;i--)
#endif      /* MN_LITTLE_ENDIAN */
                        PUT_RECV_BYTE((byte)(((word32)(tdata) >> (i<<3)) & 0x000000FF));
                     }

                  /* now get control word and possibly data */
                  tdata = (SMSC_READ32(DATA1));
                  if (recv_len == 2)
                     {
                     /* we have some data */
#if (MN_LITTLE_ENDIAN)
                     PUT_RECV_BYTE(WORD32_BYTE0(tdata));
                     PUT_RECV_BYTE(WORD32_BYTE1(tdata));
#else
                     PUT_RECV_BYTE(WORD32_BYTE3(tdata));
                     PUT_RECV_BYTE(WORD32_BYTE2(tdata));
#endif      /* MN_LITTLE_ENDIAN */
                     tdata = (tdata >> 16);
                     }

                  if (oddLength)    /* get the last byte */
                     PUT_RECV_BYTE(WORD32_BYTE0(tdata));

#else
                  for (i=0; i<recv_len;i += 2)
                     {
                     tdata = SMSC_READ(DATA1);
#if ((defined(POLECOG1E) || defined(CMXECOG1E)) || MN_LITTLE_ENDIAN)
                     PUT_RECV_BYTE(LOWBYTE(tdata));
                     PUT_RECV_BYTE(HIGHBYTE(tdata));
#else
                     PUT_RECV_BYTE(HIGHBYTE(tdata));
                     PUT_RECV_BYTE(LOWBYTE(tdata));
#endif
                     }

                  /* now get control word */
                  tdata = SMSC_READ(DATA1);
                  if (oddLength)    /* get the last byte */
                     {
#if ((defined(POLECOG1E) || defined(CMXECOG1E)) || MN_LITTLE_ENDIAN)
                     PUT_RECV_BYTE(LOWBYTE(tdata));
#else
                     PUT_RECV_BYTE(HIGHBYTE(tdata));
#endif
                     }
#endif      /* READ_32_BITS */

                  interface_ptr->recv_count += temp_len + 2;

                  /* tell upper layers we got a packet */
                  MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE);
                  }
               }
            BANKSWITCH(BANK2);
            while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
            SMSC_WRITE(MMUCOM,RELERX);
            }
         }

      /* transmitter interrupt. will only be called if there is an error
         due to the auto-release mode.
      */
      if (intr_status & TXINT)
         {
         pnr = SMSC_READ(ARRPNR) & PNR_MASK;    /* save packet number */

         /* get TX packet from FIFO reg. */
         reg_data = SMSC_READ(FIFOPORTS) & FIFO_TX_MASK;

         /* prepare to read from this packet */
         SMSC_WRITE(ARRPNR,reg_data);
         SMSC_WRITE(POINTER,TXRD);

         /* get status word */
         reg_data = SMSC_READ(DATA1);

         if (!(reg_data & TXSUC))
            {
            BANKSWITCH(BANK0);
            reg_data = SMSC_READ(TCR);
#if HARDWARE_PAD
            reg_data |= (PADEN|TXENA);       /* turn on xmit & padding */
#else
            reg_data |= (TXENA);             /* turn on xmit */
#endif      /* HARDWARE_PAD */
            SMSC_WRITE(TCR,reg_data);
            }

         /* remove the bad packet */
         BANKSWITCH(BANK2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RELEPNR);

         SMSC_WRITE(INTERRUPT,TXINT);
         /* restore the packet number register */
         reg_data = SMSC_READ(ARRPNR) & ARR_MASK;     /* get ARR reg */
         reg_data |= pnr;
         SMSC_WRITE(ARRPNR,reg_data);
         intr_status |= TXEMPTYINT;          /* force reset of txempty int */
         }

      /* transmit empty interrupt - transmitter is done sending */
      if (intr_status & TXEMPTYINT)
         {
         SMSC_WRITE(INTERRUPT,TXEMPTYINT);
         MN_XMIT_BUSY_CLEAR;
         MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
         mask_status &= ~TXEMPTYINT_EN;
         }

#if (USE_ALLOC_INT)
      if (intr_status & ALLOCINT)
         {
         /* disable ALLOCINT interrupt and signal the allocpkt function */
         /* change the mask_status variable, not the mask register directly */
         wait_for_alloc = 0;
         mask_status &= ~ALLOCINT_EN;
         }
#endif      /* (USE_ALLOC_INT) */

      /* EPH interrupt - some error occurred */
      if (intr_status & EPHINT)
         {
#if (USE_ALLOC_INT)
         wait_for_alloc = 0;
         mask_status &= ~ALLOCINT_EN;
#endif      /* (USE_ALLOC_INT) */
         BANKSWITCH(BANK0);
         eph_status = SMSC_READ(STATUS);

         /* handle link up/down */
         BANKSWITCH(BANK1);
         reg_data = SMSC_READ(CONTROL);
         reg_data &= ~LEENABLE;
         SMSC_WRITE(CONTROL,reg_data);
         reg_data |= LEENABLE;
         SMSC_WRITE(CONTROL,reg_data);

         BANKSWITCH(BANK2);
         }

#if (!(defined(POLECOG1E) || defined(CMXECOG1E)))
      /* PHY status interrupt */
      if (intr_status & MDINT)
         {
         SMSC_WRITE(INTERRUPT,MDINT);
         /* clear phy status register */
         while (mii_read(18) & _PHY_INT);
         }
#endif      /* (!(defined(POLECOG1E) || defined(CMXECOG1E))) */

      intr_status = SMSC_READ(INTERRUPT) & (mask_status >> 8);
      }

   /* restore registers */
   SMSC_WRITE(INTERRUPT,mask_status);

   SMSC_WRITE(POINTER,pntr);

   BANKSWITCH(oldbank);

#if (defined(NC30) || defined(NC308))
   ENABLE_INTERRUPTS;
#endif
}
#endif      /* MN_POLLED_ETHERNET */

/* ************************************************************************* */
/* delay function */
/* ************************************************************************* */
static void delay(void)
cmx_reentrant {
   volatile word16 junk;
   junk = SMSC_READ(4);
   junk = SMSC_READ(4);
   junk = SMSC_READ(4);
}

static void reset_mmu(void)
cmx_reentrant {
   BANKSWITCH(BANK2);
   while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
   SMSC_WRITE(MMUCOM,RESETMMU);        /* reset MMU */
   while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
}

/********************************************************
   allocate transmit packet. if successful, returns (1)
   if failure, returns 0
********************************************************/
static int allocpkt(word16 total_len)
cmx_reentrant {
   word16 i,txpktno;
#if (!MN_POLLED_ETHERNET && USE_ALLOC_INT)
   TIMER_INFO_T wait_timer;
#endif      /* (MN_POLLED_ETHERNET && USE_ALLOC_INT) */

   BANKSWITCH(BANK2);
   SMSC_WRITE( MMUCOM, ( ALLOCTX | ( total_len >> 8 ) ) );

  for (i=0;i<NUM_ALLOC_POLLS;i++)
  {
    if ( (ALLOCINT & SMSC_READ(INTERRUPT) ) == ALLOCINT)
    {
      txpktno =  SMSC_READ(ARRPNR);
      if (txpktno & FAILALL)
         return (0);
      else
         {
         txpktno >>= 8;
         SMSC_WRITE(ARRPNR,txpktno);
         return(1);
         }
    }
  }

#if (!MN_POLLED_ETHERNET && USE_ALLOC_INT)
   /* set the alloc interrupt and wait a little more */
   wait_for_alloc = 1;
   SMSC_WRITE(INTERRUPT, (SMSC_READ(INTERRUPT) | ALLOCINT_EN));

   mn_reset_timer(&wait_timer,(MN_ETHER_WAIT_TICKS));
   while (!mn_timer_expired(&wait_timer))
      {
      if (!wait_for_alloc)
         {
         txpktno =  ( (0x3f00 & SMSC_READ(ARRPNR) ) >> 8 );
         SMSC_WRITE(ARRPNR,txpktno);
         return(1);
         }
      }
#endif      /* (MN_POLLED_ETHERNET && USE_ALLOC_INT) */

   /* allocation failed */
   return(0);
}

#if (MN_POLLED_ETHERNET)
/********************************************************************
 poll enqueued packet completion of transmit for polltime (ms) long
 polltxint waits until TXSUC or time elapses
 tx will abort due to the following fatal errors :
       16 collisions
       SQET fail and STPSQET=1
       TXURN
       carrier lost and MONCSN=1
       late collision
 return(TXSUC) TXSUC occurred
 return anything else equals error
********************************************************************/
static word16 polltxint(void)
cmx_reentrant {
   word16 i,statusdat,stpsqet,moncsn,fatalerr,reg_data;

   BANKSWITCH(BANK0);

   for (i=0; i<NUM_TXSUC_POLLS; i++)
      {
      if ( (TXSUC & SMSC_READ(STATUS)) == TXSUC )
         {
         BANKSWITCH(BANK2);
         return(TXSUC);
         }
      delay();
      }

   statusdat=(SMSC_READ(STATUS));
   stpsqet=(STPSQET & SMSC_READ(TCR));
   moncsn=(MONCSN & SMSC_READ(TCR));
   fatalerr = (statusdat & (stpsqet|moncsn|TXUNRN|SIXTEENCOL|LATCOL) );
   BANKSWITCH(BANK2);

   if (fatalerr)
      {
      reg_data = (SMSC_READ(FIFOPORTS) & FIFO_TX_MASK) << 8;
      SMSC_WRITE(ARRPNR,reg_data);
      while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
      SMSC_WRITE(MMUCOM,RELEPNR);            /* free the packet */
      while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
      }

   return(fatalerr);
}
#endif      /* (MN_POLLED_ETHERNET) */

/* Used by mii_read and mii_write */
static word16 mii_info[64];
static word16 *mii_info_ptr;

/* Read a PHY register through the MII */
static word16 mii_read(byte phy_reg)
cmx_reentrant {
   word16 oldbank;
   word16 mgmt_reg;
   word16 phy_data;
   word16 one, zero;
   SCHAR i;

   phy_data = 0;

   /* Save the current bank */
   oldbank = SMSC_READ(BANKSEL);

   /* Select bank 3 */
   BANKSWITCH(BANK3);

   mgmt_reg = SMSC_READ(MGMT) & (~(MDOE | MCLK | MDI | MDO));
   one = mgmt_reg | MDOE | MDO;
   zero = mgmt_reg | MDOE;

   mii_info_ptr = mii_info;

   /* write 32 ones */
   for (i = 0; i < 32; i++)
      *mii_info_ptr++ = one;

   /* write start bits - 0 1 */
   *mii_info_ptr++ = zero;
   *mii_info_ptr++ = one;

   /* write OP code - 1 0 for read */
   *mii_info_ptr++ = one;
   *mii_info_ptr++ = zero;

   /* write phy address - assume the internal phy with address 0 is used */
   for (i = 0; i < 5; i++)
      *mii_info_ptr++ = zero;

   /* write phy_reg */
   for (i=4; i>=0; i--)
      {
      if (phy_reg & (1 << i))
         *mii_info_ptr++ = one;
      else
         *mii_info_ptr++ = zero;
      }

   /* write TA bit */
   *mii_info_ptr++ = mgmt_reg;

   /* Send it */
   for (i=0; i<47; i++)
      {
      SMSC_WRITE(MGMT, mii_info[i]);
      SMSC_WRITE(MGMT, mii_info[i] | MCLK);
      }

   /* read data */
   for (i=15; i>=0; i--)
      {
      SMSC_WRITE(MGMT, mii_info[i]);
      SMSC_WRITE(MGMT, mii_info[i] | MCLK);
      if (SMSC_READ( MGMT ) & MDI)
         phy_data |= (1 << i);
      }

   /* write TA bit */
   SMSC_WRITE(MGMT, mii_info[i]);
   SMSC_WRITE(MGMT, mii_info[i] | MCLK);

   /* Restore original bank select */
   BANKSWITCH( oldbank );

   return(phy_data);
}

/* Writes to a PHY register through the MII */
static void mii_write(byte phy_reg, word16 phy_data)
cmx_reentrant {
   word16 oldbank;
   word16 mgmt_reg;
   word16 one, zero;
   SCHAR i;

   /* Save the current bank */
   oldbank = SMSC_READ( BANKSEL );

   /* Select bank 3 */
   BANKSWITCH(BANK3);

   mgmt_reg = SMSC_READ(MGMT) & (~(MDOE | MCLK | MDI | MDO));
   one = mgmt_reg | MDOE | MDO;
   zero = mgmt_reg | MDOE;

   mii_info_ptr = mii_info;

   /* write 32 ones */
   for (i=0; i<32; i++)
      *mii_info_ptr++ = one;

   /* write start bits - 0 1 */
   *mii_info_ptr++ = zero;
   *mii_info_ptr++ = one;

   /* write OP code - 0 1 for write */
   *mii_info_ptr++ = zero;
   *mii_info_ptr++ = one;

   /* write phy address - assume the internal phy with address 0 is used */
   for (i=0; i<5; i++)
      *mii_info_ptr++ = zero;

   /* write phy_reg */
   for (i=4; i>=0; i--)
      {
      if (phy_reg & (1 << i))
         *mii_info_ptr++ = one;
      else
         *mii_info_ptr++ = zero;
      }

   /* write TA bits */
   *mii_info_ptr++ = one;
   *mii_info_ptr++ = zero;

   /* write data */
   for (i=15; i>=0; i--)
      {
      if (phy_data & (1 << i))
         *mii_info_ptr++ = one;
      else
         *mii_info_ptr++ = zero;
      }

   /* Send it */
   for (i=0; i<64; i++)
      {
      SMSC_WRITE(MGMT, mii_info[i]);
      SMSC_WRITE(MGMT, mii_info[i] | MCLK);
      }

   /* Restore original bank select */
   BANKSWITCH( oldbank );
}

/* PHY initialization. Returns 1 if successful, negative number on error.
   See SMSC doc AN79.
*/
static int phy_init(void)
cmx_reentrant {
   int retval;
   word16 phy_data;
#if (AUTO_NEGOTIATE)
   TIMER_INFO_T phy_timer;
#else
   word16 rpc_val;
#endif         /* AUTO_NEGOTIATE */

   retval = -1;

   /* Reset the PHY */
   mii_write(0, _PHY_RESET);
   MN_TASK_WAIT((MN_ONE_SECOND)/2);

#if (!(defined(POLNECV850ES) || defined(CMXNECV850ES)))
   /* If reset is not done by now we have a problem. */
   if (mii_read(0) & _PHY_RESET)
      return (retval);
#endif

#if (AUTO_NEGOTIATE)
#if (!(defined(POLNECV850ES) || defined(CMXNECV850ES)))
   /* clear phy status register */
   while (mii_read(18) & _PHY_INT);
#endif

   /* Enable auto-negotiation mode */
   BANKSWITCH(BANK0);
   SMSC_WRITE(RPCR, ANEG);
/*   BANKSWITCH(BANK2); */

#if (!(defined(POLNECV850ES) || defined(CMXNECV850ES)))
   phy_data = mii_read(1);
   /* If AUTO_NEGOTIATE was selected and we are not capable of
      auto-negotiation return an error.
   */
   if (!(phy_data & _PHY_CAP_ANEG))
      return (-2);
#endif

   /* Start auto-negotiation */
   mii_write(0, (_PHY_ANEG_EN | _PHY_ANEG_RST));
   MN_TASK_WAIT((MN_ONE_SECOND)*1.5);

   retval = -3;

   /* Wait for auto-negotiation to finish, get an error or timeout */
   mn_reset_timer(&phy_timer, (3*(MN_ONE_SECOND)));
   while (!mn_timer_expired(&phy_timer))
      {
      phy_data = mii_read(1);
      if (phy_data & _PHY_ANEG_ACK)
         {
         retval = 1;
         break;
         }
      MN_TASK_WAIT((MN_ONE_SECOND)/10);
      }

   if (retval < 0)
      return (retval);

   /* Read PHY status register to see if we need to update TCR. */
   phy_data = mii_read(18);
   if (phy_data & _PHY_DPLXDET)
      {
      /* Update the TCR register */
      BANKSWITCH(BANK0);
      SMSC_WRITE(TCR, (SMSC_READ(TCR) | FDUPLX));
      BANKSWITCH(BANK2);
      }

#else       /* !AUTO_NEGOTIATE */
#if (!(defined(POLNECV850ES) || defined(CMXNECV850ES)))
   phy_data = mii_read(1);

   /* Make sure we are capable of the requested settings */
   if (full_duplex && speed_100 && !(phy_data & _PHY_CAP_TXF))
      return (-4);
   else if (!full_duplex && speed_100 && !(phy_data & _PHY_CAP_TXH))
      return (-4);
   else if (full_duplex && !speed_100 && !(phy_data & _PHY_CAP_TF))
      return (-4);
   else if (!full_duplex && !speed_100 && !(phy_data & _PHY_CAP_TH))
      return (-4);
#endif

   if (full_duplex)
      {
      phy_data = _PHY_DPLX;
      rpc_val = DPLX;
      }
   else
      {
      phy_data = 0;
      rpc_val = 0;
      }

   if (speed_100)
      {
      phy_data |= _PHY_SPEED;
      rpc_val |= SPEED;
      }

   /* Update the phy control register */
   mii_write(0, phy_data);

   /* Update the Receive/Phy Control register */
   BANKSWITCH(BANK0);
   SMSC_WRITE(RPCR, rpc_val);

   if (full_duplex)
      {
      /* Update the TCR register */
      SMSC_WRITE(TCR, (SMSC_READ(TCR) | FDUPLX));
      }

   BANKSWITCH(BANK2);
#endif         /* AUTO_NEGOTIATE */

#if (!MN_POLLED_ETHERNET)
#if (!(defined(POLECOG1E) || defined(CMXECOG1E)))
   /* Enable Link Fail Interrupt */
   phy_data = (word16)(~(_PHY_MINT | _PHY_MLNKFAIL));
   mii_write(19, phy_data);
#endif      /* (!(defined(POLECOG1E) || defined(CMXECOG1E))) */
#endif      /* (!MN_POLLED_ETHERNET) */

   return (1);
}

#if (defined(POLECOG1E) || defined(CMXECOG1E))
/* initialization functions for eCog1 */

static void configuration_init( void )
cmx_reentrant {
   /* Port A as GPIO 0-7. */
   reg.configuration.uio_config_select_abcdef.bits.port_a = 11 ;

  /* enable GPIO0-3 as ouputs and set high. */
  reg.configuration.gpio_byte_0_control_l.word =
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_DATA_OUT_SET_0_MASK   |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_OUTPUT_ENABLE_0_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_DATA_OUT_SET_1_MASK   |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_OUTPUT_ENABLE_1_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_DATA_OUT_SET_2_MASK   |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_OUTPUT_ENABLE_2_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_DATA_OUT_SET_3_MASK   |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_L_OUTPUT_ENABLE_3_MASK  ;

  /* Use GPIO4,5,7 as inputs, GPIO 6 as output. */
   reg.configuration.gpio_byte_0_control_h.word =
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_DATA_OUT_CLEAR_0_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_OUTPUT_DISABLE_0_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_DATA_OUT_CLEAR_1_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_OUTPUT_DISABLE_1_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_DATA_OUT_SET_2_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_OUTPUT_ENABLE_2_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_DATA_OUT_CLEAR_3_MASK  |
    CONFIGURATION_GPIO_BYTE_0_CONTROL_H_OUTPUT_DISABLE_3_MASK  ;

   /* Port E as A0-7. */
   reg.configuration.uio_config_select_abcdef.bits.port_e = 0 ;

   /* Port F as A8-15. */
   reg.configuration.uio_config_select_abcdef.bits.port_f = 0 ;

   /* Port G as D0-7. */
   reg.configuration.uio_config_select_ghijkl.bits.port_g = 0 ;

   /* Port H as D8-D15. */
   reg.configuration.uio_config_select_ghijkl.bits.port_h = 0 ;

   /* Port I as EMI control lines. */
   reg.configuration.uio_config_select_ghijkl.bits.port_i = 0 ;

   reg.configuration.uio_control_output_enable.word =
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_A_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_B_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_C_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_D_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_E_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_F_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_G_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_H_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_I_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_J_MASK |
      CONFIGURATION_UIO_CONTROL_OUTPUT_ENABLE_PORT_L_MASK ;
}

/******************************************************************************
NAME
   emi_init

SYNOPSIS
   static void emi_init( void )

FUNCTION
   Initialise the external memory interface to accept the SDRAM, and to map
   the ethernet controller as SRAM.

RETURNS
   Nothing.
******************************************************************************/

static void emi_init( void )
cmx_reentrant {
   /* Initialise 1st chip select for SDRAM. */
   reg.emi.sdram_config.word = 0x940d ;
   reg.emi.sdram_refresh_period.word = 0x330c ;
   reg.emi.sdram_refresh_count.word = 0x0010 ;
   reg.emi.control_status.word = 0x0804 ;
   reg.emi.sdram_custom_address.bits.sdram_custom_address = 0x400 ;
   reg.emi.sdram_custom_command.word = 0x7c85 ;
   reg.emi.sdram_custom_address.bits.sdram_custom_address = 0x030 ;
   reg.emi.sdram_custom_command.word = 0x7c01 ;
   reg.emi.sdram_custom_command.word = 0x0c63 ;

   /* Initialise the 2nd chip select for ethernet chip, which is accessed as
    * SRAM. */
   reg.emi.sram_config1.bits.as_enable = 1 ;
   reg.emi.sram_config1.bits.bits16 = 1 ;
   reg.emi.sram_config1.bits.wait_enable = 1 ;
   reg.emi.sram_config1.bits.data_setup_enable = 1 ;
   reg.emi.sram_config1.bits.address_hold_enable = 1 ;
   reg.emi.control_status.bits.sram_enable = 1 ;
}

/******************************************************************************
NAME
   mmu_init

SYNOPSIS
   static void mmu_init( void )

FUNCTION
   Inintialise the Memory management unit

RETURNS
   Nothing.
******************************************************************************/

static void mmu_init( void )
cmx_reentrant {
   /* Enable Translators. */
   reg.mmu.translate_enable.bits.eram_cs0a_data = 1 ;
   reg.mmu.translate_enable.bits.eram_cs0b_data = 1 ;
   reg.mmu.translate_enable.bits.eram_cs1a_data = 1 ;

   reg.mmu.irom_control.bits.wait_states = 1 ;

}
#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */

#endif      /* (MN_ETHERNET) */

