/*------------------------------------------------------------------------ 
					ssi_overview.c

Description:	Return SSIs for overview.htm page
				1. Model name
				2. Firmware Version
				3. MAC Address
				4. Ethernet Port Settings

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

/*------------------------------------------------------------------------
 Function:    num_ascii()
 Description: SSI: Convert Numbers to Ascii
 Input:       Number
 Output:      Ascii Representation of Input
------------------------------------------------------------------------*/
byte num_ascii(byte indata)
cmx_reentrant
{
  if(indata>9)
  {
    indata=indata+'a'-10;
  }
  else
  {
    indata+='0';
  }
  return indata;
}

/*------------------------------------------------------------------------
 Function:    byte_ascii()
 Description: SSI: Convert the Bytes to ascii
 Input:       None
 Output:      Pointer to the data
------------------------------------------------------------------------*/
void byte_ascii(byte *str,byte *indata,byte number)
cmx_reentrant 
{
     unsigned char i;
     byte num;
     
     	for (i=0;i<number;i++)
     	{
		num=(indata[i]&0xf0)/0x10;
		*str++=num_ascii(num);
		num=indata[i]&0x0f;
		*str++=num_ascii(num);
	}
	*str='\0';
}

/*------------------------------------------------------------------------
 Function:    model_func()
 Description: SSI: Display the Model Number
 Input:       None
 Output:      String Containing the Model Number
------------------------------------------------------------------------*/
word16 model_func(byte **str_)
cmx_reentrant 
{
  strcpy(iserver.model,MODEL_NAME);      /* Copy the IServer Model Name */
  *str_=(byte *)iserver.model;           
  return (strlen((char *)iserver.model));
}


/*------------------------------------------------------------------------
 Function:    version_func()
 Description: SSI: Display the Firmware Version
 Input:       None
 Output:      String Containing the Firmware Version
------------------------------------------------------------------------*/
word16 version_func(byte **str_)
cmx_reentrant 
{
  strcpy(iserver.version,FIRMWARE_VERSION);/* Copy the IServer Ver  No: */
  *str_=(byte *)iserver.version;           
  return (strlen((char *)iserver.version));
}

/*------------------------------------------------------------------------
 Function: 	  getMac_func()
 Description: Return MAC Address in the correct format
 Input:		  String that will contain the MAC Address
 Output:	  No of Bytes of MAC Address
------------------------------------------------------------------------*/ 
word16 getMac_func(byte *str)
cmx_reentrant 
{
	word16 num_bytes;
	byte temp;
	byte * temp_str;
	byte i;
	num_bytes = 0;
	temp_str = str;
	for(i=0;i<6;i++)
	{
		temp=(eth_src_hw_addr[i]>>4)&0x0f;
		if(temp<=9)
		{
			*temp_str++=temp+'0';
		}
		else
		{
			*temp_str++=temp-0x0a+'A';
		}
		temp=eth_src_hw_addr[i]&0x0f;
		if(temp<=9)
		{
			*temp_str++=temp+'0';
		}
		else
		{
			*temp_str++=temp-0x0a+'A';
		}
		*temp_str++=':';
		num_bytes+=3;
	}
	*(--temp_str)=' ';
	return (num_bytes-1);
}

/*------------------------------------------------------------------------
 Function: 	  mac_func()
 Description: SSI: Read MAC Address
 Input:		  None
 Output:	  No of Bytes of MAC Address
------------------------------------------------------------------------*/ 
word16 mac_func(byte **str_)
cmx_reentrant 
{
  word16 i;
  byte *str;
  
  str =cgi_buff;
  *str_=cgi_buff;
  i=getMac_func(str);
  
  return i;
}


/*------------------------------------------------------------------------
 Function:    port_func()
 Description: SSI: Display information about Ethernet Port
              Autonegotiation: 0: Auto ON 
                               1: Auto Off

              Speed:           0: 10 Mbps
                               1: 100 Mbps

              Duplex:          0: Half Duplex
                               1: Full Duplex

 Input:       None
 Output:      String Containing the information
------------------------------------------------------------------------*/
word16 port_func(byte **str_)
cmx_reentrant 
{
  char str[20];

  switch(flag.autonegotiation)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"Auto"); break;
    default: strcpy(str,"Auto"); break;
  }

  switch(flag.speed)
  {
    case 0: strcat(str,"10Mbps"); break;
    case 1: strcat(str,"100Mbps"); break;
    default: strcat(str,"100Mbps"); break;
  }

  switch(flag.duplex)
  {
    case 0: strcat(str," Half"); break;
    case 1: strcat(str," Full"); break;
    default: strcat(str," Full"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    dhcp_func()
 Description: SSI: Display the DHCP Flag
 Input:       None
 Output:      String Containing the DHCP Flag
------------------------------------------------------------------------*/
word16 dhcp_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.dhcp)
  {
    case 0: strcpy(str,"Disable"); break;
    case 1: strcpy(str,"Enable"); break;
    default: strcpy(str,"Disable"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    snmp_func()
 Description: SSI: Display the SNMP Flag
 Input:       None
 Output:      String Containing the SNMP Flag
------------------------------------------------------------------------*/
word16 snmp_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.snmp)
  {
    case 0: strcpy(str,"Disable"); break;
    case 1: strcpy(str,"Enable"); break;
    default: strcpy(str,"Disable"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    modbus_func()
 Description: SSI: Display the MODBUS Flag
 Input:       None
 Output:      String Containing the MODBUS Flag
------------------------------------------------------------------------*/
word16 modbus_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(terminal_server.protocol)
  {
    case TCP_PROTOCOL: strcpy(str,"Disable"); break;
    case UDP_PROTOCOL: strcpy(str,"Disable"); break;
    case MODBUS_PROTOCOL: strcpy(str,"Enable"); break;
    default: strcpy(str,"Disable"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function: 	  get_IPAddr_func()
 Description: Return IP Address in the correct format
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
word16 get_IPAddr_func(byte *str, byte *ip_addr)
cmx_reentrant 
{
	byte bytes_added;
	word16 num_bytes;
	byte * temp_str;

	num_bytes = 0;
	temp_str = str;

	bytes_added = mn_uctoa(temp_str, ip_addr[0]);
	temp_str += bytes_added;
	num_bytes += bytes_added;
	*temp_str++ = '.';
	num_bytes++;
	bytes_added = mn_uctoa(temp_str, ip_addr[1]);
	temp_str += bytes_added;
	num_bytes += bytes_added;
	*temp_str++ = '.';
	num_bytes++;
	bytes_added = mn_uctoa(temp_str, ip_addr[2]);
	temp_str += bytes_added;
	num_bytes += bytes_added;
	*temp_str++ = '.';
	num_bytes++;
	bytes_added = mn_uctoa(temp_str, ip_addr[3]);
	num_bytes += bytes_added;

	return (num_bytes);
}


/*------------------------------------------------------------------------
 Function: 	  ip_func()
 Description: SSI: Return IP Address 
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
word16 ip_func(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  str=cgi_buff;
  *str_=cgi_buff;

  temp=get_IPAddr_func(str,(byte *)eth_src_addr);
  return (temp);
  
}



/*------------------------------------------------------------------------
 Function: 	  submask_func()
 Description: SSI: Return Subnet Mask
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
word16 submask_func(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str=cgi_buff;
  *str_=cgi_buff;
	
  temp=get_IPAddr_func(str,subnet_mask);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function: 	  dns_ip_func()
 Description: Read the DNS IP Address 
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
word16 ssi_dns(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str=cgi_buff;
  *str_=cgi_buff;

  temp=get_IPAddr_func(str,ip_dns_addr);
	
  return (temp);
}


/*------------------------------------------------------------------------
 Function: 	  gw_ip_func()
 Description: Read the Gateway IP Address 
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
word16 gw_ip_func(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str=cgi_buff;
  *str_=cgi_buff;

  temp=get_IPAddr_func(str,gateway_ip_addr);
	
  return (temp);
}


/*------------------------------------------------------------------------
 Function: 	  hname_func()
 Description: Read Hostname
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
word16 hname_func(byte **str)
cmx_reentrant 
{
  *str = (byte *)network.hostname;

  return (strlen((char *)network.hostname));
}


/*------------------------------------------------------------------------
 Function:    serial_func()
 Description: SSI: Display Serial Settings
              Baudrate:        00: 300
                               01: 600
                               02: 1200
                               03: 2400
                               04: 4800
                               05: 9600
                               06: 19200
                               07: 38400
                               08: 57600
                               09: 115200
                               10: 230400
                               11: 460800
                               12: 921600

              Parity:          0: None
                               1: Odd
                               2: Even

              Data Bits:       0: 8
                               1: 7
                               2: 6
                               3: 5
              
              Stop bits:       0: 1
                               1: 2

              Flow Control:    0: None
                               1: Software
                               2: Hardware

 Input:       None
 Output:      String Containing the information
------------------------------------------------------------------------*/
word16 serial_func(byte **str_)
cmx_reentrant 
{
  char str[30];

  switch(serial.baudrate)
  {
    case 1: strcpy(str,"300"); break;
    case 2: strcpy(str,"600"); break;
    case 3: strcpy(str,"1200"); break;
    case 4: strcpy(str,"2400"); break;
    case 5: strcpy(str,"4800"); break;
    case 6: strcpy(str,"9600"); break;
    case 7: strcpy(str,"19200"); break;
    case 8: strcpy(str,"38400"); break;
    case 9: strcpy(str,"57600"); break;
    case 10: strcpy(str,"115200"); break;
    case 11:strcpy(str,"230400"); break;
    case 12:strcpy(str,"460800"); break;
    default: strcpy(str,"300"); break;
  }

  switch(serial.parity)
  {
    case 1: strcat(str,",N"); break;
    case 2: strcat(str,",O"); break;
    case 3: strcat(str,",E"); break;
    default: strcat(str,",N"); break;
  }

  switch(serial.databits)
  {
    case 1: strcat(str,",5"); break;
	case 2: strcat(str,",6"); break;
	case 3: strcat(str,",7"); break;
	case 4: strcat(str,",8"); break;
    default: strcat(str,",8"); break;
  }

  switch(serial.stopbits)
  {
    case 1: strcat(str,",1"); break;
    case 2: strcat(str,",2"); break;
    default: strcat(str,",1"); break;
  }

  switch(serial.flowcontrol)
  {
    case 1: strcat(str,",None"); break;
    case 2: strcat(str,",Software"); break;
    case 3: strcat(str,",Hardware"); break;
    default: strcat(str,",None"); break;
  }


  *str_=(byte *)str;           
  return (strlen((char *)str));
}




