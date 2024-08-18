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
  Function:    end_char_func()
  Description: Output the end character Of Terminal Server
  Input:       Pointer to the String that will contain the Remote Port
  Output:      Length of the end character
------------------------------------------------------------------------*/
word16 end_char_func(byte **str_)
cmx_reentrant 
{
  int n;
  
  n=sprintf((char *)temp_buff,"%X",serial.endchar);/* O/P HEX as String */ 
  *str_=temp_buff;

  return ((word16)n);
}

/*------------------------------------------------------------------------
 Function:    fwd_endchar_func()
 Description: SSI: Display whether the FwdEndchar is enabled or not
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 fwd_endchar_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(serial.fwdendchar)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}



/*------------------------------------------------------------------------
  Function:    show_timeout_func()
  Description: Output the Buffering Time of Ethernet-Serial Bridge
  Input:       Pointer to the String that will contain the Time
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 buff_time_func(byte **str)
cmx_reentrant 
{
	int n;

	n=sprintf((char *)temp_buff,"%d",serial.buffering_time);
	*str=temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_pkt_len()
  Description: Output the Packet Length
  Input:       Pointer to the String that will contain the Length
  Output:      String containing Length
------------------------------------------------------------------------*/
word16 ssi_pkt_len(byte **str)
cmx_reentrant 
{
	int n;

	n = sprintf((char *)temp_buff,"%d",serial.pkt_len);
	*str=temp_buff;

	return ((word16)n);
}


/*------------------------------------------------------------------------
  Function:    itimeout_func()
  Description: Output the Inactivity Timeout
  Input:       Pointer to the String that will contain the Time
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 itimeout_func(byte **str_)
cmx_reentrant
{
	int n;

	n=sprintf((char *)temp_buff,"%d",terminal_server.timeout);
	*str_=temp_buff;

	return ((word16)n);
}

/**************************************************************************************************
  Function:    ssi_disconnect_by()
  Description: Output the disconnect by on Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 ssi_disconnect_by(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(DC_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[DC_OPTS+2][LEN_OPTIONS];
  char s[DC_OPTS][LEN_SELECTED];
  char opt[DC_OPTS][18]={"None","DSR Risen","DTR Dropped",\
    "Hard Disconnect"};
  int show_disconnect_by = 0;
  

  switch(iserver.model)
  {
    case EIT_W_2F:   show_disconnect_by = ENABLED; break;
	case EIT_W_4:    show_disconnect_by = DISABLED;  break;
    case EIT_D_2F:   show_disconnect_by = ENABLED; break;
    case EIT_D_4:    show_disconnect_by = DISABLED;  break;
    case EIT_PCB_2F: show_disconnect_by = ENABLED; break;
    case EIT_PCB_4:  show_disconnect_by = DISABLED;  break;
    case EIT_PCB_T:  show_disconnect_by = DISABLED; break;
    default:         show_disconnect_by = DISABLED; break;
  }

  if(show_disconnect_by == ENABLED)
  {
    memset(s,NULL,(DC_OPTS * LEN_SELECTED));       
  	
    switch(terminal_server.disconnect_by)
    {
      case 1:  strcpy(s[0],SELECTED);  break;
      case 2:  strcpy(s[1],SELECTED);  break;
      case 3:  strcpy(s[2],SELECTED);  break;
  	  case 4:  strcpy(s[3],SELECTED);  break;
      default: strcpy(s[0],SELECTED);  break;
    }

    sprintf(str[0],"<tr>\r\n <td> Disconnect by </td> \r\n <td align=\"right\"> <select name=\"disconnect_by\">\r\n");
    strcpy(tmp,str[0]);
  
    for(i=1;i<(DC_OPTS+1);i++)
    {
      sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
        i, s[i-1],opt[i-1]);   
        /*    str[i] = <option value=i selected="selected"> One </option> */
      strcat(tmp,str[i]);
    }

    sprintf(str[DC_OPTS+1],"</td>\r\n</tr>\r\n</select>\r\n");          /* str[4]=</select */
    strcat(tmp,str[DC_OPTS + 1]);
  }
  else
  {
    strcpy(tmp," ");
  }

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",tmp);
#endif
  
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}
