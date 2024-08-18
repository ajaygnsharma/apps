#ifndef CS8900A_H_INC
#define CS8900A_H_INC 1

/* Packet Page Addresses */
#define PP_ProductID       0x0000
#define PP_IOBaseAddr      0x0020
#define PP_IntNum          0x0022

#define PP_TxCMDReq        0x0144
#define PP_TxLength        0x0146

#define PP_IndAddr         0x0158
#define PP_IndAddr2        0x015A
#define PP_IndAddr4        0x015C

#define PP_ISQ             0x0120
#define PP_RxCFG           0x0102
#define PP_RxEvent         0x0124
#define PP_RxCTL           0x0104
#define PP_TxCFG           0x0106
#define PP_TxEvent         0x0128
#define PP_TxCMD           0x0108
#define PP_BufCFG          0x010A
#define PP_BufEvent        0x012C
#define PP_RxMISS          0x0130
#define PP_TxCOL           0x0132
#define PP_LineCTL         0x0112
#define PP_LineST          0x0134
#define PP_SelfCTL         0x0114
#define PP_SelfST          0x0136
#define PP_BusCTL          0x0116
#define PP_BusST           0x0138
#define PP_TestCTL         0x0118

/* Bit definitions */

/* RxCFG */
#define CS_SKIP_1          0x0040
#define CS_RX_OK_IE        0x0100
#define CS_BUFFER_CRC      0x0800
#define CS_CRC_ERROR_IE    0x1000
#define CS_RUNT_IE         0x2000
#define CS_EXTRADATA_IE    0x4000

/* RxEvent */
#define CS_RX_OK           0x0100
#define CS_IND_ADDR        0x0400
#define CS_BROADCAST       0x0800
#define CS_CRC_ERROR       0x1000
#define CS_RUNT            0x2000
#define CS_EXTRA_DATA      0x4000

/* RxCTL */
#define CS_IA_HASH_A       0x0040
#define CS_PROMISCUOUS_A   0x0080
#define CS_RX_OK_A         0x0100
#define CS_MULTICAST_A     0x0200
#define CS_INDIVIDUAL_A    0x0400
#define CS_BROADCAST_A     0x0800
#define CS_CRC_ERROR_A     0x1000
#define CS_RUNT_A          0x2000
#define CS_EXTRA_DATA_A    0x4000

/* TxCFG */
#define CS_TX_OK_IE        0x0100
#define CS_TX_ALL_IE       0x8FC0

/* TxEvent */
#define CS_TX_OK           0x0100
#define CS_TX_ERROR        0x86C0

/* TxCMD */
#define CS_TX_START_ALL    0x00C0
#define CS_TX_PAD_DIS      0x2000

/* BufCFG */
#define CS_RDY_4_TX_IE     0x0100
#define CS_TX_UNDERRUN_IE  0x0200
#define CS_RX_MISS_IE      0x0400
#define CS_TX_COL_OVFLO_IE 0x1000
#define CS_MISS_OVFLO_IE   0x2000
#define MN_BUFCFG_IE       0x3300

/* BufEvent */
#define CS_RDY_4_TX        0x0100
#define CS_TX_UNDERRUN     0x0200
#define CS_RX_MISS         0x0400

/* LineCTL*/
#define CS_SER_RX_ON       0x0040
#define CS_SER_TX_ON       0x0080

/* LineST */
#define CS_LINK_OK         0x0080

/* SelfCTL */
#define CS_RESET           0x0040

/* SelfST */
#define CS_INITD           0x0080

/* BusCTL */
#define CS_MEMORY_E        0x0400
#define CS_USE_SA          0x0800
#define CS_ENABLE_IRQ      0x8000

/* BusST */
#define CS_TX_BID_ERR      0x0080
#define CS_RDY_4_TX_NOW    0x0100

/* TestCTL */
#define CS_FDX             0x4000

/* Ports */
#define RECV_PORT          0x0000
#define RECV_PORT1         0x0001
#define XMIT_PORT          0x0000
#define XMIT_PORT1         0x0001
#define TX_CMD_PORT        0x0004
#define TX_LENGTH_PORT     0x0006
#define ISQ_PORT           0x0008
#define PP_POINTER_PORT    0x000A
#define PP_POINTER_PORT1   0x000B
#define PP_DATA_PORT       0x000C
#define PP_DATA_PORT1      0x000D

/* Misc. defines */
#define MIN_PKT_SIZE       60
#define EISA_REG_NUM       0x630E
#define DEFAULT_BASE       0x0300
#define CS8900A_SIG        0x3000

#endif      /* #ifndef CS8900A_H_INC */
