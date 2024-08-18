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

#define IP_OPTS         6
#define PC_OPTS         2
#define LEN_SELECTED    25             /* Length of selected="selected" */
#define LEN_OPTIONS     80             /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 

/*------------------------------------------------------------------------
 Function:    ssi_a2_e_chk()
 Description: SSI: Display whether the Alarm1 checks
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_chk(byte **str_)
cmx_reentrant 
{
  char str[400];
  char i = 0;
  char a2[10]; 
  
  a2[0] = alarm[1].enable;
  a2[1] = alarm[1].condition[0].power_reset;
  a2[2] = alarm[1].condition[0].ip_changed;
  a2[3] = alarm[1].condition[0].isvr_accessed;
  a2[4] = alarm[1].condition[0].chars;
  a2[5] = alarm[1].condition[0].isvr_condition;
  a2[6] = alarm[1].condition[0].isvr_disconnect;
  a2[7] = alarm[1].enable;
  a2[8] = alarm[1].enable;
  a2[9] = alarm[1].enable;

  strcpy((char *)str," ");
  for(i = 0; i < 10; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	switch(a2[i])
    {
      case 0: strcat((char *)str,"\" \""); break;
	  case 1: strcat((char *)str,"\"checked\""); break;
	  default:strcat((char *)str,"\" \""); break;
    }
  }
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_t_chk()
 Description: SSI: Display whether the Alarm1 checks
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_chk(byte **str_)
cmx_reentrant 
{
  char str[400];
  char i = 0;
  char a2[10]; 
  
  a2[0] = alarm[1].enable;
  a2[1] = alarm[1].condition[1].power_reset;
  a2[2] = alarm[1].condition[1].ip_changed;
  a2[3] = alarm[1].condition[1].isvr_accessed;
  a2[4] = alarm[1].condition[1].chars;
  a2[5] = alarm[1].condition[1].isvr_condition;
  a2[6] = alarm[1].condition[1].isvr_disconnect;
  a2[7] = alarm[1].enable;
  a2[8] = alarm[1].enable;
  a2[9] = alarm[1].enable;

  strcpy((char *)str," ");
  for(i = 0; i < 10; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	switch(a2[i])
    {
      case 0: strcat((char *)str,"\" \""); break;
	  case 1: strcat((char *)str,"\"checked\""); break;
	  default:strcat((char *)str,"\" \""); break;
    }
  }
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/**************************************************************************************************
  Function:    ssi_a2_ic_evt()
  Description: Output the input pin selection menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 ssi_a2_ic_evt(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp1[400];
  char tmp2[400];
  char tmp[801];

  char str[LEN_OPTIONS];
  char s[8][LEN_SELECTED];
  char opt[8][5] = {"PIN1", "PIN2", "PIN3", "PIN4", "PIN5", "PIN6", "PIN7", "PIN8"};
  int no_of_io = 0;
  char opt_dir[2][6] = {"HIGH","LOW"};

  switch(iserver.model)
  {
    case EIT_W_2F:   no_of_io = 1; break;
	case EIT_W_4:    no_of_io = 4; break;
    case EIT_D_2F:   no_of_io = 0; break;
    case EIT_D_4:    no_of_io = 2; break;
    case EIT_PCB_2F: no_of_io = 3; break;
    case EIT_PCB_4:  no_of_io = 6; break;
    case EIT_PCB_T:  no_of_io = 8; break;
    default:    no_of_io = 0; break;
  }
  
  if(no_of_io > 0)
  {
	/* Input Pin Menu */
	memset(str,NULL,sizeof(str));       
  	memset(s,NULL,sizeof(s));       
  	sprintf(str,"Input Pin <select name=a2_ip_pin  id=a2_ip_pin> ");
	strcpy(tmp1,str);

    switch(alarm[1].input_pin)  /* Selected Parity = "selected", else " " */
    {
      case 1:  strcpy(s[0],"selected=\\\"selected\\\"");  break;
      case 2:  strcpy(s[1],"selected=\\\"selected\\\"");  break;
      case 3:  strcpy(s[2],"selected=\\\"selected\\\"");  break;
      case 4:  strcpy(s[3],"selected=\\\"selected\\\"");  break;
	  case 5:  strcpy(s[4],"selected=\\\"selected\\\"");  break;
      case 6:  strcpy(s[5],"selected=\\\"selected\\\"");  break;
      case 7:  strcpy(s[6],"selected=\\\"selected\\\"");  break;
      case 8:  strcpy(s[7],"selected=\\\"selected\\\"");  break;
	  default: strcpy(s[0],"selected=\\\"selected\\\"");  break;
    }

    for(i = 1; i < (no_of_io + 1); i++)
    {
      if(gpio.direction[i-1] == 1)
	  {
	    memset(str,NULL,LEN_OPTIONS);
		sprintf(str,"<option value=%d  %s>  %s </option> ",\
        (i),s[i-1],opt[i-1]);   
        /*    str[i] = <option value=i selected="selected"> One </option> */
        strcat(tmp1,str);
	  }
    }
	
	memset(str,NULL,LEN_OPTIONS);
	sprintf(str,"</select>");            /* str[4]=</select */  //wendy
    strcat(tmp1,str);

	/* Input Condition Menu */
	memset(str,NULL,sizeof(str));       
  	memset(s,NULL,sizeof(s));       
  	sprintf(str," Input Condition <select name=a2_pin_cdn  id=a2_pin_cdn> ");
	strcpy(tmp2,str);

    switch(alarm[1].pin_condition)  /* Selected Parity = "selected", else " " */
    {
      case 1:  strcpy(s[0],"selected=\\\"selected\\\"");  break;
      case 2:  strcpy(s[1],"selected=\\\"selected\\\"");  break;
    }

    for(i = 1; i < (2 + 1); i++)
    {
      if(gpio.direction[i-1] == 1)
	  {
	    memset(str,NULL,LEN_OPTIONS);
		sprintf(str,"<option value=%d  %s>  %s </option> ",\
        (i),s[i-1],opt_dir[i-1]);   
        /*    str[i] = <option value=i selected="selected"> One </option> */
        strcat(tmp2,str);
	  }
    }
	
	memset(str,NULL,LEN_OPTIONS);
	sprintf(str,"</select>");            /* str[4]=</select */  //wendy
    strcat(tmp2,str);

	memset(tmp,NULL,sizeof(tmp));
	strcpy(tmp,tmp1);
	strcat(tmp,tmp2);
  }
  else
  {
    strcpy(tmp," ");
  }
  
  //printf("tmp=%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_sic_evt()
 Description: SSI: Display the serial port disconnected
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_sic_evt(byte **str_)
cmx_reentrant 
{
  char sic_evt[100];

  if(iserver.model == EIT_D_2F)
  {
    strcpy(sic_evt,"else if(r == 5){ a2_c[1].innerHTML=\"\"; a2_c[2].innerHTML=\"\"; }");
  }
  else
  {
    strcpy(sic_evt,"");
  }
  
  *str_=(byte *)sic_evt;           
  return (strlen((char *)sic_evt));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_char1()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_c1(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%X",alarm[1].char1);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_sspd()
 Description: SSI: Display the serial port disconnected
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_sspd(byte **str_)
cmx_reentrant 
{
  char sspd[2];

  if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F)||\
     (iserver.model == EIT_PCB_T))
  {
    strcpy(sspd,"8");
  }
  else
  {
    strcpy(sspd,"7");
  }
  
  *str_=(byte *)sspd;           
  return (strlen((char *)sspd));
}



/*------------------------------------------------------------------------
 Function:    ssi_a2_char1()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_c2(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%X",alarm[1].char2);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_ri()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_ri(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%d",alarm[1].reminder_interval);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_to()
 Description: SSI: Display the Recipients
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_to(byte **str_)
cmx_reentrant 
{
  char str[50 + 1];
  
  //sprintf(str,"%s",alarm[0].recipients);
  strcpy(str,alarm[1].recipients);  

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_msg()
 Description: SSI: Display the Message
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_msg(byte **str_)
cmx_reentrant 
{
  char str[30 + 1];
  
  //sprintf(str,"%s",alarm[0].msg);
  strcpy(str,alarm[1].msg);    

  *str_=(byte *)str;           
  return (strlen((char *)str));
}
