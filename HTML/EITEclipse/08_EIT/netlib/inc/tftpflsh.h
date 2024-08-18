/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef TFTPFLSH_H_INC
#define TFTPFLSH_H_INC   1

SCHAR mn_tftp_flash_start(byte *) cmx_reentrant;
SCHAR mn_tftp_flash_write(byte *, word16) cmx_reentrant;
void mn_tftp_flash_end(void) cmx_reentrant;

#endif   /* #ifndef TFTPFLSH_H_INC */


