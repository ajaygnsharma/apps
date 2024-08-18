/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/
#include "micronet.h"

#if (MN_SNMP && MN_SNMP_TRAP)

static int generateTrap(byte, byte, byte, SCHAR) cmx_reentrant;
static void setEnterprise(void) cmx_reentrant;
static void SNMPsetIP(SCHAR) cmx_reentrant;

/*
 file: snmptrap.c

 Traps are sent at the request of the application entity.  When the application
 determines a trap condition exists a function call is made to generateTrap.

 application values must be set for 
   trap community     
   OID of enterprise     
   IP address of agent   

 Generates a trap message
 a single variable-binding is included in each trap
 the ASN.1 encoding of the OID is placed in global ucOID prefixed by length
 example for sysUpTime0 is 08, 2b, 06, 01, 02, 01, 01, 03, 00
 gTrap-- generic trap type
      coldStart(0),
      warmStart(1),
      linkDown(2),
      linkUp(3),
      authenticationFailure(4),
      egpNeighborLoss(5),
      enterpriseSpecific(6)
 for generic trap type 0-5 sTrap -- specfic trap type is 0
 for generic trap type 6   sTrap is defined by enterprise
 the management station requests the value of the offending variable
     with a get request based on the specific trap type or included variable-Binding
*/

int mn_snmp_trap(int iName_OID, byte gTrap, byte sTrap, byte v, byte *dstaddr, \
   SCHAR interface_no)
cmx_reentrant {
   int iLength;
   int i;
   int status;
   SCHAR socket_no;

#if (MN_SNMP_VERSION2C == 0)
   if (v != 1)
      return (-badValue);   /*  V2 trap when V2 not enabled */
#else
   if ((v != 1) && (v != 2))
      return (-badValue);   /* Invalid trap number */
#endif

   if (gTrap > 6)
      return (-badValue);

   if (iName_OID < 0)
      return (-badValue);

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   status = -1;

   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   iVBTableIndex = iName_OID;
   iLength = getLengthVBTable();
   iVBTableIndex = iName_OID;
   for(i=0;i <iLength+1;i++)
      ucOID[i] = getVBTableByte();
   MN_MUTEX_RELEASE(MUTEX_SNMP);

   iLength = generateTrap(gTrap, sTrap, v, interface_no);

#ifdef snmpOutTraps0
   mn_incrementMeasVariable(snmpOutTraps0,4);      /* length of variable 4*/
#endif

   if (iLength > 0)
      {
      socket_no = mn_open(dstaddr, SNMP_TRAP_PORT, SNMP_TRAP_PORT, NO_OPEN,\
         PROTO_UDP, STD_TYPE, ucSnmpOut, MN_SNMP_OUT_BUFF_SIZE, interface_no);
      if (socket_no >= 0)
         {
         status = (int)mn_send(socket_no,ucSnmpOut,iLength);
         (void)mn_abort(socket_no);
         }
      else
         status = socket_no;
      }
   else
      status = iLength;

   return status;
}

/* send a v1 trap or v2 trap.
   For v1 trap ucOID has length Oid of variable to be included in trap.
   For v2 trap  a single variable-binding is included in each trap
   the ASN.1 encoding of the OID is placed in global ucOID prefixed by length
   example for sysUpTime0 is 08, 2b, 06, 01, 02, 01, 01, 03, 00.

*/
static int generateTrap(byte gTrap, byte sTrap, byte v, SCHAR interface_no)
cmx_reentrant {
   byte* pucPduLength;
   byte* pucVBLength;
   byte* pucLength;
   int   iLength;
   byte  ucError;
   byte  ucLength;
   byte  i;
#if MN_SNMP_VERSION2C
   byte ucTempOID[OID_BUFF_LEN];
#endif      /* MN_SNMP_VERSION2C */

   MN_MUTEX_WAIT(MUTEX_SNMP,INFINITE_WAIT);
   /* initialize output buffer*/
   pucSnmpOutPosition = ucSnmpOut;
   /* sequence tag and placeholder for length*/
   setOutSeqLen(LONG_LENGTH_1, 2);

   /* SNMP version*/
   *pucSnmpOutPosition = INTEGER;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = 0x01;      /* length 1*/
   pucSnmpOutPosition++;
#if MN_SNMP_VERSION2C
   if (v == 1)
#endif      /* MN_SNMP_VERSION2C */
      *pucSnmpOutPosition = MN_V1;     /* 0 is version 1*/
#if MN_SNMP_VERSION2C
   else
      *pucSnmpOutPosition = MN_V2C;    /* 1 is version 2*/
#endif      /* MN_SNMP_VERSION2C */
   pucSnmpOutPosition++;

   /* Community string in table or public*/
   /* OID of Community String for trap response.
      if not in table use fixedCommunity default is public.
      pucSnmpOutPosition is target
   */
   *pucSnmpOutPosition = OCTET_STRING;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = ucTrapCommunityLength;
   pucSnmpOutPosition++;
   for(i = 0; i < ucTrapCommunityLength; i++)
   {
      *pucSnmpOutPosition = ucTrapCommunity[i];
      pucSnmpOutPosition++;
   }

   /* trap and placeholder for length*/
#if MN_SNMP_VERSION2C
   if (v == 1)
#endif      /* MN_SNMP_VERSION2C */
      *pucSnmpOutPosition = trap;
#if MN_SNMP_VERSION2C
   else
      *pucSnmpOutPosition = v2_trap;
#endif      /* MN_SNMP_VERSION2C */
   pucSnmpOutPosition++;

   /* length long length 1*/
   *pucSnmpOutPosition = LONG_LENGTH_1;
   pucSnmpOutPosition++;
   /* save pointer to PDU length*/
   pucPduLength = pucSnmpOutPosition;
   /*skip length*/
   pucSnmpOutPosition++;
#if MN_SNMP_VERSION2C
   if (v == 1)
      {
#endif      /* MN_SNMP_VERSION2C */
      /* Enterprise tree*/   
      setEnterprise();
      /* IP address*/
      SNMPsetIP(interface_no);
#if MN_SNMP_VERSION2C
      }
   else
      {
      /* request ID */
      /* The two lsb's of sysUpTime is used for request ID in V2 trap */
      *pucSnmpOutPosition = INTEGER;
      pucSnmpOutPosition++;
      *pucSnmpOutPosition = 0x02;   /* length is two*/            
      pucSnmpOutPosition++;
      for(i=3;i<5;i++)
         {
         *pucSnmpOutPosition = ucMeasureData[sysUpTime0+i];
         pucSnmpOutPosition++;
         }
      }
#endif      /* MN_SNMP_VERSION2C */

   /* generic and specific trap */
   *pucSnmpOutPosition = INTEGER;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = 0x01;   /* length 1*/
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = gTrap;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = INTEGER;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = 0x01;      /* length 1*/
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = sTrap;
   pucSnmpOutPosition++;

   /* time of trap*/
#if MN_SNMP_VERSION2C
   if (v == 1)
#endif      /* MN_SNMP_VERSION2C */
      {
      /*  move time stamp to output */
      *pucSnmpOutPosition = TimeTicks;
      pucSnmpOutPosition++;
      *pucSnmpOutPosition = sysUpTime0_Length;     /*Time Ticks length 4*/
      pucSnmpOutPosition++;
      DISABLE_INTERRUPTS;
      for(i=0; i<sysUpTime0_Length; i++)
         {
         *pucSnmpOutPosition = ucMeasureData[sysUpTime0 + i];
         pucSnmpOutPosition++;
         }
      ENABLE_INTERRUPTS;
      }

   /*add sequence and length placeholder to output*/
   setOutSeqLen(LONG_LENGTH_1, 1);

   /* save for location of variable binding length*/
   pucVBLength = pucSnmpOutPosition;
   pucSnmpOutPosition ++;                       /* set to next token*/
#if MN_SNMP_VERSION2C
   if (v == 2)
      {
      /* save requested OID for last*/
      for(i=0;i<OID_BUFF_LEN;i++)
         ucTempOID[i] = ucOID[i];
      /* sysUpTime variable binding*/
      ucOID[0]=0x08;ucOID[1]=0x2b;ucOID[2]=0x06;ucOID[3]=0x01;ucOID[4]=0x02;
      ucOID[5]=0x01;ucOID[6]=0x01;ucOID[7]=0x03;ucOID[8]=0x00;
      }
#endif      /* MN_SNMP_VERSION2C */

   /* add variable binding */ 
   setOutSeqLen(LONG_LENGTH_1, 1);

   /* save for location of variable binding length*/
   pucLength = pucSnmpOutPosition;
   pucSnmpOutPosition ++;                       /* set to next token*/
   ucError = ucMoveOidVariableToOutput(NOT_GET_NEXT);
   if(ucError != noError)
      {
      MN_MUTEX_RELEASE(MUTEX_SNMP);
      return -noResponse;
      }

#if MN_SNMP_VERSION2C
   if (v == 2)
      {
      /* length of variable binding sequence*/
      ucLength = (byte)(pucSnmpOutPosition - pucLength -1);
      *pucLength = ucLength;
      /* snmpTrapOID0 variable binding*/
      ucOID[0]=0x0a;ucOID[1]=0x2b;ucOID[2]=0x06;ucOID[3]=0x01;ucOID[4]=0x06;
      ucOID[5]=0x03;ucOID[6]=0x01;ucOID[7]=0x01;ucOID[8]=0x04;ucOID[9]=0x01;ucOID[10]=0x00;
      /* add variable binding  */
      setOutSeqLen(LONG_LENGTH_1, 1);

      /* save for location of variable binding length*/
      pucLength = pucSnmpOutPosition;
      pucSnmpOutPosition ++;                       /* set to next token*/
      ucError = ucMoveOidVariableToOutput(NOT_GET_NEXT);
      if(ucError != noError)
         {
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         return -noResponse;
         }

      /* restore requested OID */
      /* length of variable binding sequence*/
      ucLength = (byte)(pucSnmpOutPosition - pucLength -1);
      *pucLength = ucLength;
      for(i=0;i<OID_BUFF_LEN;i++)
         ucOID[i] = ucTempOID[i];
      /* add variable binding */ 
      setOutSeqLen(LONG_LENGTH_1, 1);

      /* save for location of variable binding length*/
      pucLength = pucSnmpOutPosition;
      pucSnmpOutPosition ++;                       /* set to next token*/
      ucError = ucMoveOidVariableToOutput(0);
      if(ucError != noError)
         {
         MN_MUTEX_RELEASE(MUTEX_SNMP);
         return -noResponse;
         }
      }
#endif      /* MN_SNMP_VERSION2C */

   /* length of variable binding sequence*/
   ucLength = (byte)(pucSnmpOutPosition - pucLength -1);
   *pucLength = ucLength;
   /* enter message lengths*/
   iLength = (int)(pucSnmpOutPosition - pucPduLength - 1);
   *pucPduLength = (byte)iLength;
   iLength = (int)(pucSnmpOutPosition - pucVBLength - 1);
   *pucVBLength = (byte)iLength;
   iLength = (int)(pucSnmpOutPosition - ucSnmpOut - 3);
   ucSnmpOut[2] = (byte)iLength;
   MN_MUTEX_RELEASE(MUTEX_SNMP);

   return (iLength + 3);
}

/*
 Enterprise for trap response
 System sysObjectID contains Enterprise
*/
static void setEnterprise(void)
cmx_reentrant {
   byte  i;
   byte  ucLimit;

   iVBTableIndex = sysObjectID0 - 1;
   ucLimit = getVBTableByte();  /*length of system object ID*/
   *pucSnmpOutPosition = OBJECT_IDENTIFIER;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = ucLimit;
   pucSnmpOutPosition++;

   for(i = 0; i <ucLimit; i++)
   {
      iVBTableIndex = sysObjectID0 + i;
      *pucSnmpOutPosition = getVBTableByte();
      pucSnmpOutPosition++;
   }

}

/*
 sets the IP address in traps
 ip address is in MIB II.UDP....udpLocalAddress.1 (first entry in table)
 if not in table uses fixed address default  00.00.00.00
 pucSnmpOutPosition is target
*/
static void SNMPsetIP(SCHAR interface_no)
cmx_reentrant {
   byte  i;
   PINTERFACE_INFO interface_ptr;
   
   interface_ptr = MK_INTERFACE_PTR(interface_no);

   *pucSnmpOutPosition = IpAddress;
   pucSnmpOutPosition++;
   *pucSnmpOutPosition = IP_ADDR_LEN;  /*ip address length 4*/
   pucSnmpOutPosition++;

#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
   MN_MUTEX_WAIT(interface_ptr->mutex_interface,INFINITE_WAIT);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */

   for(i = 0; i < IP_ADDR_LEN; i++)
      {
      *pucSnmpOutPosition = interface_ptr->ip_src_addr[i];
      pucSnmpOutPosition++;
      }

#if (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP))
   MN_MUTEX_RELEASE(interface_ptr->mutex_interface);
#endif      /* (MN_DHCP || MN_BOOTP || MN_PING_GLEANING || !(MN_ARP)) */
}
#endif      /* (MN_SNMP && MN_SNMP_TRAP) */

