/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MNSTRING_H_INC
#define MNSTRING_H_INC   1

byte * mn_strcpy_cb(byte *, PCONST_BYTE) cmx_reentrant;
byte * mn_strncpy_cb(byte *, PCONST_BYTE, word16) cmx_reentrant;
byte * mn_strcat_cb(byte *, PCONST_BYTE) cmx_reentrant;
word16 mn_strlen_cb(PCONST_BYTE) cmx_reentrant;
byte * mn_memcpy_cb(byte *, PCONST_BYTE, word16) cmx_reentrant;
int mn_stricmp_cb(byte *, PCONST_BYTE) cmx_reentrant;
int mn_strnicmp_cb(byte *, PCONST_BYTE, word16) cmx_reentrant;
int mn_stricmp(byte *, byte *) cmx_reentrant;
int mn_strnicmp(byte *, byte *, word16) cmx_reentrant;
#endif   /* #ifndef MNSTRING_H_INC */

