/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
**********************************************************
   Added routine to convert an ASCII string to an unsigned
   long. This is needed when receiving posted files of 
   containing more that 64k bytes.
   4.9.2008 - M.J.Butcher                                */

#include "micronet.h"

/* convert an unsigned short to ascii and copy to a string
   returns number of bytes added to string
*/
byte mn_ustoa(byte *str,word16 num)
cmx_reentrant {

   byte digit,suppress;
   word16 divisor;
   byte num_bytes;
   byte * temp_str;

   num_bytes = 0;
   temp_str = str;

    divisor = 10000;
    suppress = 1;
    while( !( divisor == 1 )){
      digit = (byte)(num / divisor);
      num = num - (digit * divisor);
      divisor = divisor / 10;
      if( suppress  &&  digit )
         suppress = 0;
      if( !suppress )
         {
         digit |= '0';
         *temp_str++ = digit;
         num_bytes++;
         }
      }
   *temp_str++ = (byte)(num+'0');
   *temp_str = '\0';

   return (++num_bytes);
}

/* convert an unsigned char to ascii and copy to a string
   returns number of bytes added to string
*/
byte mn_uctoa(byte *str, byte num)
cmx_reentrant {

   byte digit,suppress;
   byte divisor;
   byte num_bytes;
   byte * temp_str;

   num_bytes = 0;
   temp_str = str;

    divisor = 100;
    suppress = 1;
    while( !( divisor == 1 )){
      digit = (byte)(num / divisor);
      num = (byte)(num - (digit * divisor));
      divisor = (byte)(divisor / 10);
      if( suppress  &&  digit )
         suppress = 0;
      if( !suppress )
         {
         digit |= '0';
         *temp_str++ = digit;
         num_bytes++;
         }
      }
   *temp_str++ = (byte)(num+'0');
   *temp_str = '\0';

   return (++num_bytes);
}

#if (MN_USE_LONG_FSIZE)
byte mn_ultoa(byte *str,word32 num)
cmx_reentrant {

   word16 digit,suppress;
   word32 divisor;
   byte num_bytes;
   byte * temp_str;

   num_bytes = 0;
   temp_str = str;

    divisor = 1000000000UL;
    suppress = 1;
    while( !( divisor == 1 )){
      digit = (word16)(num / divisor);
      num = num - (digit * divisor);
      divisor = divisor / 10;
      if( suppress  &&  digit )
         suppress = 0;
      if( !suppress )
         {
         digit |= '0';
         *temp_str++ = digit;
         num_bytes++;
         }
      }
   *temp_str++ = (byte)(num+'0');
   *temp_str = '\0';

   return (++num_bytes);
}
#endif      /* (MN_USE_LONG_FSIZE) */

#if (MN_HTTP || MN_SMTP)
/* put current IP Address in str and return number of bytes added to str */
word16 mn_getMyIPAddr_func(byte **str, PINTERFACE_INFO interface_ptr)
cmx_reentrant {
   byte bytes_added;
   word16 num_bytes;
   byte *temp_ptr;
   static byte temp_str[16];

   num_bytes = 0;
   temp_str[0] = '\0';
   temp_ptr = temp_str;

#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */
   bytes_added = mn_uctoa(temp_ptr, interface_ptr->ip_src_addr[0]);
   temp_ptr = temp_ptr + bytes_added;
   num_bytes = num_bytes + bytes_added;
   *temp_ptr++ = '.';
   num_bytes++;
   bytes_added = mn_uctoa(temp_ptr, interface_ptr->ip_src_addr[1]);
   temp_ptr = temp_ptr + bytes_added;
   num_bytes = num_bytes + bytes_added;
   *temp_ptr++ = '.';
   num_bytes++;
   bytes_added = mn_uctoa(temp_ptr, interface_ptr->ip_src_addr[2]);
   temp_ptr = temp_ptr + bytes_added;
   num_bytes = num_bytes + bytes_added;
   *temp_ptr++ = '.';
   num_bytes++;
   bytes_added = mn_uctoa(temp_ptr, interface_ptr->ip_src_addr[3]);
   num_bytes = num_bytes + bytes_added;
#if (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING)
   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_PPP || MN_DHCP || MN_BOOTP || MN_PING_GLEANING) */

   *str = temp_str;

   return (num_bytes);
}
#endif      /* (MN_HTTP || MN_SMTP) */

/* convert an ascii string to an unsigned short. assumes number is base 10. */
word16 mn_atous(byte *str)
cmx_reentrant {
   word16 retval;
   byte in_num;

   retval = 0;
   in_num = FALSE;

   /* skip everything until we hit the first number or end of string */
   while (*str)
      {
      if ((*str >= '0') && (*str <= '9'))
         {
         in_num = TRUE;
         retval = (retval * 10) + (*str - '0');
         }
      else if (in_num)     /* hit a non-number */
         break;

      str++;
      }

   return (retval);
}

#ifdef _POST_FILES
/* convert an ascii string to an unsigned long. assumes number is base 10. */
extern unsigned long mn_atous_long(byte *str)
cmx_reentrant {
   unsigned long retval;
   byte in_num;

   retval = 0;
   in_num = FALSE;

   /* skip everything until we hit the first number or end of string */
   while (*str)
      {
      if ((*str >= '0') && (*str <= '9'))
         {
         in_num = TRUE;
         retval = (retval * 10) + (*str - '0');
         }
      else if (in_num)     /* hit a non-number */
         break;

      str++;		
      }

   return (retval);
}
#endif

#if (MN_SMTP || MN_FTP_CLIENT)
/* The next two functions assume that the reply codes are three bytes long.
   These functions are used by the SMTP and FTP client modules and are
   not intended to be called by the user.
*/
word16 mn_get_reply_code(byte *buff_ptr)
cmx_reentrant {
   int i;
   byte reply_buff[4];

   for (i=0; i<3; i++)
      reply_buff[i] = *(buff_ptr+i);
   reply_buff[3] = '\0';

   return (mn_atous(reply_buff));
}

static cmx_const byte MULTI_LINE_CHAR  = '-';

/* Handle multi-line reply. Potentially this could mean reading in more
   packets.
*/
void mn_do_multi_line(SCHAR socket_no, word16 reply_code, byte * buff_ptr, \
   word16 buff_len, word16 wait_ticks)
cmx_reentrant {
   long recvd;
   int j, num_chars;
   word16 check_code, i;
   PSOCKET_INFO socket_ptr;
   byte save_buff[4];
   byte last_line;

   if (socket_no < 0)
      return;

   socket_ptr = MK_SOCKET_PTR(socket_no);

   if (buff_ptr[3] == MULTI_LINE_CHAR)
      {
      last_line = FALSE;
      while (!last_line)
         {
         i = 0;
         while (i < socket_ptr->recv_len)
            {
            /* Skip to end of line. */
            while ( (buff_ptr[i] != '\n') && (i < socket_ptr->recv_len) )
               {
               i++;
               }
            if (last_line && (buff_ptr[i] == '\n'))
               break;
            else
               i++;     /* skip LF */

            if (i >= socket_ptr->recv_len)
               {
               /* We reached the end of the packet without finding the
                  last line.
               */
               recvd = mn_recv_wait(socket_no,buff_ptr,buff_len,wait_ticks);
               if (recvd <= 0)
                  {
                  last_line = TRUE;
                  break;
                  }
               else
                  i = 0;
               }
            else
               {
               /* Get number of characters left in the buffer */
               num_chars = socket_ptr->recv_len - i;
               if (num_chars >= 4)
                  {
                  /* We have enough chars to check for last line. */
                  check_code = mn_get_reply_code(&buff_ptr[i]);
                  if ( (check_code == reply_code) && \
                        (buff_ptr[i+3] == ' '))
                     {
                     last_line = TRUE;
                     }
                  }
               else
                  {
                  /* Not enough chars to check for last line. */
                  for (j=0;j<num_chars;j++)
                     save_buff[j] = buff_ptr[i+j];

                  recvd = mn_recv_wait(socket_no, \
                     buff_ptr, buff_len, wait_ticks);
                  if (recvd <= 0)
                     {
                     last_line = TRUE;
                     break;
                     }
                  else
                     i = 0;

                  for (j=num_chars; j<4; j++)
                     save_buff[j] = buff_ptr[i++];

                  check_code = mn_get_reply_code(save_buff);
                  if ( (check_code == reply_code) && \
                        (save_buff[i+3] == ' ') )
                     {
                     last_line = TRUE;
                     }
                  }
               }
            }
         }
      }
   else
      {
      while (1)
         {
         i = 0;
         /* Skip to end of line. */
         while ( (buff_ptr[i] != '\n') && (i < socket_ptr->recv_len) )
            i++;

         if (buff_ptr[i] == '\n')
            break;

         /* We reached the end of the packet without finding the end of
            the line.
         */
         recvd = mn_recv_wait(socket_no, buff_ptr, buff_len, wait_ticks);
         if (recvd <= 0)
            break;
         }
      }
}
#endif
