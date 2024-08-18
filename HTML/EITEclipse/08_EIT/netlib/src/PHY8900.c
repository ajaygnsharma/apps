/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
/* 16-bit IO-mode driver for CS8900A chip and Phytec AT91M55800 board.
   
   Added Phytec XC161 board.
*/

#include "micronet.h"

/*
   Porting Notes:

   1. Make sure that mnconfig.h is configured properly for either ISR
      driven ethernet or MN_POLLED_ETHERNET as required.

   2. Make sure the ETHER_POLL_RECV, ETHER_INIT and ETHER_SEND(p,n) in
      ethernet.h are defined properly.

      #define ETHER_POLL_RECV       cs8900_recv()
      #define ETHER_INIT            cs8900_init()
      #define ETHER_SEND(p,n)       cs8900_send((p),(n))

   3. Set the macro BASE_ADDR below to point to the base address for the chip.

   4. Check the readWord16 and writeWord16 macros and the readPacketPage16
      and writePacketPage16 functions to make sure they work correctly
      on your setup.      

   5. Add code at the start of cs8900_init to do a hardware reset if needed.

   6. If ISR driven ethernet is used, add code to the end of the cs8900_init
      function to enable the ISR.

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

/* CS8900 definitions */
#include "cs8900a.h"

/* Hardware-specific definitions */
#if (defined(POLARME) || defined(CMXARME) || defined(POLARM) || defined(CMXARM))
#define BASE_ADDR                0x2000300
#define readWord16(Padd)         (*((word16 volatile *)((BASE_ADDR) + (Padd))))
#define writeWord16(Padd, Pdata) (*((word16 volatile *)((BASE_ADDR) + (Padd)))) = (Pdata)
#define DO_DEBUG  0

#if (!MN_POLLED_ETHERNET)
extern void IRQ_Wrapper(void);
#endif      /* (!MN_POLLED_ETHERNET) */

#elif (defined(POLXC161) || defined(CMXXC161))
#include <absacc.h>

#define BASE_ADDR                0x400300
#define readWord16(Padd)         (HVAR(unsigned int,((Padd) + BASE_ADDR)))
#define writeWord16(Padd, Pdata) (HVAR(unsigned int,((Padd) + BASE_ADDR))) = (Pdata);
#define DO_DEBUG  0
#define ADDMUL 0

#if (!MN_POLLED_ETHERNET)
#error Only polled ethernet is supported for the XC161 version of this driver.
#endif      /* (!MN_POLLED_ETHERNET) */

#elif (defined(POLC167HSE) || defined(CMXC167HSE))
#define BASE_ADDR                0x380300
#define readWord16(Padd)         (*((word16 volatile huge *)(BASE_ADDR + (Padd))))
#define writeWord16(Padd, Pdata) (*((word16 volatile huge *)(BASE_ADDR + (Padd)))) = (Pdata)
#define DO_DEBUG  0

#else
#error Need to define BASE_ADDR, readWord16 and writeWord16
#endif      /* (defined(POLARME) || defined(CMXARME)) */

#ifndef DO_DEBUG
#define DO_DEBUG  0
#endif

#if DO_DEBUG
void DiagSend(char *Str);

char Str[25];
char str1[] = "No 3000h\r\n";
char str2[] = "No 630Eh\r\n";
char str3[] = "No 0300h\r\n";
char str4[] = "Write Failed\r\n";
char str5[] = "Init Ok\r\n";
char str6[] = "No Recv\r\n";
char str7[] = "No Room\r\n";
char str8[] = "Recv OK\r\n";
char str9[] = "Send T.O.\r\n";
char str10[] = "Bid err\r\n";
char str11[] = "Send too big\r\n";
char str12[] = "Unknown ISQ\r\n";
char str13[] = "TX done ISQ\r\n";
char str14[] = "Send OK\r\n";
#endif      /* DO_DEBUG */

static word16 readPacketPage16(word16 PPadd);
static void writePacketPage16(word16 PPadd, word16 PPdata);
static void cs8900_reset(void) cmx_reentrant;
static void dummy(word16);

#if (!MN_POLLED_ETHERNET)
static volatile byte ready4tx;
#endif

#if (MN_ALLOW_MULTICAST)
#define RX_SETTING   (CS_RX_OK_A|CS_PROMISCUOUS_A|CS_BROADCAST_A)
#else
#define RX_SETTING   (CS_RX_OK_A|CS_INDIVIDUAL_A|CS_BROADCAST_A)
#endif

#define EEPROM_PRESENT  1     /* set to 1 if EEPROM is being used */

/**************************************************************** */
/* functions */
/**************************************************************** */

/* initialization. returns 1 if ok or negative number on error. */
int cs8900_init(void)
cmx_reentrant {
#if (EEPROM_PRESENT)
   word16 mac10,mac32,mac54;
#endif      /* (EEPROM_PRESENT) */

#if DO_DEBUG
   /* set for USART0 in polled mode */
   APMC_PCER  = 0x04;                  /* enable peripheral clock */
   PIOA_PDR   = 0x00018000;            /* Enable RXD0 and TXD0 */
   US0_RCR    = 0x00;                  /* clear counter registers */
   US0_TCR    = 0x00;
   US0_BRGR   = 208;                   /* 9600 baud at 32 MHz */
   US0_TTGR   = 0x10;                  /* set transmitter time guard */
   US0_MR     = 0x000008C0;            /* 8 bits, no Parity, 1 Stop bit */
   US0_CR     = 0x00000050;            /* Enable RX and TX */  
#endif      /* DO_DEBUG */

   cs8900_reset();

#if 0    /* This doesn't work for the Phytec board. */
   /* Check for presence of CS8900A by reading 3000h */
   /* from the PacketPage pointer */
   if (readWord16(PP_POINTER_PORT) != CS8900A_SIG)
      {
#if ( DO_DEBUG )
      strcpy(Str,str1);
      DiagSend(Str);
#endif
      return ETHER_INIT_ERROR;
      }
#endif

   /* Check for EISA number to be 0x630E */
   if (readPacketPage16(PP_ProductID) != EISA_REG_NUM) 
      {
#if ( DO_DEBUG )
      strcpy(Str,str2);
      DiagSend(Str);
#endif
      return ETHER_INIT_ERROR;
      }

   /* Check for Base I/O address to be 300h */
   if (readPacketPage16(PP_IOBaseAddr) != DEFAULT_BASE) 
      {
#if ( DO_DEBUG )
      strcpy(Str,str3);
      DiagSend(Str);
#endif
      return ETHER_INIT_ERROR;
      }

   /* If the code has gotten this far, then basic I/O is working between */
   /* the processor and the CS8900A is working */

   /* Set the Interrupt number to be 0 */
   writePacketPage16(PP_IntNum, 0);

   /* Verify that the PacketPage memory can be written to */
   if ((readPacketPage16(PP_IntNum) & 0x000f) != 0)
      {
#if ( DO_DEBUG )
      strcpy(Str,str4);
      DiagSend(Str);
#endif
      return ETHER_INIT_ERROR;
      }

#if (EEPROM_PRESENT)
   mac10 = readPacketPage16(PP_IndAddr);
   mac32 = readPacketPage16(PP_IndAddr2);
   mac54 = readPacketPage16(PP_IndAddr4);
   MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
#if (MN_LITTLE_ENDIAN)
   eth_src_hw_addr[0] = LOWBYTE(mac10);
   eth_src_hw_addr[1] = HIGHBYTE(mac10);
   eth_src_hw_addr[2] = LOWBYTE(mac32);
   eth_src_hw_addr[3] = HIGHBYTE(mac32);
   eth_src_hw_addr[4] = LOWBYTE(mac54);
   eth_src_hw_addr[5] = HIGHBYTE(mac54);
#else
   eth_src_hw_addr[0] = HIGHBYTE(mac10);
   eth_src_hw_addr[1] = LOWBYTE(mac10);
   eth_src_hw_addr[2] = HIGHBYTE(mac32);
   eth_src_hw_addr[3] = LOWBYTE(mac32);
   eth_src_hw_addr[4] = HIGHBYTE(mac54);
   eth_src_hw_addr[5] = LOWBYTE(mac54);
#endif      /* MN_LITTLE_ENDIAN */
   MN_MUTEX_RELEASE(MUTEX_MN_INFO);
#else
   MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
   /* write the MAC address to Individual Address */
   /* Important: The IA needs to be byte reversed */
   writePacketPage16(PP_IndAddr, MK_WORD16(eth_src_hw_addr[1],eth_src_hw_addr[0]));
   writePacketPage16(PP_IndAddr2, MK_WORD16(eth_src_hw_addr[3],eth_src_hw_addr[2]));
   writePacketPage16(PP_IndAddr4, MK_WORD16(eth_src_hw_addr[5],eth_src_hw_addr[4]));
   MN_MUTEX_RELEASE(MUTEX_MN_INFO);
#endif      /* (EEPROM_PRESENT) */

#if (MN_POLLED_ETHERNET)
   /* setup for polled mode */
   writePacketPage16(PP_BufCFG, 0);
   writePacketPage16(PP_RxCFG, 0);                 /* No Rx interrupt */
   writePacketPage16(PP_RxCTL, RX_SETTING);
   writePacketPage16(PP_TxCFG, 0);                 /* No Tx interrupt */

   writePacketPage16(PP_TestCTL, 0);      /* half Duplex */
   writePacketPage16(PP_BusCTL, 0);       /* No interrupts */

   /* enable receive and transmit */
   writePacketPage16(PP_LineCTL, (CS_SER_RX_ON|CS_SER_TX_ON) );
#else
   ready4tx = FALSE;

   /* setup for interrupt driven mode */
   writePacketPage16(PP_BufCFG, MN_BUFCFG_IE);
   writePacketPage16(PP_RxCFG, CS_RX_OK_IE);
   writePacketPage16(PP_RxCTL, RX_SETTING);
   writePacketPage16(PP_TxCFG, CS_TX_ALL_IE);

/*   writePacketPage16(PP_TestCTL, CS_FDX); */     /* full Duplex */
   writePacketPage16(PP_TestCTL, 0);               /* half Duplex */
   writePacketPage16(PP_BusCTL, CS_ENABLE_IRQ);    /* Enable interrupts */

   /* enable receive and transmit */
   writePacketPage16(PP_LineCTL, (CS_SER_RX_ON|CS_SER_TX_ON) );

#if (defined(POLARME) || defined(CMXARME) || defined(POLARM) || defined(CMXARM))
   /* Enable ethernet interrupt */
   AIC_IDCR  = (1 << IRQ2_ID);         /* disable IRQ2 interrupt */
   AIC_ICCR  = (1 << IRQ2_ID);         /* clear IRQ2 interrupt */
   PIOA_PDR  = (1 << 11);              /* enable peripheral control */
   AIC_SVR27 = (word32)IRQ_Wrapper;    /* vector to irq isr */
   AIC_SMR27 = 0x45;                   /* high-level sensitive, priority = 5 */
   AIC_IECR  = (1 << IRQ2_ID);         /* enable IRQ2 interrupt */
#elif (defined(POLC167HSE) || defined(CMXC167HSE))
   /* setup cs8900 interrupt */
   DISABLE_INTERRUPTS;
   P2 |= 0x8000;                 /* setup fast interrupt 7 */
   DP2 &= 0x7FFF;
   CCM3 = 0x1000;
   CC15IC = 0x6B;                /* IE = 1, ILVL = 10, GLVL = 3 */
   ENABLE_INTERRUPTS;
#endif

#endif      /* (MN_POLLED_ETHERNET) */

#if ( DO_DEBUG )
   strcpy(Str,str5);
   DiagSend(Str);
#endif
   return 1;
}

/* Transmit a frame in polled mode. The ethernet header must in the frame
   to be sent. Returns the number of bytes sent or negative number on error.
   The ethernet header and other header info is in the xmit_buff. The TCP
   or UDP data is pointed to by socket_ptr->send_ptr. xmit_sock_len is the
   number of data bytes to be sent in the current packet.
*/
int cs8900_send(PSOCKET_INFO socket_ptr, word16 xmit_sock_len)
cmx_reentrant {
   word16 i, data;
   word16 xmit_buff_len;
   word16 total_len;
   int retval;
   byte oddLength;
   TIMER_INFO_T wait_timer;
   byte *MsgSendPointer;
   PINTERFACE_INFO interface_ptr;

   retval = 0;
   oddLength = FALSE;
   MsgSendPointer = BYTE_PTR_NULL;
   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   /* make sure there is something to send */
   if (interface_ptr->send_out_ptr != interface_ptr->send_in_ptr)
      {
      /* calculate the number of bytes to send */
      xmit_buff_len = interface_ptr->send_in_ptr - interface_ptr->send_out_ptr;

      if (xmit_sock_len && socket_ptr->send_ptr != PTR_NULL)
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
      if (total_len > ETHER_SIZE)
         {
#if ( DO_DEBUG )
         strcpy(Str,str11);
         DiagSend(Str);
#endif
         return (ETHER_SEND_ERROR);
         }

      writeWord16(TX_CMD_PORT, CS_TX_START_ALL);   /* bid for frame storage */
      writeWord16(TX_LENGTH_PORT, total_len);         /* write the length */
   
      /* wait for TX ready */
      mn_reset_timer(&wait_timer,(MN_ETHER_WAIT_TICKS));
      while (!mn_timer_expired(&wait_timer))
         {
#if 1    /* MN_POLLED_ETHERNET */
         if (readPacketPage16(PP_BusST) & CS_RDY_4_TX_NOW)
#else
         /* the CS_RDY_4_TX interrupt is not working so we need to
            poll above for now.
         */
         if (ready4tx)
#endif
            {
            retval = (int)total_len;
            break;
            }
         }
   
      if (!retval)
         {
#if ( DO_DEBUG )
         strcpy(Str,str9);
         DiagSend(Str);
#endif
         return (UNABLE_TO_SEND);
         }
   
      if ((readPacketPage16(PP_BusST) & CS_TX_BID_ERR))
         {
#if ( DO_DEBUG )
         strcpy(Str,str10);
         DiagSend(Str);
#endif
         return (ETHER_SEND_ERROR);
         }

      /* write the headers */
      for (i=0;i<xmit_buff_len;i += 2)
         {
         data = LSHIFT8(mn_get_xmit_byte());
         data += (word16)(mn_get_xmit_byte());
         data = hs2net(data);
         writeWord16(XMIT_PORT,data);
         }

      if (oddLength)
         xmit_sock_len--;     /* need even count for the loop */
   
      /* write the data */
      for (i=0;i<xmit_sock_len;i += 2)
         {
         data = LSHIFT8(*MsgSendPointer++);
         data += (word16)(*MsgSendPointer++);
         data = hs2net(data);
         writeWord16(XMIT_PORT,data);
         }
   
      if (oddLength)
         {
         data = LSHIFT8(*MsgSendPointer);
         data = hs2net(data);
         writeWord16(XMIT_PORT,data);
         }

#if (MN_POLLED_ETHERNET)
      while (1)
         {
         i = readPacketPage16(PP_TxEvent);
         if (i & CS_TX_ERROR)
            {
            retval = ETHER_SEND_ERROR;
            break;
            }
         else if (i & CS_TX_OK)
            {
#if ( DO_DEBUG )
            strcpy(Str,str14);
            DiagSend(Str);
#endif
            break;
            }
         }

#elif ( DO_DEBUG )
      strcpy(Str,str14);
      DiagSend(Str);
#endif

      }

   return (retval);
}

#if (MN_POLLED_ETHERNET)
/* receives a frame in polled mode. returns number of bytes received if
   successful or negative number on error.
   Copies entire frame including ethernet header into the receive buffer.
*/
int cs8900_recv(void)
cmx_reentrant {
   word16 recv_status, recv_len, i;
#if (MN_ETHER_WAIT_TICKS)
   TIMER_INFO_T wait_timer;
#endif
   int retval;
   int oddLength;
   word16 data_word;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   retval = 0;

   if (!(readPacketPage16(PP_RxEvent) & CS_RX_OK))
      {
#if (MN_ETHER_WAIT_TICKS)
      mn_reset_timer(&wait_timer,(MN_ETHER_WAIT_TICKS));
      while (!mn_timer_expired(&wait_timer))
         {
         if (readPacketPage16(PP_RxEvent) & CS_RX_OK)
            {
            retval = 1;
            break;
            }
#if (RTOS_USED != RTOS_NONE)
         MN_TASK_WAIT(1);
#endif      /* (RTOS_USED != RTOS_NONE) */
         }
      if (!retval)
#endif
         return (SOCKET_TIMED_OUT);
      }

   recv_status = readWord16(RECV_PORT);
   dummy(recv_status);  /* force compiler to not ignore recv_status */
   recv_len = readWord16(RECV_PORT);
   oddLength = recv_len & 0x01;

   if ( (interface_ptr->recv_buff_size - interface_ptr->recv_count - 2) >= recv_len && \
         (recv_len >= MIN_PKT_SIZE))
      {
      retval = recv_len;

      MN_TASK_LOCK;

      /* put recv_len into buffer so all packets start with the length.
         Do NOT remove the next two lines.
      */
      mn_put_recv_byte(HIGHBYTE(recv_len));
      mn_put_recv_byte(LOWBYTE(recv_len));

      if (oddLength)
         recv_len--;    /* need even count for the loop */

      for (i=0; i<recv_len;i += 2)
         {
         data_word = readWord16(RECV_PORT);

#if (MN_LITTLE_ENDIAN)
         mn_put_recv_byte(LOWBYTE(data_word));
         mn_put_recv_byte(HIGHBYTE(data_word));
#else
         mn_put_recv_byte(HIGHBYTE(data_word));
         mn_put_recv_byte(LOWBYTE(data_word));
#endif
         }

      if (oddLength)    /* get the last byte */
         {
         data_word = readWord16(RECV_PORT);

#if (MN_LITTLE_ENDIAN)
         mn_put_recv_byte(LOWBYTE(data_word));
#else
         mn_put_recv_byte(HIGHBYTE(data_word));
#endif
         }

      MN_TASK_UNLOCK;
      }
   else
      {
      /* not enough room to hold this packet so get rid of it */
      writePacketPage16(PP_RxCFG, CS_SKIP_1);            /* skip this frame */
      retval = ETHER_RECV_ERROR;
      }

   return (retval);
}

#else
/* !MN_POLLED_ETHERNET */

#if (defined(POLARME) || defined(CMXARME) || defined(POLARM) || defined(CMXARM))
void ether_isr(void)
#elif (defined(POLC167HSE) || defined(CMXC167HSE))
#if (RTOS_USED == RTOS_NONE)
#if defined(__C166__)      /* Keil C16x */
void EtherInt(void) interrupt 0x1F using ETHREGS
#elif defined(_C166)       /* Tasking C16x */
interrupt (0x1F) using (ETHREGS) void EtherInt(void)
#endif
#else
void EtherInt(void) cmx_reentrant
#endif      /* (RTOS_USED == RTOS_NONE) */
#endif      /* (defined(POLARME) || defined(CMXARME)) */
{
   word16 IRQSource;
   word16 recv_status, recv_len, i;
   byte oddLength;
   word16 data_word;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   /* Read Interrupt Service Queue */
   while ((IRQSource = readPacketPage16(PP_ISQ)) != 0)
      {
      switch(IRQSource & 0x003F)
         {
         case 0x04:              /* Receive Interrupt occurred */
            /* IRQSource contains the RxEvent Register contents */

            /* only RxOK should generate a receiver interrupt */
            if ((IRQSource & CS_RX_OK) == 0)
               break;

            recv_status = readWord16(RECV_PORT);
#if ( DO_DEBUG )
            mn_ustoa((byte *)Str,recv_status);
            DiagSend(Str);
            Str[0] = ' ';
            Str[1] = '\0';
            DiagSend(Str);
#else
            dummy(recv_status);  /* force compiler to not ignore recv_status */
#endif
            recv_len = readWord16(RECV_PORT);
            oddLength = (byte)(recv_len & 0x01);
#if ( DO_DEBUG )
            mn_ustoa((byte *)Str,recv_len);
            DiagSend(Str);
            Str[0] = ' ';
            Str[1] = '\0';
            DiagSend(Str);
#endif

            if ( (interface_ptr->recv_buff_size - interface_ptr->recv_count - 2) >= recv_len && \
                  (recv_len >= MIN_PKT_SIZE))
               {
               /* put recv_len into buffer so all packets start with the
                  length. Do NOT remove the next two lines.
               */
               mn_put_recv_byte(HIGHBYTE(recv_len));
               mn_put_recv_byte(LOWBYTE(recv_len));

               if (oddLength)
                  recv_len--;    /* need even count for the loop */

               for (i=0; i<recv_len;i += 2)
                  {
                  data_word = readWord16(RECV_PORT);

#if (MN_LITTLE_ENDIAN)
                  mn_put_recv_byte(LOWBYTE(data_word));
                  mn_put_recv_byte(HIGHBYTE(data_word));
#else
                  mn_put_recv_byte(HIGHBYTE(data_word));
                  mn_put_recv_byte(LOWBYTE(data_word));
#endif
                  }

               if (oddLength)    /* get the last byte */
                  {
                  data_word = readWord16(RECV_PORT);

#if (MN_LITTLE_ENDIAN)
                  mn_put_recv_byte(LOWBYTE(data_word));
#else
                  mn_put_recv_byte(HIGHBYTE(data_word));
#endif
                  }
#if ( DO_DEBUG )
               strcpy(Str,str8);
               DiagSend(Str);
#endif
               /* tell upper layers we got a packet */
               MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE);
               }
            else
               {
               /* not enough room to hold this packet so get rid of it */
#if ( DO_DEBUG )
               strcpy(Str,str7);
               DiagSend(Str);
#endif
               writePacketPage16(PP_RxCFG, CS_SKIP_1);   /* skip this frame */
               }
            break;

         case 0x08:              /* Transmit Interrupt occurred */
            /* IRQSource contains the TxEvent Register contents */
            /* Will get here on TxOK or an error. Either way assume the
               transmit is done.
            */

#if ( DO_DEBUG )
            strcpy(Str,str13);
            DiagSend(Str);
#endif
            MN_XMIT_BUSY_CLEAR;
            MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
            break;

         case 0x0C:              /* Buffer Interrupt occurred */
            /* IRQSource contains the BufEvent Register contents */
            if (IRQSource & CS_RDY_4_TX)
               ready4tx = TRUE;
            else if (IRQSource & CS_TX_UNDERRUN)
               {
               MN_XMIT_BUSY_CLEAR;
               MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
               }
            break;

         case 0x10:              /* Receive Buffer Miss Interrupt occurred */
            /* IRQSource contains the RxMISS Register contents */
            /* read the miss counter to clear it */
            dummy(readPacketPage16(PP_RxMISS));
            break;

         case 0x12:              /* Transmit Collision Counter Interrupt occurred */
            /* IRQSource contains the TxCOLL Register contents */
            /* read the collision counter to clear it */
            dummy(readPacketPage16(PP_TxCOL));
            break;

         default:
            /* This should never occur */
#if ( DO_DEBUG )
            strcpy(Str,str12);
            DiagSend(Str);
#endif
            break;
         }
      }
}
#endif      /* (MN_POLLED_ETHERNET) */

/*********************************************************/
/* Returns the data read from PacketPage address, PPadd */
static word16 readPacketPage16(word16 PPadd)
{
   writeWord16(PP_POINTER_PORT,PPadd);
   return (readWord16(PP_DATA_PORT));
}

/* Writes the data, PPdata, to the PacketPage address, PPadd */
static void writePacketPage16(word16 PPadd, word16 PPdata)
{
   writeWord16(PP_POINTER_PORT,PPadd);
   writeWord16(PP_DATA_PORT,PPdata);
}

static void cs8900_reset(void)
cmx_reentrant {
   /* The code below messes up the ethernet controller when using rev 1179.4
      of the phyCORE-AT91M55800A board. The code works fine when using rev
      1179.3.
   */
#if 0
   writePacketPage16(PP_SelfCTL, CS_RESET);        /* reset command */
   MN_TASK_WAIT(2);

   do
      {
      MN_TASK_WAIT(1);
      /* wait until reset bit is cleared */
      if (readPacketPage16(PP_SelfCTL) & CS_RESET)
         continue;
      }
      /* wait till CS_INITD bit is set */
   while (!(readPacketPage16(PP_SelfST) & CS_INITD));
#endif
}

/* Needed to force compiler to not ignore recv_status above */
static void dummy(word16 d)
{
   d=d;
}

#if DO_DEBUG
void DiagSend(char *Str)
{
   char i;

   i = 0; 
   
   while(Str[i] != 0)
      {   
#if (defined(POLARME) || defined(CMXARME) || defined(POLARM) || defined(CMXARM))
      while (!(US0_CSR & US_TXRDY));
      US0_THR = Str[i++];
      while (!(US0_CSR & US_TXEMPTY));
#endif      /* (defined(POLARME) || defined(CMXARME)) */
      }  
}
#endif      /* DO_DEBUG */

#endif      /* (MN_ETHERNET) */

