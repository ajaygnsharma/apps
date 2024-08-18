/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef ARP_H_INC
#define ARP_H_INC 1

#define ARP_CODE_HB  0x00
#define ARP_REQUEST  0x01
#define ARP_REPLY    0x02

#if MN_ARP
typedef struct arp_info_s {
   byte eth_dest_ip_addr[IP_ADDR_LEN];
   byte eth_dest_hw_addr[ETH_ADDR_LEN];
#if MN_ARP_TIMEOUT
   word16 time_to_live;
#endif      /* MN_ARP_TIMEOUT */
#if (RTOS_USED == RTOS_NONE)
   byte flag;
#endif      /* (RTOS_USED == RTOS_NONE) */
} ARP_INFO_T;

typedef ARP_INFO_T * PARP_INFO;

#define ARP_ENTRY_IN_USE   1

void mn_arp_init(void) cmx_reentrant;
void mn_arp_update(byte [], byte []) cmx_reentrant;
PARP_INFO mn_arp_lookup(byte []) cmx_reentrant;
#if MN_ARP_TIMEOUT
#if (defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_arp_update_timer(void);
#else
void mn_arp_update_timer(void) cmx_reentrant;
#endif      /* (defined(_CC51)) */
#endif      /* MN_ARP_TIMEOUT */
#endif      /* MN_ARP */

SCHAR mn_arp_process_packet(void) cmx_reentrant;

#endif   /* #ifndef ARP_H_INC */

