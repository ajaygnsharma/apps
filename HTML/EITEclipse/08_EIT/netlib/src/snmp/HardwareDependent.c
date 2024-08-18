/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
#include "micronet.h"
#include "np_includes.h"

#if MN_SNMP

#if (!(defined(POLC8051F124) || defined(CMXC8051F124) || \
   defined(POLC8051F120) || defined(CMXC8051F120)))
#include "vbtable.c"
#endif      /* (!(defined(POLC8051F124) || defined(CMXC8051F124))) */

void printVBTable(void)
{
  int i;
  for(i = 0; i < sizeof(VBTable); i++)
  {
    printf("VBTable[%d]=%0.2X\n\r",i,VBTable[i]);
  }
}
  
/*
 file: hardwareDependent
 The variable binding VBTtable is kept in flash memory and includes
 the OID's,the values of variables that slowly change and the 
 array index in the measureData[] array of variables that change
 often.  The method of reading and writing to flash is application
 dependent.
*/
void getVBTableFromFlash(void)
{
  memcpy(&VBTable[sysContact0],snmp.contact,SNMP_CONTACT_LEN);
  memcpy(&VBTable[sysLocation0],snmp.location,SNMP_LOCATION_LEN);
}


void setVBTableToFlash(void)
{
  memcpy(snmp.contact,&VBTable[sysContact0],SNMP_CONTACT_LEN);
  memcpy(snmp.location,&VBTable[sysLocation0],SNMP_LOCATION_LEN);
}

/*
 global variable iVBTIndex is current position
 return is variable binding table contents at current position
 current position is incremented
*/
byte getVBTableByte(void)
cmx_reentrant {
   byte  ucTbyte;
   ucTbyte = mn_flashRead(iVBTableIndex);
   iVBTableIndex++;
   return ucTbyte;
}

/*
 global variable iVBTIndex is current position
 return is variable binding table contents at current position
 current position is incremented
*/
void setVBTableByte(byte ucByte) 
cmx_reentrant {
   byte ucTemp;
   ucTemp = ucByte;
   mn_flashWrite(iVBTableIndex, ucTemp);
   flag.snmp_var_set = TRUE;
   iVBTableIndex++;
}

#if (!(defined(POLC8051F124) || defined(CMXC8051F124) || \
   defined(POLC8051F120) || defined(CMXC8051F120)))
/* Change the two functions below as needed. */

byte mn_flashRead(int iIndex)
cmx_reentrant 
{
   byte retval;

   retval = VBTable[iIndex];

   return (retval);
}

void mn_flashWrite(int iIndex, byte ucTemp)
cmx_reentrant 
{
  VBTable[iIndex] = ucTemp;
}
#endif      /* (!(defined(POLC8051F124) || defined(CMXC8051F124))) */

#endif      /* MN_SNMP */

