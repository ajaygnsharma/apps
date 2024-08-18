/*----------------------------------------------------------------------------
 *      LPC2378 Ethernet Definitions
 *----------------------------------------------------------------------------
 *      Name:    EMAC.H
 *      Purpose: Philips LPC2378 EMAC hardware definitions
 *--------------------------------------------------------------------------*/

#ifndef LPC23XX_ETH_H
#define LPC23XX_ETH_H

#ifndef MAC_BASE_ADDR
#define MAC_BASE_ADDR      0xFFE00000
#endif
#ifndef MAC_MODULEID
#define MAC_MODULEID (*(volatile word32 *)(MAC_BASE_ADDR + 0xFFC)) /* Module ID reg (RO) */
#endif
#define OLD_EMAC_MODULE_ID ((0x3902 << 16) | 0x2000)

/* EMAC Memory Buffer configuration for 16K Ethernet RAM. */
#define NUM_RX_FRAG         4    /* Num.of RX Fragments, must be 4 or less */
#define NUM_TX_FRAG         3    /* Num.of TX Fragments */
#define ETH_FRAG_SIZE       1536 /* Fragment size MUST be a multiple of 4 */

#define ETH_MAX_FLEN        1536        /* Max. Ethernet Frame Size */
#define MIN_PACKET_SIZE     60

/* EMAC variables located in 16K Ethernet SRAM */
#define RX_DESC_BASE        0x7FE00000
#define RX_STAT_BASE        (RX_DESC_BASE + (NUM_RX_FRAG*8))
#define TX_DESC_BASE        (RX_STAT_BASE + (NUM_RX_FRAG*8))
#define TX_STAT_BASE        (TX_DESC_BASE + (NUM_TX_FRAG*8))
#define RX_BUF_BASE         (TX_STAT_BASE + (NUM_TX_FRAG*4))
#define TX_BUF_BASE         (RX_BUF_BASE  + (NUM_RX_FRAG*ETH_FRAG_SIZE))

/* RX and TX descriptor and status definitions. */
#define RX_DESC_PACKET(i)   (*(unsigned int *)(RX_DESC_BASE   + (8*i)))
#define RX_DESC_CTRL(i)     (*(unsigned int *)(RX_DESC_BASE+4 + (8*i)))
#define RX_STAT_INFO(i)     (*(unsigned int *)(RX_STAT_BASE   + (8*i)))
#define RX_STAT_HASHCRC(i)  (*(unsigned int *)(RX_STAT_BASE+4 + (8*i)))
#define TX_DESC_PACKET(i)   (*(unsigned int *)(TX_DESC_BASE   + (8*i)))
#define TX_DESC_CTRL(i)     (*(unsigned int *)(TX_DESC_BASE+4 + (8*i)))
#define TX_STAT_INFO(i)     (*(unsigned int *)(TX_STAT_BASE   + (4*i)))
#define RX_BUF(i)           (RX_BUF_BASE + (ETH_FRAG_SIZE*i))
#define TX_BUF(i)           (TX_BUF_BASE + (ETH_FRAG_SIZE*i))

/* MAC Configuration Register 1 */
#define MAC1_REC_EN         0x00000001  /* Receive Enable                    */
#define MAC1_PASS_ALL       0x00000002  /* Pass All Receive Frames           */
#define MAC1_RX_FLOWC       0x00000004  /* RX Flow Control                   */
#define MAC1_TX_FLOWC       0x00000008  /* TX Flow Control                   */
#define MAC1_LOOPB          0x00000010  /* Loop Back Mode                    */
#define MAC1_RES_TX         0x00000100  /* Reset TX Logic                    */
#define MAC1_RES_MCS_TX     0x00000200  /* Reset MAC TX Control Sublayer     */
#define MAC1_RES_RX         0x00000400  /* Reset RX Logic                    */
#define MAC1_RES_MCS_RX     0x00000800  /* Reset MAC RX Control Sublayer     */
#define MAC1_SIM_RES        0x00004000  /* Simulation Reset                  */
#define MAC1_SOFT_RES       0x00008000  /* Soft Reset MAC                    */

/* MAC Configuration Register 2 */
#define MAC2_FULL_DUP       0x00000001  /* Full Duplex Mode                  */
#define MAC2_FRM_LEN_CHK    0x00000002  /* Frame Length Checking             */
#define MAC2_HUGE_FRM_EN    0x00000004  /* Huge Frame Enable                 */
#define MAC2_DLY_CRC        0x00000008  /* Delayed CRC Mode                  */
#define MAC2_CRC_EN         0x00000010  /* Append CRC to every Frame         */
#define MAC2_PAD_EN         0x00000020  /* Pad all Short Frames              */
#define MAC2_VLAN_PAD_EN    0x00000040  /* VLAN Pad Enable                   */
#define MAC2_ADET_PAD_EN    0x00000080  /* Auto Detect Pad Enable            */
#define MAC2_PPREAM_ENF     0x00000100  /* Pure Preamble Enforcement         */
#define MAC2_LPREAM_ENF     0x00000200  /* Long Preamble Enforcement         */
#define MAC2_NO_BACKOFF     0x00001000  /* No Backoff Algorithm              */
#define MAC2_BACK_PRESSURE  0x00002000  /* Backoff Presurre / No Backoff     */
#define MAC2_EXCESS_DEF     0x00004000  /* Excess Defer                      */

/* Back-to-Back Inter-Packet-Gap Register */
#define IPGT_FULL_DUP       0x00000015  /* Recommended value for Full Duplex */
#define IPGT_HALF_DUP       0x00000012  /* Recommended value for Half Duplex */

/* Non Back-to-Back Inter-Packet-Gap Register */
#define IPGR_DEF            0x00000C12  /* Recommended value                 */

/* Collision Window/Retry Register */
#define CLRT_DEF            0x0000370F  /* Default value                     */

/* PHY Support Register */
#define SUPP_SPEED          0x00000100  /* Reduced MII Logic Current Speed   */
#define SUPP_RES_RMII       0x00000800  /* Reset Reduced MII Logic           */

/* Test Register */
#define TEST_SHCUT_PQUANTA  0x00000001  /* Shortcut Pause Quanta             */
#define TEST_TST_PAUSE      0x00000002  /* Test Pause                        */
#define TEST_TST_BACKP      0x00000004  /* Test Back Pressure                */

/* MII Management Configuration Register */
#define MCFG_SCAN_INC       0x00000001  /* Scan Increment PHY Address        */
#define MCFG_SUPP_PREAM     0x00000002  /* Suppress Preamble                 */
#define MCFG_CLK_SEL        0x0000001C  /* Clock Select Mask                 */
#define MCFG_CLK_DIV4       0x00000000  /* MII clock is host clock / 4  */
#define MCFG_CLK_DIV6       0x00000008  /* MII clock is host clock / 6  */
#define MCFG_CLK_DIV8       0x0000000C  /* MII clock is host clock / 8  */
#define MCFG_CLK_DIV10      0x00000010  /* MII clock is host clock / 10 */
#define MCFG_CLK_DIV14      0x00000014  /* MII clock is host clock / 14 */
#define MCFG_CLK_DIV20      0x00000018  /* MII clock is host clock / 20 */
#define MCFG_CLK_DIV28      0x0000001C  /* MII clock is host clock / 28 */
#define MCFG_RES_MII        0x00008000  /* Reset MII Management Hardware     */

/* MII Management Command Register */
#define MCMD_READ           0x00000001  /* MII Read                          */
#define MCMD_SCAN           0x00000002  /* MII Scan continuously             */

#define MII_WR_TOUT         0x00050000  /* MII Write timeout count           */
#define MII_RD_TOUT         0x00050000  /* MII Read timeout count            */

/* MII Management Address Register */
#define MADR_REG_ADR        0x0000001F  /* MII Register Address Mask         */
#define MADR_PHY_ADR        0x00001F00  /* PHY Address Mask                  */

/* MII Management Indicators Register */
#define MIND_BUSY           0x00000001  /* MII is Busy                       */
#define MIND_SCAN           0x00000002  /* MII Scanning in Progress          */
#define MIND_NOT_VAL        0x00000004  /* MII Read Data not valid           */
#define MIND_MII_LINK_FAIL  0x00000008  /* MII Link Failed                   */

/* Command Register */
#define CR_RX_EN            0x00000001  /* Enable Receive                    */
#define CR_TX_EN            0x00000002  /* Enable Transmit                   */
#define CR_REG_RES          0x00000008  /* Reset Host Registers              */
#define CR_TX_RES           0x00000010  /* Reset Transmit Datapath           */
#define CR_RX_RES           0x00000020  /* Reset Receive Datapath            */
#define CR_PASS_RUNT_FRM    0x00000040  /* Pass Runt Frames                  */
#define CR_PASS_RX_FILT     0x00000080  /* Pass RX Filter                    */
#define CR_TX_FLOW_CTRL     0x00000100  /* TX Flow Control                   */
#define CR_RMII             0x00000200  /* Reduced MII Interface             */
#define CR_FULL_DUP         0x00000400  /* Full Duplex                       */

/* Status Register */
#define SR_RX_ACTIVE        0x00000001  /* Receive active                   */
#define SR_TX_ACTIVE        0x00000002  /* Transmit active                  */

/* Transmit Status Vector 0 Register */
#define TSV0_CRC_ERR        0x00000001  /* CRC error                         */
#define TSV0_LEN_CHKERR     0x00000002  /* Length Check Error                */
#define TSV0_LEN_OUTRNG     0x00000004  /* Length Out of Range               */
#define TSV0_DONE           0x00000008  /* Tramsmission Completed            */
#define TSV0_MCAST          0x00000010  /* Multicast Destination             */
#define TSV0_BCAST          0x00000020  /* Broadcast Destination             */
#define TSV0_PKT_DEFER      0x00000040  /* Packet Deferred                   */
#define TSV0_EXC_DEFER      0x00000080  /* Excessive Packet Deferral         */
#define TSV0_EXC_COLL       0x00000100  /* Excessive Collision               */
#define TSV0_LATE_COLL      0x00000200  /* Late Collision Occured            */
#define TSV0_GIANT          0x00000400  /* Giant Frame                       */
#define TSV0_UNDERRUN       0x00000800  /* Buffer Underrun                   */
#define TSV0_BYTES          0x0FFFF000  /* Total Bytes Transferred           */
#define TSV0_CTRL_FRAME     0x10000000  /* Control Frame                     */
#define TSV0_PAUSE          0x20000000  /* Pause Frame                       */
#define TSV0_BACK_PRESS     0x40000000  /* Backpressure Method Applied       */
#define TSV0_VLAN           0x80000000  /* VLAN Frame                        */

/* Transmit Status Vector 1 Register */
#define TSV1_BYTE_CNT       0x0000FFFF  /* Transmit Byte Count               */
#define TSV1_COLL_CNT       0x000F0000  /* Transmit Collision Count          */

/* Receive Status Vector Register */
#define RSV_BYTE_CNT        0x0000FFFF  /* Receive Byte Count                */
#define RSV_PKT_IGNORED     0x00010000  /* Packet Previously Ignored         */
#define RSV_RXDV_SEEN       0x00020000  /* RXDV Event Previously Seen        */
#define RSV_CARR_SEEN       0x00040000  /* Carrier Event Previously Seen     */
#define RSV_REC_CODEV       0x00080000  /* Receive Code Violation            */
#define RSV_CRC_ERR         0x00100000  /* CRC Error                         */
#define RSV_LEN_CHKERR      0x00200000  /* Length Check Error                */
#define RSV_LEN_OUTRNG      0x00400000  /* Length Out of Range               */
#define RSV_REC_OK          0x00800000  /* Frame Received OK                 */
#define RSV_MCAST           0x01000000  /* Multicast Frame                   */
#define RSV_BCAST           0x02000000  /* Broadcast Frame                   */
#define RSV_DRIB_NIBB       0x04000000  /* Dribble Nibble                    */
#define RSV_CTRL_FRAME      0x08000000  /* Control Frame                     */
#define RSV_PAUSE           0x10000000  /* Pause Frame                       */
#define RSV_UNSUPP_OPC      0x20000000  /* Unsupported Opcode                */
#define RSV_VLAN            0x40000000  /* VLAN Frame                        */

/* Flow Control Counter Register */
#define FCC_MIRR_CNT        0x0000FFFF  /* Mirror Counter                    */
#define FCC_PAUSE_TIM       0xFFFF0000  /* Pause Timer                       */

/* Flow Control Status Register */
#define FCS_MIRR_CNT        0x0000FFFF  /* Mirror Counter Current            */

/* Receive Filter Control Register */
#define RFC_UCAST_EN        0x00000001  /* Accept Unicast Frames Enable      */
#define RFC_BCAST_EN        0x00000002  /* Accept Broadcast Frames Enable    */
#define RFC_MCAST_EN        0x00000004  /* Accept Multicast Frames Enable    */
#define RFC_UCAST_HASH_EN   0x00000008  /* Accept Unicast Hash Filter Frames */
#define RFC_MCAST_HASH_EN   0x00000010  /* Accept Multicast Hash Filter Fram.*/
#define RFC_PERFECT_EN      0x00000020  /* Accept Perfect Match Enable       */
#define RFC_MAGP_WOL_EN     0x00001000  /* Magic Packet Filter WoL Enable    */
#define RFC_PFILT_WOL_EN    0x00002000  /* Perfect Filter WoL Enable         */

/* Receive Filter WoL Status/Clear Registers */
#define WOL_UCAST           0x00000001  /* Unicast Frame caused WoL          */
#define WOL_BCAST           0x00000002  /* Broadcast Frame caused WoL        */
#define WOL_MCAST           0x00000004  /* Multicast Frame caused WoL        */
#define WOL_UCAST_HASH      0x00000008  /* Unicast Hash Filter Frame WoL     */
#define WOL_MCAST_HASH      0x00000010  /* Multicast Hash Filter Frame WoL   */
#define WOL_PERFECT         0x00000020  /* Perfect Filter WoL                */
#define WOL_RX_FILTER       0x00000080  /* RX Filter caused WoL              */
#define WOL_MAG_PACKET      0x00000100  /* Magic Packet Filter caused WoL    */

/* Interrupt Status/Enable/Clear/Set Registers */
#define INT_RX_OVERRUN      0x00000001  /* Overrun Error in RX Queue         */
#define INT_RX_ERR          0x00000002  /* Receive Error                     */
#define INT_RX_FIN          0x00000004  /* RX Finished Process Descriptors   */
#define INT_RX_DONE         0x00000008  /* Receive Done                      */
#define INT_TX_UNDERRUN     0x00000010  /* Transmit Underrun                 */
#define INT_TX_ERR          0x00000020  /* Transmit Error                    */
#define INT_TX_FIN          0x00000040  /* TX Finished Process Descriptors   */
#define INT_TX_DONE         0x00000080  /* Transmit Done                     */
#define INT_SOFT_INT        0x00001000  /* Software Triggered Interrupt      */
#define INT_WAKEUP          0x00002000  /* Wakeup Event Interrupt            */

/* Power Down Register */
#define PD_POWER_DOWN       0x80000000  /* Power Down MAC                    */

/* RX Descriptor Control Word */
#define RCTRL_SIZE          0x000007FF  /* Buffer size mask                  */
#define RCTRL_INT           0x80000000  /* Generate RxDone Interrupt         */

/* RX Status Hash CRC Word */
#define RHASH_SA            0x000001FF  /* Hash CRC for Source Address       */
#define RHASH_DA            0x001FF000  /* Hash CRC for Destination Address  */

/* RX Status Information Word */
#define RINFO_SIZE          0x000007FF  /* Data size in bytes                */
#define RINFO_CTRL_FRAME    0x00040000  /* Control Frame                     */
#define RINFO_VLAN          0x00080000  /* VLAN Frame                        */
#define RINFO_FAIL_FILT     0x00100000  /* RX Filter Failed                  */
#define RINFO_MCAST         0x00200000  /* Multicast Frame                   */
#define RINFO_BCAST         0x00400000  /* Broadcast Frame                   */
#define RINFO_CRC_ERR       0x00800000  /* CRC Error in Frame                */
#define RINFO_SYM_ERR       0x01000000  /* Symbol Error from PHY             */
#define RINFO_LEN_ERR       0x02000000  /* Length Error                      */
#define RINFO_RANGE_ERR     0x04000000  /* Range Error (exceeded max. size)  */
#define RINFO_ALIGN_ERR     0x08000000  /* Alignment Error                   */
#define RINFO_OVERRUN       0x10000000  /* Receive overrun                   */
#define RINFO_NO_DESCR      0x20000000  /* No new Descriptor available       */
#define RINFO_LAST_FLAG     0x40000000  /* Last Fragment in Frame            */
#define RINFO_ERR           0x80000000  /* Error Occured (OR of all errors)  */

#define RINFO_ERR_MASK     (RINFO_CRC_ERR   | RINFO_SYM_ERR | \
                            RINFO_LEN_ERR   | RINFO_ALIGN_ERR | RINFO_OVERRUN)

/* TX Descriptor Control Word */
#define TCTRL_SIZE          0x000007FF  /* Size of data buffer in bytes      */
#define TCTRL_OVERRIDE      0x04000000  /* Override Default MAC Registers    */
#define TCTRL_HUGE          0x08000000  /* Enable Huge Frame                 */
#define TCTRL_PAD           0x10000000  /* Pad short Frames to 64 bytes      */
#define TCTRL_CRC           0x20000000  /* Append a hardware CRC to Frame    */
#define TCTRL_LAST          0x40000000  /* Last Descriptor for TX Frame      */
#define TCTRL_INT           0x80000000  /* Generate TxDone Interrupt         */

/* TX Status Information Word */
#define TINFO_COL_CNT       0x01E00000  /* Collision Count                   */
#define TINFO_DEFER         0x02000000  /* Packet Deferred (not an error)    */
#define TINFO_EXCESS_DEF    0x04000000  /* Excessive Deferral                */
#define TINFO_EXCESS_COL    0x08000000  /* Excessive Collision               */
#define TINFO_LATE_COL      0x10000000  /* Late Collision Occured            */
#define TINFO_UNDERRUN      0x20000000  /* Transmit Underrun                 */
#define TINFO_NO_DESCR      0x40000000  /* No new Descriptor available       */
#define TINFO_ERR           0x80000000  /* Error Occured (OR of all errors)  */

/* DP83848C PHY Registers */
#define PHY_REG_BMCR        0x00        /* Basic Mode Control Register       */
#define PHY_REG_BMSR        0x01        /* Basic Mode Status Register        */
#define PHY_REG_IDR1        0x02        /* PHY Identifier 1                  */
#define PHY_REG_IDR2        0x03        /* PHY Identifier 2                  */
#define PHY_REG_ANAR        0x04        /* Auto-Negotiation Advertisement    */
#define PHY_REG_ANLPAR      0x05        /* Auto-Neg. Link Partner Abitily    */
#define PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
#define PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */

/* PHY Extended Registers */
#define PHY_REG_STS         0x10        /* Status Register                   */
#define PHY_REG_MICR        0x11        /* MII Interrupt Control Register    */
#define PHY_REG_MISR        0x12        /* MII Interrupt Status Register     */
#define PHY_REG_FCSCR       0x14        /* False Carrier Sense Counter       */
#define PHY_REG_RECR        0x15        /* Receive Error Counter             */
#define PHY_REG_PCSR        0x16        /* PCS Sublayer Config. and Status   */
#define PHY_REG_RBR         0x17        /* RMII and Bypass Register          */
#define PHY_REG_LEDCR       0x18        /* LED Direct Control Register       */
#define PHY_REG_PHYCR       0x19        /* PHY Control Register              */
#define PHY_REG_10BTSCR     0x1A        /* 10Base-T Status/Control Register  */
#define PHY_REG_CDCTRL1     0x1B        /* CD Test Control and BIST Extens.  */
#define PHY_REG_EDCR        0x1D        /* Energy Detect Control Register    */

/* PHY_REG_PHYCON is for the KS8721BL/KSZ8041NL/KSZ8001L  */
#define PHY_REG_PHYCON      0x1F        /* 100BASE-TX PHY Controller register */

/* PHY reg.0 Control Register */
#define _PHY_RESET    0x8000
#define _PHY_LPBK     0x4000
#define _PHY_SPEED    0x2000
#define _PHY_ANEG_EN  0x1000
#define _PHY_PDN      0x0800
#define _PHY_MII_DIS  0x0400
#define _PHY_ANEG_RST 0x0200
#define _PHY_DPLX     0x0100
#define _PHY_COLST    0x0080

/* PHY reg.1 Status Register */
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

/* PHY reg.4 Auto-Negotiation Advertisement Register */
/* PHY reg.5 Auto-Negotiation Remote End Capability Register */
#define _PHY_NP       0x8000
#define _PHY_ACK      0x4000
#define _PHY_RF       0x2000
#define _PHY_T4       0x0200
#define _PHY_TX_FDX   0x0100
#define _PHY_TX_HDX   0x0080
#define _PHY_10_FDX   0x0040
#define _PHY_10_HDX   0x0020
#define _PHY_CSMA     0x0001

/* PHY reg.17 Status Register - DP83848 */
#define _PHY_MII_INTR   0x0080
#define _PHY_REM_FAULT  0x0040
#define _PHY_FULL_DLPX  0x0004
#define _PHY_SPEED10    0x0002
#define _PHY_LINK_UP    0x0001

/* PHY reg 0x1F - KS8721BL/KSZ8041NL/KSZ8001L */
#define _PHY_OP_MODE_MASK  0x001C
#define _PHY_OP_10_HDX     0x0004
#define _PHY_OP_100_HDX    0x0008
#define _PHY_OP_10_FDX     0x0014
#define _PHY_OP_100_FDX    0x0018

#define DP83848C_DEF_ADR    0x0001      /* Default PHY device address */
#define DP83848C_ID1        0x2000      /* PHY Identifier 1           */
#define DP83848C_ID2        0x5C90      /* PHY Identifier 2           */

#define KS8721BL_ID1        0x0022      /* PHY Identifier 1            */
#define KS8721BL_ID2        0x1619      /* PHY Identifier 2            */

#define KSZ8041NL_ID1       0x0022      /* PHY Identifier 1            */
#define KSZ8041NL_ID2       0x1512      /* PHY Identifier 2            */

#define EA_BOARD_DEF_ADR    0x0000      /* Default PHY device address */
#define KSZ8001L_ID1        0x0022      /* PHY Identifier 1            */
#define KSZ8001L_ID2        0x161A      /* PHY Identifier 2            */

#define _PHY_ID2_MASK       0xFFF0      /* To mask off the revision */

#if (defined(__IAR_SYSTEMS_ICC__))
#define MAC_MAC1              MAC1
#define MAC_MAC2              MAC2
#define MAC_COMMAND           COMMAND
#define MAC_MAXF              MAXF
#define MAC_CLRT              CLRT
#define MAC_IPGR              IPGR
#define MAC_IPGT              IPGT
#define MAC_SUPP              SUPP
#define MAC_TEST              TEST
#define MAC_SA0               SA0
#define MAC_SA1               SA1
#define MAC_SA2               SA2
#define MAC_RXFILTERCTRL      RXFILTERCTRL
#define MAC_RXDESCRIPTOR      RXDESCRIPTOR
#define MAC_RXSTATUS          RXSTATUS
#define MAC_RXDESCRIPTORNUM   RXDESCRIPTORNUMBER
#define MAC_RXPRODUCEINDEX    RXPRODUCEINDEX
#define MAC_RXCONSUMEINDEX    RXCONSUMEINDEX
#define MAC_TXDESCRIPTOR      TXDESCRIPTOR
#define MAC_TXSTATUS          TXSTATUS
#define MAC_TXDESCRIPTORNUM   TXDESCRIPTORNUMBER
#define MAC_TXPRODUCEINDEX    TXPRODUCEINDEX
#define MAC_TXCONSUMEINDEX    TXCONSUMEINDEX
#define MAC_INTENABLE         INTENABLE
#define MAC_INTCLEAR          INTCLEAR
#define MAC_INTSTATUS         INTSTATUS
#define MAC_MCFG              MCFG
#define MAC_MCMD              MCMD
#define MAC_MADR              MADR
#define MAC_MIND              MIND
#define MAC_MRDD              MRDD
#define MAC_MWTD              MWTD
#endif

#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

