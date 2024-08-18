/*------------------------------------------------------------------------ 
					ssi_network.c

Description:	Return SSIs for network.htm page
				1. DHCP Flag Enabled/Disabled
                2. Autonegotiation Enabled/Disabled
				3. 10 Mbps Enabled / Disabled
				4. 100 Mbps Enabled / Disabled
				5. Full Duplex Enabled/Disabled
                6. Half Duplex Enabled/Disabled

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

#define P_OPTS 3                       /* No of Options for Protocol    */
#define LEN_SELECTED 25                /* Length of selected="selected" */
#define LEN_OPTIONS  80                /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 


/*------------------------------------------------------------------------
 Function:    dhcpflag_func()
 Description: SSI: Display the DHCP Flag
 Input:       None
 Output:      String Containing the DHCP Flag
------------------------------------------------------------------------*/
word16 dhcpflag_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.dhcp)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
  Function:    baud_func()
  Description: Output the Baudrate Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_protocol(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(P_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[P_OPTS+2][LEN_OPTIONS];
  char s[P_OPTS][LEN_SELECTED];
  char opt[P_OPTS][10]={"TCP","UDP","ModbusTCP"};
  
  memset(s,NULL,(P_OPTS * LEN_SELECTED));       
  	
  switch(terminal_server.protocol)/* Selected Protocol = "selected", else " " */
  {
    case TCP_PROTOCOL:    strcpy(s[0],SELECTED);  break;
    case UDP_PROTOCOL:    strcpy(s[1],SELECTED);  break;
    case MODBUS_PROTOCOL: strcpy(s[2],SELECTED);  break;
    default:              strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"protocol\">\r\n");/*str[0]=<select name=\"P\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(P_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=%d %s> %s </option>\r\n",\
      i,s[i-1],opt[i-1]);   
        /*    str[i] = <option value=i selected="selected"> 8 </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[P_OPTS+1],"</select>\r\n");            /* str[5]=</select */
  strcat(tmp,str[P_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}

/*------------------------------------------------------------------------
 Function:    autonegotiate_func()
 Description: SSI: Display the Autonegotiate Flag
 Input:       None
 Output:      String Containing the Autonegotiate Flag
------------------------------------------------------------------------*/
word16 autonegotiate_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.autonegotiation)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    s10_func()
 Description: SSI: Display status of 10 Mbps speed
 Input:       None
 Output:      Checked: if Speed = 10
              " " :    if speed = 100
------------------------------------------------------------------------*/
word16 s10_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.speed)
  {
    case 0: strcpy(str,"checked"); break;
    case 1: strcpy(str," "); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    s100_func()
 Description: SSI: Display status of 100 Mbps speed
 Input:       None
 Output:      " ":        if Speed = 10
              "checked" : if speed = 100
------------------------------------------------------------------------*/
word16 s100_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.speed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}



/*------------------------------------------------------------------------
 Function:    fduplex_func()
 Description: SSI: Display Status of Full Duplex
 Input:       None
 Output:      Checked: if Full Duplex
              " " :    if Half Duplex
------------------------------------------------------------------------*/
word16 fduplex_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.duplex)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    hduplex_func()
 Description: SSI: Display Status of Half Duplex
 Input:       None
 Output:      Checked: if Half Duplex
              " " :    if Full Duplex
------------------------------------------------------------------------*/
word16 hduplex_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.duplex)
  {
    case 0: strcpy(str,"checked"); break;
    case 1: strcpy(str," "); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

//wendy
/*------------------------------------------------------------------------
 Function:    WebPort_func()
 Description: SSI: Display the HTTP Port
 Input:       None
 Output:      Port Number
------------------------------------------------------------------------*/
word16 WebPort_func(byte **str_)
cmx_reentrant 
{
  int n;
  
  n=sprintf((char *)temp_buff,"%d",flag.HTTP_PORT);
  *str_=temp_buff;

  return ((word16)n);
 
}
