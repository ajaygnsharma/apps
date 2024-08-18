/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
#include "micronet.h"

#if MN_SNMP

static byte ucFindOID(byte next) cmx_reentrant;

/*
 file: utility.cpp
 common files 
*/

/*
 decode length in snmp input 
 length of oid or variable limited to 255 bytes
 decode ASN length  -- only 1 or 2 length bytes allowed
 Returns 8 bit value of length 
 input global pointer pucArray points to first octet of length
 length is returned
 pucSnmpInPosition is set to next after length
*/
word16 getLengthSnmpIn(void)
cmx_reentrant {
   word16 usLength;

   usLength = 0;
   /* check for length of length 2*/
   if(*pucSnmpInPosition == (byte)LONG_LENGTH_2)
   {
      pucSnmpInPosition ++;
      usLength = LSHIFT8(*pucSnmpInPosition);
      pucSnmpInPosition ++;
   }
   /* check for length of length 1*/
   else if(*pucSnmpInPosition == LONG_LENGTH_1)
   {
      pucSnmpInPosition ++;
   }

   usLength = usLength + *pucSnmpInPosition;
   pucSnmpInPosition ++;

   return (usLength);
}

/*
 decode length in VBTable
 global index iVBTableIndex is depth in variable binding table
 length of oid or variable limited to 255 bytes
 decode ASN length  -- only 1 or 2 length bytes allowed
 Returns 8 bit value of length 
 result iVBTableIndex points to next
*/
byte getLengthVBTable(void)
cmx_reentrant {
   byte ucTemp;

   ucTemp = getVBTableByte();
   /* check for length of length 2*/
   if(ucTemp == (byte)LONG_LENGTH_2)
   {
      iVBTableIndex ++;          /* to remain less than 255 second byte  must be zero*/
      ucTemp =getVBTableByte();
   }
   /* check for length of length 1*/
   else if(ucTemp == LONG_LENGTH_1)
   {
      ucTemp =getVBTableByte();
   }
   return ucTemp;
}

/*
 [OID length][OID value][access][tag][length][variable]
      or                        [ff][2 type][6 parameters][tag][length][2 RAM address] 
 Input variable next -- 0 finds OID -- if not 0 gives next
 global variable pucArray points to length of OID to be found in table 
 Result is OID position in table of OID or OID of next higher variable-binding.
 Returns 0 if in table 1 if not in table 
 Reuses RAM ucSnmpIn for compare
*/
static byte ucFindOID(byte next)
cmx_reentrant {
   byte  ucVBLength;
   byte  ucArrayLength;
   byte  i;
   byte  ucLimit;
   byte  ucTemp;
   byte  a;
   byte* pucOidPosition;
   int   iVBTableCurrent;
#if MN_SNMP_VERSION2C
   byte  ucObjectIndex;
#endif
   
   /* start of variable binding table & start of input variable binding*/
   iVBTableIndex = 0;

   /*
    if V2 
    length = length - index length  and  save index
    only last index -- double index first index reported as no object 
   */

   /* table is terminated when OID start is 0*/
   while( getVBTableByte() != END_OF_TABLE)
   {
      /* return index to variable binding length*/
      iVBTableIndex --;
      /* save start of this comparison*/
      iVBTableCurrent = iVBTableIndex;
      pucOidPosition = ucOID;
      /*length of OID's*/
      ucVBLength = getLengthVBTable();
      ucArrayLength = *pucOidPosition; /* length is first value one byte*/
      pucOidPosition++;
      /*
       if V2
       reduce length by index length
      */
#if MN_SNMP_VERSION2C
      ucObjectIndex = *(ucOID + ucArrayLength);
#endif

      /* check with OID has shortest length, */
      if(ucArrayLength < ucVBLength) 
         ucLimit = ucArrayLength;
      else
         ucLimit = ucVBLength;
      
      /* determine largest a=0 is equal; a=1 is table larger;
         a=2 is input is larger
      */
      a = 0;      /* start assume equal*/
      for(i=0; i < ucLimit; i++)
      {
         ucTemp = getVBTableByte();
         if((*pucOidPosition < ucTemp) && (a ==0))
            a = 1;   /* table is larger*/
         if((*pucOidPosition > ucTemp) && (a ==0))
            a = 2;   /* input is larger*/
         pucOidPosition ++;
      }

      /* check for find OID or find next OID*/
      if(next == NOT_GET_NEXT)
      {
         /* for get */
         /* check if IOD's are equal */
         if ((a == 0) && (ucArrayLength == ucVBLength))  /* 0 is equal */
         {
            /*
             if V2
             compare index if equal return no error
             if  not return no instnace
            */
#if MN_SNMP_VERSION2C
            iVBTableIndex --;
            ucTemp = getVBTableByte();       /* table index points to oid index */
            if(ucObjectIndex != ucTemp)
            {
               if(ucObjectIndex == 0)        /* oid ending in 0 has not a table */
                  return (noSuchObject);
               else
                  return(noSuchInstance);
            }

#endif

            iVBTableIndex = iVBTableCurrent;
            return(noError);
         }

      }
      else
      {
         /* for get next */
         /* Check if OID in Table larger than input*/
         /*a=0 is equal; a=1 is table larger; a=2 is input is larger*/
         if( ((a == 0) && (ucArrayLength < ucVBLength)) || (a == 1) )
         {
            iVBTableIndex = iVBTableCurrent;
            return (noError);
         }
      }

      /*set input pointer to next variable-binding tag*/
   
      /* skip over remaining table OID*/
      if ( ucArrayLength < ucVBLength)
      {
         iVBTableIndex = iVBTableIndex + ucVBLength;
         iVBTableIndex = iVBTableIndex - ucArrayLength;
      }
      /* skip [access]*/
      iVBTableIndex++;

      /* if not measurement get length of variable*/
      ucTemp = getVBTableByte();
      if(ucTemp != AGENT_VARIABLE)
      {
         /* variable length*/
         ucVBLength = getLengthVBTable();

         /* table pointer to next OID*/
         iVBTableIndex = iVBTableIndex + ucVBLength;
      }
      else
      {
         /* rest of entry is fixed length for measurement*/
         iVBTableIndex += 12;    /* adjust pointer*/
      }
   }
   /* OID was not in table, table pointer is at next higher OID 
    if V2
    no such object
   */
#if MN_SNMP_VERSION2C
   if(ucVersionIn == MN_V2C)
   {
      if (next == NOT_GET_NEXT)
         return (noSuchObject);
      else
         return (endOfMibView);
   }
#endif

   return(noSuchName);
}

/*
 move token tag length and variable from input to output
 input pucSnmpInPosition at token
 output puucSnmpInPosition at next token
*/
void MoveOidToTemp(void)
cmx_reentrant {
   byte  ucLength;
   byte  i;
   byte* pucTemp;

   /* skip tag*/
   pucSnmpInPosition++;
   /* length of variable*/
   ucLength = (byte)getLengthSnmpIn();
   pucTemp = ucOID;
   *pucTemp = ucLength;
   pucTemp++;
   /*  temp to be used for look up of OID*/
   for (i = 0; i < (ucLength) ; i++)
   {
      *pucTemp = *pucSnmpInPosition;
      pucTemp++;
      pucSnmpInPosition++;
   }
}

/*
 move token tag length and variable from input to output
 input pucSnmpInPosition at token
 output puucSnmpInPosition at next token
*/
void MoveDataInputToOutput(void)
cmx_reentrant {
   byte  ucLength;
   byte  i;
   byte* pucTemp;

   /* move tag*/
   *pucSnmpOutPosition = *pucSnmpInPosition;
   pucSnmpInPosition++;
   pucSnmpOutPosition++;
   /* length of variable*/
   ucLength = (byte)getLengthSnmpIn();
   /* output uses short length for individual variables*/
   *pucSnmpOutPosition = ucLength;
   pucSnmpOutPosition++;
   /* move variable to output -- and temp to be used for look up of OID*/
   pucTemp = ucOID;
   *pucTemp = ucLength;
   pucTemp++;
   for (i = 0; i < (ucLength) ; i++)
   {
      *pucSnmpOutPosition = *pucSnmpInPosition;
      *pucTemp = *pucSnmpInPosition;
      pucTemp++;
      pucSnmpInPosition++;
      pucSnmpOutPosition++;
   }
}

/*
 ucOID contains OID of variable
 tag length OID tag length and variable are moved to output
 output placed by pointer pucSnmpOutPosition
 [OID length][OID value][access][tag][length][variable]
      or                        [ff][2 type][6 parameters][tag][length][2 RAM address] 
*/
byte ucMoveOidVariableToOutput(byte ucNext)
cmx_reentrant {
   byte ucError;
   byte ucVBLength;
   byte ucTag;
   byte i;
   byte ucIndex;
   short sIndex;
	byte ucTemp;			
	byte * pucOutLength;	
#if MN_SNMP_VERSION2C
   byte* pucTemp;
#endif

   /*Find OID in table*/
   ucError = ucFindOID(ucNext);
   /* set tag*/
   *pucSnmpOutPosition = OBJECT_IDENTIFIER;
   if(incOut() == TOO_BIG) return TOO_BIG;
   /*
    if V2 
    if error
    move input to output
    change type to no object or no instance
    return 0
   */
#if MN_SNMP_VERSION2C
   if((ucVersionIn == MN_V2C) && (ucError != noError))
   {
      /* move OID to output -*/
      pucTemp = ucOID;
      ucVBLength = *pucTemp;
      pucTemp++;
      *pucSnmpOutPosition = ucVBLength;
      if(incOut() == TOO_BIG) return TOO_BIG;
      for (i = 0; i < (ucVBLength) ; i++)
      {
         *pucSnmpOutPosition = *pucTemp;
         pucTemp++;
         pucSnmpOutPosition++;
      }
      *pucSnmpOutPosition = ucError;
      if(incOut() == TOO_BIG) return TOO_BIG;
      *pucSnmpOutPosition = 0;
      if(incOut() == TOO_BIG) return TOO_BIG;
#ifdef snmpOutGenErrs0
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         mn_incrementMeasVariable(snmpOutGenErrs0,4);    /* length of variable 4*/
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
      return (genErr);
   }
#endif

   /* get length and set output*/
   ucVBLength = getLengthVBTable();
   *pucSnmpOutPosition = ucVBLength;
   if(incOut() == TOO_BIG) return TOO_BIG;
   for(i = 0; i<ucVBLength; i++)
   {
      *pucSnmpOutPosition = getVBTableByte();
      if(incOut() == TOO_BIG) return TOO_BIG;
   }

   /* skip [access]*/
   iVBTableIndex ++;       
   /* save type and index*/
   ucTag = getVBTableByte();
   ucIndex = ucTag;
   if(ucTag == AGENT_VARIABLE)
   {
      iVBTableIndex += 8;        /* length of parameters in VBTable*/
      ucTag = getVBTableByte();
   }
   /* move tag to output*/
	if(ucTag==DisplayString)				
		*pucSnmpOutPosition = OCTET_STRING;	
	else									
		*pucSnmpOutPosition = ucTag;	
   pucSnmpOutPosition++;

   /* get Length and move to output*/
	pucOutLength = pucSnmpOutPosition;			
   ucVBLength = getLengthVBTable();
   *pucSnmpOutPosition = ucVBLength;
   if(incOut() == TOO_BIG) return TOO_BIG;
   if(ucIndex != 0xff)
   {
      /* static value move from VBTable*/
      for (i = 0; i < ucVBLength;i++)
      {
			ucTemp = getVBTableByte();
			if((ucTag == DisplayString) && (ucTemp == 0))	
			{
				*pucOutLength = i;
				break;
			}											
			*pucSnmpOutPosition = ucTemp;
         if(incOut() == TOO_BIG) return TOO_BIG;
      }
   }
   else
   {
      /* Ram Index*/
      sIndex = (short)getVBTableByte();
      sIndex <<= 8;
      sIndex |= (short)getVBTableByte();
      /* measured value move from RAM*/
      for (i = 0; i < ucVBLength; i++)
      {
			ucTemp = ucMeasureData[sIndex+i];
			if((ucTag == DisplayString) && (ucTemp == 0))	
			{
				*pucOutLength = i;
				break;
			}											
			*pucSnmpOutPosition = ucTemp;
         if(incOut() == TOO_BIG) return TOO_BIG;
      }
   }

   return ucError;

}

/*
 [OID length][OID value][access][type][type][tag][length][variable]
 checks (in table) (read-write access) (variable length < = storage)
 input pucSnmpInPosition is at message sequence tag
*/
byte ucCheckSetVariables(void)
cmx_reentrant {
   byte ucError;
   byte ucErrorIndex;
   byte ucVBLength;
   byte ucVariableTag;
   byte ucVariableLength;
   byte ucTemp;


   /* initialize variable index*/
   ucErrorIndex = 0x00;
   pucSnmpOutPosition = ucSnmpOut;      /* safe place */

   /* check all variables*/
   while(*pucSnmpInPosition == SEQUENCE)
   {
      /* next variable index*/
      ucErrorIndex += 0x10;      /* variable index in upper four bits*/

      /* skip sequence tag and length -- sequence of (OID-variable) sequences*/
      pucSnmpInPosition++;
      (void)getLengthSnmpIn();

      /* parse input OID and variable */
      
      /* move [tag][length][OID] to temp ucOID[]*/
      /* also to output but will be covered later */
      MoveOidToTemp();

      /*get variable tag*/ 
      ucVariableTag =*pucSnmpInPosition;
      
      /* get variablelength*/
      pucSnmpInPosition++;
      ucVariableLength = (byte)getLengthSnmpIn();
      
      /* set address to next token*/
      pucSnmpInPosition = pucSnmpInPosition + ucVariableLength;

      /* compare to VBTable and report errors */

      /* find OID in table */
      ucError = ucFindOID(NOT_GET_NEXT);
      if(ucError != noError)
      {
#ifdef snmpOutNoSuchNames0
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         mn_incrementMeasVariable(snmpOutNoSuchNames0,4);      /* length of variable 4*/
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
         ucTemp = ucErrorIndex | noSuchName;
#if MN_SNMP_VERSION2C
         if(ucVersionIn == MN_V2C)
            ucTemp = ucErrorIndex | noCreation;
#endif
         return(ucTemp);
      }

      /* skip length and OID in table*/
      ucVBLength = getLengthVBTable();
      iVBTableIndex = iVBTableIndex + ucVBLength;

      /* check access for read write 0x1 is readWrite*/
      if(getVBTableByte() != SNMP_READ_WRITE)
      {
         ucTemp = ucErrorIndex | readOnly;
#if MN_SNMP_VERSION2C
         if(ucVersionIn == MN_V2C)
            ucTemp = ucErrorIndex | notWritable;
#endif
         return(ucTemp);
      }

		/* check tag */
		ucTemp = getVBTableByte();
		if(ucTemp == AGENT_VARIABLE)
		{
		 	iVBTableIndex += 8;
			ucTemp = getVBTableByte();
		}

		if((ucVariableTag != ucTemp)
		  && ((ucTemp != DisplayString) || (ucVariableTag != OCTET_STRING)))
      {
#ifdef snmpOutBadValues0
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         mn_incrementMeasVariable(snmpOutBadValues0,4);     /* length of variable 4*/
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
         ucTemp = ucErrorIndex | badValue;
#if MN_SNMP_VERSION2C
         if(ucVersionIn == MN_V2C)
            ucTemp = ucErrorIndex | wrongType;
#endif
         return(ucTemp);
      }

      /* check length*/
      if(ucVariableLength > getLengthVBTable())
      {
#ifdef snmpOutBadValues0
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         mn_incrementMeasVariable(snmpOutBadValues0,4);     /* length of variable 4*/
         MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
         ucTemp = ucErrorIndex | badValue;
#if MN_SNMP_VERSION2C
         if(ucVersionIn == MN_V2C)
            ucTemp = ucErrorIndex | wrongLength;
#endif
         return(ucTemp);
      }
      /* variable may be set*/
   }
   return noError;
}

/*
 set variables must be Static -- in non-volitle memory
 [OID length][OID value][access][type][type][tag][length][variable]
*/
void SetVariables(void)
cmx_reentrant {
   byte ucVBLength;
   byte i;
   short sIndex;
   byte ucLength;
   byte ucType;
   byte ucTemp;
	byte ucTag;

   sIndex = 0;

   /* set destination address */

   /* set all variables*/
   while(*pucSnmpInPosition == SEQUENCE)
   {
      /* skip sequence tag and length -- sequence of (OID-variable) sequences*/
      pucSnmpInPosition++;
      ucLength = (byte)getLengthSnmpIn();

      /* move [tag][length][OID] to temp ucOID[]*/
      /* also to output but will be covered later */
      MoveOidToTemp();

      /*Find OID in table 0 is not next*/
      (void)ucFindOID(0);

      /* skip OID*/
      ucVBLength = getLengthVBTable();
      iVBTableIndex = iVBTableIndex + ucVBLength;

      /* skip [access]*/
      iVBTableIndex ++;

      /* save [type] */
      ucType = getVBTableByte(); /* save [type]*/
		ucTag = ucType;					
		if(ucType >= RAM_STORAGE)
		{
			iVBTableIndex += 8;
			ucTag = getVBTableByte();		
		}

      /*variable length*/
      ucVBLength = getLengthVBTable();

      /* if in RAM set ram index*/
      if(ucType >= RAM_STORAGE)
      {
         /* ram index*/
			sIndex = (short)getVBTableByte();
			sIndex <<= 8;
			sIndex |= (short)getVBTableByte();
      }

      /* get input variable address */

      /* skip variable tag */ 
      pucSnmpInPosition ++;

      /*  get variable length*/
      ucLength = (byte)getLengthSnmpIn();

      /* move from input to variable */ 
      for(i = 0; i <ucVBLength; i++)
      {
			/* pad if short*/
			if (i > (ucLength-1))
			{
				if(ucTag == DisplayString)	
					ucTemp = 0x00;		/* string nulls*/		
				else							
					ucTemp = 0x20;		/* ascii spaces*/
			}
         else
         {
            ucTemp = *pucSnmpInPosition;
            pucSnmpInPosition++;
         }

         /* store in static or RAM*/
         if(ucType < RAM_STORAGE)
         {
            /* static value move to VBTable*/
            setVBTableByte(ucTemp);
         }
         else
         {
            ucMeasureData[sIndex] = ucTemp;
            sIndex++;
         }
      }
#ifdef snmpInTotalSetVars0
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      mn_incrementMeasVariable(snmpInTotalSetVars0,4);      /* length of variable 4*/
      MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif

   }

}

/* write sequence and length codes to output, then skip desired number
   of spots.
*/
void setOutSeqLen(byte len_type, byte num_skip)
cmx_reentrant {
   *pucSnmpOutPosition = SEQUENCE;        /* sequence */
   pucSnmpOutPosition ++;
   *pucSnmpOutPosition = len_type;        /* Length code */
   pucSnmpOutPosition = pucSnmpOutPosition + num_skip;
}

/*
 increment agent variable
 msb -- --  lsb
*/
void mn_incrementMeasVariable(word16 parameter, byte length)
cmx_reentrant {
   byte ucArray[4];
   byte i;

   if ((length > 4) || ((length + parameter) > SNMP_DATA_LENGTH))
      return;

   mn_getRamVariable(ucArray,parameter,length);
   for(i=0;i<(length);i++)
   {
      if(i==0)             /* increment first character*/
      {
         ucArray[length-1]++; /* least significant at highest location*/
      }
      else 
      {
         if(ucArray[length-i] == 0) /* check for roll over*/
         {
            ucArray[length-1-i]++;  /* increment next digit*/
         }
         else
         {
            break;               /* quit on no roll over*/
         }
      }
   }
   mn_setRamVariable(ucArray,parameter,length);
}

void mn_getRamVariable(byte* ucArray,word16 parameter, byte length)
cmx_reentrant {
   int i;

   if ((length + parameter) <= SNMP_DATA_LENGTH)
      {
      MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
      for(i=0;i<length;i++)
         ucArray[i] = ucMeasureData[parameter + i];
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      }
}

void mn_setRamVariable(byte* ucArray,word16 parameter, byte length)
cmx_reentrant {
   int i;

   if ((length + parameter) <= SNMP_DATA_LENGTH)
      {
      MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
      for(i=0;i<length;i++)
         ucMeasureData[parameter + i] = ucArray[i];
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      }
}

void mn_getFlashVariable(byte* ucArray, word16 parameter, byte length)
cmx_reentrant {
   int i;
   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   for(i=0;i<length;i++)
   {
      iVBTableIndex = parameter + i;
      ucArray[i] = getVBTableByte();
   }
   MN_MUTEX_RELEASE(MUTEX_SNMP);
}

void mn_setFlashVariable(byte *ucArray,word16 parameter, byte length)
cmx_reentrant {
   int i;
   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   iVBTableIndex = parameter;
   for(i=0;i<length;i++)
   {
      setVBTableByte(ucArray[i]);
   }
   MN_MUTEX_RELEASE(MUTEX_SNMP);
}

/* checks for output message too long */
/* usage is if(incOut() == TOO_BIG) return 0; */
byte incOut(void)
cmx_reentrant {

   pucSnmpOutPosition++;
   if((pucSnmpOutPosition - ucSnmpOut) > MN_SNMP_OUT_BUFF_SIZE)
   {
#ifdef snmpOutTooBigs0
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      mn_incrementMeasVariable(snmpOutTooBigs0,4);    /* length of variable 4*/
      MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
#endif
      return TOO_BIG;
   }
   else
      return noError;
}
#endif      /* MN_SNMP */

