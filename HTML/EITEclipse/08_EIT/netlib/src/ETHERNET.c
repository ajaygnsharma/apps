/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if MN_ETHERNET

static void sendHWAddr(PSOCKET_INFO, byte) cmx_reentrant;
#if MN_ARP
static byte same_net_number(byte *ip_addr1,byte *ip_addr2) cmx_reentrant;
#endif      /* MN_ARP */

/********************************************************************/

int mn_ether_init(void)
cmx_reentrant {
   return (ETHER_INIT);       /* macro defined in ethernet.h */
}

/* Processes the ethernet header, handling ARP calls if necessary. Returns 1
   if the packet is an IP packet, returns 0 if arp packet, otherwise returns
   -1.
*/
SCHAR ether_recv_header(void)
cmx_reentrant {
   byte eth_type_hb, eth_type_lb;
   byte i;
   SCHAR retval;
#if (!MN_ARP)
   byte isBcast;

   isBcast = TRUE;
#endif      /* (!MN_ARP) */

   retval = -1;

#if MN_DHCP
   if (dhcp_lease.dhcp_state != DHCP_DEAD)
      {
#endif      /* MN_DHCP */

      /* ethernet header format:

         dest hw addr (6 bytes), source hw addr (6 bytes)

         the next two bytes are 0x0800 for IP and 0x0806 for ARP. There
         are other possiblities but we ignore them for now.
      */
      for (i=0;i<ETH_ADDR_LEN;i++)
         {
#if (!MN_ARP)
         if ( mn_recv_byte() != 0xff )
            isBcast = FALSE;
#else
         (void)mn_recv_byte();
#endif      /* (!MN_ARP) */
         }

      /* The CS8900 does the filtering for us so we know the addr is ok if
         we get here. This may need to change for other ethernet chips.
      */


      /* save the hw addr for use later */
      MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
      for (i=0;i<ETH_ADDR_LEN;i++)
         eth_dest_hw_addr[i] = (byte)mn_recv_byte();
      MN_MUTEX_RELEASE(MUTEX_MN_INFO);

      /* check type for IP, ARP or unknown */
      eth_type_hb = (byte)mn_recv_byte();
      eth_type_lb = (byte)mn_recv_byte();

      if (eth_type_hb == ETH_TYPE_HB)
         {
         switch (eth_type_lb)
            {
            case ETH_IP_LB:                /* IP */
               retval = 1;
               break;
            case ETH_ARP_LB:               /* ARP */
#if (MN_BOOTP)
               /* we can't process ARP packets if in bootp mode */
               if (bootpMode != BOOTP_ACTIVE)
#elif (MN_DHCP)
               if ((bootpMode != BOOTP_ACTIVE) || \
                     (dhcp_lease.dhcp_state >= DHCP_OK))
#endif      /* (MN_BOOTP || MN_DHCP) */
                  {
#if (!MN_ARP)
                  if (isBcast)
                     {
#endif      /* (!MN_ARP) */
                     /* handle ARP packet */
                     retval = mn_arp_process_packet();            
#if (!MN_ARP)
                     }
#endif      /* (!MN_ARP) */
                  }
               break;
            default:                /* unknown type */
               break;
            }
         }
   
      /* need to discard the rest of the packet if no further processing */
      if (retval <= 0)
         mn_ip_discard_packet();

#if MN_DHCP
      }
#endif      /* MN_DHCP */

   return (retval);
}

/* returns 1 if able to start the packet, otherwise returns a negative number.
*/
SCHAR mn_ether_start_packet(PSOCKET_INFO socket_ptr,word16 eth_type,byte isBcast)
cmx_reentrant {
#if MN_ARP
   PARP_INFO parp;
   PINTERFACE_INFO interface_ptr;
#endif      /* MN_ARP */
#if (MN_ARP)
   byte i;
#endif      /* (MN_ARP || MN_ALLOW_BROADCAST) */

   if (socket_ptr == PTR_NULL)
      return (BAD_SOCKET_DATA);

#if MN_DHCP
   if (dhcp_lease.dhcp_state == DHCP_DEAD)
      return (DHCP_LEASE_EXPIRED);
#endif      /* MN_DHCP */

   switch (eth_type)
      {
      case ETH_IP_TYPE:             /* IP */
#if (MN_ALLOW_MULTICAST)
         if (socket_ptr->socket_type & MULTICAST_TYPE)
            {
            /* make hw address for the multicast address */
            socket_ptr->eth_dest_hw_addr[0] = 0x01;
            socket_ptr->eth_dest_hw_addr[1] = 0x00;
            socket_ptr->eth_dest_hw_addr[2] = 0x5E;
            socket_ptr->eth_dest_hw_addr[3] = (byte)(socket_ptr->ip_dest_addr[1] & 0x7F);
            socket_ptr->eth_dest_hw_addr[4] = socket_ptr->ip_dest_addr[2];
            socket_ptr->eth_dest_hw_addr[5] = socket_ptr->ip_dest_addr[3];
            }
         else
#endif      /* (MN_ALLOW_MULTICAST) */
            {
            /* Check for broadcast address */
            if (socket_ptr->ip_dest_addr[3] == 255)
               isBcast = TRUE;

#if MN_ARP
            interface_ptr = MK_INTERFACE_PTR(eth_interface_no);
            if (!isBcast)
               {
               /* do arp lookup, sending an ARP request if neccessary.
                  if the destination is not local and we are using a gateway
                  then use the gateway hardware address.
               */
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
               MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
               MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
               if ( (gateway_ip_addr[0] == 255) || (gateway_ip_addr[0] == 0) || \
                     same_net_number(socket_ptr->ip_dest_addr, \
                        interface_ptr->ip_src_addr) )
                  {
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
                  MN_MUTEX_RELEASE(MUTEX_MN_INFO);
                  MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
                  parp = mn_arp_lookup(socket_ptr->ip_dest_addr);
                  }
               else
                  {
#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
                  MN_MUTEX_RELEASE(MUTEX_MN_INFO);
                  MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
                  parp = mn_arp_lookup(gateway_ip_addr);
                  }

               if (parp != PTR_NULL)
                  {
                  /* update info */
                  for (i=0;i<ETH_ADDR_LEN;i++)
                     socket_ptr->eth_dest_hw_addr[i] = parp->eth_dest_hw_addr[i];
                  }
               else
                  return (ARP_REQUEST_FAILED);               /* error */
               }
#endif      /* MN_ARP */
            }

         if (!mn_transmit_ready(eth_interface_no))
            return (UNABLE_TO_SEND);

         sendHWAddr(socket_ptr,isBcast);
         /* send type */
         mn_send_byte(ETH_TYPE_HB);
         mn_send_byte(ETH_IP_LB);
         break;
      default:                /* unknown type */
         break;
      }

   return (1);
}

void start_arp_packet(PSOCKET_INFO socket_ptr,byte isBcast)
cmx_reentrant {
   if (socket_ptr != PTR_NULL)
      {
      sendHWAddr(socket_ptr,isBcast);
      mn_send_byte(ETH_TYPE_HB);          /* packet type */
      mn_send_byte(ETH_ARP_LB);
      mn_send_byte(ETHERNET_10MB_HB);     /* hardware type */
      mn_send_byte(ETHERNET_10MB_LB);
      mn_send_byte(ETH_TYPE_HB);          /* protocol space == IP */
      mn_send_byte(ETH_IP_LB);
      mn_send_byte(ETH_ADDR_LEN);         /* lengths */
      mn_send_byte(IP_ADDR_LEN);
      }
}

/**********************************************************************/

/* put hardware addresses in xmit buffer */
static void sendHWAddr(PSOCKET_INFO socket_ptr,byte isBcast)
cmx_reentrant {
   byte i;

   if (socket_ptr != PTR_NULL)
      {
      if (isBcast)
         {
         for (i=0;i<ETH_ADDR_LEN;i++)
            mn_send_byte(0xff);
         }
      else
         {
         /* send dest hw address */
         for (i=0;i<ETH_ADDR_LEN;i++)
            mn_send_byte(socket_ptr->eth_dest_hw_addr[i]);
         }

      MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
      /* send src hw address */
      for (i=0;i<ETH_ADDR_LEN;i++)
         mn_send_byte(eth_src_hw_addr[i]);
      MN_MUTEX_RELEASE(MUTEX_MN_INFO);
      }
}

#if MN_ARP
/* Returns TRUE if both IP addresses are on the same subnet. */
static byte same_net_number(byte *ip_addr1,byte *ip_addr2)
cmx_reentrant {
   word32 net_num, mask_num, num1, num2;
   byte retval;

   net_num = MK_WORD32(ip_addr1[0],ip_addr1[1],ip_addr1[2],ip_addr1[3]);
   mask_num = MK_WORD32(subnet_mask[0],subnet_mask[1],subnet_mask[2],\
      subnet_mask[3]);
   num1 = (net_num & mask_num);

   net_num = MK_WORD32(ip_addr2[0],ip_addr2[1],ip_addr2[2],ip_addr2[3]);
   num2 = (net_num & mask_num);

   if (num1 == num2)
      retval = TRUE;
   else
      retval = FALSE;

   return (retval);
}
#endif      /* MN_ARP */

#endif   /* MN_ETHERNET */


