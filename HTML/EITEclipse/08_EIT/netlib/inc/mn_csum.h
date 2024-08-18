/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_CSUM_H_INC
#define MN_CSUM_H_INC  1

word32 mn_udp_tcp_start_checksum(byte, word16, word16, word16, byte *, byte *) cmx_reentrant;
word32 mn_data_send_checksum(word32,byte *,word16) cmx_reentrant;
#if (MN_SEND_ROM_PKT)
word32 mn_rom_data_send_checksum(word32,PCONST_BYTE,word16) cmx_reentrant;
#endif
word16 mn_udp_tcp_end_checksum(word32) cmx_reentrant;

#endif   /* #ifndef MN_CSUM_H_INC */

