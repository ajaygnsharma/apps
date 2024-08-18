/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if (MN_DNS)
#if (defined(MCHP_C18))
#pragma udata mn_dns2
#endif
byte dns_recv_buff[MN_DNS_RECV_BUFF_SIZE];
#if (defined(MCHP_C18))
#pragma udata
#endif
byte dns_xmit_buff[MN_DNS_XMIT_BUFF_SIZE];

/* Sends a DNS Query and waits for the reply. The IP address is copied
   into addr_ptr. Returns positive number if successful, negative number
   on error.
*/
int mn_dns_get_addr(char * name, SCHAR interface_no, byte * addr_ptr)
cmx_reentrant {
   byte i;
   SCHAR socket_no;
   int retval;
   int sent;

   retval = INVALID_VALUE;

   socket_no = mn_dns_start(interface_no);
   if (socket_no < 0)
      return (socket_no);                    /* mn_open failed. */

   for (i=0; i<MN_DNS_SEND_TRYS; i++)
      {
      sent = mn_dns_send(name, socket_no);
      if (sent > 0)
         {
         retval = mn_dns_get_reply(socket_no, addr_ptr, MN_DNS_WAIT_TICKS);
         if (retval > 0)
            break;
         }
      else
         retval = sent;
      }

   mn_dns_end(socket_no);

   return (retval);
}

/* Open a socket for DNS. Returns result of mn_open or negative
   number on error.
*/
SCHAR mn_dns_start(SCHAR interface_no)
cmx_reentrant {
   SCHAR socket_no;
   PINTERFACE_INFO interface_ptr;

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(interface_no);
   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);

   /* Check if we have a DNS IP address for the interface. */
   if ((interface_ptr->ip_dns_addr[0] == 0) || \
         (interface_ptr->ip_dns_addr[0] == 255))
      {
      MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
      return(INVALID_VALUE);
      }

   socket_no = mn_open(interface_ptr->ip_dns_addr, (DEFAULT_PORT+DNS_PORT), \
      DNS_PORT, NO_OPEN, PROTO_UDP, STD_TYPE, dns_recv_buff, \
      MN_DNS_RECV_BUFF_SIZE, interface_no);

   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
   return (socket_no);
}

/* Send a DNS Query on the passed socket. Name must be null terminated.
   Returns positive number if successful, negative number on error.
*/
int mn_dns_send(char * name, SCHAR socket_no)
cmx_reentrant {
   byte *xmit_ptr;
   byte *len_ptr;
   int len, retval;
   char c;
   PSOCKET_INFO socket_ptr;

   /* Check name length */
   len = strlen(name);
   if ((len <= 0) || (len > MAX_NAME_LEN))
      return(INVALID_VALUE);

   if ((socket_no < 0) || (socket_no >= MN_NUM_SOCKETS))
      return (BAD_SOCKET_DATA);

   if (!SOCKET_ACTIVE(socket_no))
      return (SOCKET_NOT_FOUND);

   socket_ptr = MK_SOCKET_PTR(socket_no);

   MN_MUTEX_WAIT(MUTEX_DNS,INFINITE_WAIT);
   xmit_ptr = dns_xmit_buff;

   /* ID */
   *xmit_ptr++ = socket_ptr->interface_no;
   *xmit_ptr++ = DNS_ID_LB;

   /* opcode, recursion, rcode */
   *xmit_ptr++ = 1;
   *xmit_ptr++ = 0;

   /* QDCOUNT */
   *xmit_ptr++ = 0;
   *xmit_ptr++ = 1;

   /* other counts */
   *xmit_ptr++ = 0;
   *xmit_ptr++ = 0;
   *xmit_ptr++ = 0;
   *xmit_ptr++ = 0;
   *xmit_ptr++ = 0;
   *xmit_ptr++ = 0;

   /* QNAME */
   /* separate name into labels preceeded by the label length.
      e.g. cmx.com becomes 3cmx3com, the name ends in a null byte.
   */
   len_ptr = xmit_ptr;        /* save location in buffer to write length */
   xmit_ptr++;
   len = 0;
   c = *name;
   while (c)
      {
      if (isLDHchar(c))
         {
         /* valid label character to write it and increment length */
         *xmit_ptr++ = c;
         len++;
         if (len > MAX_LABEL_LEN)
            return(INVALID_VALUE);
         }
      else
         {
         /* end of label so write length and reset */
         *len_ptr = (byte)len;
         len_ptr = xmit_ptr;
         xmit_ptr++;
         len = 0;
         }
      name++;
      c = *name;
      }

   /* write the length for the last label */
   if (len > 0)
      *len_ptr = (byte)len;

   *xmit_ptr++ = '\0';     /* write the null byte to end the name */

   /* QTYPE */
   *xmit_ptr++ = 0;
   *xmit_ptr++ = HOST_ADDR_TYPE;

   /* QCLASS */
   *xmit_ptr++ = 0;
   *xmit_ptr++ = INTERNET_CLASS;

   /* send the packet */
   retval = (int)mn_send(socket_no, dns_xmit_buff, (xmit_ptr - dns_xmit_buff));

   MN_MUTEX_RELEASE(MUTEX_DNS);
   return (retval);
}

/* Waits for reply to DNS Query on passed socket.
   Returns positive number if successful, negative number on error.
*/
int mn_dns_get_reply(SCHAR socket_no, byte * addr_ptr, word16 wait_ticks)
cmx_reentrant {
   byte *recv_ptr;
   byte temp_code;
   word16 qdcount, ancount, temp_val, rdlength;
   int retval;
   PSOCKET_INFO socket_ptr;

   if (addr_ptr == PTR_NULL)
      return(INVALID_VALUE);

   MN_MUTEX_WAIT(MUTEX_DNS,INFINITE_WAIT);
   retval = (int)mn_recv_wait(socket_no, dns_recv_buff, MN_DNS_RECV_BUFF_SIZE, \
      wait_ticks);

   if (retval > MIN_DNS_PACKET_LEN)
      {
      recv_ptr = dns_recv_buff;
      socket_ptr = MK_SOCKET_PTR(socket_no);

      /* Check if ID bytes match what we sent. */
      if (*recv_ptr++ != (byte)socket_ptr->interface_no)
         {
         MN_MUTEX_RELEASE(MUTEX_DNS);
         return (DNS_ID_ERROR);
         }

      if (*recv_ptr++ != DNS_ID_LB)
         {
         MN_MUTEX_RELEASE(MUTEX_DNS);
         return (DNS_ID_ERROR);
         }

      temp_code = *recv_ptr++;
      /* Make sure it is a response to a standard query and not truncated. */
      if (!(temp_code & DNS_REPSONSE) || (temp_code & DNS_OPCODE_MASK))
         {
         MN_MUTEX_RELEASE(MUTEX_DNS);
         return (DNS_OPCODE_ERROR);
         }

      temp_code = *recv_ptr++;
      /* See if an error code was returned. */
      if (temp_code & DNS_RCODE_MASK)
         {
         MN_MUTEX_RELEASE(MUTEX_DNS);
         return (DNS_RCODE_ERROR);
         }

      /* Get question and answer counts. */
      qdcount = LSHIFT8(*recv_ptr++);
      qdcount += (word16)(*recv_ptr++);
      ancount = LSHIFT8(*recv_ptr++);
      ancount += (word16)(*recv_ptr++);

      /* Ensure the counts make sense. */
      if ((qdcount > 1) || (ancount == 0))
         {
         MN_MUTEX_RELEASE(MUTEX_DNS);
         return (DNS_COUNT_ERROR);
         }

      /* Skip nscount and arcount. */
      recv_ptr += 4;

      if (qdcount)
         {
         /* Skip past the question section. */
         while (*recv_ptr++);
         recv_ptr += 4;
         }

      /* Process the answers. */
      rdlength = 0;

      /* Loop through answers until we get one with the correct class and
         type and has an IP address.
      */
      while (ancount && (rdlength != IP_ADDR_LEN))
         {
         ancount--;
         retval = DNS_NOT_FOUND;

         if ((*recv_ptr & DNS_COMPRESSION) == DNS_COMPRESSION)
            recv_ptr += 2;          /* Skip compression bytes. */
         else
            while (*recv_ptr++);    /* Skip the name. */

         /* Check type. */
         temp_val = LSHIFT8(*recv_ptr++);
         temp_val += (word16)(*recv_ptr++);
         if (temp_val != HOST_ADDR_TYPE)
            retval = DNS_TYPE_ERROR;

         /* Check class. */
         temp_val = LSHIFT8(*recv_ptr++);
         temp_val += (word16)(*recv_ptr++);
         if (temp_val != INTERNET_CLASS)
            retval = DNS_CLASS_ERROR;

         /* Skip TTL. */
         recv_ptr += 4;

         /* Get length of RDATA. */
         rdlength = LSHIFT8(*recv_ptr++);
         rdlength += (word16)(*recv_ptr++);
         if ((rdlength == IP_ADDR_LEN) && (retval == DNS_NOT_FOUND))
            {
            /* Copy the IP address. */
            *addr_ptr++ = *recv_ptr++;
            *addr_ptr++ = *recv_ptr++;
            *addr_ptr++ = *recv_ptr++;
            *addr_ptr = *recv_ptr++;
            retval = 1;
            }
         else
            {
            recv_ptr += rdlength;
            rdlength = 0;
            }
         }
      }
   else if (retval >= 0)
      retval = INVALID_VALUE;

   MN_MUTEX_RELEASE(MUTEX_DNS);
   return (retval);
}

void mn_dns_end(SCHAR socket_no)
cmx_reentrant {
   (void)mn_abort(socket_no);
}
#endif      /* (MN_DNS) */
