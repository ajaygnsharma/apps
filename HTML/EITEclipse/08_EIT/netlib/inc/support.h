/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
**********************************************************
   Added prototype for mn_atous_long().
   4.9.2008 - M.J.Butcher                                */

#ifndef SUPPORT_H_INC
#define SUPPORT_H_INC   1

byte mn_ustoa(byte *str,word16 num) cmx_reentrant;
byte mn_uctoa(byte *str, byte num) cmx_reentrant;
#if (MN_HTTP || MN_SMTP)
word16 mn_getMyIPAddr_func(byte **, PINTERFACE_INFO) cmx_reentrant;
#endif      /* (MN_HTTP || MN_SMTP) */
word16 mn_atous(byte *str) cmx_reentrant;
unsigned long mn_atous_long(byte *str) cmx_reentrant;                    // version to retrieve long values
#if (MN_USE_LONG_FSIZE)
byte mn_ultoa(byte *str,word32 num) cmx_reentrant;
#endif      /* (MN_USE_LONG_FSIZE) */
#if (MN_SMTP || MN_FTP_CLIENT)
word16 mn_get_reply_code(byte *) cmx_reentrant;
void mn_do_multi_line(SCHAR, word16, byte *, word16, word16) cmx_reentrant;
#endif

#endif   /* #ifndef SUPPORT_H_INC */

