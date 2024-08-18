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
#define I_OPTS  4                      /* No of Options for Interface   */
#define MS_OPTS 2                      /* No of Options for RS485 M/S   */
#define R4_OPTS 2                      /* RS485 type */

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


/**************************************************************************************************
  Function:    flowcontrol_func()
  Description: Output the Stopbit Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 flowcontrol_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(F_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[F_OPTS+2][LEN_OPTIONS];
  char s[F_OPTS][LEN_SELECTED];
  char opt[F_OPTS][10]={"None","Software","Hardware"};
  int no_of_opts = 0;

  switch(iserver.model)
  {
    case EIT_W_2F:   no_of_opts = 3; break;
	case EIT_W_4:    no_of_opts = 2; break;
    case EIT_D_2F:   no_of_opts = 3; break;
    case EIT_D_4:    no_of_opts = 2; break;
    case EIT_PCB_2F: no_of_opts = 3; break;
    case EIT_PCB_4:  no_of_opts = 2; break;
    case EIT_PCB_T:  no_of_opts = 2; break;
    default:         no_of_opts = 2; break;
  }
  
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
  
  for(i = 1; i < (no_of_opts + 1); i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i,s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[no_of_opts + 1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[no_of_opts + 1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}



/**************************************************************************************************
  Function:    ssi_rs485_type()
  Description: Output the RS485 Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 ssi_rs485type(byte **str_)
cmx_reentrant 
{
  int show_rs485_type;
  int i;
  char tmp[(R4_OPTS * LEN_OPTIONS) + LEN_SELECTED + 150 ];
  char str[R4_OPTS+2][LEN_OPTIONS];
  char s[R4_OPTS][LEN_SELECTED];
  char opt[R4_OPTS][10]={"Half","Full"};
  
  switch(iserver.model)
  {
    case EIT_W_2F:   show_rs485_type = DISABLED; break;
	case EIT_W_4:    show_rs485_type = ENABLED;  break;
    case EIT_D_2F:   show_rs485_type = DISABLED; break;
    case EIT_D_4:    show_rs485_type = ENABLED;  break;
    case EIT_PCB_2F: show_rs485_type = DISABLED; break;
    case EIT_PCB_4:  show_rs485_type = ENABLED;  break;
    case EIT_PCB_T:  show_rs485_type = DISABLED; break;
    default:         show_rs485_type = DISABLED; break;
  }

  if(show_rs485_type == ENABLED)
  {
    memset(s,NULL,(R4_OPTS * LEN_SELECTED));       
  	
    switch(flag.rs485type)  /* Selected Parity = "selected", else " " */
    {
      case 1:  strcpy(s[0],SELECTED);  break;
      case 2:  strcpy(s[1],SELECTED);  break;
      default: strcpy(s[0],SELECTED);  break;
    }

    sprintf(str[0],"<tr>\r\n<td>RS485 Mode </td>\r\n <td align=\"right\"> <select name=\"R4\">\r\n");
	/*str[0]=<select name=\"S\">*/
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
  }
  else
  { 
    strcpy(tmp," ");
  }

  *str_ = (byte *)tmp; 
    
  return (strlen((char *)*str_));
}

/**************************************************************************************************
 Function:    ssi_rs485timer()
 Description: SSI: Display the rs485 timer
 Input:       None
 Output:      String containing the value
**************************************************************************************************/
word16 ssi_rs485timer(byte **str_)
cmx_reentrant 
{
  int show_rs485_timer;
  char str[200];

  switch(iserver.model)
  {
    case EIT_W_2F:   show_rs485_timer = DISABLED; break;
	case EIT_W_4:    show_rs485_timer = ENABLED;  break;
    case EIT_D_2F:   show_rs485_timer = DISABLED; break;
    case EIT_D_4:    show_rs485_timer = ENABLED;  break;
    case EIT_PCB_2F: show_rs485_timer = DISABLED; break;
    case EIT_PCB_4:  show_rs485_timer = ENABLED;  break;
    case EIT_PCB_T:  show_rs485_timer = DISABLED; break;
    default:         show_rs485_timer = DISABLED; break;
  }

  if(show_rs485_timer == ENABLED)
  {
    sprintf(str,"<tr>\r\n <td> 2-wire RS485 Timer </td>\r\n <td align=\"right\">\r\n\
      <input type=\"text\" name=\"rs485timer\" id=\"rs485timer\" value=\"%d\" size=\"4\"\
      maxlength=\"4\"> ms\r\n</td>\r\n</tr>",serial.rs485timer);
  }
  else
  {
    sprintf(str," ");
  }
     
  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
  Function:    ssi_rs485txrx()
  Description: Output the Interface Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 ssi_rs485txrx(byte **str_)
cmx_reentrant 
{
  int i;
  int show_rs485txrx;
  char tmp[(MS_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[MS_OPTS+2][LEN_OPTIONS];
  char s[MS_OPTS][LEN_SELECTED];
  char opt[MS_OPTS][10] = {"Master","Slave"};
  
  switch(iserver.model)
  {
    case EIT_W_2F:   show_rs485txrx = DISABLED; break;
	case EIT_W_4:    show_rs485txrx = ENABLED;  break;
    case EIT_D_2F:   show_rs485txrx = DISABLED; break;
    case EIT_D_4:    show_rs485txrx = ENABLED;  break;
    case EIT_PCB_2F: show_rs485txrx = DISABLED; break;
    case EIT_PCB_4:  show_rs485txrx = ENABLED;  break;
    case EIT_PCB_T:  show_rs485txrx = DISABLED; break;
    default:         show_rs485txrx = DISABLED; break;
  }

  if(show_rs485txrx == ENABLED)
  {
    memset(s,NULL,(MS_OPTS * LEN_SELECTED));       
  	
    switch(flag.rs485txrx)  /* Selected Parity = "selected", else " " */
    {
      case 1:  strcpy(s[0],SELECTED);  break;
      case 2:  strcpy(s[1],SELECTED);  break;
      default: strcpy(s[0],SELECTED);  break;
    }

    sprintf(str[0],"<tr>\r\n<td> 2-wire RS485 Type </td>\r\n <td align=\"right\"><select name=\"rs485txrx\">\r\n");
	/*str[0]=<select name=\"S\">*/
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
  }
  else
  {
    strcpy(tmp," ");
  }
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
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
