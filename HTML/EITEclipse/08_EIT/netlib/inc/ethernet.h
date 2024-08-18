/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef ETHERNET_H_INC
#define ETHERNET_H_INC  1

#define ETH_HEADER_LEN  14
#define ETHER_SIZE      1518

/* packet types */
#define ETH_IP_TYPE  0x0800
#define ETH_ARP_TYPE 0x0806
#define ETH_TYPE_HB  0x08
#define ETH_IP_LB    0x00
#define ETH_ARP_LB   0x06

/* hardware type */
#define ETHERNET_10MB_HB   0x00
#define ETHERNET_10MB_LB   0x01

int mn_ether_init(void) cmx_reentrant;
SCHAR ether_recv_header(void) cmx_reentrant;
SCHAR mn_ether_start_packet(PSOCKET_INFO,word16,byte) cmx_reentrant;
void start_arp_packet(PSOCKET_INFO,byte) cmx_reentrant;

/* ----------------------------------------- */
/* Ethernet macros                           */
/* ----------------------------------------- */

/* the function defined for ETHER_POLL_RECV should return number of bytes
   received if successful or negative number on error. define this macro
   only if using the ethernet chip in polled mode.

   the function defined for ETHER_INIT should return 1 if initialization
   successful or negative number on error.

   the function defined for ETHER_SEND should return the number of bytes
   sent or negative number on error. It takes two parameters, a pointer to
   the socket used and the number of data bytes sent. Note that the number
   of data bytes sent will be zero for TCP control packets and ARP packets.
*/
#if (defined(POLAVRR) || defined(CMXAVRR))
int rtl8019_recv(void) cmx_reentrant;
int rtl8019_init(void) cmx_reentrant;
int rtl8019_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       rtl8019_recv()
#define ETHER_INIT            rtl8019_init()
#define ETHER_SEND(p,n)       rtl8019_send((p),(n))
#elif (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || defined(CMXPIC18E))
int rtl8019_recv(void) cmx_reentrant;
int rtl8019_init(void) cmx_reentrant;
int rtl8019_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       rtl8019_recv()
#define ETHER_INIT            rtl8019_init()
#define ETHER_SEND(p,n)       rtl8019_send((p),(n))
#elif (defined(POLP18F97J60) || defined(CMXP18F97J60))
#if MN_INTERNAL_ETHERNET
int eth97j60_recv(void) cmx_reentrant;
int eth97j60_init(void) cmx_reentrant;
int eth97j60_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       eth97j60_recv()
#define ETHER_INIT            eth97j60_init()
#define ETHER_SEND(p,n)       eth97j60_send((p),(n))
#else
int enc28j60_recv(void) cmx_reentrant;
int enc28j60_init(void) cmx_reentrant;
int enc28j60_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       enc28j60_recv()
#define ETHER_INIT            enc28j60_init()
#define ETHER_SEND(p,n)       enc28j60_send((p),(n))
#endif
#elif (defined(POLPIC24F) || defined(CMXPIC24F) || \
   defined(POLPIC24H) || defined(CMXPIC24H) || \
   defined(POLDSPIC33) || defined(CMXDSPIC33) || \
   defined(POLPIC32) || defined(CMXPIC32))
int enc28j60_recv(void) cmx_reentrant;
int enc28j60_init(void) cmx_reentrant;
int enc28j60_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       enc28j60_recv()
#define ETHER_INIT            enc28j60_init()
#define ETHER_SEND(p,n)       enc28j60_send((p),(n))
#elif (defined(POLDSPICE) || defined(CMXDSPICE))
int rtl8019_recv(void) cmx_reentrant;
int rtl8019_init(void) cmx_reentrant;
int rtl8019_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       rtl8019_recv()
#define ETHER_INIT            rtl8019_init()
#define ETHER_SEND(p,n)       rtl8019_send((p),(n))
#elif (defined(POLDSPICPRO) || defined(CMXDSPICPRO))
/* wireless ethernet driver settings */
int wlan_recv(void) cmx_reentrant;
int wlan_init(void) cmx_reentrant;
int wlan_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       wlan_recv()
#define ETHER_INIT            wlan_init()
#define ETHER_SEND(p,n)       wlan_send((p),(n))
#elif (defined(POLPHY165) || defined(CMXPHY165) || defined(POLTQS167) || \
   defined(POL188) || defined(CMXTQS167) || defined(CMX188))
int smsc91C96_recv(void) cmx_reentrant;
int smsc91C96_init(void) cmx_reentrant;
int smsc91C96_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C96_recv()
#define ETHER_INIT            smsc91C96_init()
#define ETHER_SEND(p,n)       smsc91C96_send((p),(n))
#elif (defined(POLH8S2674) || defined(CMXH8S2674))
int smsc91C96_recv(void) cmx_reentrant;
int smsc91C96_init(void) cmx_reentrant;
int smsc91C96_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C96_recv()
#define ETHER_INIT            smsc91C96_init()
#define ETHER_SEND(p,n)       smsc91C96_send((p),(n))
#elif (defined(POLH8S2329) || defined(CMXH8S2329))
int smsc91C96_recv(void) cmx_reentrant;
int smsc91C96_init(void) cmx_reentrant;
int smsc91C96_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C96_recv()
#define ETHER_INIT            smsc91C96_init()
#define ETHER_SEND(p,n)       smsc91C96_send((p),(n))
#elif (defined(POLTRI51_SMSC) || defined(CMXTRI51_SMSC))
int smsc91C111_recv(void) cmx_reentrant;
int smsc91C111_init(void) cmx_reentrant;
int smsc91C111_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C111_recv()
#define ETHER_INIT            smsc91C111_init()
#define ETHER_SEND(p,n)       smsc91C111_send((p),(n))
#elif (defined(POLDS80C400) || defined(CMXDS80C400))
int tini400_recv(void) cmx_reentrant;
int tini400_init(void) cmx_reentrant;
int tini400_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       tini400_recv()
#define ETHER_INIT            tini400_init()
#define ETHER_SEND(p,n)       tini400_send((p),(n))
#elif (defined(POLR8051XC_MAC_L) || defined(CMXR8051XC_MAC_L))
int mac_l_recv(void) cmx_reentrant;
int mac_l_init(void) cmx_reentrant;
int mac_l_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       mac_l_recv()
#define ETHER_INIT            mac_l_init()
#define ETHER_SEND(p,n)       mac_l_send((p),(n))
#elif (defined(POLC8051F120) || defined(CMXC8051F120))
int cp2200_init(void) cmx_reentrant;
int cp2200_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV
#define ETHER_INIT            cp2200_init()
#define ETHER_SEND(p,n)       cp2200_send((p),(n))
#elif (defined(POLECOG1E) || defined(CMXECOG1E))
int smsc91C111_recv(void) cmx_reentrant;
int smsc91C111_init(void) cmx_reentrant;
int smsc91C111_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C111_recv()
#define ETHER_INIT            smsc91C111_init()
#define ETHER_SEND(p,n)       smsc91C111_send((p),(n))
#elif (defined(POLM16CP) || defined(CMXM16CP))
int smsc91C111_recv(void) cmx_reentrant;
int smsc91C111_init(void) cmx_reentrant;
int smsc91C111_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C111_recv()
#define ETHER_INIT            smsc91C111_init()
#define ETHER_SEND(p,n)       smsc91C111_send((p),(n))
#elif (defined(POLM16C654) || defined(CMXM16C654))
int smsc91C96_recv(void) cmx_reentrant;
int smsc91C96_init(void) cmx_reentrant;
int smsc91C96_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C96_recv()
#define ETHER_INIT            smsc91C96_init()
#define ETHER_SEND(p,n)       smsc91C96_send(p,n)
#elif (defined(POLM32C) || defined(CMXM32C))
int enc28j60_recv(void) cmx_reentrant;
int enc28j60_init(void) cmx_reentrant;
int enc28j60_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       enc28j60_recv()
#define ETHER_INIT            enc28j60_init()
#define ETHER_SEND(p,n)       enc28j60_send((p),(n))
#elif (defined(POLARM2294) || defined(CMXARM2294))
int smsc91C111_recv(void) cmx_reentrant;
int smsc91C111_init(void) cmx_reentrant;
int smsc91C111_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C111_recv()
#define ETHER_INIT            smsc91C111_init()
#define ETHER_SEND(p,n)       smsc91C111_send((p),(n))
#elif (defined(POLNECV850ES) || defined(CMXNECV850ES))
int smsc91C111_recv(void) cmx_reentrant;
int smsc91C111_init(void) cmx_reentrant;
int smsc91C111_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C111_recv()
#define ETHER_INIT            smsc91C111_init()
#define ETHER_SEND(p,n)       smsc91C111_send((p),(n))
#elif (defined(POLV850EPH3) || defined(CMXV850EPH3))
int smsc9218_recv(void) cmx_reentrant;
int smsc9218_init(void) cmx_reentrant;
int smsc9218_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc9218_recv()
#define ETHER_INIT            smsc9218_init()
#define ETHER_SEND(p,n)       smsc9218_send((p),(n))
#elif (defined(POLM16C654) || defined(CMXM16C654))
int smsc91C96_recv(void) cmx_reentrant;
int smsc91C96_init(void) cmx_reentrant;
int smsc91C96_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C96_recv()
#define ETHER_INIT            smsc91C96_init()
#define ETHER_SEND(p,n)       smsc91C96_send((p),(n))
#elif (defined(POLST7E) || defined(CMXST7E))
int rtl8019_recv(void) cmx_reentrant;
int rtl8019_init(void) cmx_reentrant;
int rtl8019_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       rtl8019_recv()
#define ETHER_INIT            rtl8019_init()
#define ETHER_SEND(p,n)       rtl8019_send((p),(n))
#elif (defined(POLHC12E) || defined(CMXHC12E))
int smsc91C111_recv(void) cmx_reentrant;
int smsc91C111_init(void) cmx_reentrant;
int smsc91C111_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C111_recv()
#define ETHER_INIT            smsc91C111_init()
#define ETHER_SEND(p,n)       smsc91C111_send((p),(n))
#elif (defined(POLHC12NE64) || defined(CMXHC12NE64))
int ne64_init(void) cmx_reentrant;
int ne64_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV
#define ETHER_INIT            ne64_init()
#define ETHER_SEND(p,n)       ne64_send((p),(n))
#elif (defined(POLH8S2329) || defined(CMXH8S2329))
int smsc91C96_recv(void) cmx_reentrant;
int smsc91C96_init(void) cmx_reentrant;
int smsc91C96_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       smsc91C96_recv()
#define ETHER_INIT            smsc91C96_init()
#define ETHER_SEND(p,n)       smsc91C96_send((p),(n))
#elif (defined(POLH8S2238) || defined(CMXH8S2238))
/* wireless ethernet driver settings */
int wlan_recv(void) cmx_reentrant;
int wlan_init(void) cmx_reentrant;
int wlan_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       wlan_recv()
#define ETHER_INIT            wlan_init()
#define ETHER_SEND(p,n)       wlan_send((p),(n))
#elif (defined(POLH8S2472) || defined(CMXH8S2472))
int h8s2472_recv(void) cmx_reentrant;
int h8s2472_init(void) cmx_reentrant;
int h8s2472_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       h8s2472_recv()
#define ETHER_INIT            h8s2472_init()
#define ETHER_SEND(p,n)       h8s2472_send((p),(n))
#elif (defined(POLARM9) || defined(CMXARM9) || \
   defined(POLSAM7X256) || defined(CMXSAM7X256) || \
   defined(POLSAM9263) || defined(CMXSAM9263) || \
   defined(POLSAM9260) || defined(CMXSAM9260))
int AT91F_EmacRecv(void) cmx_reentrant;
int AT91F_EmacEntry(void) cmx_reentrant;
int AT91F_EmacSend(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       AT91F_EmacRecv()
#define ETHER_INIT            AT91F_EmacEntry()
#define ETHER_SEND(p,n)       AT91F_EmacSend((p),(n))
#elif (defined(POLSAM7SE512) || defined(CMXSAM7SE512))
int dm9000_recv(void) cmx_reentrant;
int dm9000_init(void) cmx_reentrant;
int dm9000_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       dm9000_recv()
#define ETHER_INIT            dm9000_init()
#define ETHER_SEND(p,n)       dm9000_send((p),(n))
#elif (defined(POLEZ80F91) || defined(CMXEZ80F91))
int f91eth_recv(void) cmx_reentrant;
int f91eth_init(void) cmx_reentrant;
int f91eth_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       f91eth_recv()
#define ETHER_INIT            f91eth_init()
#define ETHER_SEND(p,n)       f91eth_send((p),(n))
#elif (defined(POLEZ8E) || defined(CMXEZ8E))
int rtl8019_recv(void) cmx_reentrant;
int rtl8019_init(void) cmx_reentrant;
int rtl8019_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       rtl8019_recv()
#define ETHER_INIT            rtl8019_init()
#define ETHER_SEND(p,n)       rtl8019_send((p),(n))
#elif (defined(POLMCBXC167) || defined(CMXMCBXC167))
int ax88796_recv(void) cmx_reentrant;
int ax88796_init(void) cmx_reentrant;
int ax88796_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       ax88796_recv()
#define ETHER_INIT            ax88796_init()
#define ETHER_SEND(p,n)       ax88796_send((p),(n))
#elif (defined(POLTI55X) || defined(CMXTI55X))
#if 0
int enc28j60_recv(void) cmx_reentrant;
int enc28j60_init(void) cmx_reentrant;
int enc28j60_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       enc28j60_recv()
#define ETHER_INIT            enc28j60_init()
#define ETHER_SEND(p,n)       enc28j60_send((p),(n))
#else
int cs8900_recv(void) cmx_reentrant;
int cs8900_init(void) cmx_reentrant;
int cs8900_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       cs8900_recv()
#define ETHER_INIT            cs8900_init()
#define ETHER_SEND(p,n)       cs8900_send((p),(n))
#endif
#elif (defined(POLMCF5282) || defined(CMXMCF5282) || \
      defined(POLMCF5208) || defined(CMXMCF5208) || \
      defined(POLMCF5223) || defined(CMXMCF5223) || \
      defined(POLMCF5235) || defined(CMXMCF5235))
int m5282fec_recv(void) cmx_reentrant;
int m5282fec_init(void) cmx_reentrant;
int m5282fec_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       m5282fec_recv()
#define ETHER_INIT            m5282fec_init()
#define ETHER_SEND(p,n)       m5282fec_send((p),(n))
#elif (defined(POLMPC5566) || defined(CMXMPC5566))
int mpc55xxfec_recv(void) cmx_reentrant;
int mpc55xxfec_init(void) cmx_reentrant;
int mpc55xxfec_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       mpc55xxfec_recv()
#define ETHER_INIT            mpc55xxfec_init()
#define ETHER_SEND(p,n)       mpc55xxfec_send((p),(n))
#elif (defined(POLARM2368) || defined(CMXARM2368) || \
   defined(POLARM2378) || defined(CMXARM2378) || \
   defined(POLARM2468) || defined(CMXARM2468))
int lpc2368_recv(void) cmx_reentrant;
int lpc2368_init(void) cmx_reentrant;
int lpc2368_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       lpc2368_recv()
#define ETHER_INIT            lpc2368_init()
#define ETHER_SEND(p,n)       lpc2368_send((p),(n))
#elif (defined(POLARMSTR9) || defined(CMXARMSTR9) || \
   defined(POLARMSTR9R) || defined(CMXARMSTR9R))
int str91x_recv(void) cmx_reentrant;
int str91x_init(void) cmx_reentrant;
int str91x_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       str91x_recv()
#define ETHER_INIT            str91x_init()
#define ETHER_SEND(p,n)       str91x_send((p),(n))
#elif (defined(POLLM3Sx9xx) || defined(CMXLM3Sx9xx))
int lm3s_recv(void) cmx_reentrant;
int lm3s_init(void) cmx_reentrant;
int lm3s_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       lm3s_recv()
#define ETHER_INIT            lm3s_init()
#define ETHER_SEND(p,n)       lm3s_send((p),(n))
#else
#if 0
/* wireless ethernet driver settings */
int wlan_recv(void) cmx_reentrant;
int wlan_init(void) cmx_reentrant;
int wlan_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       wlan_recv()
#define ETHER_INIT            wlan_init()
#define ETHER_SEND(p,n)       wlan_send((p),(n))
#else
int cs8900_recv(void) cmx_reentrant;
int cs8900_init(void) cmx_reentrant;
int cs8900_send(PSOCKET_INFO, word16) cmx_reentrant;
#define ETHER_POLL_RECV       cs8900_recv()
#define ETHER_INIT            cs8900_init()
#define ETHER_SEND(p,n)       cs8900_send((p),(n))
#endif
#endif

#if (!defined(ETHER_INIT))
#error Ethernet selected but not set up! Possibly the wrong batch/project file was used.
#endif
#if (MN_POLLED_ETHERNET && !defined(ETHER_POLL_RECV))
#error Polled Ethernet selected but not set up! Possibly the wrong batch/project file was used.
#endif

#endif   /* #ifndef ETHERNET_H_INC */

