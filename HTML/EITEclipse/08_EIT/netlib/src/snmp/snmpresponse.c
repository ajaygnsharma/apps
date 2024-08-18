/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
#include "micronet.h"

#if MN_SNMP

static word16 getErrorV1(byte ucErrorCode) cmx_reentrant;

/* ------------------------------------------------ */
/* file: response.cpp */
/* responds to get, getnext, set */
/* ------------------------------------------------ */

/* ------------------------------------------------ */
/*input pucSnmpInPos is at [SNMP PDU tag] */
/* ------------------------------------------------ */
word16 ucGetResponse(void)
cmx_reentrant {
   word16 usLength;
   byte* pucVBLength;
   byte* pucVBSeqLength;
   byte* pucPudLength;
   byte  ucNext;
   byte  ucSet;
   byte* pucSnmpInStart;
   byte  ucErrorCode;
   byte  ucTemp;
   word16 usTotalLength;

   usTotalLength = 0;

   /* check for SNMP PDU type of input*/
   if((*pucSnmpInPosition == getnext_request)||(*pucSnmpInPosition == getbulk_request))
      ucNext = GET_NEXT;
   else
      ucNext = NOT_GET_NEXT;
   /* check for set*/
   if(*pucSnmpInPosition == set_request)
      ucSet = MN_SET;
   else
      ucSet = NOT_SET;

   /* skip length of input SNMP message*/
   pucSnmpInPosition ++;
   usLength = getLengthSnmpIn();

   /*add smmp get response type and length placeholder to output [a2][81][placeholder]*/
   *pucSnmpOutPosition = (byte)get_response;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = LONG_LENGTH_2;         /* messages are length 2 */
   pucSnmpOutPosition ++;                       /* set to length*/
   /* save for location of SNMP PUD length*/
   pucPudLength = pucSnmpOutPosition;
   pucSnmpOutPosition += 2;                     /* set to next token*/
   
   /* request ID from input transfered to output*/
   MoveDataInputToOutput();
   
   /* request error-status transfered to output -- value is placeholder*/
   MoveDataInputToOutput();

   /* request error-index transfered to output -- value is placeholde*/
   /* location is pucErrorStatus + 3*/
   MoveDataInputToOutput();

   /* skip sequence tag and length -- sequence of (OID-variable) sequences*/
   pucSnmpInPosition++;
   usLength = getLengthSnmpIn();

   /*add sequence and length placelolder to output*/
   setOutSeqLen(LONG_LENGTH_2, 1);

   /* save for location of variable binding length*/
   pucVBSeqLength = pucSnmpOutPosition;
   pucSnmpOutPosition += 2;                      /* set to next token*/
   /* ------------------------------------------------ */
   if(ucSet == MN_SET)
   {
      /* -------------- set's ---------------------- */
      /* save start position of [OID][variable]for data transfer*/
      pucSnmpInStart = pucSnmpInPosition;
      /* check that all variables can be set*/
      ucErrorCode = ucCheckSetVariables();
      if(ucErrorCode == noError)
      {
         /* set variables to input values*/
         pucSnmpInPosition = pucSnmpInStart;
         SetVariables();
      }
      /* response is same with and without error only error code changes*/
      usTotalLength = getErrorV1(ucErrorCode);
#ifdef snmpInSetRequests0
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      mn_incrementMeasVariable(snmpInSetRequests0,4);    /* length of variable 4*/
      MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
      return usTotalLength;
   }
   else
   {
      /* ------------ get's and get next's --------------- */
      /* get all (OID-variable) sequences*/
      ucErrorCode = 0x00;     /* initial value no error and no parameter count*/
      while(*pucSnmpInPosition == SEQUENCE)
      {
         /* parameter in error*/
         ucErrorCode += 0x10;    /* uperr four bit s count parameter number*/

         /* skip sequence tag and length -- sequence of (OID-variable) sequences*/
         pucSnmpInPosition++;
         usLength = getLengthSnmpIn();

         /*add sequence and length placelolder to output*/
         *pucSnmpOutPosition = (byte)SEQUENCE;
         if(incOut() == TOO_BIG) return 0;
         *pucSnmpOutPosition = LONG_LENGTH_1;         /* all lengths long one byte*/
         if(incOut() == TOO_BIG) return 0;
         /* save for location of variable binding length*/
         pucVBLength = pucSnmpOutPosition;
         if(incOut() == TOO_BIG) return 0;
         /* move OID and variable*/ 
         MoveOidToTemp();
         ucTemp = ucMoveOidVariableToOutput(ucNext);
         if(ucTemp == TOO_BIG) return 0;
         if(ucTemp != noError)
         {
         /* ------------------------------------------------ */
#ifdef snmpOutNoSuchNames0
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         mn_incrementMeasVariable(snmpOutNoSuchNames0,4);      /* length of variable 4*/
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
#if MN_SNMP_VERSION2C
            if(ucVersionIn != MN_V2C)
#endif
            {
               ucErrorCode |= ucTemp;
               usTotalLength = getErrorV1(ucErrorCode);
               return usTotalLength;
            }
         }
         /* length of variable binding sequence*/
         usLength = (word16)(pucSnmpOutPosition - pucVBLength -1);
         *pucVBLength = (byte)usLength;
         /* skip [0x5][0x00]*/
         pucSnmpInPosition += 2;       /* adjust pointer*/
#ifdef snmpInTotalReqVars0
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         mn_incrementMeasVariable(snmpInTotalReqVars0,4);      /* length of variable 4*/
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
         if(ucNext != GET_NEXT)
         {
#ifdef snmpInGetRequests0
            MN_MUTEX_RELEASE(MUTEX_SNMP);
            mn_incrementMeasVariable(snmpInGetRequests0,4);    /* length of variable 4*/
            MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
         }
         else
         {
#ifdef snmpInGetNexts0
            MN_MUTEX_RELEASE(MUTEX_SNMP);
            mn_incrementMeasVariable(snmpInGetNexts0,4);    /* length of variable 4*/
            MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
         }
      /* ---------- fill in lengths -------------------- */
      /* length of SNMP message*/
      usLength = (word16)(pucSnmpOutPosition - ucSnmpOut - 4);
      usTotalLength = usLength + 4;    /* add length of initial sequence*/
      ucTemp = (char)(usLength>>8);
      ucSnmpOut[2] = ucTemp;
      ucTemp = (char)(usLength & 0x00ff);
      ucSnmpOut[3] = ucTemp;
      /* length of PDU*/
      usLength = (word16)(pucSnmpOutPosition - pucPudLength - 2);
      ucTemp = (char)(usLength>>8);
      *pucPudLength = ucTemp;
      ucTemp = (char)(usLength & 0x00ff);
      *(pucPudLength+1) = ucTemp;
      /* length of variable binding sequence*/
      usLength = (word16)(pucSnmpOutPosition - pucVBSeqLength - 2);
      ucTemp = (char)(usLength>>8);
      *pucVBSeqLength = ucTemp;
      ucTemp = (char)(usLength & 0x00ff);
      *(pucVBSeqLength+1) = ucTemp;
      } 
      return usTotalLength;
   }
}  
/* ------------------------------------------------ */
/*input pucSnmpInPos is at [SNMP PDU tag] */
/* ------------------------------------------------ */
static word16 getErrorV1(byte ucErrorCode)
cmx_reentrant {
   word16 usLength;
   word16 usTotalLength;
   word16 i;
   byte* pucPDU;
   byte* pucStatus;
   byte* pucIndex;
   int   iDifference;
   word16 usTemp;
   byte ucErrCnt;

   /* start of  message*/ 
   pucSnmpInPosition = ucSnmpIn;

   /* get length of input sequence*/
   pucSnmpInPosition ++;
   usTotalLength = getLengthSnmpIn();
	usTemp = pucSnmpInPosition - ucSnmpIn;
	usTotalLength += usTemp;

   /* skip version and community tag [02][01][00][04]*/
   pucSnmpInPosition += 4;    /* adjust pointer*/

   /* skip community string*/
   usLength = getLengthSnmpIn();
   pucSnmpInPosition += usLength;

   /* location of PDU type*/
   pucPDU = pucSnmpInPosition;

   /* skip PDU tag and length*/
   pucSnmpInPosition ++;
   (void)getLengthSnmpIn();

   /* skip request Id*/
   pucSnmpInPosition ++;
   usLength = getLengthSnmpIn();
   pucSnmpInPosition += usLength;

   /* go to error status*/ 
   pucSnmpInPosition ++;
   usLength = getLengthSnmpIn();
   pucStatus = pucSnmpInPosition;

   /* go to error index [status][tag]*/
   pucSnmpInPosition += 2;    /* adjust pointer*/
   usLength = getLengthSnmpIn();
   pucIndex = pucSnmpInPosition;

   /* clear output*/
   for(i = 0; i < MN_SNMP_OUT_BUFF_SIZE; i++)
      ucSnmpOut[i] = 0x00;

   /* move input to output*/
   pucSnmpInPosition = ucSnmpIn;
   pucSnmpOutPosition = ucSnmpOut;
   for(i = 0; i < usTotalLength; i++)
   {
      *pucSnmpOutPosition = *pucSnmpInPosition;
/*      pucSnmpOutPosition++; */
      if(incOut() == TOO_BIG) return 0;
      pucSnmpInPosition++;
   }

   /* set to get response*/
   iDifference = (int)(pucPDU - ucSnmpIn);
   pucSnmpOutPosition = ucSnmpOut + iDifference;
   *pucSnmpOutPosition = get_response;

   /* set error status*/
   iDifference = (int)(pucStatus - ucSnmpIn);
   pucSnmpOutPosition = ucSnmpOut + iDifference;
   *pucSnmpOutPosition = ucErrorCode & 0x0f;    /* error code in lower bits*/

   /* set error index*/
   iDifference = (int)(pucIndex - ucSnmpIn);
   pucSnmpOutPosition = ucSnmpOut + iDifference;
   ucErrCnt = ucErrorCode >> 4;         /*error code count in upper bits*/
   *pucSnmpOutPosition = ucErrCnt;

   return usTotalLength;
}
#endif      /* MN_SNMP */

