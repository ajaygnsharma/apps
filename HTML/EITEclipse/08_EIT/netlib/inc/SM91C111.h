#ifndef SM91C111_H
#define SM91C111_H      1

/* For eCog1 board with LAN91C111 ethernet.
   Also for Custom M16C/62P board with LAN91C113 ethernet and
   Phytec phyCORE-LPC2294.
*/

#define MIN_PACKET_SIZE    64

#define BANK0   0x0000
#define BANK1   0x0001
#define BANK2   0x0002
#define BANK3   0x0003
#define BANK7   0x0007

#if (defined(POLECOG1E) || defined(CMXECOG1E))
/* The following offset are word16 offsets, not byte offsets.
   For all other processors byte offsets must be used.
*/
#define BANKSEL   (0x07)
#define TCR       (0x00)       /* bank 0 registers */
#define STATUS    (0x01)
#define RCR       (0x02)
#define COUNTER   (0x03)
#define MIR       (0x04)
#define RPCR      (0x05)
#define CONFIG    (0x00)       /* bank 1 registers */
#define BASE      (0x01)
#define IA10      (0x02)
#define IA32      (0x03)
#define IA54      (0x04)
#define GENERAL   (0x05)
#define CONTROL   (0x06)
#define MMUCOM    (0x00)       /* bank 2 registers */
#define ARRPNR    (0x01)
#define FIFOPORTS (0x02)
#define POINTER   (0x03)
#define DATA1     (0x04)
#define DATA2     (0x05)
#define INTERRUPT (0x06)
#define MT10      (0x00)       /* bank 3 registers */
#define MT32      (0x01)
#define MT54      (0x02)
#define MT76      (0x03)
#define MGMT      (0x04)
#define REVISION  (0x05)
#define ERCV      (0x06)

#elif (defined(POLNECV850ES) || defined(CMXNECV850ES))
#define BANKSEL   (0x0e << 1)
#define TCR       (0x00)       /* bank 0 registers */
#define STATUS    (0x02 << 1)
#define RCR       (0x04 << 1)
#define COUNTER   (0x06 << 1)
#define MIR       (0x08 << 1)
#define RPCR      (0x0a << 1)
#define CONFIG    (0x00)       /* bank 1 registers */
#define BASE      (0x02 << 1)
#define IA10      (0x04 << 1)
#define IA32      (0x06 << 1)
#define IA54      (0x08 << 1)
#define GENERAL   (0x0a << 1)
#define CONTROL   (0x0c << 1)
#define MMUCOM    (0x00)       /* bank 2 registers */
#define ARRPNR    (0x02 << 1)
#define FIFOPORTS (0x04 << 1)
#define POINTER   (0x06 << 1)
#define DATA1     (0x08 << 1)
#define DATA2     (0x0a << 1)
#define INTERRUPT (0x0c << 1)
#define MT10      (0x00)       /* bank 3 registers */
#define MT32      (0x02 << 1)
#define MT54      (0x04 << 1)
#define MT76      (0x06 << 1)
#define MGMT      (0x08 << 1)
#define REVISION  (0x0a << 1)
#define ERCV      (0x0c << 1)

#else

#define BANKSEL   (0x0e)
#define TCR       (0x00)       /* bank 0 registers */
#define STATUS    (0x02)
#define RCR       (0x04)
#define COUNTER   (0x06)
#define MIR       (0x08)
#define RPCR      (0x0a)
#define CONFIG    (0x00)       /* bank 1 registers */
#define BASE      (0x02)
#define IA10      (0x04)
#define IA32      (0x06)
#define IA54      (0x08)
#define GENERAL   (0x0a)
#define CONTROL   (0x0c)
#define MMUCOM    (0x00)       /* bank 2 registers */
#define ARRPNR    (0x02)
#define FIFOPORTS (0x04)
#define POINTER   (0x06)
#define DATA1     (0x08)
#define DATA2     (0x0a)
#define INTERRUPT (0x0c)
#define MT10      (0x00)       /* bank 3 registers */
#define MT32      (0x02)
#define MT54      (0x04)
#define MT76      (0x06)
#define MGMT      (0x08)
#define REVISION  (0x0a)
#define ERCV      (0x0c)

#endif      /* (defined(POLECOG1E) || defined(CMXECOG1E)) */

#define SWFDUP     0x8000       /* tcr register bits */
#define EPHLOOP    0x2000
#define STPSQET    0x1000
#define FDUPLX     0x0800
#define MONCSN     0x0400
#define NOCRC      0x0100
#define PADEN      0x0080
#define FORCOL     0x0004
#define LOOP       0x0002
#define TXENA      0x0001

#define TXUNRN     0x8000       /* eph status bits */
#define LINKOK     0x4000
#define CTRROL     0x1000
#define EXCDEF     0x0800
#define LOSTCAR    0x0400
#define LATCOL     0x0200
#define TXDEFR     0x0080
#define LTXBRD     0x0040
#define SQET       0x0020
#define SIXTEENCOL 0x0010
#define LTXMULT    0x0008
#define MULCOL     0x0004
#define SNGLCOL    0x0002
#define TXSUC      0x0001

#define SOFTRST    0x8000       /* rcr register bits */
#define FILTCAR    0x4000
#define ABORT_ENB  0x2000
#define STRIPCRC   0x0200
#define RXEN       0x0100
#define ALMUL      0x0004
#define PRMS       0x0002
#define RXABORT    0x0001

#define SPEED        0x2000      /* rpcr register bits */
#define DPLX         0x1000
#define ANEG         0x0800

#define EPH_POWER_EN 0x8000      /* configuration register bits */

#define RCVBAD       0x4000      /* control register bits */
#define AUTORELEASE  0x0800
#define LEENABLE     0x0080
#define CRENABLE     0x0040
#define TEENABLE     0x0020
#define EEPROMSEL    0x0004
#define RELOAD       0x0002
#define STORE        0x0001

#define BUSY         0x0001      /* mmu command register bits */
#define NOOP         0x0000
#define ALLOCTX      0x0020
#define RESETMMU     0x0040
#define REMOVERX     0x0060
#define RELERX       0x0080
#define RELEPNR      0x00a0
#define QUEUETX      0x00c0
#define RESETTXFIFO  0X00e0

#define FAILALL      0x8000      /* allocation result register */

#define REMPTY       0x8000      /* fifoports register bits */
#define TEMPTY       0x0080

#define RXRD         0xe000      /* pointer register bits */
#define RXWR         0xc000
#define TXRD         0x6000
#define TXWR         0x4000
#define ETEN         0x1000
#define NOTEMPTY     0x0800

#define MDINT        0x0080      /* interrupt status register bits */
#define ERCVINT      0x0040
#define EPHINT       0x0020
#define RXOVRNINT    0x0010
#define ALLOCINT     0x0008
#define TXEMPTYINT   0x0004
#define TXINT        0x0002
#define RXINT        0x0001

#define MDINT_EN      0x8000      /* interrupt mask register bits */
#define ERCVINT_EN    0x4000
#define EPHINT_EN     0x2000
#define RXOVRNINT_EN  0x1000
#define ALLOCINT_EN   0x0800
#define TXEMPTYINT_EN 0x0400
#define TXINT_EN      0x0200
#define RXINT_EN      0x0100

#define MDOE          0x0008
#define MCLK          0x0004
#define MDI           0x0002
#define MDO           0x0001

#define FIFO_TX_MASK  0x007f
#define PNR_MASK      0x00FF
#define ARR_MASK      0xFF00

#define ALGNERR       0x8000     /* receive frame status word */
#define BRODCAST      0x4000
#define BADCRC        0x2000
#define ODDFRM        0x1000
#define TOOLONG       0x0800
#define TOOSHORT      0x0400

#define CLEAR         0x0000

/* PHY reg.0 Control Register (section 9.1, pg.71) */
#define _PHY_RESET    0x8000
#define _PHY_LPBK     0x4000
#define _PHY_SPEED    0x2000
#define _PHY_ANEG_EN  0x1000
#define _PHY_PDN      0x0800
#define _PHY_MII_DIS  0x0400
#define _PHY_ANEG_RST 0x0200
#define _PHY_DPLX     0x0100
#define _PHY_COLST    0x0080

/* PHY reg.1 Status Register (section 9.2, pg.72) */
#define _PHY_CAP_T4   0x8000
#define _PHY_CAP_TXF  0x4000
#define _PHY_CAP_TXH  0x2000
#define _PHY_CAP_TF   0x1000
#define _PHY_CAP_TH   0x0800
#define _PHY_CAP_SUPR 0x0040
#define _PHY_ANEG_ACK 0x0020
#define _PHY_REM_FLT  0x0010
#define _PHY_CAP_ANEG 0x0008
#define _PHY_LINK     0x0004
#define _PHY_JAB      0x0002
#define _PHY_EXREG    0x0001

/* PHY reg.2&3 PHY Identifier Register (section 9.3, pg.73) */
/* not covered here */

/* PHY reg.4 Auto-Negotiation Advertisement Register
            (section 9.4, pg.73) */
/* PHY reg.5 Auto-Negotiation Remote End Capability
             Register (section 9.5, pg.74) */
#define _PHY_NP       0x8000
#define _PHY_ACK      0x4000
#define _PHY_RF       0x2000
#define _PHY_T4       0x0200
#define _PHY_TX_FDX   0x0100
#define _PHY_TX_HDX   0x0080
#define _PHY_10_FDX   0x0040
#define _PHY_10_HDX   0x0020
#define _PHY_CSMA     0x0001

/* PHY reg.16 Configuration 1 (section 9.6, pg.74) */
#define _PHY_LNKDIS   0x8000
#define _PHY_XMTDIS   0x4000
#define _PHY_XMTPDN   0x2000
#define _PHY_BYPSCR   0x0400
#define _PHY_UNSCDS   0x0200
#define _PHY_EQLZR    0x0100
#define _PHY_CABLE    0x0080
#define _PHY_RLVL0    0x0040
#define _PHY_TLVL3    0x0020
#define _PHY_TLVL2    0x0010
#define _PHY_TLVL1    0x0008
#define _PHY_TLVL0    0x0004
#define _PHY_TRF1     0x0002
#define _PHY_TRF0     0x0001

/* PHY reg.17 Configuration 2 (section 9.7, pg.75) */
#define _PHY_APOLDIS  0x0020
#define _PHY_JABDIS   0x0010
#define _PHY_MREG     0x0008
#define _PHY_INTMDIO  0x0004

/* PHY reg.18 Status Output (section 9.8, pg.76) */
#define _PHY_INT      0x8000
#define _PHY_LNKFAIL  0x4000
#define _PHY_LOSSSYNC 0x2000
#define _PHY_CWRD     0x1000
#define _PHY_SSD      0x0800
#define _PHY_ESD      0x0400
#define _PHY_RPOL     0x0200
#define _PHY_JAB_STAT 0x0100  /* in documentation just _PHY_JAB */
#define _PHY_SPDDET   0x0080
#define _PHY_DPLXDET  0x0040

/* PHY reg.19 Mask (section 9.9, pg.77) */
#define _PHY_MINT     0x8000
#define _PHY_MLNKFAIL 0x4000
#define _PHY_MLOSSSYN 0x2000
#define _PHY_MCWRD    0x1000
#define _PHY_MSSD     0x0800
#define _PHY_MESD     0x0400
#define _PHY_MRPOL    0x0200
#define _PHY_MJAB     0x0100
#define _PHY_MSPDDT   0x0080
#define _PHY_MDPLDT   0x0040

#endif   /* SM91C111_H */
