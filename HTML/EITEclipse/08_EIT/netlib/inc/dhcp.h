/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef DHCP_H_INC
#define DHCP_H_INC   1

/* we will be using the bootpMode variable for DHCP also since other parts
   of the stack are already using this variable. It will make things simpler
   than saying: if (bootpMode == BOOTP_ACTIVE || dhcpMode == DHCP_ACTIVE)
*/
#define BOOTP_ACTIVE          1
#define BOOTP_NO_BCAST        0
#define BOOTP_INACTIVE        -1

#define BOOTP_OP_REQ          1
#define BOOTP_OP_REP          2

#define BOOTP_BROADCAST_ADDR  0xffffffff

#define DHCP_INFINITE_LEASE   0xffffffff
#define DHCP_RETRY_TIME       4

/* interval to wake up mn_dhcp_lease_task */
#define DHCP_WAIT_TIME        (10*(MN_ONE_SECOND))

#define DHCPDISCOVER 1
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPDECLINE  4
#define DHCPACK      5
#define DHCPNAK      6
#define DHCPRELEASE  7
#define DHCPINFORM   8

#define DHCP_DEAD          0
#define DHCP_OK            9
#define DHCP_RENEWING      10
#define DHCP_REBINDING     11

#define SUBNET_MASK_CODE      1
#define SUBNET_MASK_LEN       4
#define ROUTER_CODE           3
#define DNS_SERVER_CODE       6
#define REQ_IP_ADDR_CODE      50
#define REQ_IP_ADDR_LEN       4
#define REQ_IP_LEASE_CODE     51
#define REQ_IP_LEASE_LEN      4
#define DHCP_MSG_TYPE_CODE    53
#define DHCP_MSG_TYPE_LEN     1
#define DHCP_SERVER_ID_CODE   54
#define DHCP_SERVER_ID_LEN    4
#define PARAMETER_LIST_CODE   55
#define DHCP_CLIENT_ID_CODE   61
#define DHCP_CLIENT_ID_LEN    7

#define DHCP_HOSTNAME_CODE    12
#define TFTP_SERVER_CODE      66

#if (MN_DNS)
#define NUM_PARAMETERS        3
#else
#define NUM_PARAMETERS        2
#endif

#define PAD_BYTE     0x00
#define END_BYTE     0xff
#define COOKIE_LEN   4
#define MSG_TYPE_POS 6

#define DHCP_MAC_LEN          16
#define DHCP_SNAME_LEN        64    /* length of server name in DHCP structure */
#define DHCP_FILE_LEN         128   /* length of filename in DHCP structure */
#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLMSP430) || defined(CMXMSP430) || \
   defined(POLP18F97J60) || defined(CMXP18F97J60))
/* Per RFC2131 the DHCP_OPT_LEN should be at least 312. To save RAM on some
   processors we set this to 64 in violation of the RFC. It works on the DHCP
   servers we have tested, but may not work for other DHCP servers. Even
   with DHCP_OPT_LEN set to 64 the size of the DHCP_INFO_T structure is 
   300 bytes.
*/
#define DHCP_OPT_LEN          64   /* length of options in DHCP structure */
#else
#define DHCP_OPT_LEN          312  /* length of options in DHCP structure */
                                    /* for DHCP this should be at least 312 */
#endif

/* size of structure is 548 bytes minimum */
typedef struct dhcp_s
{
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

   byte chaddr[DHCP_MAC_LEN];    /* client hardware address */

   byte sname[DHCP_SNAME_LEN];   /* optional server host name */
   byte file[DHCP_FILE_LEN];     /* boot file name */

   byte options[DHCP_OPT_LEN];   /* options */
} DHCP_INFO_T;

typedef DHCP_INFO_T * PDHCP_INFO;

typedef struct dhcp_lease_s
{
   word32 org_lease_time;                 /* last requested lease time */
   volatile word32 lease_time;            /* seconds left in current lease */
   word32 t1_renew_time;                  /* time to make renew request */
   word32 t2_renew_time;                  /* time to make rebind request */
   volatile byte dhcp_state;              /* current dhcp state */
   byte infinite_lease;                   /* infinite lease TRUE or FALSE */
   byte server_id[DHCP_SERVER_ID_LEN];    /* DHCP server IP address */
} DHCP_LEASE_T;

typedef DHCP_LEASE_T * PDHCP_LEASE;

void mn_dhcp_init(void) cmx_reentrant;
int mn_dhcp_start(byte *, word32) cmx_reentrant;
int mn_dhcp_release(void) cmx_reentrant;
int mn_dhcp_renew(word32) cmx_reentrant;
#if (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_dhcp_update_timer(void);
#else
void mn_dhcp_update_timer(void) cmx_reentrant;
#endif      /* (defined(_CC51)) */

#endif   /* #ifndef DHCP_H_INC */


