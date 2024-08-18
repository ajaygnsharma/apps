/*------------------------------------------------------------------------ 
					SSI_ALARM2.C

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
 Function:    ssi_alarm2_en()
 Description: SSI: Display whether the Alarm2 is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_alarm2_en(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].enable)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}




/*------------------------------------------------------------------------
 Function:    ssi_a2_e_pr()
 Description: SSI: Display whether the Email in Alarm2 is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_pr(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].condition[0].power_reset)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_t_pr()
 Description: SSI: Display whether the trap in Alarm2 is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_pr(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].condition[1].power_reset)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_e_ipc()
 Description: SSI: Display whether the IP Changed Email in Alarm2 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_ipc(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].condition[0].ip_changed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_t_ipc()
 Description: SSI: Display whether the IP Changed trap in Alarm2 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_ipc(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[1].condition[1].ip_changed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_e_isa()
 Description: SSI: Display whether the Iserver was accessed in Alarm2 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_isa(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].condition[0].isvr_accessed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_t_isa()
 Description: SSI: Display whether the Iserver was accessed in Alarm2 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_isa(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[1].condition[1].isvr_accessed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_e_spd()
 Description: SSI: Display whether the Serial Port is connected or not 
              Alarm2 is enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_spd(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].condition[0].isvr_disconnect)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_t_spd()
 Description: SSI: Display whether the Serial Port  is connected or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_spd(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[1].condition[1].isvr_disconnect)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/**************************************************************************************************
  Function:    ssi_a2_ip_pin()
  Description: Output the input pin selection menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 ssi_a2_ip_pin(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(4 * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[LEN_OPTIONS];
  char s[8][LEN_SELECTED];
  char opt[8][5] = {"PIN1", "PIN2", "PIN3", "PIN4", "PIN5", "PIN6", "PIN7", "PIN8"};
  int no_of_io = 0;

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
	memset(str,NULL,sizeof(str));       
  	memset(s,NULL,sizeof(s));       
  	sprintf(str,"<select name=\"a2_ip_pin\">\r\n");
	strcpy(tmp,str);

    switch(alarm[1].input_pin)  /* Selected Parity = "selected", else " " */
    {
      case 1:  strcpy(s[0],SELECTED);  break;
      case 2:  strcpy(s[1],SELECTED);  break;
      case 3:  strcpy(s[2],SELECTED);  break;
      case 4:  strcpy(s[3],SELECTED);  break;
	  case 5:  strcpy(s[4],SELECTED);  break;
      case 6:  strcpy(s[5],SELECTED);  break;
      case 7:  strcpy(s[6],SELECTED);  break;
      case 8:  strcpy(s[7],SELECTED);  break;
	  default: strcpy(s[0],SELECTED);  break;
    }

    for(i = 1; i < (no_of_io + 1); i++)
    {
      if(gpio.direction[i-1] == 1)
	  {
	    memset(str,NULL,LEN_OPTIONS);
		sprintf(str,"<option value=\"%d\" %s> %s </option>\r\n",\
        (i),s[i-1],opt[i-1]);   
        /*    str[i] = <option value=i selected="selected"> One </option> */
        strcat(tmp,str);
	  }
    }
	
	memset(str,NULL,LEN_OPTIONS);
	sprintf(str,"</select>\r\n");            /* str[4]=</select */  //wendy
    strcat(tmp,str);
  }
  else
  {
    strcpy(tmp," ");
  }
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/**************************************************************************************************
  Function:    ssi_a2_pin_cdn()
  Description: Output the pin condition selection menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
**************************************************************************************************/
word16 ssi_a2_pin_cdn(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(PC_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[LEN_OPTIONS];
  char s[PC_OPTS][LEN_SELECTED];
  char opt[PC_OPTS][5] = {"LOW", "HIGH"};
  
  if(iserver.model == EIT_D_2F)
  {
    strcpy(tmp," ");
  }
  else
  {
    memset(s,NULL,sizeof(s));       
    memset(str,NULL,sizeof(str));       
    sprintf(str,"<select name=\"a2_ip_pin\">\r\n");
    strcpy(tmp,str);
	
    switch(alarm[1].pin_condition)  /* Selected Parity = "selected", else " " */
    {
     case 1:  strcpy(s[0],SELECTED);  break;
     case 2:  strcpy(s[1],SELECTED);  break;
     default: strcpy(s[0],SELECTED);  break;
    }

    for(i = 1; i < (PC_OPTS + 1); i++)
    {
      memset(str,NULL,LEN_OPTIONS);
	  sprintf(str,"<option value=\"%d\" %s> %s </option>\r\n",\
        (i),s[i-1],opt[i-1]);   
       /*    str[i] = <option value=i selected="selected"> One </option> */
      strcat(tmp,str);
    }

    memset(str,NULL,LEN_OPTIONS);
    sprintf(str,"</select>\r\n");            /* str[4]=</select */
    strcat(tmp,str);
  }
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_e_ic()
 Description: SSI: Display whether email alarm for Input Pin Condition 
              is enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_ic(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  if(iserver.model == EIT_D_2F)
  {
    strcpy(str," ");
  }
  else
  {
    switch(alarm[1].condition[0].isvr_condition)
    {
      case 0: strcpy(str," "); break;
      case 1: strcpy(str,"checked"); break;
      default: strcpy(str," "); break;
    }
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_t_ic()
 Description: SSI: Display whether TRAP alarm for Input Pin Condition is
              enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_ic(byte **str_)
cmx_reentrant
{
  char str[10];
  
  if(iserver.model == EIT_D_2F)
  {
    strcpy(str," ");
  }
  else
  {
    switch(alarm[1].condition[1].isvr_condition)
    {
      case 0: strcpy(str," "); break;
      case 1: strcpy(str,"checked"); break;
      default: strcpy(str," "); break;
    }
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_e_c()
 Description: SSI: Display whether email alarm for character Condition 
              is enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_e_c(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[1].condition[0].chars)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a2_t_c()
 Description: SSI: Display whether TRAP alarm for Input Pin Condition is
              enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_t_c(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[1].condition[1].chars)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_char1()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_char1(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%X",alarm[1].char1);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_char1()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a2_char2(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%X",alarm[1].char2);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a2_ri()
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
