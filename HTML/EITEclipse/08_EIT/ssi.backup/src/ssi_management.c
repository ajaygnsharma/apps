/*------------------------------------------------------------------------ 
					SSI_SECURITY.C

Description:	Return SSIs for security.htm page
				1. Serial Port Console Access Enabled/Disabled
                2. Webserver Enabled/Disabled
				3. 
				4. 
				5. 
                6. 

Author:			Ajay Sharma

Rev:			0

History:
------------------------------------------------------------------------*/
#include "micronet.h"
#include "np_includes.h"

#define LEN_CONTACT     11
#define LEN_LOCATION    11

/*------------------------------------------------------------------------
 Function:    ssi_snmp()
 Description: SSI: Display whether the SNMP is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_snmp_en(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  //flag.smtp = TRUE;  ///////DO we need to change this?
  switch(flag.snmp)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_contact()
 Description: SSI: Display SNMP Contact Information
 Input:       String containing Information
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_contact(byte **str_)
cmx_reentrant 
{
  
  char str[LEN_CONTACT + 1];
  
  mn_getFlashVariable((byte *)str,sysContact0,SNMP_CONTACT_LEN);
  str[SNMP_CONTACT_LEN]='\0';
  
  *str_=(byte *)str;
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_location()
 Description: SSI: Display SNMP Location Information
 Input:       String containing Information
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_location(byte **str_)
cmx_reentrant 
{
  
  char str[LEN_LOCATION + 1];
  
  mn_getFlashVariable((byte *)str,sysLocation0,LEN_LOCATION);
  str[LEN_LOCATION]='\0';
  
  *str_=(byte *)str;
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_trap()
 Description: SSI: Display whether the Trap is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_trap_en(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  //flag.smtp = TRUE;  ///////DO we need to change this?
  switch(flag.trap_en)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_smtp()
 Description: SSI: Display whether the Email is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_smtp(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  //flag.smtp = TRUE;  ///////DO we need to change this?
  switch(flag.smtp)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
  Function:    ssi_trap_ip()
  Description: SSI: Output the SNMP TRAP Server IP 
  Input:       Pointer to the String that will contain the  IP Address
  Output:      Length of the IP Address
------------------------------------------------------------------------*/
word16 ssi_trap_ip(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
	
  str=cgi_buff;
  *str_=cgi_buff;
	
  //nm.trap_ip[0] = 192;
  //nm.trap_ip[1] = 168;
  //nm.trap_ip[2] = 1;
  //nm.trap_ip[3] = 200;

  temp=get_IPAddr_func(str,(byte *)snmp.trap_ip);

  return (temp);
}


/*------------------------------------------------------------------------
  Function:    ssi_smtpip()
  Description: SSI: Output the SMTP Server IP 
  Input:       Pointer to the String that will contain the  IP Address
  Output:      Length of the IP Address
------------------------------------------------------------------------*/
word16 ssi_smtpip(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
	
  str=cgi_buff;
  *str_=cgi_buff;
	
//  smtp.server_ip[0] = 192;
//  smtp.server_ip[1] = 168;
//  smtp.server_ip[2] = 1;
//  smtp.server_ip[3] = 105;

  temp=get_IPAddr_func(str,(byte *)smtp.server_ip);

  return (temp);
}

/*------------------------------------------------------------------------
  Function:    ssi_community()
  Description: SSI: Output the SNMP Community
  Input:       Pointer to the String that will contain the Community String
  Output:      Length of the String
------------------------------------------------------------------------*/
word16 ssi_community(byte **str_)
cmx_reentrant 
{
  word16 temp;

  *str_ = (byte *)snmp.community;
	
  temp = strlen(snmp.community);

  return (temp);
}

/*------------------------------------------------------------------------
  Function:    ssi_from()
  Description: SSI: Output the Senders Email Address
  Input:       Pointer to the String that will contain the  Senders email
  Output:      Length of the Email Address
------------------------------------------------------------------------*/
word16 ssi_from(byte **str_)
cmx_reentrant 
{
  //strcpy((char *)smtp.from,"asharma@newportus.com");
  *str_=(byte *)smtp.from;           
  return (strlen((char *)smtp.from));
}


/*------------------------------------------------------------------------
  Function:    ssi_sub()
  Description: SSI: Output the Subject of the email
  Input:       Pointer to the String that will contain the Subject
  Output:      Length of the Subject
------------------------------------------------------------------------*/
word16 ssi_sub(byte **str_)
cmx_reentrant 
{
  //strcpy((char *)smtp.subject,"none");
  *str_=(byte *)smtp.subject;
  return (strlen((char *)smtp.subject));
}
