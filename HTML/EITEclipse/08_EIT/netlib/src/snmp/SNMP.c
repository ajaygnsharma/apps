/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
#include "micronet.h"

#if MN_SNMP

/* Community Names*/
byte  ucWriteCommunityName[]= {"private"};
byte  ucReadCommunityName[]= {"public"};
byte  ucTrapCommunity[] = {"public"};
byte  ucTrapCommunityLength = 6;

/* communications and buffer variables*/
#if (defined(MCHP_C18))
#pragma udata mn_snmp2
#endif
byte  ucSnmpIn[MN_SNMP_IN_BUFF_SIZE];    /* input from link*/
#if (defined(MCHP_C18))
#pragma udata
#endif
byte* pucSnmpInPosition;      /* pointer to current position in current input table*/
#if (defined(MCHP_C18))
#pragma udata mn_snmp3
#endif
byte  ucSnmpOut[MN_SNMP_OUT_BUFF_SIZE];     /* output to link*/
#if (defined(MCHP_C18))
#pragma udata
#endif
byte* pucSnmpOutPosition;     /* pointer to current position in output table*/

/* Agent definitions*/
byte ucMeasureData[SNMP_DATA_LENGTH];

/* global variables*/
int   iVBTableIndex;       /* index into FLASH variable binding table*/
byte  ucVersionIn;         /* input version*/
byte  ucOID[OID_BUFF_LEN]; /* temporary for OID*/

static byte ucCheckMessageLength(word16 usNumChar) cmx_reentrant;
static byte ucCheckVersion(void) cmx_reentrant;
static byte ucCheckCommunity(void) cmx_reentrant;

/* Must be called before mn_timer_init(). */
void mn_snmp_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   MEMSET(ucMeasureData, 0, sizeof(ucMeasureData));
   MN_MUTEX_RELEASE(MUTEX_SNMP);
}

/*
 Detemines the response for SNMP mesage
 return number of characters  if message to be transmitted
*/
word16 mn_SNMP(word16 usNumChar)
cmx_reentrant {
   word16 usResponseNumChar;
   byte   ucError;
   int    i;

   /* increment number of packets received*/
#ifdef snmpInPkts0
   mn_incrementMeasVariable(snmpInPkts0,4);     /* length of variable 4*/
#endif
   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);

   /* clear output buffer*/
   for(i=0;i<MN_SNMP_OUT_BUFF_SIZE;i++)
      ucSnmpOut[i] = 0;

   /* set pointers to input and output buffers*/
   pucSnmpInPosition = ucSnmpIn;
   pucSnmpOutPosition = ucSnmpOut;

   /* check ASN syntax of header to SNMP type*/
   ucError = ucCheckMessageLength(usNumChar);
   MN_MUTEX_RELEASE(MUTEX_SNMP);
   if ( ucError != noError)
   {
#ifdef snmpSilentDrops0
      mn_incrementMeasVariable(snmpSilentDrops0,4);      /* length of variable 4*/
#endif
      return(0);
   }
   if (ucCheckVersion() != noError)
   {
#ifdef snmpInBadVersions0
      mn_incrementMeasVariable(snmpInBadVersions0,4);    /* length of variable 4*/
#endif
#ifdef snmpSilentDrops0
      mn_incrementMeasVariable(snmpSilentDrops0,4);      /* length of variable 4*/
#endif
      return(0);
   }

   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   ucError = ucCheckCommunity();
   MN_MUTEX_RELEASE(MUTEX_SNMP);
   if (ucError != noError)
   {
#ifdef snmpInBadCommunityNames0
      mn_incrementMeasVariable(snmpInBadCommunityNames0,4);    /* length of variable 4*/
#endif
#ifdef snmpSilentDrops0
      mn_incrementMeasVariable(snmpSilentDrops0,4);      /* length of variable 4*/
#endif
      return(0);
   }

   /* all others get a response data or error*/
   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   usResponseNumChar = ucGetResponse();

   /* clear input*/
   for(i=0; i < MN_SNMP_IN_BUFF_SIZE; i++)
      ucSnmpIn[i] = 0;

   MN_MUTEX_RELEASE(MUTEX_SNMP);
   if(usResponseNumChar > 10)    /* less than 10 is not a valid packet*/
      {
#ifdef snmpOutPkts0
      mn_incrementMeasVariable(snmpOutPkts0,4);    /* length of variable 4*/
#endif
      return (usResponseNumChar);
      }
   else
      {
#ifdef snmpSilentDrops0
      mn_incrementMeasVariable(snmpSilentDrops0,4);      /* length of variable 4*/
#endif
      return(0);
      }
}

/*
 Check for total message length correct
 starts response with [30][81][placeholder for response length(ucSnmpOut+2)]
 getLengthSnmpIn is adjusted to next token (version)
*/
static byte ucCheckMessageLength(word16 usNumChar)
cmx_reentrant {
   unsigned short usLength;
   unsigned short usTemp;

   /* message longer than maximum is error */
   if(MN_SNMP_IN_BUFF_SIZE < usNumChar)
      {
#ifdef snmpInTooBigs0
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      mn_incrementMeasVariable(snmpInTooBigs0,4);     /* length of variable 4*/
      MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
      return(noResponse);
      }
   /* message is sequence check for error*/
   if (*pucSnmpInPosition != SEQUENCE)
      return (noResponse);
   /* message length*/
   pucSnmpInPosition++;
   if((*pucSnmpInPosition) < LONG_LENGTH_MIN)   /* single byte length*/
   {
      usTemp = 2;       /*length of sequence and length*/
   }
   else
   {
      usTemp = (*pucSnmpInPosition)&((byte)0x0f);
      usTemp += 2;
   }
   usLength = getLengthSnmpIn();
   /* test for length of message correct*/
   if(usLength != (usNumChar - usTemp))
      return (noResponse);
   /* set sequence and prepare slot for  two byte return length*/
   setOutSeqLen(LONG_LENGTH_2, 3);
   return (noError);
}

/*
 Check for version 1 or version 1 and 2c
 adds version to output [02][01][00] or [02][01][01]
 pucSnmpInPosition is adjusted to next token (community string)
*/
static byte ucCheckVersion(void)
cmx_reentrant {
   ucVersionIn = *(pucSnmpInPosition+2);
#if MN_SNMP_VERSION2C
   /* Version 1 and 2c supported*/
   if( (*pucSnmpInPosition == (byte)INTEGER) &&
      (*(pucSnmpInPosition+1) == (byte)0x01)&&  /*length 1*/
      ((*(pucSnmpInPosition+2) == (byte)MN_V1)
       ||(*(pucSnmpInPosition+2) == (byte)MN_V2C))
      )
#else
   /* Version 1 only supported*/
   if( (*pucSnmpInPosition == (byte)INTEGER) &&
      (*(pucSnmpInPosition+1) == (byte)0x01)&&  /*length 1*/
      (*(pucSnmpInPosition+2) == (byte)MN_V1)
      )
#endif
   {
      /* Version in response or no response*/
      *pucSnmpOutPosition = (byte)INTEGER;
      pucSnmpOutPosition ++;
      *pucSnmpOutPosition = (byte)0x01;         /*length*/
      pucSnmpOutPosition ++;
      *pucSnmpOutPosition = ucVersionIn;              /*version*/
      pucSnmpOutPosition ++;
      pucSnmpInPosition += 3;       /* set input position to community string*/
      return(noError);
   }
   else
   {
      return (noResponse);
   }
}

/*
 Check for community
 pucSnmpInPosition is adjusted to next token (SNMP PDU)
*/
static byte ucCheckCommunity(void)
cmx_reentrant {
   word16 usLength;
   byte   ucType;
   word16 ii;
   byte* pucCommunityName;
   byte* pucCommunityName1;

   /* tag is octet string*/
   if (*pucSnmpInPosition != OCTET_STRING)
      return (noResponse);
   pucSnmpInPosition ++;
   /* find length of community name*/
   usLength = getLengthSnmpIn();
   /* check for snmp set or other */
   ucType = *(pucSnmpInPosition + usLength);
   pucCommunityName = ucWriteCommunityName;        /* names are assigned in header*/
   if(ucType == set_request)
   {
      pucCommunityName1 = PTR_NULL;
      if (usLength != strlen((char *)ucWriteCommunityName))
         return(noResponse);
   }
   else
   {
      pucCommunityName1 = ucReadCommunityName;
      if ((usLength != strlen((char *)ucWriteCommunityName)) && \
            (usLength != strlen((char *)ucReadCommunityName)))
         return(noResponse);
   }

   /* check community name */
   for ( ii = 0; ii < usLength ; ii++)
   {
      if ((*pucSnmpInPosition != *pucCommunityName ) && \
         ((pucCommunityName1 == PTR_NULL) || (*pucSnmpInPosition != *pucCommunityName1)))
      {
         return(noResponse);
      }
      pucSnmpInPosition++;
      pucCommunityName++;
      if (pucCommunityName1 != PTR_NULL)
         pucCommunityName1++;
   }

/* move community string from input to output*/
   pucSnmpInPosition -= (usLength + 2);      /* back pointer up to start of community string*/
   for ( ii = 0; ii < (usLength+2) ; ii++)
   {
      *pucSnmpOutPosition = *pucSnmpInPosition;
      pucSnmpInPosition++;
      pucSnmpOutPosition++;
   }

   return (noError);
}

#if (RTOS_USED != RTOS_NONE)
void mn_snmp_reply_task(void)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
   SCHAR socket_no;
   word16 usNumChar;
   SCHAR interface_no;

   wait_for_mn_init();

   socket_no = -1;

   while (1)
      {
#if (MN_NUM_INTERFACES == 1)
      interface_no = 0;
#else
      interface_no = IF_ANY;
#endif
      /* Make sure we always have a socket listening on port 161. */
      if (mn_find_socket(SNMP_PORT, 0, null_addr, PROTO_UDP, \
            interface_no) == PTR_NULL)
         {
         socket_no = mn_open(null_addr, SNMP_PORT, 0, NO_OPEN, PROTO_UDP, \
            STD_TYPE, ucSnmpIn, MN_SNMP_IN_BUFF_SIZE, interface_no);
         }

      if (socket_no >= 0)
         {
         if (mn_recv(socket_no, ucSnmpIn, MN_SNMP_IN_BUFF_SIZE) > 0)
            {
            socket_ptr = MK_SOCKET_PTR(socket_no);
            usNumChar = socket_ptr->recv_len;
            usNumChar = mn_SNMP(usNumChar);
            if (usNumChar > 0 )
               (void)mn_send(socket_no,ucSnmpOut,usNumChar);
            }

         (void)mn_abort(socket_no);
         }
      }
}
#endif      /* (RTOS_USED != RTOS_NONE) */
#endif      /* MN_SNMP */

