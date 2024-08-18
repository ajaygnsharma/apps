/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef TFTP_H_INC
#define TFTP_H_INC   1

#define TFTP_DATA_SIZE        512   /* Size of data packet */
#define TFTP_HEADER_SIZE      4
#define TFTP_PACKET_SIZE      (TFTP_DATA_SIZE + TFTP_HEADER_SIZE)
#define TFTP_SERVER_INIT_TID  (TFTP_SERVER_PORT)

/* Op code types */
#define TFTP_RRQ     0x0001   /* Used by client */
#define TFTP_WRQ     0x0002   /* Not used by client */
#define TFTP_DATA    0x0003   /* Used by client  */
#define TFTP_ACK     0x0004   /* Used by client */
#define TFTP_ERROR   0x0005   /* Used by client */
#define TFTP_OPT_ACK 0x0006   /* Used by client */

/* error codes */
#define TFTP_ERR_ND                 0x0000 /* Used */
#define TFTP_ERR_FILENOTFOUND       0x0001 /* Used */
#define TFTP_ERR_ACCESS_VIOLATION   0x0002 /* Not used by client */
#define TFTP_ERR_OUT_OF_SPACE       0x0003 /* Used */
#define TFTP_ERR_ILL_TFTP_OP        0x0004 /* Used */
#define TFTP_ERR_UNKNOWN_TID        0x0005 /* Used */
#define TFTP_ERR_DUPLICATE_FILE     0x0006 /* Not used by client */
#define TFTP_ERR_NO_USER            0x0007 /* Not used by client */
#define TFTP_UNREQUESTED_OPTION     0x0008 /* Used */

/* Offsets into tftp_pkt array */
#define OFS_OPCODE0  0
#define OFS_OPCODE1  1
#define OFS_BLOCK0   2
#define OFS_BLOCK1   3
#define OFS_ERRCODE0 2
#define OFS_ERRCODE1 3
#define OFS_DATA0    2
#define OFS_DATA2    4

long mn_tftp_get_file(byte *, byte *, byte *, long, SCHAR) cmx_reentrant;

#endif   /* #ifndef TFTP_H_INC */


