/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

/* Set the following to 1 for faster, but larger code, or set to 0 for
   smaller, but slower code.
*/
#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60))
#define OPTIMIZE_FOR_SPEED    0
#else
#define OPTIMIZE_FOR_SPEED    1
#endif

/* run checksum on pseudo header and ports.
   pseudo header is:
                  0      7 8     15 16    23 24    31
                 +--------+--------+--------+--------+
                 |          source address           |
                 +--------+--------+--------+--------+
                 |        destination address        |
                 +--------+--------+--------+--------+
                 |  zero  |protocol| TCP/UDP length  |
                 +--------+--------+--------+--------+
*/

#if (!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1))
/* bww Modified to allow us to specify src ip addr. */

word32 mn_udp_tcp_start_checksum(byte proto, word16 len, word16 port1, word16 port2, \
                         byte *srcaddr, byte *dstaddr)
cmx_reentrant {
   word32 csum;

   csum = CSUM_WORD16(srcaddr[0],srcaddr[1]);
   csum += CSUM_WORD16(srcaddr[2],srcaddr[3]);
   csum += CSUM_WORD16(dstaddr[0],dstaddr[1]);
   csum += CSUM_WORD16(dstaddr[2],dstaddr[3]);
   csum = csum + proto;
   if (proto == PROTO_UDP) /* finish UDP header checksum */
      csum += len;
   csum += len;
   csum += port1;
   csum += port2;

   return (csum);
}

/* update the checksum from the data to be sent */
word32 mn_data_send_checksum(word32 csum, byte * buff_ptr, word16 data_len)
cmx_reentrant {
#if (OPTIMIZE_FOR_SPEED && !MN_DSP)
   word32 csum32;
   word16 temp_data;
   byte misaligned;
   byte odd_byte;
   word16 *data_ptr;

   if (!data_len)
      return (csum);

   csum32 = 0;
   temp_data = 0;
   misaligned = FALSE;

   if ((word32)buff_ptr & 0x01)
      {
      misaligned = TRUE;
      data_len--;
      temp_data = LSHIFT8(*((byte *)buff_ptr));
      data_ptr = (word16 *)((byte *)buff_ptr + 1);
      }
   else
      data_ptr = (word16 *)buff_ptr;

   odd_byte = (byte)(data_len & 0x01);
   data_len = data_len >> 1;               /* convert bytes to words */

   while (data_len >= 8)
      {
      data_len = data_len - 8;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      csum32 += *data_ptr++;
      }

   for (; data_len; data_len--)
      csum32 += *data_ptr++;

   while (csum32 >> 16)
      csum32 = (csum32 & 0xFFFF) + (csum32 >> 16);

   /* Swap bytes if needed. */
#if MN_LITTLE_ENDIAN
   if (!misaligned)
      csum32 = hs2net((word16)csum32);
#else
   if (misaligned)
      csum32 = (word16)((((word16)csum32)>>8) | (((word16)csum32)<<8));
#endif

   if (odd_byte)
      {
      if (misaligned)
         temp_data = temp_data + *((byte *)data_ptr);
      else
         temp_data = LSHIFT8(*((byte *)data_ptr));
      }

   if (temp_data)
      csum32 = csum32 + temp_data;

   return (csum+csum32);

#else
   word16 temp_data;
   byte odd_byte;

   if (data_len)
      {
      odd_byte = (byte)(data_len & 0x01);
      data_len = data_len >> 1;               /* convert bytes to words */
      /* we could do this more efficiently if we knew that buff_ptr
         always had the proper alignment.  This also assumes that the
         string pointed to by buff_ptr is always terminated with a NULL.

         modified 12/18/00 so string does not have to be NULL terminated.
      */
      while (data_len--)            /* sum up data */
         {
         temp_data = LSHIFT8(*buff_ptr++);
         temp_data |= (word16)(*buff_ptr++);
         csum += temp_data;
         }

      if (odd_byte)
         {
         temp_data = LSHIFT8(*buff_ptr);
         temp_data &= 0xff00;
         csum += temp_data;
         }
      }

   return (csum);
#endif
}

#if (MN_SEND_ROM_PKT)
word32 mn_rom_data_send_checksum(word32 csum, PCONST_BYTE buff_ptr, word16 data_len)
cmx_reentrant {
   word16 temp_data;
   byte odd_byte;

   if (data_len)
      {
      odd_byte = (byte)(data_len & 0x01);
      data_len = data_len >> 1;               /* convert bytes to words */
      while (data_len--)            /* sum up data */
         {
         temp_data = LSHIFT8(*buff_ptr++);
         temp_data |= (word16)(*buff_ptr++);
         csum += temp_data;
         }

      if (odd_byte)
         {
         temp_data = LSHIFT8(*buff_ptr);
         temp_data &= 0xff00;
         csum += temp_data;
         }
      }

   return (csum);
}
#endif
#endif

#if ((!(MN_USE_HW_CHKSUM && MN_ETHERNET && MN_NUM_INTERFACES == 1)) || MN_IGMP)
/* finish checksum calculation */
word16 mn_udp_tcp_end_checksum(word32 csum)
cmx_reentrant {
   while (csum >> 16)
      csum = (csum & 0xFFFF) + (csum >> 16);

   return ((word16)(~csum));
}
#endif
