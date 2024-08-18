#ifndef PHYSMC65_H_INC
#define PHYSMC65_H_INC  1

#define MIN_PACKET_SIZE    64

/* Header file for SMSC91C96 Definitions
   SYS TEC Computer GmbH Oktober 1999
   M. Berthel (m.b.)
*/

/* if not using the Phytec C165 board change base to match the base address
   of your board.
*/
#ifdef PHYTEC_MM_C165
#define base 0x401800 /* m.b. */
#endif
#ifdef TQ_STK_UE
#define base 0x900300 /* TQ Systems STK UE */
#endif
#ifdef TQ_ETH
#define base 0x400300 /* TQ Systems STK ETH */
#endif
#if (defined(POL188) || defined(CMX188))
#define base 0x300
#endif
#if (defined(POLH8S2674) || defined(CMXH8S2674))
#define base 0xF80000
#endif
#if (defined(POLH8S2329) || defined(CMXH8S2329))
#define base 0x200300
#endif
#if (defined(POLM16C654) || defined(CMXM16C654))
#define base 0x300
#endif
#ifndef base
#error define base address for Ethernet IO
#endif

#define BANK0   0x0000
#define BANK1   0x0001
#define BANK2   0x0002
#define BANK3   0x0003
#define BANK4   0x0004
#define BANK5   0x0005
#define BANK6   0x0006
#define BANK7   0x0007

#define BANKSEL   (0x0e)
#define TCR       (0x00)       /* bank 0 registers */
#define STATUS    (0x02)
#define RCR       (0x04)
#define COUNTER   (0x06)
#define MIR       (0x08)
#define MCR       (0x0a)
#define TEST      (0x0c)
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
#define INTR_MASK (0x0d)
#define MT10      (0x00)       /* bank 3 registers */
#define MT32      (0x02)
#define MT54      (0x04)
#define MT76      (0x06)
#define MGMT      (0x08)
#define REVISION  (0x0a)
#define ERCV      (0x0c)

#define ECOR      (0x00)       /* bank 4 registers */
#define EBASE0    (0x04)
#define EBASE1    (0x06)

#define MCOR      (0x00)       /* bank 5 registers */
#define PRR       (0x02)
#define IOEIR     (0x04)
#define MBASE1    (0x06)
#define MSIZE     (0x08)

#define FDSE       0x8000       /* tcr register bits */
#define ETEN_TYPE  0x4000
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
#define RXOVRN     0x2000
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
#define STRIPCRC   0x0200
#define RXEN       0x0100
#define ALMUL      0x0004
#define PRMS       0x0002
#define RXABORT    0x0001

#define MIISELECT  0x8000      /* configuration register bits */
#define NOWAITST   0x1000
#define FULLSTEP   0x0400
#define SETSQLCH   0X0200
#define AUISELECT  0x0100
#define SIXTEENBIT 0x0080
#define INTSEL     0x0006

#define RCVBAD     0x4000      /* control register bits */
#define PWRDN      0x2000
#define AUTORELEASE 0x0800
#define LEENABLE   0x0080
#define CRENABLE   0x0040
#define TEENABLE   0x0020
#define EEPROMSEL  0x0004
#define RELOAD     0x0002
#define STORE      0x0001

#define BUSY       0x0001      /* mmu command register bits */
#define NOOP       0x0000
#define ALLOCTX    0x0020
#define RESETMMU   0x0040
#define REMOVERX   0x0060
#define RESETTXFIFO 0X00e0
#define RELERX     0x0080
#define RELEPNR    0x00a0
#define QUEUETX    0x00c0

#define FAILALL    0x8000      /* allocation result register */

#define REMPTY     0x8000      /* fifoports register bits */
#define TEMPTY     0x0080

#define RXRD       0xe000      /* pointer register bits */
#define RXWR       0xc000
#define TXRD       0x6000
#define TXWR       0x4000
#define ETEN       0x1000
#define NOTEMPTY   0x0800

#define ERCVINT    0x0040      /* interrupt status register bits */
#define EPHINT     0x0020
#define RXOVRNINT  0x0010
#define ALLOCINT   0x0008
#define TXEMPTYINT 0x0004
#define TXINT      0x0002
#define RXINT      0x0001

#define ERCVINT_EN    0x4000      /* interrupt mask register bits */
#define EPHINT_EN     0x2000
#define RXOVRNINT_EN  0x1000
#define ALLOCINT_EN   0x0800
#define TXEMPTYINT_EN 0x0400
#define TXINT_EN      0x0200
#define RXINT_EN      0x0100

#define ERCVINTMASK     0x4000   /* interrupt mask register */
#define EPHINTMASK      0x2000
#define RXOVRNINTMASK   0x1000
#define ALLOCINTMASK    0x0800
#define TXEMPTYINTMASK  0x0400
#define TXINTMASK       0x0200
#define RXINTMASK       0x0100

#define ALGNERR    0x8000     /* receive frame status word */
#define BRODCAST   0x4000
#define BADCRC     0x2000
#define ODDFRM     0x1000
#define TOOLONG    0x0800
#define TOOSHORT   0x0400

#define EPHERR     0x8630

#define CLEAR        0x0000
#define FIFO_TX_MASK 0x007f
#define PNR_MASK     0x00FF
#define ARR_MASK     0xFF00

#endif   /* #ifndef PHYSMC65_H_INC */

