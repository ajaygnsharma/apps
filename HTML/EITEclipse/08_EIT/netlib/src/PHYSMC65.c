/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

/* Driver for Phytec C165 board with SMC91C96 ethernet. Assumes 16-bit host.
   Auto-release mode is used for transmit.

   Also runs on TQ Systems ETH plugin board and TQ STK 167 UE minimodule (jh)

   Added code to work with 80C188 board. (pjb)

   Added code to work with H8S/2764 board. (pjb)

   Added code to work with H8S/2329 board. (pjb)

   Added code to work with custom MSA0654 board. (pjb)
*/

#if defined(__C166__)      /* Keil C16x */
#pragma FIX167 
#endif      /* defined(__C166__) */

#include "micronet.h"

/*
   Porting Notes:

   1. Make sure that mnconfig.h is configured properly for either ISR
      driven ethernet or MN_POLLED_ETHERNET as required.

   2. Make sure the ETHER_POLL_RECV, ETHER_INIT and ETHER_SEND(p,n) in
      ethernet.h are defined properly.

      #define ETHER_POLL_RECV       smsc91C96_recv()
      #define ETHER_INIT            smsc91C96_init()
      #define ETHER_SEND(p,n)       smsc91C96_send((p),(n))

   3. Set the macro base in physmc65.h to point to the base address for the
      chip.

   4. Check the SMSC_READ and SMSC_WRITE macros to make sure they work
      correctly on your setup.      

   5. If the chip's MAC address is stored in EEPROM set the define
      EEPROM_PRESENT below to 1, otherwise set it to 0. The other defines
      below may be changed if desired but it is probably not necessary.

   6. If ISR driven ethernet is used, add code to the end of the
      smsc91C96_init function to enable the ISR.

   7. Change the function declaration for the ISR function EtherInt to
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

#include "physmc65.h"

#ifdef TQ_ETH
#if (!MN_POLLED_ETHERNET)
#error Only polled ethernet is supported for the TQ Systems version of this driver.
#endif      /* (!MN_POLLED_ETHERNET) */
#endif      /* TQ_ETH */

#define EEPROM_PRESENT  0     /* set to 1 if EEPROM is being used */
#define HARDWARE_PAD    1     /* set to 1 to enable hardware padding */
#define NUM_ALLOC_POLLS 255   /* times to poll if allocate completed */
#define NUM_TXSUC_POLLS 500   /* times to poll for TXSUC */

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

static int allocpkt(word16 total_len) cmx_reentrant;
static void smsc_delay(void) cmx_reentrant;
#if (MN_POLLED_ETHERNET)
static word16 polltxint(void) cmx_reentrant;
#endif      /* (MN_POLLED_ETHERNET) */

/* depending on the hardware used, the macros SMSC_WRITE and SMSC_READ may
   need to be changed.
*/
#if (defined(POL188) || defined(CMX188))
#if (!MN_POLLED_ETHERNET)
void __interrupt __far EtherInt();
#endif      /* (!MN_POLLED_ETHERNET) */
#define EIGHT_BIT_IO 1

#elif (defined(POLH8S2674) || defined(CMXH8S2674))
#undef EEPROM_PRESENT
#define EEPROM_PRESENT  1

#define EIGHT_BIT_IO 0
#define SWAPBYTES(w)    ((word16)(((w)>>8) | ((w)<<8)))
static void smsc_write(word16, word16);
static word16 smsc_read(word16);
#define SMSC_WRITE(OFFSET,DATA)  smsc_write((OFFSET),(DATA)) 
#define SMSC_READ(OFFSET)        smsc_read(OFFSET)

#elif (defined(POLH8S2329) || defined(CMXH8S2329))
#undef EEPROM_PRESENT
#define EEPROM_PRESENT  0

#define EIGHT_BIT_IO 0
#define SWAPBYTES(w)    ((word16)(((w)>>8) | ((w)<<8)))
static void smsc_write(word16, word16);
static word16 smsc_read(word16);
#define SMSC_WRITE(OFFSET,DATA)  smsc_write((OFFSET),(DATA)) 
#define SMSC_READ(OFFSET)        smsc_read(OFFSET)

#elif (defined(POLM16C654) || defined(CMXM16C654))
#if (!MN_POLLED_ETHERNET)
volatile byte link_is_up;
volatile byte nLoopCnt;
#define ISR_LOOP_COUNT  5
#endif

#define EIGHT_BIT_IO 0
extern void m654_init(void);
extern void m654_write(word16, byte);
#pragma PARAMETER m654_write(R0,R1L);
extern byte m654_read(word16);
#pragma PARAMETER m654_read(R0);
static void smsc_write(word16, word16);
static word16 smsc_read(word16);
#define SMSC_WRITE(OFFSET,DATA)  smsc_write((OFFSET),(DATA)) 
#define SMSC_READ(OFFSET)        smsc_read(OFFSET)
#define SMSC_WRITE_DATA(OFFSET,DATA)  smsc_write((OFFSET),(DATA)) 
#define SMSC_READ_DATA(OFFSET)        smsc_read(OFFSET)

#else
#define EIGHT_BIT_IO 0
#define SMSC_WRITE(OFFSET,DATA)  *((word16 far *)((base)+(word16)(OFFSET))) = (DATA)
#define SMSC_READ(OFFSET)        (*((volatile word16 far *)((base)+(word16)(OFFSET))))
#endif      /* POL188 */

#if (EIGHT_BIT_IO)
static void smsc_write(word16, word16) cmx_reentrant;
static word16 smsc_read(word16) cmx_reentrant;
#define SMSC_WRITE(OFFSET,DATA)  smsc_write((OFFSET),(DATA)) 
#define SMSC_READ(OFFSET)        smsc_read(OFFSET)
#endif      /* (EIGHT_BIT_IO) */

#define BANKSWITCH(B)            SMSC_WRITE(BANKSEL,(B))
#define PUT_RECV_BYTE(A)         { *(interface_ptr->recv_in_ptr) = (A); \
   interface_ptr->recv_in_ptr++; \
   if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr) \
      interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr; }

#if (MN_ALLOW_MULTICAST)
#define RCR_SETTING     (STRIPCRC|RXEN|PRMS)
#else
#define RCR_SETTING     (STRIPCRC|RXEN)
#endif

/*********************************************************/

int smsc91C96_init(void)
cmx_reentrant {
   word16 reg_data;
#if (EEPROM_PRESENT)
   word16 mac10,mac32,mac54;
#endif      /* (EEPROM_PRESENT) */
#if (!MN_POLLED_ETHERNET && (defined(POLH8S2674) || defined(CMXH8S2674) || \
      defined(POLH8S2329) || defined(CMXH8S2329)))
   volatile byte junk;
#endif   /* (!MN_POLLED_ETHERNET && (defined(POLH8S2674) || defined(CMXH8S2674))) */

#if (defined(POLM16C654) || defined(CMXM16C654))
#if (!MN_POLLED_ETHERNET)
   link_is_up = FALSE;
#endif

   m654_init();
#endif      /* (defined(POLM16C654) || defined(CMXM16C654)) */

   BANKSWITCH(0);
   SMSC_WRITE(RCR,SOFTRST);   
   smsc_delay();
   SMSC_WRITE(RCR,CLEAR);
   smsc_delay();
   SMSC_WRITE(TCR,CLEAR);

   BANKSWITCH(1);
   /* Note: remove the following two lines if porting to the SMSC91C11x */
   reg_data = SMSC_READ(CONFIG);
   SMSC_WRITE(CONFIG,(reg_data & 0xFeff));      /* use 10BASE-T */

   reg_data = SMSC_READ(CONTROL);
#if (MN_POLLED_ETHERNET)
   SMSC_WRITE(CONTROL,reg_data | AUTORELEASE);  /* auto-release TX packets */
#else
   /* auto-release TX packets, Link error enable and xmit error enable */
   SMSC_WRITE(CONTROL,reg_data | AUTORELEASE | LEENABLE | TEENABLE);
#endif      /* (MN_POLLED_ETHERNET) */

   /* make sure we can read what we just wrote */
   reg_data = SMSC_READ(CONTROL);
   if (!(reg_data & AUTORELEASE))
      return ETHER_INIT_ERROR;

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

   BANKSWITCH(2);
   SMSC_WRITE(MMUCOM,RESETMMU);           /* reset MMU */
   while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */

   reg_data = SMSC_READ(INTERRUPT);
   SMSC_WRITE(INTERRUPT,reg_data & 0x00FF);     /* disable interrupts */

   BANKSWITCH(0);
   SMSC_WRITE(RCR,RCR_SETTING);         /* strip CRC from received packets */

#if (MN_POLLED_ETHERNET)
#if HARDWARE_PAD
   SMSC_WRITE(TCR,PADEN|FDUPLX);                 /* turn on hardware padding */
#endif      /* HARDWARE_PAD */
#else          /* interrupt driven */
#if HARDWARE_PAD
   SMSC_WRITE(TCR,PADEN|TXENA|FDUPLX);        /* turn on hardware padding & xmit */
#else
   SMSC_WRITE(TCR,TXENA|FDUPLX);              /* turn on xmit */
#endif      /* HARDWARE_PAD */
#endif      /* MN_POLLED_ETHERNET */

   BANKSWITCH(2);

#if (!MN_POLLED_ETHERNET)
#if (USE_ALLOC_INT)
   wait_for_alloc = 0;
#endif      /* (USE_ALLOC_INT) */

   /* setup interrupts */
   reg_data = SMSC_READ(INTERRUPT);
   reg_data |= (RXINT_EN | TXINT_EN | RXOVRNINT_EN | EPHINT_EN);
   SMSC_WRITE(INTERRUPT,reg_data);
   smsc_delay();

#if (defined(POL188) || defined(CMX188))
/*   setvect(ETH_INT,EtherInt); */
/*   reg_data = INWORD(IMASK); */
/*   reg_data &= (~0x40); */      /* clear INT2 bit */
/*   OUTWORD(IMASK,reg_data); */
   OUTWORD(I2CON,0x11);          /* enable SMSC91C9x int */
#elif (defined(POLH8S2674) || defined(CMXH8S2674))
   P_INTC.IER.BIT.IRQ0E = 0;     /* disable IRQ0 interrupt */
   junk = P_INTC.ISR.BIT.IRQ0F;
   P_INTC.ISR.BIT.IRQ0F = 0;     /* clear IRQ0 interrupt */
   P_INTC.ISCR.BIT.IRQ0SC = 2;   /* interrupt on rising edge */
   P_INTC.ITSR.BIT.ITS0 = 1;     /* select P80 as input pin */
   P_INTC.IPRA.BIT.UU = 4;       /* set priority to 4 */
   P_INTC.IER.BIT.IRQ0E = 1;     /* enable IRQ0 interrupt */
#elif (defined(POLH8S2329) || defined(CMXH8S2329))
   P_INTC.IER.BIT.IRQ0E = 0;     /* disable IRQ0 interrupt */
   junk = P_INTC.ISR.BIT.IRQ0F;
   P_INTC.ISR.BIT.IRQ0F = 0;     /* clear IRQ0 interrupt */
   P_INTC.ISCR.BIT.IRQ0SC = 2;   /* interrupt on rising edge */
   P_INTC.IPRA.BIT.HIGH = 4;     /* set priority to 4 */
   P_INTC.IER.BIT.IRQ0E = 1;     /* enable IRQ0 interrupt */
#elif (defined(POLM16C654) || defined(CMXM16C654))
   DISABLE_INTERRUPTS;
   INT0IC = 0x14;             /* set interrupt level to 4, rising edge */
//   INT0IC = 0x04;             /* set interrupt level to 4, falling edge */
   ENABLE_INTERRUPTS;
#else
   /* C16x */
   EXICON |= 0x1000;             /* setup fast interrupt 6 */
   DP2   &= 0xBFFF;
   CC14IC = 0x77;                /* IE = 1, ILVL = 13, GLVL = 3 */
#endif
#endif      /* (!MN_POLLED_ETHERNET) */

   /* If porting to the SMC91C11x add code to detect and setup PHY here */

#if (defined(POLM16C654) || defined(CMXM16C654))
#if (!MN_POLLED_ETHERNET)
   link_is_up = TRUE;
#endif
#endif

   return 1;
}

int smsc91C96_send(PSOCKET_INFO socket_ptr, word16 xmit_sock_len)
cmx_reentrant {
   word16 i, data;
   word16 xmit_buff_len;
   word16 total_len;
   int retval;
   byte oddLength;
   byte *MsgSendPointer;
#if (MN_POLLED_ETHERNET)
   int tcrdat;
#endif      /* MN_POLLED_ETHERNET */
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
         oddLength = (byte)(xmit_sock_len & 0x0001);
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

      if (allocpkt(total_len)) 
         {
         retval = total_len;

#if (MN_POLLED_ETHERNET)
         BANKSWITCH(0);
         tcrdat = SMSC_READ(TCR);
         SMSC_WRITE(TCR,tcrdat|TXENA);    /* enable transmit */
#endif      /* MN_POLLED_ETHERNET */

         BANKSWITCH(2);
         SMSC_WRITE(POINTER,TXWR);
         SMSC_WRITE(DATA1,0x0000);        /* status word */

         /* add 6 for status word, byte count and control word */
         SMSC_WRITE(DATA1,total_len+6);

         /* write the header */
         xmit_buff_len >>= 1;
         for (i=0;i<xmit_buff_len;i++)
            {
#if (defined(POLH8S2674) || defined(CMXH8S2674) || defined(POLH8S2329) || \
   defined(CMXH8S2329))
            data = (word16)mn_get_xmit_byte();
            data += LSHIFT8(mn_get_xmit_byte());
#else
            data = LSHIFT8(mn_get_xmit_byte());
            data += (word16)(mn_get_xmit_byte());
            data = hs2net(data);
#endif      /* (defined(POLH8S2674) || defined(CMXH8S2674)) */
            SMSC_WRITE(DATA1,data);
            }

         /* write the data */
         if (xmit_sock_len)
            {
            if (oddLength)
               xmit_sock_len--;     /* need even count for the loop */

            xmit_sock_len >>= 1;
   
            /* write the data */
            for (i=0;i<xmit_sock_len;i++)
               {
#if (defined(POLH8S2674) || defined(CMXH8S2674) || defined(POLH8S2329) || \
   defined(CMXH8S2329))
               data = (word16)(*MsgSendPointer++);
               data += LSHIFT8(*MsgSendPointer++);
#else
               data = LSHIFT8(*MsgSendPointer++);
               data += (word16)(*MsgSendPointer++);
               data = hs2net(data);
#endif      /* (defined(POLH8S2674) || defined(CMXH8S2674)) */
               SMSC_WRITE(DATA1,data);
               }
   
            if (oddLength)
               {
#if (defined(POLH8S2674) || defined(CMXH8S2674) || defined(POLH8S2329) || \
   defined(CMXH8S2329))
               data = (word16)(*MsgSendPointer);
#else
               data = LSHIFT8(*MsgSendPointer);
               data = hs2net(data);
#endif      /* (defined(POLH8S2674) || defined(CMXH8S2674)) */
               SMSC_WRITE(DATA1,data);
               }
            }

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
         SMSC_WRITE(INTERRUPT,(SMSC_READ(INTERRUPT) | TXEMPTYINT_EN));
#endif      /* (!MN_POLLED_ETHERNET) */

         /* queue it up */
         SMSC_WRITE(MMUCOM,QUEUETX);

#if (MN_POLLED_ETHERNET)
         if (polltxint() != TXSUC)
            retval = ETHER_SEND_ERROR;

         BANKSWITCH(0);
         SMSC_WRITE(TCR,tcrdat);
         BANKSWITCH(2);
#endif      /* (MN_POLLED_ETHERNET) */
         }
      else
         retval = ETHER_SEND_ERROR;

      }

   return (retval);
}

#if (MN_POLLED_ETHERNET)
/* receives a frame in polled mode. returns number of bytes received if
   successful or negative number on error.
   Copies entire frame including ethernet header into the receive buffer.
*/
int smsc91C96_recv(void)
cmx_reentrant {
   word16 recv_status;
   word16 recv_len, i, tdata;
#if (MN_ETHER_WAIT_TICKS)
   TIMER_INFO_T wait_timer;
#endif
   int retval;
   int oddLength;
   word16 intr_status;
   PINTERFACE_INFO interface_ptr;

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
#if 0
         BANKSWITCH(0);
         SMSC_WRITE(RCR,RCR_SETTING);      /* enable receive */
         BANKSWITCH(2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RESETMMU);        /* reset MMU */
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         MN_XMIT_BUSY_CLEAR;
#endif
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
      retval = ETHER_RECV_ERROR;
      }
   else if ( intr_status & RXINT )
      retval = 1;
#endif

   if (retval == 1)
      {
      if ( (REMPTY & SMSC_READ(FIFOPORTS)) != REMPTY )
         {
         SMSC_WRITE(POINTER,RXRD);
         recv_status = (SMSC_READ(DATA1));
         recv_len = 0x0fff & SMSC_READ(DATA1);

         /* make sure there were no errors in the packet */
         if ((recv_status & (BADCRC|ALGNERR|TOOSHORT|TOOLONG)) || \
               recv_len < 6)
            retval = ETHER_RECV_ERROR;
         else
            {
            /* subtract for status, byte count, and control word. */
            recv_len -= 6;

            oddLength = recv_status & ODDFRM;
            /* adjust length for odd byte */
/*            recv_len = oddLength ? recv_len + 1 : recv_len; */
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

               recv_len >>= 1;

               for (i=0; i<recv_len; i++)
                  {
                  tdata = SMSC_READ(DATA1);
#if (MN_LITTLE_ENDIAN || defined(POLH8S2674) || defined(CMXH8S2674) || \
   defined(POLH8S2329) || defined(CMXH8S2329))
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
#if (MN_LITTLE_ENDIAN || defined(POLH8S2674) || defined(CMXH8S2674) || \
   defined(POLH8S2329) || defined(CMXH8S2329))
                  PUT_RECV_BYTE(LOWBYTE(tdata));
#else
                  PUT_RECV_BYTE(HIGHBYTE(tdata));
#endif
                  }

               interface_ptr->recv_count += retval + 2;
               }
            else           /* not enough room to hold this packet */
               retval = ETHER_RECV_ERROR;
            }

         BANKSWITCH(2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RELERX); 
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */

         MN_TASK_UNLOCK;
         }
      else
         retval = 0;
      }
   else if (retval == 0)
      retval = SOCKET_TIMED_OUT;

   return (retval);
}

#else       /* interrupt driven */
#if (RTOS_USED == RTOS_NONE)
#if defined(__C166__)      /* Keil C16x */
void EtherInt(void) interrupt 0x1E using ETHREGS
#elif defined(_C166)       /* Tasking C16x */
interrupt (0x1E) using (ETHREGS) void EtherInt(void)
#elif (defined(POL188) || defined(CMX188))
void __interrupt __far EtherInt()
#elif (defined(POLH8S2674) || defined(CMXH8S2674))
#if (defined(HITACHI_H8))
#pragma interrupt(EtherInt)
/* need to setup interrupt vector in startup code */
void EtherInt(void)
#endif      /* HITACHI_H8 */
#elif (defined(POLH8S2329) || defined(CMXH8S2329))
#if (defined(HITACHI_H8))
__interrupt(vect=16) void EtherInt(void)
#endif      /* HITACHI_H8 */
#elif (defined(NC30))            /* Mitsubishi M16C */
void EtherInt(void);
#pragma INTERRUPT EtherInt
void EtherInt(void)
#endif      /* defined(__C166__) */
#else
#if defined(__C166__)      /* Keil C16x */
void EtherInt(void) cmx_reentrant
#elif (defined(POL188) || defined(CMX188))
void __far EtherInt() cmx_reentrant
#elif (defined(POLH8S2674) || defined(CMXH8S2674))
#if (defined(HITACHI_H8))
void ether_isr(void) cmx_reentrant
#endif      /* HITACHI_H8 */
#elif (defined(POLH8S2329) || defined(CMXH8S2329))
#if (defined(HITACHI_H8))
void ether_isr(void)
#endif      /* HITACHI_H8 */
#elif (defined(NC30))            /* Mitsubishi M16C */
void EtherInt(void);
void EtherInt(void)
#endif      /* defined(__C166__) */
#endif      /* (RTOS_USED == RTOS_NONE) */
{
   word16 oldbank;
   word16 pntr, mask_status, pnr;
   word16 intr_status, reg_data, eph_status;
   word16 recv_status;
   word16 recv_len, i, tdata, temp_len;
   int oddLength;
#if (!MN_POLLED_ETHERNET && (defined(POLH8S2674) || defined(CMXH8S2674) || \
      defined(POLH8S2329) || defined(CMXH8S2329)))
   volatile byte junk;
#endif   /* (!MN_POLLED_ETHERNET && (defined(POLH8S2674) || defined(CMXH8S2674))) */
   PINTERFACE_INFO interface_ptr;

#if (defined(POLH8S2674) || defined(CMXH8S2674) || \
      defined(POLM16C654) || defined(CMXM16C654))
   DISABLE_INTERRUPTS;
#endif

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

#if (defined(POLH8S2674) || defined(CMXH8S2674) || \
      defined(POLH8S2329) || defined(CMXH8S2329))
   junk = P_INTC.ISR.BIT.IRQ0F;
   P_INTC.ISR.BIT.IRQ0F = 0;     /* clear IRQ0 interrupt */
#endif

   oldbank = SMSC_READ(BANKSEL) & 0x0007; /* get current bank */

   BANKSWITCH(2);

   /* read interrupt status & mask, status in low byte, mask in high byte */
   mask_status = SMSC_READ(INTERRUPT);
   SMSC_WRITE(INTERRUPT,mask_status & 0x00FF);  /* clear mask and ack interrupts */

   pntr = SMSC_READ(POINTER);

   /* get the valid interrupts to process */
   intr_status = (mask_status & 0x00FF) & (mask_status>>8);
   mask_status &= 0xFF00;

#ifdef ISR_LOOP_COUNT
   nLoopCnt = ISR_LOOP_COUNT;
#endif

   while (intr_status)
      {
#if 0
      if (intr_status & RXOVRNINT)    /* receive overrun */
         {
         BANKSWITCH(0);
         SMSC_WRITE(RCR,RCR_SETTING);      /* enable receive */
         BANKSWITCH(2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RESETMMU);        /* reset MMU */
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         intr_status = SMSC_READ(INTERRUPT) & (mask_status >> 8);
#if (USE_ALLOC_INT)
         intr_status |= ALLOCINT;            /* force reset of alloc int */
#endif      /* (USE_ALLOC_INT) */
         intr_status |= TXEMPTYINT;          /* force reset of txempty int */
         }
#endif

      /* receiver interrupt */
      if (intr_status & RXINT)
         {
         if ( (REMPTY & SMSC_READ(FIFOPORTS)) != REMPTY )
            {
            SMSC_WRITE(POINTER,RXRD);
            recv_status = (SMSC_READ(DATA1));
            recv_len = 0x0fff & SMSC_READ(DATA1);

            /* make sure there were no errors in the packet */
            if (!(recv_status & (BADCRC|ALGNERR|TOOSHORT|TOOLONG)) && \
                  recv_len >= 6)
               {
               /* subtract for status, byte count, and control word. */
               recv_len -= 6;

               oddLength = recv_status & ODDFRM;
               /* adjust length for odd byte */
/*               recv_len = oddLength ? recv_len + 1 : recv_len; */
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

                  recv_len >>= 1;

                  for (i=0; i<recv_len; i++)
                     {
                     tdata = SMSC_READ(DATA1);
#if (MN_LITTLE_ENDIAN || defined(POLH8S2674) || defined(CMXH8S2674) || \
   defined(POLH8S2329) || defined(CMXH8S2329))
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
#if (MN_LITTLE_ENDIAN || defined(POLH8S2674) || defined(CMXH8S2674) || \
   defined(POLH8S2329) || defined(CMXH8S2329))
                     PUT_RECV_BYTE(LOWBYTE(tdata));
#else
                     PUT_RECV_BYTE(HIGHBYTE(tdata));
#endif
                     }

                  interface_ptr->recv_count += temp_len + 2;

                  /* tell upper layers we got a packet */
                  MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE);
                  }
               }
            BANKSWITCH(2);
            while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
            SMSC_WRITE(MMUCOM,RELERX);
            while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
            }
         }

      /* transmit empty interrupt - transmitter is done sending */
      if (intr_status & TXEMPTYINT)
         {
         MN_XMIT_BUSY_CLEAR;
         MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
         mask_status &= (word16)(~TXEMPTYINT_EN);
         }

#if (USE_ALLOC_INT)
      if (intr_status & ALLOCINT)
         {
         /* disable ALLOCINT interrupt and signal the allocpkt function */
         /* change the mask_status variable, not the mask register directly */
         wait_for_alloc = 0;
         mask_status &= (word16)(~ALLOCINT_EN);
         }
#endif      /* (USE_ALLOC_INT) */

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
            BANKSWITCH(0);
            reg_data = SMSC_READ(TCR);
#if HARDWARE_PAD
            reg_data |= (PADEN|TXENA);       /* turn on xmit & padding */
#else
            reg_data |= (TXENA);             /* turn on xmit */
#endif      /* HARDWARE_PAD */
            SMSC_WRITE(TCR,reg_data);
            }

         /* remove the bad packet */
         BANKSWITCH(2);
         while ( SMSC_READ(MMUCOM) & BUSY ); /* wait for MMU to become not busy */
         SMSC_WRITE(MMUCOM,RELEPNR); 

         /* restore the packet number register */
         reg_data = SMSC_READ(ARRPNR) & ARR_MASK;     /* get ARR reg */
         reg_data |= pnr;
         SMSC_WRITE(ARRPNR,reg_data);
         MN_XMIT_BUSY_CLEAR;
         MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
         mask_status &= (word16)(~TXEMPTYINT_EN);
         }

      /* EPH interrupt - some error occurred */
      if (intr_status & EPHINT)
         {
#if (USE_ALLOC_INT)
         wait_for_alloc = 0;
         mask_status &= ~ALLOCINT_EN;
#endif      /* (USE_ALLOC_INT) */
         BANKSWITCH(0);
         eph_status = SMSC_READ(STATUS);

#if (defined(POLM16C654) || defined(CMXM16C654))
#if (!MN_POLLED_ETHERNET)
         if (eph_status & LINKOK)
            link_is_up = TRUE;
         else
            link_is_up = FALSE;
#endif
#endif

         /* ack link up/down */
         BANKSWITCH(1);
         reg_data = SMSC_READ(CONTROL);
         reg_data &= (word16)(~LEENABLE);
         SMSC_WRITE(CONTROL,reg_data);
         reg_data |= LEENABLE;
         SMSC_WRITE(CONTROL,reg_data);

         BANKSWITCH(2);
         }

      intr_status = SMSC_READ(INTERRUPT) & (mask_status >> 8);

#ifdef ISR_LOOP_COUNT
      nLoopCnt--;
      if (!nLoopCnt)
         intr_status = 0;
#endif
      }

   /* restore registers */
   SMSC_WRITE(INTERRUPT,mask_status);
   SMSC_WRITE(POINTER,pntr);
   BANKSWITCH(oldbank);

#if (defined(POL188) || defined(CMX188))
   OUTWORD(EOI_REGISTER,GENERAL_ACK);
#endif

#if (defined(POLH8S2674) || defined(CMXH8S2674) || \
      defined(POLM16C654) || defined(CMXM16C654))
   ENABLE_INTERRUPTS;
#endif
}
#endif      /* MN_POLLED_ETHERNET */

/*************************************************************************
 delay function
 *************************************************************************/
static void smsc_delay(void)
cmx_reentrant {
   volatile word16 junk;
   junk = SMSC_READ(4);
   junk = SMSC_READ(4);
   junk = SMSC_READ(4);
}

/********************************************************
   allocate transmit packet. if successful, returns (1)
   if failure, returns 0
********************************************************/
static int allocpkt(word16 total_len)
cmx_reentrant {
   word16 num_pages,i,txpktno;
#if (!MN_POLLED_ETHERNET && USE_ALLOC_INT)
   TIMER_INFO_T wait_timer;
#endif      /* (!MN_POLLED_ETHERNET) */

   /* add 6 for status word, byte count and control word */
  num_pages = (total_len+6) >> 8;

  SMSC_WRITE(MMUCOM,(ALLOCTX|num_pages));

  for (i=0;i<NUM_ALLOC_POLLS;i++)  
  {
    if ( (ALLOCINT & SMSC_READ(INTERRUPT) ) == ALLOCINT)  
    {
      txpktno =  ( (0x3f00 & SMSC_READ(ARRPNR) ) >> 8 ); 
      SMSC_WRITE(ARRPNR,txpktno);
      return(1);
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
#if (RTOS_USED != RTOS_NONE)
      MN_TASK_WAIT(1);
#endif      /* (RTOS_USED != RTOS_NONE) */
      }
#endif      /* (!MN_POLLED_ETHERNET && USE_ALLOC_INT) */

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

   BANKSWITCH(0);  

   for (i=0; i<NUM_TXSUC_POLLS; i++) 
      {
      if ( (TXSUC & SMSC_READ(STATUS)) == TXSUC ) 
         {
         BANKSWITCH(2);
         return(TXSUC);
         } 
      smsc_delay();
      }

   statusdat=(SMSC_READ(STATUS));
   stpsqet=(STPSQET & SMSC_READ(TCR));
   moncsn=(MONCSN & SMSC_READ(TCR));
   fatalerr = (statusdat & (stpsqet|moncsn|TXUNRN|SIXTEENCOL|LATCOL) );
   BANKSWITCH(2);  

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

#if (EIGHT_BIT_IO)
/* output a word to the chip low byte first then high byte */
static void smsc_write(word16 offset, word16 tdata)
cmx_reentrant {
   OUTBYTE( (base+offset), LOWBYTE(tdata) );
   OUTBYTE( (base+offset+1), HIGHBYTE(tdata) );
}

/* input a word from the chip low byte first then high byte */
static word16 smsc_read(word16 offset)
cmx_reentrant {
   word16 retval;

   retval = INBYTE(base+offset);
   retval += LSHIFT8(INBYTE(base+offset+1));

   return (retval);
}
#endif      /* (EIGHT_BIT_IO) */

#if (defined(POLH8S2674) || defined(CMXH8S2674) || \
   defined(POLH8S2329) || defined(CMXH8S2329))
static void smsc_write(word16 offset, word16 tdata)
cmx_reentrant {
   tdata = SWAPBYTES(tdata);
   *((word16 *)(base+(word16)(offset))) = tdata;
}

static word16 smsc_read(word16 offset)
cmx_reentrant {
   word16 retval;

   retval = *((volatile word16 *)(base+(word16)(offset)));
   retval = SWAPBYTES(retval);

   return (retval);
}
#endif      /* (defined(POLH8S2674) || defined(CMXH8S2674)) */

#if (defined(POLM16C654) || defined(CMXM16C654))
/* output a word to the chip low byte first then high byte */
static void smsc_write(word16 offset, word16 tdata)
{
   DISABLE_INTERRUPTS;
   /* Slow the clock speed when accessing the ethernet chip */
   PRCR = 0x03;   // Disable protect register
   CM1  = 0xA0;      // BCLK =  f1/4
   PRCR = 0x00;   // Enable protect register
   m654_write( (base+offset), LOWBYTE(tdata) );
   m654_write( (base+offset+1), HIGHBYTE(tdata) );
   PRCR = 0x03;   // Disable protect register
   CM1  = 0x20;      // BCLK =  f1/1
   PRCR = 0x00;   // Enable protect register
   ENABLE_INTERRUPTS;
}

/* input a word from the chip low byte first then high byte */
static word16 smsc_read(word16 offset)
{
   word16 retval;

   DISABLE_INTERRUPTS;
   /* Slow the clock speed when accessing the ethernet chip */
   PRCR = 0x03;   // Disable protect register
   CM1  = 0xA0;      // BCLK =  f1/4
   PRCR = 0x00;   // Enable protect register
   retval = m654_read(base+offset);
   retval += LSHIFT8(m654_read(base+offset+1));
   PRCR = 0x03;   // Disable protect register
   CM1  = 0x20;      // BCLK =  f1/1
   PRCR = 0x00;   // Enable protect register
   ENABLE_INTERRUPTS;

   return (retval);
}
#endif      /* (defined(POLM16C654) || defined(CMXM16C654)) */
#endif      /* (MN_ETHERNET) */

