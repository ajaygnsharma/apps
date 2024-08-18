/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if (MN_BOOTP && MN_ETHERNET)

/* Global mode variable.  Set to true, bootp is actively seeking broadcast
   packets. Set to false, bootp is inactive.  The bootpMode variable is used
   in other IP functions for checking checksums and filtering packets.
*/

SCHAR bootpMode;

/* Local prototypes */
static byte *find_tag_type(byte, PBOOTP_INFO) cmx_reentrant;
static void copy_subnet_mask(PBOOTP_INFO) cmx_reentrant;
static void copy_gateway(PBOOTP_INFO) cmx_reentrant;
#if (MN_DNS)
static void copy_dns_addr(PBOOTP_INFO,PINTERFACE_INFO) cmx_reentrant;
#endif      /* (MN_DNS) */

/* bootp initialization */
void mn_bootp_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_BOOTP_DHCP,INFINITE_WAIT);
   bootpMode = BOOTP_INACTIVE;
   MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);
}

/************************************************************************
* Function     : mn_bootp
* Input        : File name pointer to request file from server, use PTR_NULL
*                for default file (if bootp server has one).
*             Pointer to a bootp information structure to be filled with
*             relevant bootp data if successful.
*
* Output       : Returns 1 if successful, negative number if error, 0
*                 if no reply from remote.
*
* Description  : If src_ip_addr = 0.0.0.0 or BOOTP_REQUEST_IP is set, will
*             request bootp to get a new IP for us.
*
*             Forces broadcast to bootp server.
*
*             If successful, the BOOTP_INFO structure is returned with
*             the data replied by the server.
*
*             See RFC951 for description of all BOOTP_INFO structure
*             fields.
*
*             The function makes up to BOOTP_RESEND_TRYS requests to the
*             server.
*
* Recommendations:
*             If using bootp, set mnconfig.h file variables:
*             BOOTP, BOOTP_REQUEST_IP
*
*             These are the 'typical' settings where an IP is requested
*             and you don't know who the server is out there.  This is
*             the 'safest' way of operating bootp.
*
*             Do not modify the global variable bootpMode _EVER_.
*************************************************************************/

int mn_bootp(byte *fileName, PBOOTP_INFO bpi)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
   SCHAR i;
   SCHAR broadcast_n;
   int retval;
   int status;
   int timeout;
   word32 xid;
   word32 recv_xid;
   word16 bootp_seconds;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   /* Assumption: mn_init() has been called
      Assumption: mn_timer_init() has been called

      Process required for init:

      1. Open socket
      2. Fill bootp struct with appropriate info
      3. Send bootp struct to broadcast addr 255.255.255.255
      4. Wait for response
      5. If timeout, increase timeout period (max at 60 seconds) and
         try again.

         LOOP UNTIL RESPONSE
      6. If response, look for IP.  If no IP, error-- return PTR_NULL
         else return ARP cache.  Caller can get IP from cache.

   */

   retval = 0;
   bootp_seconds = 0;

   MN_MUTEX_WAIT(MUTEX_BOOTP_DHCP,INFINITE_WAIT);
   /* Open socket */

   if ((broadcast_n = mn_open_socket(broadcast_addr, BOOTP_CLIENT_PORT,
                  BOOTP_SERVER_PORT, PROTO_UDP, STD_TYPE,
                 (byte *)bpi, sizeof(BOOTP_INFO_T),eth_interface_no)) >= 0)
      {

      MEMSET((void *)bpi, 0, sizeof(BOOTP_INFO_T));

#if MN_BOOTP_REQUEST_IP
      /* Request IP, must set source to 0.0.0.0 */

      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      MEMSET((void *)interface_ptr->ip_src_addr, 0, IP_ADDR_LEN);
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif

      /* Ready to go.  Loop until packet with our HW address arrives, even
         if we already knew our IP before hand, the HW will always be
         copied to received packet anyway.
      */

      socket_ptr = MK_SOCKET_PTR(broadcast_n);

      /* Allow reception of broadcast packets during bootp */
      bootpMode = BOOTP_ACTIVE;

      for (i=0;i<MN_BOOTP_RESEND_TRYS;i++)
         {

         /* Fill only necessary data each time we make this request as
            we use the same buffer for read/write.
         */

         bpi->op = BOOTP_OP_REQ;
         bpi->htype = ETHERNET_10MB_LB;
         bpi->hlen = ETH_ADDR_LEN;
         bpi->flag[0] = 0x80;    /* Broadcast flag */

#if ( ( defined(POLZ180E) || defined(CMXZ180E) ) &&  defined(ZILOG) )
         /* A Zilog Z180 compiler bug prevents us from using the 5 lines
            below. BOOTP should work without it though.
         */
#else
         /* setup magic number (cookie) per RFC1542 */
         bpi->vend[0] = 99;
         bpi->vend[1] = 130;
         bpi->vend[2] = 83;
         bpi->vend[3] = 99;
         bpi->vend[4] = 255;     /* mark end of options */
#endif

         MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
         /*  Copy our hardware address. */
         (void)memcpy((void *)bpi->chaddr, (void *)eth_src_hw_addr, ETH_ADDR_LEN);
         MN_MUTEX_RELEASE(MUTEX_MN_INFO);

         /* If a local IP has been defined, bootp will merely connect to
            the bootp server to return a filename.
         */
         MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
         (void)memcpy((void *)bpi->ciaddr, (void *)interface_ptr->ip_src_addr, \
            IP_ADDR_LEN);
         MN_MUTEX_RELEASE(interface_ptr->mutex_interface);

         /* Set the request file name for TFTP */
         if (fileName != PTR_NULL)
            strcpy((char *)bpi->file, (char *)fileName);
         else
            bpi->file[0] = '\0';

         /* "xid is set to a 'random' transaction ID." [RFC951 sec 7.1] */
         xid = (word32) (( ((word32)MN_GET_TICK+1) << 16) | (MN_GET_TICK+1));

         bpi->xid[0] = WORD32_BYTE0(xid);
         bpi->xid[1] = WORD32_BYTE1(xid);
         bpi->xid[2] = WORD32_BYTE2(xid);
         bpi->xid[3] = WORD32_BYTE3(xid);

         /* Send the Data */
         socket_ptr->send_ptr = (byte *) bpi;
         socket_ptr->send_len = (sizeof(BOOTP_INFO_T));
         status = (int)mn_udp_send(socket_ptr);

         if (status > 0)
            {
            /* In a perfect world, we'd get a packet addressed to our
               hardware address.
            */
            status = (int)mn_recv(broadcast_n, (byte *)bpi, sizeof(BOOTP_INFO_T));
            }

         /* "After the average backoff reaches 60 seconds, it should be
            increased no further [timeout]..." [RFC951 sec. 7.2]
         */

         if (status <= 0)
            {
            timeout = ((bootp_seconds | 1) << 1) & 0x3f; /* About 60 seconds */

            /* Number of seconds we've tried to get bootp.  "As the number
               gets larger, certain servers may feel more sympathetic towards
               a client they don't normally service." [RFC 951 sec 7.1]
            */

            bootp_seconds = bootp_seconds + timeout;

            bpi->secs[0] = HIGHBYTE(bootp_seconds);
            bpi->secs[1] = LOWBYTE(bootp_seconds);

            MN_TASK_WAIT(bootp_seconds * (MN_ONE_SECOND));
            continue;
            }

         /* Loop until a reply packet from bootp server has our transaction
            ID and is a reply to a bootp request.
         */
         recv_xid = bpi->xid[3];
         recv_xid = recv_xid << 8 | bpi->xid[2];
         recv_xid = recv_xid << 8 | bpi->xid[1];
         recv_xid = recv_xid << 8 | bpi->xid[0];

         if ( (xid == recv_xid) && (bpi->op == BOOTP_OP_REP))
            {
#if MN_BOOTP_REQUEST_IP
            MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
            (void)memcpy(interface_ptr->ip_src_addr, (void *)bpi->yiaddr, \
               IP_ADDR_LEN);
            MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif
            /* get subnet mask and gateway from vendor options field
               if available.
            */
            MN_MUTEX_WAIT(MUTEX_MN_INFO,INFINITE_WAIT);
            copy_subnet_mask(bpi);
            copy_gateway(bpi);
            MN_MUTEX_RELEASE(MUTEX_MN_INFO);
#if (MN_DNS)
            copy_dns_addr(bpi,interface_ptr);
#endif      /* (MN_DNS) */

            retval = 1;
            break;
            }
         }     /* end of for loop */

      /* Disallow further broadcast packets. This puts the system back
         into 'normal' mode.
      */
      bootpMode = BOOTP_INACTIVE;

      /* Clean up before exit */
      (void)mn_abort(broadcast_n);
      }
   else                          /* couldn't open socket */
      retval = broadcast_n;      /* return error from mn_open */

   MN_MUTEX_RELEASE(MUTEX_BOOTP_DHCP);
   return (retval);
}

/*
   this function returns a pointer to the desired tag_type or PTR_NULL
   if the tag-type in the options field is not found.

   we are looking at
      tag-type
      tag-len
      tag_len number of data bytes
      tag-type
      tag-len
      tag_len number of data bytes
      ...
      end_byte
*/
static byte *find_tag_type(byte tag_type,PBOOTP_INFO bpi)
cmx_reentrant {
   byte *ptr;
   byte *end;
   byte len;

   /* point to start & end of options. first 4 bytes of the vend field
      should be the magic number (cookie).
   */
   ptr = &bpi->vend[COOKIE_LEN];
   end = ptr + BOOTP_VENDOR_LEN - 1;

   while (1)
      {
      if (*ptr == tag_type)
         {
         /* found what we were looking for */
         break;
         }
      else if ((*ptr == END_BYTE) || (ptr > end))
         {
         /* reached the end without finding what we were looking for */
         ptr = BYTE_PTR_NULL;
         break;
         }
      else if (*ptr == PAD_BYTE)
         {
         /* skip to next tag-type */
         do
            {
            ptr++;
            }
         while (*ptr == PAD_BYTE);
         }
      else
         {
         ptr++;                     /* point to length byte */
         len = (byte)(*ptr + 1);    /* skip to next tag_type */
         ptr = ptr + len;
         }
      }

   return (ptr);
}

static void copy_subnet_mask(PBOOTP_INFO bpi)
cmx_reentrant {
  char *ptr;

   /* get pointer to subnet mask tag */
   ptr = (char *)find_tag_type(SUBNET_MASK_CODE, bpi);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data */
      (void)memcpy((void *)subnet_mask, (void *)ptr, IP_ADDR_LEN);
      }
}

static void copy_gateway(PBOOTP_INFO bpi)
cmx_reentrant {
  char *ptr;

   /* get pointer to router (gateways) tag */
   ptr = (char *)find_tag_type(ROUTER_CODE,bpi);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data, use first ip address only */
      (void)memcpy((void *)gateway_ip_addr, (void *)ptr, IP_ADDR_LEN);
      }
}

#if (MN_DNS)
static void copy_dns_addr(PBOOTP_INFO bpi, PINTERFACE_INFO interface_ptr)
cmx_reentrant {
  char *ptr;

   /* get pointer to DNS server tag */
   ptr = (char *)find_tag_type(DNS_SERVER_CODE,bpi);
   if (ptr != PTR_NULL)
      {
      ptr = ptr + 2;      /* skip to data, use first ip address only */
      MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
      (void)memcpy((void *)interface_ptr->ip_dns_addr, (void *)ptr, IP_ADDR_LEN);
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      }
}
#endif      /* (MN_DNS) */

#endif      /* MN_BOOTP && MN_ETHERNET */


