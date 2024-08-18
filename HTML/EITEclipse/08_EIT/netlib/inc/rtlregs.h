/* definitions for Realtek 8019 driver */

#ifndef RTLREGS_H_INC
#define RTLREGS_H_INC 1

/*
 * Register offset applicable to all register pages.
 */
#define NIC_CR 0x00        /* Command register */
#define NIC_IOPORT 0x10    /* I/O data port */
#define NIC_RESET 0x1f     /* Reset port */

/*
 * Page 0 register offsets.
 */
#define NIC_PG0_CLDA0 0x01    /* Current local DMA address 0 */
#define NIC_PG0_PSTART 0x01      /* Page start register */
#define NIC_PG0_CLDA1 0x02    /* Current local DMA address 1 */
#define NIC_PG0_PSTOP 0x02    /* Page stop register */
#define NIC_PG0_BNRY 0x03     /* Boundary pointer */
#define NIC_PG0_TSR 0x04      /* Transmit status register */
#define NIC_PG0_TPSR 0x04     /* Transmit page start address */
#define NIC_PG0_NCR 0x05      /* Number of collisions register */
#define NIC_PG0_TBCR0 0x05    /* Transmit byte count register 0 */
#define NIC_PG0_FIFO 0x06     /* FIFO */
#define NIC_PG0_TBCR1 0x06    /* Transmit byte count register 1 */
#define NIC_PG0_ISR 0x07      /* Interrupt status register */
#define NIC_PG0_CRDA0 0x08    /* Current remote DMA address 0 */
#define NIC_PG0_RSAR0 0x08    /* Remote start address register 0 */
#define NIC_PG0_CRDA1 0x09    /* Current remote DMA address 1 */
#define NIC_PG0_RSAR1 0x09    /* Remote start address register 1 */
#define NIC_PG0_RBCR0 0x0a    /* Remote byte count register 0 */
#define NIC_PG0_RBCR1 0x0b    /* Remote byte count register 1 */
#define NIC_PG0_RSR 0x0c      /* Receive status register */
#define NIC_PG0_RCR 0x0c      /* Receive configuration register */
#define NIC_PG0_CNTR0 0x0d    /* Tally counter 0 (frame alignment errors) */
#define NIC_PG0_TCR 0x0d      /* Transmit configuration register */
#define NIC_PG0_CNTR1 0x0e    /* Tally counter 1 (CRC errors) */
#define NIC_PG0_DCR 0x0e      /* Data configuration register */
#define NIC_PG0_CNTR2 0x0f    /* Tally counter 2 (Missed packet errors) */
#define NIC_PG0_IMR 0x0f      /* Interrupt mask register */

/*
 * Page 1 register offsets.
 */
#define NIC_PG1_PAR0 0x01     /* Physical address register 0 */
#define NIC_PG1_PAR1 0x02     /* Physical address register 1 */
#define NIC_PG1_PAR2 0x03     /* Physical address register 2 */
#define NIC_PG1_PAR3 0x04     /* Physical address register 3 */
#define NIC_PG1_PAR4 0x05     /* Physical address register 4 */
#define NIC_PG1_PAR5 0x06     /* Physical address register 5 */
#define NIC_PG1_CURR 0x07     /* Current page register */
#define NIC_PG1_MAR0 0x08     /* Multicast address register 0 */
#define NIC_PG1_MAR1 0x09     /* Multicast address register 1 */
#define NIC_PG1_MAR2 0x0a     /* Multicast address register 2 */
#define NIC_PG1_MAR3 0x0b     /* Multicast address register 3 */
#define NIC_PG1_MAR4 0x0c     /* Multicast address register 4 */
#define NIC_PG1_MAR5 0x0d     /* Multicast address register 5 */
#define NIC_PG1_MAR6 0x0e     /* Multicast address register 6 */
#define NIC_PG1_MAR7 0x0f     /* Multicast address register 7 */

/*
 * Page 2 register offsets.
 */
#define NIC_PG2_PSTART 0x01      /* Page start register */
#define NIC_PG2_CLDA0 0x01    /* Current local DMA address 0 */
#define NIC_PG2_PSTOP 0x02    /* Page stop register */
#define NIC_PG2_CLDA1 0x02    /* Current local DMA address 1 */
#define NIC_PG2_RNP 0x03      /* Remote next packet pointer */
#define NIC_PG2_TSPR 0x04     /* Transmit page start register */
#define NIC_PG2_LNP 0x05      /* Local next packet pointer */
#define NIC_PG2_ACU 0x06      /* Address counter (upper) */
#define NIC_PG2_ACL 0x07      /* Address counter (lower) */
#define NIC_PG2_RCR 0x0c      /* Receive configuration register */
#define NIC_PG2_TCR 0x0d      /* Transmit configuration register */
#define NIC_PG2_DCR 0x0e      /* Data configuration register */
#define NIC_PG2_IMR 0x0f      /* Interrupt mask register */

/*
 * Page 3 register offsets.
 */
#define NIC_PG3_EECR     0x01    /* EEPROM command register */
#define NIC_PG3_BPAGE    0x02    /* Boot-ROM page register */
#define NIC_PG3_CONFIG0  0x03    /* Configuration register 0 (r/o) */
#define NIC_PG3_CONFIG1  0x04    /* Configuration register 1 */
#define NIC_PG3_CONFIG2  0x05    /* Configuration register 2 */
#define NIC_PG3_CONFIG3  0x06    /* Configuration register 3 */
#define NIC_PG3_CSNSAV   0x08    /* CSN save register (r/o) */
#define NIC_PG3_HLTCLK   0x09    /* Halt clock */
#define NIC_PG3_INTR     0x0b    /* Interrupt pins (r/o) */

/*
 * Command register bits.
 */
#define NIC_CR_STP 0x01    /* Stop */
#define NIC_CR_STA 0x02    /* Start */
#define NIC_CR_TXP 0x04    /* Transmit packet */
#define NIC_CR_RD0 0x08    /* Remote DMA command bit 0 */
#define NIC_CR_RD1 0x10    /* Remote DMA command bit 1 */
#define NIC_CR_RD2 0x20    /* Remote DMA command bit 2 */
#define NIC_CR_PS0 0x40    /* Page select bit 0 */
#define NIC_CR_PS1 0x80    /* Page select bit 1 */

/*
 * Interrupt status register bits.
 */
#define NIC_ISR_PRX 0x01      /* Packet received */
#define NIC_ISR_PTX 0x02      /* Packet transmitted */
#define NIC_ISR_RXE 0x04      /* Receive error */
#define NIC_ISR_TXE 0x08      /* Transmit error */
#define NIC_ISR_OVW 0x10      /* Overwrite warning */
#define NIC_ISR_CNT 0x20      /* Counter overflow */
#define NIC_ISR_RDC 0x40      /* Remote DMA complete */
#define NIC_ISR_RST 0x80      /* Reset status */

/*
 * Interrupt mask register bits.
 */
#define NIC_IMR_PRXE 0x01     /* Packet received interrupt enable */
#define NIC_IMR_PTXE 0x02     /* Packet transmitted interrupt enable */
#define NIC_IMR_RXEE 0x04     /* Receive error interrupt enable */
#define NIC_IMR_TXEE 0x08     /* Transmit error interrupt enable */
#define NIC_IMR_OVWE 0x10     /* Overwrite warning interrupt enable */
#define NIC_IMR_CNTE 0x20     /* Counter overflow interrupt enable */
#define NIC_IMR_RCDE 0x40     /* Remote DMA complete interrupt enable */

/*
 * Data configuration register bits.
 */
#define NIC_DCR_WTS 0x01      /* Word transfer select */
#define NIC_DCR_BOS 0x02      /* Byte order select */
#define NIC_DCR_LAS 0x04      /* Long address select */
#define NIC_DCR_LS 0x08    /* Loopback select */
#define NIC_DCR_AR 0x10    /* Auto-initialize remote */
#define NIC_DCR_FT0 0x20      /* FIFO threshold select bit 0 */
#define NIC_DCR_FT1 0x40      /* FIFO threshold select bit 1 */

/*
 * Transmit configuration register bits.
 */
#define NIC_TCR_CRC 0x01      /* Inhibit CRC */
#define NIC_TCR_LB0 0x02      /* Encoded loopback control bit 0 */
#define NIC_TCR_LB1 0x04      /* Encoded loopback control bit 1 */
#define NIC_TCR_ATD 0x08      /* Auto transmit disable */
#define NIC_TCR_OFST 0x10     /* Collision offset enable */

/*
 * Transmit status register bits.
 */
#define NIC_TSR_PTX 0x01      /* Packet transmitted */
#define NIC_TSR_COL 0x04      /* Transmit collided */
#define NIC_TSR_ABT 0x08      /* Transmit aborted */
#define NIC_TSR_CRS 0x10      /* Carrier sense lost */
#define NIC_TSR_FU  0x20      /* FIFO underrun */
#define NIC_TSR_CDH 0x40      /* CD heartbeat */
#define NIC_TSR_OWC 0x80      /* Out of window collision */

/*
 * Receive configuration register bits.
 */
#define NIC_RCR_SEP 0x01      /* Save errored packets */
#define NIC_RCR_AR 0x02    /* Accept runt packets */
#define NIC_RCR_AB 0x04    /* Accept broadcast */
#define NIC_RCR_AM 0x08    /* Accept multicast */
#define NIC_RCR_PRO 0x10      /* Promiscuous physical */
#define NIC_RCR_MON 0x20      /* Monitor mode */

/*
 * Receive status register bits.
 */
#define NIC_RSR_PRX 0x01      /* Packet received intact */
#define NIC_RSR_CRC 0x02      /* CRC error */
#define NIC_RSR_FAE 0x04      /* Frame alignment error */
#define NIC_RSR_FO 0x08    /* FIFO overrun */
#define NIC_RSR_MPA 0x10      /* Missed packet */
#define NIC_RSR_PHY 0x20      /* Physical/multicast address */
#define NIC_RSR_DIS 0x40      /* Receiver disabled */
#define NIC_RSR_DFR 0x80      /* Deferring */

/*
 * EEPROM command register bits.
 */
#define NIC_EECR_EEM1  0x80    /* EEPROM Operating Mode */
#define NIC_EECR_EEM0  0x40    /* 0 0 Normal operation */
                                   /* 0 1 Auto-load */
                                   /* 1 0 9346 programming */
                                   /* 1 1 Config register write enab */
#define NIC_EECR_EECS  0x08    /* EEPROM Chip Select */
#define NIC_EECR_EESK  0x04    /* EEPROM Clock */
#define NIC_EECR_EEDI  0x02    /* EEPROM Data In */
#define NIC_EECR_EEDO  0x01    /* EEPROM Data Out */

/*
 * Configuration register 2 bits.
 */
#define NIC_CONFIG2_PL1   0x80 /* Network media type */
#define NIC_CONFIG2_PL0   0x40 /* 0 0 TP/CX auto-detect */
                                   /* 0 1 10baseT */
                                   /* 1 0 10base5 */
                                   /* 1 1 10base2 */
#define NIC_CONFIG2_BSELB 0x20 /* BROM disable */
#define NIC_CONFIG2_BS4   0x10 /* BROM size/base */
#define NIC_CONFIG2_BS3   0x08
#define NIC_CONFIG2_BS2   0x04
#define NIC_CONFIG2_BS1   0x02
#define NIC_CONFIG2_BS0   0x01

/*
 * Configuration register 3 bits
 */
#define NIC_CONFIG3_PNP     0x80 /* PnP Mode */
#define NIC_CONFIG3_FUDUP   0x40 /* Full duplex */
#define NIC_CONFIG3_LEDS1   0x20 /* LED1/2 pin configuration */
                                 /* 0 LED1 == LED_RX, LED2 == LED_TX */
                                 /* 1 LED1 == LED_CRS, LED2 == MCSB */
#define NIC_CONFIG3_LEDS0   0x10 /* LED0 pin configration */
                                 /* 0 LED0 pin == LED_COL */
                                 /* 1 LED0 pin == LED_LINK */
#define NIC_CONFIG3_SLEEP   0x04 /* Sleep mode */
#define NIC_CONFIG3_PWRDN   0x02 /* Power Down */
#define NIC_CONFIG3_ACTIVEB 0x01 /* inverse of bit 0 in PnP Act Reg */


#define USE_16BIT  0             /* 0 = 8-bit mode, 1 = 16-bit mode */


/* Setup the nic_read and nic_write macros */
#if (defined(POLX86) || defined(CMXX86))

#define nic_read(reg)         INBYTE((base) + (reg))
#define nic_write(reg, data)  OUTBYTE(((base) + (reg)),(data))

#elif (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))

byte readByte(byte reg) cmx_reentrant;
void writeByte(byte reg, byte dat) cmx_reentrant;
#define nic_read(reg)         readByte((reg))
#define nic_write(reg, data)  writeByte((reg),(data))

#if (defined(MCHP_C18))
#define NIC_CTRL_TRIS      (TRISE)
#define NIC_RESET_IO       (PORTEbits.RE2)
#define NIC_IOW_IO         (PORTEbits.RE1)
#define NIC_IOR_IO         (PORTEbits.RE0)
#define NIC_ADDR_IO        (PORTB)
#define NIC_DATA_IO        (PORTD)
#define SET_NIC_READ()     (TRISD = 0xff)
#define SET_NIC_WRITE()    (TRISD = 0x00)
#define PORTA_RA5          (PORTAbits.RA5)
#define LATA2              (LATAbits.LATA2)
#define LATA3              (LATAbits.LATA3)
#define INTCON2_RBPU       (INTCON2bits.RBPU)
#define NOP                Nop()
#elif (defined(HI_TECH_C))
#define NIC_CTRL_TRIS      (TRISE)
#define NIC_RESET_IO       (RE2)
#define NIC_IOW_IO         (RE1)
#define NIC_IOR_IO         (RE0)
#define NIC_ADDR_IO        (PORTB)
#define NIC_DATA_IO        (PORTD)
#define SET_NIC_READ()     (TRISD = 0xff)
#define SET_NIC_WRITE()    (TRISD = 0x00)
#define PORTA_RA5          (RA5)
#if 0
/* Some derivatives, such as the PIC18C452, define LATA2 and LATA3
   as LA2 and LA3.
*/
#define LATA2              (LA2)
#define LATA3              (LA3)
#endif
#define INTCON2_RBPU       (RBPU)
#define NOP                asm(" nop")
#elif (defined(__IAR_SYSTEMS_ICC__))
#define NIC_CTRL_TRIS      (TRISE)
#define NIC_RESET_IO       (RE2)
#define NIC_IOW_IO         (RE1)
#define NIC_IOR_IO         (RE0)
#define NIC_ADDR_IO        (PORTB)
#define NIC_DATA_IO        (PORTD)
#define SET_NIC_READ()     (TRISD = 0xff)
#define SET_NIC_WRITE()    (TRISD = 0x00)
#define PORTA_RA5          (RA5)
__SFR_BITS(__nonbanked,LATAbits,0xF89,LA0,LA1,LA2,LA3,LA4,LA5,LA6,LA7);
#define LATA2              (LA2)
#define LATA3              (LA3)
#define INTCON2_RBPU       (RBPU)
#define NOP                __no_operation()
#endif

#define WRITE_NIC_ADDR(a)  { NIC_ADDR_IO = (a); TRISB = 0xe0; }

#elif (defined(POLDSPICE) || defined(CMXDSPICE))

byte readByte(byte reg);
void writeByte(byte reg, byte dat);
#define nic_read(reg)         readByte((reg))
#define nic_write(reg, data)  writeByte((reg),(data))

/* Comment out the following two lines for 8-bit mode. */
#undef USE_16BIT
#define USE_16BIT  1

#if (USE_16BIT)
word16 readByte16(byte regAddr);
void writeByte16(byte regAddr, word16 regData);
#define nic_read16(reg)          readByte16((reg))
#define nic_write16(reg, data)   writeByte16((reg),(data))
#endif      /* (USE_16BIT) */

#if (defined(MCHP_C30))

#define  RSTDRV  LATBbits.LATB15
#define  IOWR    LATBbits.LATB14
#define  IORD    LATBbits.LATB13
#define  ALE     LATBbits.LATB12

#endif      /* (defined(MCHP_C30)) */

#elif (defined(POLST7E) || defined(CMXST7E))
/* structs to keep track of port states */
extern struct port_state_s {
   byte DDR_state;
   byte OR_state;
   byte DR_state;
} AS, BS, CS, DS, ES, FS, GS, HS;

byte readByte(byte regAddr) cmx_reentrant;
void writeByte(byte regAddr, byte regData) cmx_reentrant;
#define nic_read(reg)         readByte((reg))
#define nic_write(reg, data)  writeByte((reg),(data))

#define NOP                _asm("nop\n")

#elif (defined(POLEZ8E) || defined(CMXEZ8E))
byte readByte(byte regAddr) cmx_reentrant;
void writeByte(byte regAddr, byte regData) cmx_reentrant;
#define nic_read(reg)         readByte((reg))
#define nic_write(reg, data)  writeByte((reg),(data))

#define NOP                asm(" nop\n")
#define DATA_DIR           0x01   /* Data Direction */
#define ALT_FUN            0x02   /* Alternate Function */
#define OUT_CTL            0x03   /* Output Control (Open-Drain) */
#define HDR_EN             0x04   /* High Drive Enable */
#define SMRS_EN            0x05   /* Stop Mode Recovery Source Enable */

#else

#define nic_read(reg)         (*((byte volatile *)((base) + (reg))))
#define nic_write(reg, data)  (*((byte volatile *)((base) + (reg)))) = (data)

#endif      /* (defined(POLX86) || defined(CMXX86)) */

#endif   /* #ifndef RTLREGS_H_INC */

