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


/*------------------------------------------------------------------------
 Function:    ssi_spca()
 Description: SSI: Display whether the Serial Port Console access is 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_spca(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.serial_console)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_wsa()
 Description: SSI: Display whether the Webserver access is 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_wsa(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.webserver)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_ica()
 Description: SSI: Display whether the Internet Console Access is 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ica(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.internet_console)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_ipx()
 Description: SSI: Display whether the IP exclusivity is 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ipx(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(flag.security)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_login_pass()
 Description: SSI: Display Login Password
 Input:       String containing Password
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_login(byte **str_)
cmx_reentrant 
{
  remove_space((byte *)password.login);
  *str_=(byte *)password.login;
  return (strlen(password.login)); 
}


/*------------------------------------------------------------------------
 Function:    ssi_admin_pass()
 Description: SSI: Display Admin Password
 Input:       String containing Password
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_admin(byte **str_)
cmx_reentrant 
{
  remove_space((byte *)password.admin);
  *str_=(byte *)password.admin;
  return (strlen(password.admin)); 
}


/*------------------------------------------------------------------------
 Function:    ssi_serial_pass()
 Description: SSI: Display serial Password
 Input:       String containing Password
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_serial(byte **str_)
cmx_reentrant 
{
  remove_space((byte *)password.serial);
  *str_=(byte *)password.serial;
  return (strlen(password.serial)); 
}


/*------------------------------------------------------------------------
 Function:    ssi_ip1()
 Description: SSI: Display 1st IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip1(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[0]);
  
  return (temp);
}

/*------------------------------------------------------------------------
 Function:    ssi_ip2()
 Description: SSI: Display 2nd IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip2(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[1]);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function:    ssi_ip3()
 Description: SSI: Display 3rd IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip3(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[2]);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function:    ssi_ip4()
 Description: SSI: Display 4th IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip4(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[3]);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function:    ssi_ip5()
 Description: SSI: Display 5th IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip5(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[4]);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function:    ssi_ip6()
 Description: SSI: Display 6th IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip6(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[5]);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function:    ssi_ip7()
 Description: SSI: Display 7th IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip7(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[6]);
  
  return (temp);
}


/*------------------------------------------------------------------------
 Function:    ssi_ip8()
 Description: SSI: Display 8th IP address allowed to connect.
 Input:       String containing IP address
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_ip8(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
  
  str = cgi_buff;
  *str_ = cgi_buff;

  temp = get_IPAddr_func(str,exclusivity.ip[7]);
  
  return (temp);
}


