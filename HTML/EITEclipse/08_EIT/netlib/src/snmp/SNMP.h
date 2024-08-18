/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
********************************************************/

#ifndef SNMP_H_INC
#define SNMP_H_INC 1

#include "AgentVariable.h"       /* file of RAM variable index*/
#include "NonVolatileVariable.h" /* file of Flash variable index*/
#include "VariableLength.h"      /* file of variable lengths*/
#include "OidIndex.h"

#define     OID_BUFF_LEN         64

#define     get_request       0xa0
#define     getnext_request   0xa1
#define     get_response      0xa2
#define     set_request       0xa3
#define     trap              0xa4
#define     getbulk_request   0xa5
#define     v2_trap           0xa7

/* SNMP Definitions*/
#define     noError        0x00
#define     tooBig         0x01
#define     noSuchName     0x02
#define     badValue       0x03
#define     readOnly       0x04
#define     genErr         0x05
#define     noSuchObject   0x80
#define     noSuchInstance 0x81
#define     endOfMibView   0x82
#define     wrongType      0x07
#define     wrongLength    0x08
#define     noCreation     0x0b
#define     notWritable    0x12
#define     TOO_BIG        0x40        /* out message too large*/

#define     noResponse     0x0a

#define     Null           0x05
#define     INTEGER        0x02
#define     OCTET_STRING   0x04
#define     OBJECT_IDENTIFIER 0x06
#define     SEQUENCE       0x30
#define     IpAddress      0x40
#define     Counter        0x41
#define     Gauge          0x42
#define     TimeTicks      0x43
#define     Opaque         0x44
#define     DisplayString  0x70

#define     LONG_LENGTH_MIN   0x80     /* lengths less than 80 have are short lenths*/
#define     RAM_STORAGE    0x80     /* flag in VB Table type inticating value in RAM*/
#define     LONG_LENGTH_1  0x81     /* long length of length one*/
#define     LONG_LENGTH_2  0x82     /* long length of length two*/
#define     END_OF_TABLE   0x00     /* at oid length in VBTable for end of table*/
#define     AGENT_VARIABLE 0xff     /* type used in VBTable for agent variable*/

#define     MN_V1          0
#define     MN_V2C         1

#define     GET_NEXT       1        /* flag for get next input*/     
#define     NOT_GET_NEXT   0
#define     MN_SET         1        /* flag for set input*/
#define     NOT_SET        0
#define     SNMP_READ_WRITE   1        /* VBTable encoding of read write*/


/* Community names*/
extern byte ucWriteCommunityName[];
extern byte ucReadCommunityName[];
extern byte ucTrapCommunity[];
extern byte ucTrapCommunityLength;

/* communications and buffer variables*/
extern byte    ucSnmpIn[MN_SNMP_IN_BUFF_SIZE];     /* input from link*/
extern byte*   pucSnmpInPosition;      /* pointer to current position in current input table*/
extern byte    ucSnmpOut[MN_SNMP_OUT_BUFF_SIZE];   /* output to link*/
extern byte*   pucSnmpOutPosition;     /* pointer to current position in output table*/

/* Agent definitions*/
extern byte ucMeasureData[SNMP_DATA_LENGTH]; /*agent variables in RAM*/ 

/* global variables*/
extern int  iVBTableIndex;       /* index into FLASH variable binding table*/
extern byte ucVersionIn;         /* input version*/   
extern byte ucOID[OID_BUFF_LEN]; /* temporary for OID*/

/* function prototypes */
/* snmp*/
void mn_snmp_init(void) cmx_reentrant;
word16  mn_SNMP(word16 usNumChar) cmx_reentrant;
#if (RTOS_USED != RTOS_NONE)
void mn_snmp_reply_task(void) cmx_reentrant;
#endif      /* (RTOS_USED != RTOS_NONE) */

/* Response*/
word16  ucGetResponse(void) cmx_reentrant;

/* Utility */
word16 getLengthSnmpIn(void) cmx_reentrant;
byte  getLengthVBTable( void) cmx_reentrant;
void  MoveOidToTemp(void) cmx_reentrant;
void  MoveDataInputToOutput(void) cmx_reentrant;
byte  ucMoveOidVariableToOutput(byte ucNext) cmx_reentrant;
byte  ucCheckSetVariables(void) cmx_reentrant;
void  SetVariables(void) cmx_reentrant;
void  mn_incrementMeasVariable(word16 parameter, byte length) cmx_reentrant;
void  mn_getRamVariable(byte* ucArray,word16 parameter, byte length) cmx_reentrant;
void  mn_setRamVariable(byte* ucArray,word16 parameter, byte length) cmx_reentrant;
void  mn_getFlashVariable(byte* ucArray,word16 parameter, byte length) cmx_reentrant;
void  mn_setFlashVariable(byte* ucArray,word16 parameter, byte length) cmx_reentrant;
byte  incOut(void) cmx_reentrant;
void  setOutSeqLen(byte len_type, byte num_skip) cmx_reentrant;

/* Trap*/
#if (MN_SNMP && MN_SNMP_TRAP)
int  mn_snmp_trap(int, byte, byte, byte, byte *, SCHAR) cmx_reentrant;
#endif      /* (MN_SNMP && MN_SNMP_TRAP) */

/* hardware dependent*/
byte  getVBTableByte(void) cmx_reentrant;         
void  setVBTableByte(byte) cmx_reentrant;
byte  mn_flashRead(int) cmx_reentrant;
void  mn_flashWrite(int, byte) cmx_reentrant;

/* Ajay Sharma:- Set and Read The VBTable from Flash */
void setVBTableToFlash(void);
void getVBTableFromFlash(void);

/* snmptick */
#if ( (RTOS_USED == RTOS_NONE) && defined(_CC51))     /* Tasking 8051 */
_noregaddr void mn_snmp_tick(void) cmx_reentrant;
#else
void mn_snmp_tick(void) cmx_reentrant;
#endif
#if (RTOS_USED != RTOS_NONE)
void mn_snmp_timer_task(void) cmx_reentrant;
#endif      /* (RTOS_USED != RTOS_NONE) */

#endif   /* #ifndef SNMP_H_INC */

