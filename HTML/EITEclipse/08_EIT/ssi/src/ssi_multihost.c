/*------------------------------------------------------------------------ 
					SSI_SERIAL.C

Description:	Return SSIs for serial.htm page
				1. 
                2. 
				3. 
				4. 
				5. 
                6. Half Duplex Enabled/Disabled

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/
#include "micronet.h"
#include "np_includes.h"

/*------------------------------------------------------------------------
  Function:    ssi_connection_type()
  Description: Output the connection type on Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_connection_type(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(CT_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[CT_OPTS+2][LEN_OPTIONS];
  char s[CT_OPTS][LEN_SELECTED];
  char opt[CT_OPTS][13]={"Disable","Simultaneous","Sequential"};
  
  memset(s,NULL,(CT_OPTS * LEN_SELECTED));
  	
  switch(multihost.connection_type)
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
	default: strcpy(s[0],SELECTED);  break;
  }

  //sprintf(str[0],"<select name=\"connection_type\">\r\n");
  sprintf(str[0],"\r\n"); 		  //wendy

  strcpy(tmp,str[0]);
  
  for(i=1;i<(CT_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i, s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[CT_OPTS+1],"</select>\r\n");          /* str[4]=</select */
  strcat(tmp,str[CT_OPTS + 1]);

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",tmp);
#endif
  
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    ssi_retry_counter()
  Description: Output the Retry Counter
  Input:       Pointer to the String that will contain the value
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_retry_counter(byte **str_)
cmx_reentrant
{
  int n;

  n = sprintf((char *)temp_buff,"%d",multihost.retry_counter);
  *str_ = temp_buff;

  return ((word16)n);
}


/*------------------------------------------------------------------------
  Function:    ssi_retry_timeout()
  Description: Output the Retry Timeout
  Input:       Pointer to the String that will contain the value
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_retry_timeout(byte **str_)
cmx_reentrant
{
  int n;

  n = sprintf((char *)temp_buff,"%d",multihost.retry_timeout);
  *str_ = temp_buff;

  return ((word16)n);
}


/*------------------------------------------------------------------------
 Function:    ssi_multihost_ip_address()
 Description: Show the Multihost IP Address
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 ssi_mipaddr(byte **str_)
cmx_reentrant 
{
  int i;
  byte read_buff[300];
  byte *temp;
  
  temp = cgi_buff;
  strcpy((char *)read_buff," ");
  
  for(i = 0; i < 12; i++)
  {
    if(i != 0)
      strcat((char *)read_buff,",");        
    strcat((char *)read_buff,"\"");
    get_IPAddr_func(temp,(byte *)multihost.rip[i]);
	strcat((char *)read_buff,(char *)temp);
    strcat((char *)read_buff,"\"");
  }
  
  *str_ = (byte *)read_buff;

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",read_buff);
#endif

  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
 Function:    ssi_multihost_ip_address()
 Description: Show the Multihost IP Address
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 ssi_multi_port(byte **str_)
cmx_reentrant 
{
  int i;
  byte read_buff[300];
  char temp[10];

  strcpy((char *)read_buff," ");
  
  for(i = 0; i < 12; i++)
  {
    memset(temp,NULL,10);
	if(i != 0)
      strcat((char *)read_buff,",");
    strcat((char *)read_buff,"\"");
    sprintf(temp,"%d",(char *)multihost.rp[i]);
	strcat((char *)read_buff,temp);
	strcat((char *)read_buff,"\"");
  }
  
  *str_ = (byte *)read_buff;

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",read_buff);
#endif
  
  return (strlen((char *)*str_));
}



