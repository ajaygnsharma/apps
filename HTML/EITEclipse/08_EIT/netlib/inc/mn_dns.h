/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_DNS_H_INC
#define MN_DNS_H_INC 1

#define MIN_DNS_PACKET_LEN 16
#define DNS_ID_LB          0xAB

#define MAX_LABEL_LEN   63
#define MAX_NAME_LEN    255
#define HOST_ADDR_TYPE  1
#define INTERNET_CLASS  1
#define ANY_CLASS       255

#define DNS_REPSONSE    0x80
#define DNS_OPCODE_MASK 0x7A
#define DNS_RCODE_MASK  0x7F
#define DNS_COMPRESSION 0xC0

#define DNS_NO_ERROR          0
#define DNS_FORMAT_ERROR      1
#define DNS_SERVER_FAILURE    2
#define DNS_NAME_ERROR        3
#define DNS_NOT_IMPLEMENTED   4
#define DNS_REFUSED           5

/* Check if the character is a letter, digit or hyphen */
#define isLDHchar(c) \
(((((c)&0xdf) >= 'A') && (((c)&0xdf) <= 'Z')) || (((c) >= '0') && ((c) <= '9')) || ((c) == '-'))

int mn_dns_get_addr(char *, SCHAR, byte *) cmx_reentrant;
SCHAR mn_dns_start(SCHAR) cmx_reentrant;
int mn_dns_send(char *, SCHAR) cmx_reentrant;
int mn_dns_get_reply(SCHAR, byte *, word16) cmx_reentrant;
void mn_dns_end(SCHAR) cmx_reentrant;

#endif   /* #ifndef MN_DNS_H_INC */

