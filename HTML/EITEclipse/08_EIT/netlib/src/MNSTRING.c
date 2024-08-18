/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

static byte mn_tolower(byte c) cmx_reentrant;

/* string functions where source is CONST_BYTE, destination is RAM */

byte * mn_strcpy_cb(byte *dest, PCONST_BYTE src)
cmx_reentrant {
   byte *ptr;

   ptr = dest;

   while ( (*dest++ = *src++) );

   return (ptr);
}

byte * mn_strncpy_cb(byte *dest, PCONST_BYTE src, word16 len)
cmx_reentrant {
   byte *ptr;

   ptr = dest;

   for ( ; len > 0; len--)
      {
      if (!(*dest++ = *src++))
         break;
      }

   for ( ; len > 0; len--)
      *dest++ = '\0';

   return (ptr);
}

byte * mn_strcat_cb(byte *dest, PCONST_BYTE src)
cmx_reentrant {
   byte *ptr;

   ptr = dest;

   while (*dest)
      dest++;

   while ( (*dest++ = *src++) );

   return (ptr);
}

word16 mn_strlen_cb(PCONST_BYTE src)
cmx_reentrant {
   PCONST_BYTE ptr;

   ptr = src;

   while (*src)
      src++;

   return ((word16)(src - ptr));
}

byte * mn_memcpy_cb(byte *dest, PCONST_BYTE src, word16 len)
cmx_reentrant {
   byte *ptr;

   ptr = dest;

   for ( ; len > 0; len--)
      *dest++ = *src++;

   return (ptr);
}

/* case-insensitive string compare. returns 0 if strings are equal,
   non-zero if strings are not equal.
*/
int mn_stricmp_cb(byte *s1, PCONST_BYTE s2)
cmx_reentrant {
   byte b1, b2;

   do
      {
      b1 = mn_tolower(*s1);
      s1++;
      b2 = mn_tolower(*s2);
      s2++;
      }
   while ((b1 != '\0') && (b1 == b2));

   return (b1 - b2);
}

int mn_strnicmp_cb(byte *s1, PCONST_BYTE s2, word16 len)
cmx_reentrant {
   byte b1, b2;

   b1 = b2 = 0;

   for ( ; len > 0; len--)
      {
      b1 = mn_tolower(*s1);
      s1++;
      b2 = mn_tolower(*s2);
      s2++;
      if ((b1 == '\0') || (b1 != b2))
         break;
      }

   return (b1 - b2);
}

/* ---------------------------------------------------------------------- */
/* other string functions */

/* case-insensitive string compare. returns 0 if strings are equal,
   non-zero if strings are not equal.
*/
int mn_stricmp(byte *s1, byte *s2)
cmx_reentrant {
   byte b1, b2;

   do
      {
      b1 = mn_tolower(*s1);
      s1++;
      b2 = mn_tolower(*s2);
      s2++;
      }
   while ((b1 != '\0') && (b1 == b2));

   return (b1 - b2);
}

/* Case-insensitive string compare for a specified length.
   Returns 0 if strings are equal, non-zero if strings are not equal.
*/
int mn_strnicmp(byte *s1, byte *s2, word16 len)
cmx_reentrant {
   byte b1, b2;

   b1 = b2 = 0;

   for ( ; len > 0; len--)
      {
      b1 = mn_tolower(*s1);
      s1++;
      b2 = mn_tolower(*s2);
      s2++;
      if ((b1 == '\0') || (b1 != b2))
         break;
      }

   return (b1 - b2);
}

static byte mn_tolower(byte c)
cmx_reentrant {
/*   return ( (c >= 'A' && c <= 'Z') ? (c | 0x20) : c); */

   if ((c >= 'A') && (c <= 'Z'))
      (c |= 0x20);

   return c;
}
