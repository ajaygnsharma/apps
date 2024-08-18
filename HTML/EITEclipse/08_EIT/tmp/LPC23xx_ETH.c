/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

/* Ethernet driver for Keil LPC2368 board with DP83848 PHY.
   Added support for IAR LPC-2378-STK board with Micrel KS8721BL PHY.
   Added support for Embedded Artists LPC2468 board with Micrel KSZ8001L PHY.
*/


#include "micronet.h"

/*
   Porting Notes:

   1. Make sure that mnconfig.h is configured properly for either ISR
      driven ethernet or MN_POLLED_ETHERNET as required.

   2. Make sure the ETHER_POLL_RECV, ETHER_INIT and ETHER_SEND(p,n) in
      ethernet.h are defined properly.

      #define ETHER_POLL_RECV       lpc2368_recv()
      #define ETHER_INIT            lpc2368_init()
      #define ETHER_SEND(p,n)       lpc2368_send((p),(n))

   3. If ISR driven ethernet is used, add code to the end of the lpc2368_init
      function to enable the ISR.

   4. Change the function declaration for the ISR function ether_isr to
      the declaration required for your hardware and C compiler. Every
      compiler does this differently so consult your compiler manual if
      necessary. This step is not needed if using MN_POLLED_ETHERNET.

*/

#if (MN_ETHERNET)
#include "lpc23xx_eth.h"

#if (!(USE_SEND_BUFF))
#error USE_SEND_BUFF must be set to 1 for this driver.
#endif      /* (!(USE_SEND_BUFF)) */

#if (!(USE_RECV_BUFF))
#error USE_RECV_BUFF must be set to 1 for this driver.
#endif      /* (!(USE_RECV_BUFF)) */

#if (MN_USE_HW_CHKSUM)
#error MN_USE_HW_CHKSUM not supported by this driver.
#endif

#define DO_DEBUG  0     /* set to 1 if debug info to uart is needed */
#if (DO_DEBUG)
void DiagSend(char *Str) cmx_reentrant;
#endif

/* set the following to 1 to have the chip auto-negotiate parameters, or set
   to 0 to use the full_duplex and speed_100 settings below.
*/
#define AUTO_NEGOTIATE     1
#if (!AUTO_NEGOTIATE)
int full_duplex = 0;             /* set to 1 for full duplex */
int speed_100 = 0;               /* set to 1 for 100Base-T, 0 for 10Base-T */
#endif      /* AUTO_NEGOTIATE */

#define PUT_RECV_BYTE(A) { \
   *(interface_ptr->recv_in_ptr) = (A); \
   interface_ptr->recv_in_ptr++; \
   if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr) \
      interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr; \
}

#if (defined(__CC_ARM))        /* Keil Realview tools */
#define INLINE __inline
#else
#define INLINE inline
#endif

/* local function prototypes */
static word16 mii_read(byte, byte) cmx_reentrant;
static void mii_write(byte, byte, word16) cmx_reentrant;
static int phy_init(void) cmx_reentrant;
static INLINE void ReleaseRxDescriptor(word32);

#if (!MN_POLLED_ETHERNET)
void ether_isr(void);
#endif

static byte phy_addr;

/*********************************************************/

/* initialization. returns 1 if ok or negative number on error. */
int lpc2368_init(void)
cmx_reentrant {
   int retval, i;
   volatile word32 regVal;

#if (DO_DEBUG)
#if (defined(POLARM2468) || defined(CMXARM2468))
   PINSEL7 |= 0x0000000F;	            /* P3.16 TXD1, P3.17 RXD1 */
#else
   PINSEL0 |= 0x40000000;               /* Enable TxD1                       */
   PINSEL1 |= 0x00000001;               /* Enable RxD1                       */
#endif
   U1FDR    = 0;                        /* Fractional divider not used       */
   U1LCR    = 0x83;                     /* 8 bits, no Parity, 1 Stop bit     */
   /* Baud Rates @ 12.0MHz VPB Clock: 38400 = 19, 19200 = 39, 9600 = 78 */
   U1DLL = 19;
   U1DLM    = 0;                        /* High divisor latch = 0            */
   U1LCR    = 0x03;                     /* DLAB = 0                          */

   DiagSend("Init start\r\n");
#endif

   /* disable ethernet VIC interrupt */
   VICIntEnClr = (1 << 21);

   /* Power Up the EMAC controller. */
   PCONP |= 0x40000000;

   /* Enable Ethernet Pins. */

   /* P1.6, ENET-TX_CLK, has to be set for EMAC to address a BUG in
      the Rev '-' silicon (see errata). On the new rev (Rev 'A', released
      in 06/2007), and all later revs, P1.6 must NOT be set.
   */
   regVal = MAC_MODULEID;
   if ( regVal == OLD_EMAC_MODULE_ID )
      {
      /* This is the rev.'-' ID. */
      PINSEL2 = 0x50151105;/* selects P1[0,1,4,6,8,9,10,14,15] */
      }
   else
      {
      /* On rev. 'A', MAC_MODULEID should NOT equal to OLD_EMAC_MODULE_ID,
         P1.6 should NOT be set.
      */
      PINSEL2 = 0x50150105;/* selects P1[0,1,4,8,9,10,14,15] */
      }
   PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;

   /* Reset all EMAC internal modules. */
   MAC_MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX | \
              MAC1_SIM_RES | MAC1_SOFT_RES;
   MAC_COMMAND = CR_REG_RES | CR_TX_RES | CR_RX_RES;

   mn_wait_ticks(1);

   /* disable ethernet interrupts */
   MAC_INTENABLE = 0;

   /* Reset MII and set clock divider. */
   /* Assumes host clock is set to 48 MHz.
      If the host clock is not 48 MHz then the appropriate clock
      divider setting should be used.
   */
   MAC_MCFG = MCFG_RES_MII | MCFG_CLK_DIV20;
   mn_wait_ticks(1);
   MAC_MCFG &= ~MCFG_RES_MII;

   /* Initialize MAC control registers. */
#if (MN_ALLOW_MULTICAST)
   MAC_MAC1 = MAC1_PASS_ALL;
#else
   MAC_MAC1 = 0;
#endif
   MAC_MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
   MAC_MAXF = ETH_MAX_FLEN;
   MAC_CLRT = CLRT_DEF;
   MAC_IPGR = IPGR_DEF;
   MAC_IPGT = IPGT_HALF_DUP;

   /* Enable RMII interface. */
   MAC_COMMAND = CR_RMII;

   /* Reset RMII to 10Base-T */
   MAC_SUPP = 0;

   MAC_TEST = 0;

   /* Discover the PHY address the board is using. */
   for (i=0; i<32; i++)
      {
      if (mii_read(i, PHY_REG_BMCR) != 0xFFFF)
         {
         phy_addr = i;
         break;
         }
      }

   if (i == 32)
      {
#if (DO_DEBUG)
      DiagSend("PHY not found\r\n");
#endif
      return (ETHER_INIT_ERROR);
      }

   retval = phy_init();
   if (retval < 0)
      {
#if (DO_DEBUG)
      DiagSend("PHY Init failed\r\n");
#endif
      return (retval);
      }

   /* write the MAC address to Station Address */
   MAC_SA0 = MK_WORD16(eth_src_hw_addr[1],eth_src_hw_addr[0]);
   MAC_SA1 = MK_WORD16(eth_src_hw_addr[3],eth_src_hw_addr[2]);
   MAC_SA2 = MK_WORD16(eth_src_hw_addr[5],eth_src_hw_addr[4]);

   /* Initialize receive descriptors. */
   for (i = 0; i < NUM_RX_FRAG; i++)
      {
      RX_DESC_PACKET(i)  = RX_BUF(i);
      RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
      RX_STAT_INFO(i)    = 0;
      RX_STAT_HASHCRC(i) = 0;
      }

   /* Set EMAC Receive Descriptor Registers. */
   MAC_RXDESCRIPTOR    = RX_DESC_BASE;
   MAC_RXSTATUS        = RX_STAT_BASE;
   MAC_RXDESCRIPTORNUM = NUM_RX_FRAG-1;

   /* Rx Descriptors Point to 0 */
   MAC_RXCONSUMEINDEX  = 0;

   /* Initialize transmit descriptors. */
   for (i = 0; i < NUM_TX_FRAG; i++)
      {
      TX_DESC_PACKET(i) = TX_BUF(i);
      TX_DESC_CTRL(i)   = 0;
      TX_STAT_INFO(i)   = 0;
      }

   /* Set EMAC Transmit Descriptor Registers. */
   MAC_TXDESCRIPTOR    = TX_DESC_BASE;
   MAC_TXSTATUS        = TX_STAT_BASE;
   MAC_TXDESCRIPTORNUM = NUM_TX_FRAG-1;

   /* Tx Descriptors Point to 0 */
   MAC_TXPRODUCEINDEX  = 0;

   /* Set the receive filter */
#if (MN_ALLOW_MULTICAST)
   MAC_RXFILTERCTRL = RFC_UCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN | RFC_MCAST_EN;
#else
   MAC_RXFILTERCTRL = RFC_UCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;
#endif

   /* Clear all interrupts */
   MAC_INTCLEAR  = 0x000030FF;

#if (!MN_POLLED_ETHERNET)
   VICVectAddr21 = (word32)ether_isr;  /* set interrupt vector in slot 21 */
   VICVectCntl21 = 21;                 /* use it for ethernet Interrupt */
   VICIntEnable = (1 << 21);           /* Enable ethernet Interrupt */

   /* Enable EMAC interrupts. */
   MAC_INTENABLE = INT_RX_DONE | INT_RX_OVERRUN | INT_TX_DONE | INT_TX_UNDERRUN | INT_TX_ERR;
#endif

   /* Enable EMAC receive and transmit */
   MAC_COMMAND  |= (CR_RX_EN | CR_TX_EN);
   MAC_MAC1     |= MAC1_REC_EN;

#if (DO_DEBUG)
   DiagSend("Init end\r\n");
#endif

   return (1);
}

/* Transmit a frame. The ethernet header must in the frame to be sent.
   Returns the number of bytes sent or negative number on error.
   The ethernet header and other header info is in the xmit_buff. The TCP
   or UDP data is pointed to by socket_ptr->send_ptr. xmit_sock_len is the
   number of data bytes to be sent in the current packet.
*/
int lpc2368_send(PSOCKET_INFO socket_ptr, word16 xmit_sock_len)
cmx_reentrant {
   word16 xmit_buff_len;
   word16 total_len;
   int retval;
   byte *MsgSendPointer;
   byte *buff_ptr;
   word32 tx_index;
   PINTERFACE_INFO interface_ptr;
#if (MN_POLLED_ETHERNET)
   volatile word32 int_status;
   TIMER_INFO_T wait_timer;
#endif

   /* Make sure that a descriptor is available. */
   tx_index = MAC_TXPRODUCEINDEX;
   if (tx_index == (MAC_TXCONSUMEINDEX - 1))
      {
#if (DO_DEBUG)
      DiagSend("Send descriptors full\r\n");
#endif
      return (ETHER_SEND_ERROR);
      }

   retval = 0;
   MsgSendPointer = PTR_NULL;
   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   /* make sure there is something to send */
   if (interface_ptr->send_out_ptr != interface_ptr->send_in_ptr)
      {
      /* calculate the number of bytes to send */
      xmit_buff_len = interface_ptr->send_in_ptr - interface_ptr->send_out_ptr;
      if (xmit_sock_len)
         {
         if (socket_ptr->send_ptr != PTR_NULL)
            MsgSendPointer = socket_ptr->send_ptr;
         else
            {
#if (DO_DEBUG)
            DiagSend("Bad send pointer\r\n");
#endif
            return (ETHER_SEND_ERROR);
            }
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
#if (DO_DEBUG)
         DiagSend("Send too large\r\n");
#endif
         return (ETHER_SEND_ERROR);
         }

      if (total_len < MIN_PACKET_SIZE)
         total_len = MIN_PACKET_SIZE;

      retval = total_len;

      buff_ptr = (byte *)TX_DESC_PACKET(tx_index);
      TX_DESC_CTRL(tx_index) = (total_len - 1) | TCTRL_LAST | TCTRL_INT;

      /* Copy the buffer(s) into the transmit buffer. */
      memcpy(buff_ptr, interface_ptr->send_out_ptr, xmit_buff_len);
      if (xmit_sock_len)
         memcpy((buff_ptr+xmit_buff_len), MsgSendPointer, xmit_sock_len);

      /* increment the transmit descriptor index, wrapping if necessary. */
      tx_index++;
      if (tx_index >= NUM_TX_FRAG)
         tx_index = 0;
      MAC_TXPRODUCEINDEX = tx_index;  /* this will start the transmission. */

#if (MN_POLLED_ETHERNET)
      mn_reset_timer(&wait_timer,(MN_ONE_SECOND));
      while (!mn_timer_expired(&wait_timer))
         {
         int_status = MAC_INTSTATUS;
         if (int_status & (INT_TX_ERR|INT_TX_UNDERRUN))
            break;
         else if (int_status & (INT_TX_FIN|INT_TX_DONE))
            return (retval);
         }

      retval = ETHER_SEND_ERROR;
#endif
      }

   return (retval);
}

/* Increment the receive descriptor index, wrapping if necessary. */
static INLINE void ReleaseRxDescriptor(word32 rx_index)
{
   rx_index++;
   if (rx_index == NUM_RX_FRAG)
      rx_index = 0;
   MAC_RXCONSUMEINDEX = rx_index;
}

#if (MN_POLLED_ETHERNET)
/* receives a frame in polled mode. returns number of bytes received if
   successful or negative number on error.
   Copies entire frame including ethernet header into the receive buffer.
*/
int lpc2368_recv(void)
cmx_reentrant {
   word16 recv_len, i;
   byte *buff_ptr;
   word32 rx_index;
#if (MN_ETHER_WAIT_TICKS)
   TIMER_INFO_T wait_timer;
#endif
   int retval;
   PINTERFACE_INFO interface_ptr;
#if (DO_DEBUG)
   byte buff[16];
   word16 temp;
#endif
   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   retval = 0;
   rx_index = 0;

   /* Check for a received packet */
#if (MN_ETHER_WAIT_TICKS)
   if (MAC_RXPRODUCEINDEX == MAC_RXCONSUMEINDEX)
      {
      mn_reset_timer(&wait_timer,(MN_ETHER_WAIT_TICKS));
      while (!mn_timer_expired(&wait_timer))
         {
         if (MAC_RXPRODUCEINDEX != MAC_RXCONSUMEINDEX)
            {
            retval = 1;
            break;
            }
#if (RTOS_USED != RTOS_NONE)
         MN_TASK_WAIT(1);
#endif      /* (RTOS_USED != RTOS_NONE) */
         }
      }
   else
      retval = 1;
#else
   if (MAC_RXPRODUCEINDEX != MAC_RXCONSUMEINDEX)
      retval = 1;
#endif

   if (retval == 1)
      {
      rx_index = MAC_RXCONSUMEINDEX;

      /* Check for errors in the packet. */
      if (RX_STAT_INFO(rx_index) & (RINFO_ERR_MASK | RINFO_NO_DESCR))
         {
#if (DO_DEBUG)
         DiagSend("Recv error - ");
         temp = (word16)(RX_STAT_INFO(rx_index) >> 16);
         mn_ustoa(buff, temp);
         DiagSend((char *)buff);
         DiagSend("\r\n");
#endif
         retval = ETHER_RECV_ERROR;
         ReleaseRxDescriptor(rx_index);
         }
      }

   if (retval == 1)
      {
      /* Note: below we are subtracting three because we need to subtract
         four for the CRC bytes but the length in the descriptors is
         already the length - 1.
      */
      recv_len = (word16)((RX_STAT_INFO(rx_index) & RINFO_SIZE) - 3);
      buff_ptr = (byte *)RX_DESC_PACKET(rx_index);

      if ( (interface_ptr->recv_buff_size - interface_ptr->recv_count - 2) >= recv_len)
         {
         retval = recv_len;

         MN_TASK_LOCK;

         /* put recv_len into buffer so all packets start with the
            length. Do NOT remove the next two lines.
         */
         PUT_RECV_BYTE(HIGHBYTE(recv_len));
         PUT_RECV_BYTE(LOWBYTE(recv_len));

         /* Copy packet into the system receive buffer. */
         for (i=0; i<recv_len; i++)
            PUT_RECV_BYTE(*buff_ptr++);

         interface_ptr->recv_count += retval + 2;
         MN_TASK_UNLOCK;
         }
      else           /* not enough room to hold this packet */
         {
#if (DO_DEBUG)
         DiagSend("Recv - not enough room\r\n");
#endif
         retval = ETHER_RECV_ERROR;
         }

      ReleaseRxDescriptor(rx_index);
      }
   else if (retval == 0)
      retval = SOCKET_TIMED_OUT;

   return (retval);
}

#else       /* interrupt-driven */

void ether_isr(void)
{
   word16 recv_len, i;
   byte *buff_ptr;
   volatile word32 rx_index;
   volatile word32 status;
#if (DO_DEBUG)
   byte buff[16];
   word16 temp;
#endif
   PINTERFACE_INFO interface_ptr;

   /* Read and clear interrupt status. */
   status = MAC_INTSTATUS;
   MAC_INTCLEAR = status;

   if (status & INT_RX_OVERRUN)
      {
      /* Need to reset the ethernet receive. */
      MAC_COMMAND |= CR_RX_RES;
#if (DO_DEBUG)
      DiagSend("Recv overrun\r\n");
#endif
      }
   else if (status & INT_RX_DONE)
      {
      while (1)
         {
         rx_index = MAC_RXCONSUMEINDEX;
         if (MAC_RXPRODUCEINDEX == rx_index)
            break;

         /* Check for errors in the packet. */
         if (RX_STAT_INFO(rx_index) & (RINFO_ERR_MASK | RINFO_NO_DESCR))
            {
#if (DO_DEBUG)
            DiagSend("Recv error - ");
            temp = (word16)(RX_STAT_INFO(rx_index) >> 16);
            mn_ustoa(buff, temp);
            DiagSend((char *)buff);
            DiagSend("\r\n");
#endif
            }
         else
            {
            /* Note: below we are subtracting three because we need to
               subtract four for the CRC bytes but the length in the
               descriptors is already the length - 1.
            */
            recv_len = (word16)((RX_STAT_INFO(rx_index) & RINFO_SIZE) - 3);
            buff_ptr = (byte *)RX_DESC_PACKET(rx_index);
            interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

            if ( (interface_ptr->recv_buff_size - \
                  interface_ptr->recv_count - 2) >= recv_len)
               {
               /* put recv_len into buffer so all packets start with the
                  length. Do NOT remove the next two lines.
               */
               PUT_RECV_BYTE(HIGHBYTE(recv_len));
               PUT_RECV_BYTE(LOWBYTE(recv_len));

               /* Copy packet into the system receive buffer. */
               for (i=0; i<recv_len; i++)
                  PUT_RECV_BYTE(*buff_ptr++);

               interface_ptr->recv_count += recv_len + 2;

               /* tell upper layers we got a packet */
               MN_ISR_SIGNAL_POST(SIGNAL_RECEIVE);
               }
#if (DO_DEBUG)
            else           /* not enough room to hold this packet */
               DiagSend("Recv - not enough room\r\n");
#endif
            }

         ReleaseRxDescriptor(rx_index);
         }
      }

   if (status & (INT_TX_DONE | INT_TX_ERR | INT_TX_UNDERRUN))
      {
      if (status & INT_TX_UNDERRUN)
         {
         /* Need to reset the ethernet transmit. */
         MAC_COMMAND |= CR_TX_RES;
#if (DO_DEBUG)
         DiagSend("Tx underrun\r\n");
#endif
         }

#if (DO_DEBUG)
      if (status & INT_TX_ERR)
         DiagSend("Tx err\r\n");
#endif

      /* Will get here on TX_DONE or an error. Either way assume the
         transmit is done.
      */
      MN_XMIT_BUSY_CLEAR;
      MN_ISR_SIGNAL_POST(SIGNAL_TRANSMIT);
      }
}
#endif      /* MN_POLLED_ETHERNET */

/*********************************************************/
/* Read a PHY register through the MII */
static word16 mii_read(byte paddr, byte phy_reg)
cmx_reentrant {
   word16 phy_data;
   TIMER_INFO_T mii_timer;

   phy_data = 0;

   MAC_MCMD = MCMD_READ;
   MAC_MADR = ((word32)paddr << 8) | phy_reg;

   mn_reset_timer(&mii_timer, (1*(MN_ONE_SECOND)));
   while (!mn_timer_expired(&mii_timer))
      {
       if ((MAC_MIND & MIND_BUSY) == 0)
         {
         MAC_MCMD = 0;
         phy_data = (word16)MAC_MRDD;
         break;
         }
      }

   return (phy_data);
}

/* Writes to a PHY register through the MII */
static void mii_write(byte paddr, byte phy_reg, word16 phy_data)
cmx_reentrant {
   TIMER_INFO_T mii_timer;

   MAC_MCMD = 0;
   MAC_MADR = ((word32)paddr << 8) | phy_reg;
   MAC_MWTD = phy_data;

   mn_reset_timer(&mii_timer, (1*(MN_ONE_SECOND)));
   while (!mn_timer_expired(&mii_timer))
      {
       if ((MAC_MIND & MIND_BUSY) == 0)
         break;
      }
}

/* PHY initialization. Returns 1 if successful, negative number on error.
*/
static int phy_init(void)
cmx_reentrant {
   int retval;
   word16 phy_data, id1, id2;
#if (AUTO_NEGOTIATE)
   TIMER_INFO_T phy_timer;
#endif         /* AUTO_NEGOTIATE */

   retval = -1;

   /* Reset the PHY */
   mii_write(phy_addr, PHY_REG_BMCR, _PHY_RESET);
   MN_TASK_WAIT((MN_ONE_SECOND)/2);

   /* If reset is not done by now we have a problem. */
   if (mii_read(phy_addr, PHY_REG_BMCR) & _PHY_RESET)
      return (retval);

#if (AUTO_NEGOTIATE)
   phy_data = mii_read(phy_addr, PHY_REG_BMSR);
   /* If AUTO_NEGOTIATE was selected and we are not capable of
      auto-negotiation return an error.
   */
   if (!(phy_data & _PHY_CAP_ANEG))
      return (-2);

   /* Start auto-negotiation */
   mii_write(phy_addr, PHY_REG_BMCR, (_PHY_ANEG_EN | _PHY_ANEG_RST));
   MN_TASK_WAIT((MN_ONE_SECOND)*1.5);

   retval = -3;

   /* Wait for auto-negotiation to finish, get an error or timeout */
   mn_reset_timer(&phy_timer, (3*(MN_ONE_SECOND)));
   while (!mn_timer_expired(&phy_timer))
      {
      phy_data = mii_read(phy_addr, PHY_REG_BMSR);
      if (phy_data & _PHY_ANEG_ACK)
         {
         retval = 1;
         break;
         }
      MN_TASK_WAIT((MN_ONE_SECOND)/10);
      }

   if (retval < 0)
      return (retval);

#else       /* !AUTO_NEGOTIATE */
   phy_data = mii_read(phy_addr, PHY_REG_BMSR);

   /* Make sure we are capable of the requested settings */
   if (full_duplex && speed_100 && !(phy_data & _PHY_CAP_TXF))
      return (-4);
   else if (!full_duplex && speed_100 && !(phy_data & _PHY_CAP_TXH))
      return (-4);
   else if (full_duplex && !speed_100 && !(phy_data & _PHY_CAP_TF))
      return (-4);
   else if (!full_duplex && !speed_100 && !(phy_data & _PHY_CAP_TH))
      return (-4);

   if (full_duplex)
      {
      phy_data = _PHY_DPLX;
      }
   else
      {
      phy_data = 0;
      }

   if (speed_100)
      {
      phy_data |= _PHY_SPEED;
      }

   /* Update the phy control register */
   mii_write(phy_addr, PHY_REG_BMCR, phy_data);

   MN_TASK_WAIT((MN_ONE_SECOND)*2);
#endif         /* AUTO_NEGOTIATE */

   retval = -5;

   /* Check for PHY type. Only the DP83848 and KS8721 are supported for now. */
   id1 = mii_read(phy_addr, PHY_REG_IDR1);
   id2 = mii_read(phy_addr, PHY_REG_IDR2);
   if ((id1 == DP83848C_ID1) && (id2 == DP83848C_ID2))
      {
      retval = -6;
#if (DO_DEBUG)
      DiagSend("DP83848 PHY detected\r\n");
#endif
      /* Check if link is up. */
      phy_data = mii_read(phy_addr, PHY_REG_STS);
      if (!(phy_data & _PHY_LINK_UP))
         return (retval);

      if (phy_data & _PHY_FULL_DLPX)
         {
#if (DO_DEBUG)
         DiagSend("Full duplex\r\n");
#endif
         MAC_MAC2    |= MAC2_FULL_DUP;
         MAC_COMMAND |= CR_FULL_DUP;
         MAC_IPGT     = IPGT_FULL_DUP;
         }
      else
         {
#if (DO_DEBUG)
         DiagSend("Half duplex\r\n");
#endif
         }

      if (phy_data & _PHY_SPEED10)
         {
#if (DO_DEBUG)
         DiagSend("10Base-T\r\n");
#endif
         MAC_SUPP = 0;
         }
      else
         {
#if (DO_DEBUG)
         DiagSend("100Base-T\r\n");
#endif
         MAC_SUPP = SUPP_SPEED;
         }

      retval = 1;
      }
   else if (((id1 == KS8721BL_ID1) && (id2 == KS8721BL_ID2)) || \
         ((id1 == KSZ8041NL_ID1) && (id2 == KSZ8041NL_ID2)) || \
         ((id1 == KSZ8001L_ID1) && (id2 == KSZ8001L_ID2)))
      {
      retval = -6;
#if (DO_DEBUG)
      if (id2 == KS8721BL_ID2)
         DiagSend("KS8721BL PHY detected\r\n");
      else if (id2 == KSZ8041NL_ID2)
         DiagSend("KSZ8041NL PHY detected\r\n");
      else if (id2 == KSZ8001L_ID2)
         DiagSend("KSZ8001L PHY detected\r\n");
#endif

      /* Check if link is up. */
      phy_data = mii_read(phy_addr, PHY_REG_BMSR);
      if (!(phy_data & _PHY_LINK))
         return (retval);

      /* Check 100BASE-TX PHY Controller register (0x1F) for duplex and
         speed settings.
      */
      phy_data = mii_read(phy_addr, PHY_REG_PHYCON);
      phy_data &= _PHY_OP_MODE_MASK;
      if ((phy_data == _PHY_OP_10_FDX) || (phy_data == _PHY_OP_100_FDX))
         {
#if (DO_DEBUG)
         DiagSend("Full duplex\r\n");
#endif
         MAC_MAC2    |= MAC2_FULL_DUP;
         MAC_COMMAND |= CR_FULL_DUP;
         MAC_IPGT     = IPGT_FULL_DUP;
         }
      else
         {
#if (DO_DEBUG)
         DiagSend("Half duplex\r\n");
#endif
         }

      if ((phy_data == _PHY_OP_10_HDX) || (phy_data == _PHY_OP_10_FDX))
         {
#if (DO_DEBUG)
         DiagSend("10Base-T\r\n");
#endif
         MAC_SUPP = 0;
         }
      else
         {
#if (DO_DEBUG)
         DiagSend("100Base-T\r\n");
#endif
         MAC_SUPP = SUPP_SPEED;
         }

      retval = 1;
      }

   return (retval);
}

#if (DO_DEBUG)
void DiagSend(char *Str)
cmx_reentrant {
   char i;

   i = 0;

   while(Str[i] != 0)
      {
      while (!(U1LSR & 0x20));
      U1THR = Str[i++];
      }
}
#endif
#endif      /* (MN_ETHERNET) */
