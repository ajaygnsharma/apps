/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef BOOTP_H_INC
#define BOOTP_H_INC  1

#define BOOTP_ACTIVE          1
#define BOOTP_INACTIVE        -1

#define BOOTP_OP_REQ          1
#define BOOTP_OP_REP          2

#define BOOTP_BROADCAST_ADDR  0xffffffff

/* Per RFC1542 the bootp header must be 300 bytes minimum. With the following
   sizes the bootp header structure below is 300 bytes.
*/
#define BOOTP_MAC_LEN         16
#define BOOTP_FILE_LEN        128   /* length of filename in BOOTP structure */
#define BOOTP_SNAME_LEN       64 /* length of server name in BOOTP structure */
#define BOOTP_VENDOR_LEN      64 /* length of vendor name in BOOTP structure */
                                 /* for DHCP this should be at least 312 */

/* vend option codes */
#define SUBNET_MASK_CODE      1
#define ROUTER_CODE           3
#define DNS_SERVER_CODE       6

/* other vend option definitions */
#define PAD_BYTE     0x00
#define END_BYTE     0xff
#define COOKIE_LEN   4

typedef struct bootp_s
{
   /* All field names match RFC951 except flag which is per RFC1542,
      it is unused in RFC951.
   */

   byte op;                      /* opcode, request or reply */
   byte htype;                   /* hardware type */
   byte hlen;                    /* hardware address length */
   byte hops;                    /* always zero for clients */

   byte xid[4];                  /* random transaction ID */

   byte secs[2];                 /* seconds elapsed since trying to boot */
   byte flag[2];                 /* broadcast flag */

   byte ciaddr[IP_ADDR_LEN];     /* client IP address submitted */
   byte yiaddr[IP_ADDR_LEN];     /* client IP address returned by server */
   byte siaddr[IP_ADDR_LEN];     /* server IP address returned by server */
   byte giaddr[IP_ADDR_LEN];     /* optional gateway IP address */

   byte chaddr[BOOTP_MAC_LEN];   /* client hardware address */

   byte sname[BOOTP_SNAME_LEN];  /* optional server host name */
   byte file[BOOTP_FILE_LEN];    /* boot file name */

   byte vend[BOOTP_VENDOR_LEN];  /* optional vendor-specific area */
} BOOTP_INFO_T;

typedef BOOTP_INFO_T * PBOOTP_INFO;

void mn_bootp_init(void) cmx_reentrant;
int mn_bootp(byte *fileName, PBOOTP_INFO bpi) cmx_reentrant;

#endif   /* #ifndef BOOTP_H_INC */


