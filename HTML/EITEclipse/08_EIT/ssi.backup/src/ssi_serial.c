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

#define B       12                     /* No of Options for Baud Rate   */
#define D_OPTS  4                      /* No of Options for Databits    */
#define P_OPTS  3                      /* No of Options for Parity      */
#define S_OPTS  2                      /* No of Options for Stopbit     */
#define F_OPTS  3                      /* No of Options for FlowCtrl    */
#define N_OPTS  6                      /* No of Options for No of Conns */
#define I_OPTS  4                      /* No of Options for Interface   */
#define CO_OPTS 3                      /* Connection Control options    */
#define RCO_OPTS 4  				   /* Tunneling Connection Control  */
#define MS_OPTS 2                      /* No of Options for RS485 M/S   */
#define DC_OPTS 4                      /* Disconnect by                 */
#define CT_OPTS 3                      /* Connection type multihost     */
#define R4_OPTS 2                      /* RS485 type */
#define LEN_SELECTED 25                /* Length of selected="selected" */
#define LEN_OPTIONS  80                /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 

/*------------------------------------------------------------------------
  Function:    baud_func()
  Description: Output the Baudrate Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 baud_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[750];
  char str[B+2][50];
  char b[B][10];
  char opt[B][7]={"300","600","1200","2400","4800","9600","19200",\
    "38400","57600","115200","230400","460800"};
  
  memset(b,NULL,(B * 10));       
  	
  switch(serial.baudrate) /* Selected baudrate = "selected", else " "   */
  {
    case 1: strcpy(b[0],"selected");  break;
    case 2: strcpy(b[1],"selected");  break;
    case 3: strcpy(b[2],"selected");  break;
    case 4: strcpy(b[3],"selected");  break;
    case 5: strcpy(b[4],"selected");  break;
    case 6: strcpy(b[5],"selected");  break;
    case 7: strcpy(b[6],"selected");  break;
    case 8: strcpy(b[7],"selected");  break;
    case 9: strcpy(b[8],"selected");  break;
    case 10: strcpy(b[9],"selected");  break;
    case 11: strcpy(b[10],"selected");break;
    case 12: strcpy(b[11],"selected");break;
    default: strcpy(b[0],"selected"); break;
  }

  strcpy(str[0],"<select name=\"B\">");     /*str[0]=<select name=\"B\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(B+1);i++)
  {
    sprintf(str[i],"<option value=%d %s=\"%s\"> %s </option>\r\n",\
      i, b[i-1],b[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> 9600 </option>*/
    strcat(tmp,str[i]);
  }

  strcpy(str[B+1],"</select>");                     /* str[14]=</select */
  strcat(tmp,str[B+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    databits_func()
  Description: Output the Databit Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 databits_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(D_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[D_OPTS+2][LEN_OPTIONS];
  char s[D_OPTS][LEN_SELECTED];
  char opt[D_OPTS][10]={"5","6","7","8"};
  
  memset(s,NULL,(D_OPTS * LEN_SELECTED));       
  	
  switch(serial.databits) /* Selected Databits = "selected", else " "   */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
    case 4:  strcpy(s[3],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"D\">\r\n");/*str[0]=<select name=\"P\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(D_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=%d %s> %s </option>\r\n",\
      i, s[i-1],opt[i-1]);   
        /*    str[i] = <option value=i selected="selected"> 8 </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[D_OPTS+1],"</select>\r\n");            /* str[5]=</select */
  strcat(tmp,str[D_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    parity_func()
  Description: Output the Parity Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 parity_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(P_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[P_OPTS+2][LEN_OPTIONS];
  char s[P_OPTS][LEN_SELECTED];
  char opt[P_OPTS][10]={"None","Even","Odd"};
  
  memset(s,NULL,(P_OPTS * LEN_SELECTED));       
  	
  switch(serial.parity)     /* Selected Parity = "selected", else " "   */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"P\">\r\n");/*str[0]=<select name=\"P\">*/
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
  Function:    stopbit_func()
  Description: Output the Stopbit Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 stopbit_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(S_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[S_OPTS+2][LEN_OPTIONS];
  char s[S_OPTS][LEN_SELECTED];
  char opt[S_OPTS][10]={"1","2"};
  
  memset(s,NULL,(S_OPTS * LEN_SELECTED));       
  	
  switch(serial.stopbits)     /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"S\">\r\n");/*str[0]=<select name=\"S\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(S_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i,s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[S_OPTS+1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[S_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    flowcontrol_func()
  Description: Output the Stopbit Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 flowcontrol_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(F_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[F_OPTS+2][LEN_OPTIONS];
  char s[F_OPTS][LEN_SELECTED];
  char opt[F_OPTS][10]={"None","Hardware","Software"};
  
  memset(s,NULL,(F_OPTS * LEN_SELECTED));       
  	
  switch(serial.flowcontrol)  /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"F\">\r\n");/*str[0]=<select name=\"S\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(F_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i,s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[F_OPTS+1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[F_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}



/*------------------------------------------------------------------------
  Function:    ssi_rs485_type()
  Description: Output the RS485 Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_rs485type(byte **str_)
cmx_reentrant 
{
  
#if(MODEL_TYPE == EIT_RS485)
  int i;
  char tmp[(R4_OPTS * LEN_OPTIONS) + LEN_SELECTED + 150 ];
  char str[R4_OPTS+2][LEN_OPTIONS];
  char s[R4_OPTS][LEN_SELECTED];
  char opt[R4_OPTS][10]={"Half","Full"};
  
  memset(s,NULL,(R4_OPTS * LEN_SELECTED));       
  	
  switch(flag.rs485type)  /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<tr>\r\n<td>RS485 Mode </td>\r\n <td align=\"right\"> <select name=\"R4\">\r\n");/*str[0]=<select name=\"S\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(R4_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i,s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[R4_OPTS+1],"</select>\r\n</td>\r\n</tr>");            /* str[4]=</select */
  strcat(tmp,str[R4_OPTS+1]);
  
#else
 
  char tmp[2];
  strcpy(tmp," ");
#endif
  
  *str_ = (byte *)tmp; 
  return (strlen((char *)*str_));
}



/*------------------------------------------------------------------------
 Function:    ssi_rs485timer()
 Description: SSI: Display the rs485 timer
 Input:       None
 Output:      String containing the value
------------------------------------------------------------------------*/
word16 ssi_rs485timer(byte **str_)
cmx_reentrant 
{

#if(MODEL_TYPE == EIT_RS485)
  char str[200];

  sprintf(str,"<tr>\r\n <td> 2-wire RS485 Timer </td>\r\n <td align=\"right\">\r\n\
  <input type=\"text\" name=\"rs485timer\" id=\"rs485timer\" value=\"%d\" size=\"4\"\
   maxlength=\"4\"> ms\r\n</td>\r\n</tr>",serial.rs485timer);
#else
  char str[2];

  sprintf(str," ");
#endif
     
  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
  Function:    ssi_rs485txrx()
  Description: Output the Interface Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_rs485txrx(byte **str_)
cmx_reentrant 
{
  
#if(MODEL_TYPE == EIT_RS485)
  int i;
  char tmp[(MS_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[MS_OPTS+2][LEN_OPTIONS];
  char s[MS_OPTS][LEN_SELECTED];
  char opt[MS_OPTS][10] = {"Master","Slave"};
  
  memset(s,NULL,(MS_OPTS * LEN_SELECTED));       
  	
  switch(flag.rs485txrx)  /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<tr>\r\n<td> 2-wire RS485 Type </td>\r\n <td align=\"right\"><select name=\"rs485txrx\">\r\n");/*str[0]=<select name=\"S\">*/
  strcpy(tmp,str[0]);
  
  for(i = 1; i < (MS_OPTS + 1); i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i, s[i-1], opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[MS_OPTS+1],"</select>\r\n</td>\r\n</tr>");            /* str[4]=</select */
  strcat(tmp,str[MS_OPTS+1]);

#else
  char tmp[2];
  strcpy(tmp," ");

#endif  
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    no_of_conns_func()
  Description: Output the No of Connections Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 no_of_conns_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(N_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[N_OPTS+2][LEN_OPTIONS];
  char s[N_OPTS][LEN_SELECTED];
  char opt[N_OPTS][10]={"0","1","2","3","4","5"};
  
  memset(s,NULL,(N_OPTS * LEN_SELECTED));       
  	
  switch(terminal_server.no_connections)/* Parity = "selected",else " " */
  {
    case 0:  strcpy(s[0],SELECTED);  break;
    case 1:  strcpy(s[1],SELECTED);  break;
    case 2:  strcpy(s[2],SELECTED);  break;
    case 3:  strcpy(s[3],SELECTED);  break;
    case 4:  strcpy(s[4],SELECTED);  break;
    case 5:  strcpy(s[5],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"N\">\r\n");/*str[0]=<select name=\"S\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(N_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i, s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[N_OPTS+1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[N_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    lp_func()
  Description: Output the Terminal Server's Local Port
  Input:       Pointer to the String that will contain the Local port
  Output:      Length of the Port Number
------------------------------------------------------------------------*/
word16 lp_func(byte **str_)
cmx_reentrant 
{
  int n;

  n=sprintf((char *)temp_buff,"%d",terminal_server.local_port);
  *str_=temp_buff;

  return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_connect_on()
  Description: Output the Connect on Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_connect_on(byte **str_)
cmx_reentrant 
{

#if(MODEL_TYPE == EIT_RS232_FULL)
  int i;
  char tmp[(CO_OPTS * LEN_OPTIONS) + LEN_SELECTED + 150 ];
  char str[CO_OPTS+2][LEN_OPTIONS*2];
  char s[CO_OPTS][LEN_SELECTED*2];
  char opt[CO_OPTS][20]={"Always","Rise DTR","Drop DTR"};
  
  memset(s,NULL,(CO_OPTS * LEN_SELECTED));       
  	
  switch(terminal_server.connect_on)    /* Parity = "selected",else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<tr>\r\n<td> Connect on </td>\r\n <td align=\"right\"> \r\n<select name=\"connect_on\">\r\n");
  strcpy(tmp,str[0]);
  
  for(i=1;i<(CO_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i, s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[CO_OPTS+1],"</select>\r\n</td> </tr>");           /* str[4]=</select */
  strcat(tmp,str[CO_OPTS+1]);

#else
  char tmp[2];
  strcpy(tmp," ");

#endif

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",tmp);
#endif
  
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}



/*------------------------------------------------------------------------
  Function:    rmt_ip_func()
  Description: Output the Remote IP Of Terminal Server
  Input:       Pointer to the String that will contain the  IP Address
  Output:      Length of the IP Address
------------------------------------------------------------------------*/
word16 rmt_ip_func(byte **str_)
cmx_reentrant 
{
  word16 temp;
  byte *str;
	
  str=cgi_buff;
  *str_=cgi_buff;
	
  temp = get_IPAddr_func(str,(byte *)tunneling.remote_ip);

  return (temp);
}


/*------------------------------------------------------------------------
  Function:    rmt_port_func()
  Description: Output the Remote Port Of Terminal Server
  Input:       Pointer to the String that will contain the Remote Port
  Output:      Length of the Remote Port
------------------------------------------------------------------------*/
word16 rmt_port_func(byte **str_)
cmx_reentrant 
{
	int n;

	n=sprintf((char *)temp_buff,"%d",tunneling.remote_port);
	*str_=temp_buff;

	return ((word16)n);
}


/*------------------------------------------------------------------------
  Function:    ssi_retry_timer()
  Description: Output the Retry Timer
  Input:       Pointer to the String that will contain the Time
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_retry_timer(byte **str_)
cmx_reentrant
{
  int n;

  memset(temp_buff,NULL,sizeof(temp_buff));

  n = sprintf( (char *)temp_buff, "%d", tunneling.retry_timer );
  *str_ = temp_buff;

#if ((DEBUG_LEVEL == HIGH))
  printf("tunneling.retry_timer=%d\n\r",tunneling.retry_timer);
#endif

  return ((word16)n);
}


/*------------------------------------------------------------------------
  Function:    ssi_remote_connect_on()
  Description: Output the Remote Connect on Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_remote_connect_on(byte **str_)
cmx_reentrant 
{
#if(MODEL_TYPE == EIT_RS232_FULL)
  int i;
  char tmp[(RCO_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[RCO_OPTS+2][LEN_OPTIONS];
  char s[RCO_OPTS][LEN_SELECTED];
  char opt[RCO_OPTS][15]={"Always","Any Character","DSR Risen",\
    "DSR Dropped"};
  
  memset(s,NULL,(RCO_OPTS * LEN_SELECTED));       
  	
  switch(tunneling.connect_on)          /* Parity = "selected",else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
	case 4:  strcpy(s[3],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<tr>\r\n<td> Connect on </td>\r\n <td align=\"right\">\r\n<select name=\"remote_connect_on\">\r\n");
  strcpy(tmp,str[0]);
  
  for(i=1;i<(RCO_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i, s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[RCO_OPTS+1],"</select>\r\n</td>\r\n</tr>");          /* str[4]=</select */
  strcat(tmp,str[RCO_OPTS+1]);

#else
  char tmp[2];
  strcpy(tmp," ");

#endif

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",tmp);
#endif
  
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}

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


/*------------------------------------------------------------------------
 Function:    dhcpflag_func()
 Description: SSI: Display whether the Remote Tunneling is enabled or not
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 rmt_en_func(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(tunneling.remote_enable)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str,"checked"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
  Function:    ssi_disconnect_by()
  Description: Output the disconnect by on Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_disconnect_by(byte **str_)
cmx_reentrant 
{

#if(MODEL_TYPE == EIT_RS232_FULL)
  int i;
  char tmp[(DC_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[DC_OPTS+2][LEN_OPTIONS];
  char s[DC_OPTS][LEN_SELECTED];
  char opt[DC_OPTS][18]={"None","DSR Risen","DTR Dropped",\
    "Hard Disconnect"};
  
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

#else
  char tmp[2];
  strcpy(tmp," ");

#endif


#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",tmp);
#endif
  
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


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

  sprintf(str[0],"<select name=\"connection_type\">\r\n");
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



