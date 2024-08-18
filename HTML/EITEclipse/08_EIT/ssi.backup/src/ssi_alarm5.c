/*------------------------------------------------------------------------ 
					SSI_ALARM5.C

Description:	Return SSIs for alarm5.htm page
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
 Function:    ssi_alarm5_en()
 Description: SSI: Display whether the Alarm5 is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_alarm5_en(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].enable)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}




/*------------------------------------------------------------------------
 Function:    ssi_a5_e_pr()
 Description: SSI: Display whether the Email in Alarm5 is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_e_pr(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[0].power_reset)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_t_pr()
 Description: SSI: Display whether the trap in Alarm5 is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_t_pr(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[1].power_reset)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_e_ipc()
 Description: SSI: Display whether the IP Changed Email in Alarm5 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_e_ipc(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[0].ip_changed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_t_ipc()
 Description: SSI: Display whether the IP Changed trap in Alarm5 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_t_ipc(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[4].condition[1].ip_changed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_e_isa()
 Description: SSI: Display whether the Iserver was accessed in Alarm5 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_e_isa(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[0].isvr_accessed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_t_isa()
 Description: SSI: Display whether the Iserver was accessed in Alarm5 is 
              enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_t_isa(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[4].condition[1].isvr_accessed)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_e_spd()
 Description: SSI: Display whether the Serial Port is connected or not 
              Alarm5 is enabled or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_e_spd(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[0].isvr_disconnect)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_t_spd()
 Description: SSI: Display whether the Serial Port  is connected or not 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_t_spd(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[4].condition[1].isvr_disconnect)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
  Function:    ssi_a5_ip_pin()
  Description: Output the input pin selection menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_a5_ip_pin(byte **str_)
cmx_reentrant 
{

#if((MODEL_TYPE == EIT_RS232_GPIO) || (MODEL_TYPE == EIT_RS232_TTL))
  int i;
  char tmp[(IP_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[IP_OPTS+2][LEN_OPTIONS];
  char s[IP_OPTS][LEN_SELECTED];
  char opt[IP_OPTS][5] = {"PIN1", "PIN2", "PIN3", "PIN4", "PIN5", "PIN6"};
  
  memset(s,NULL,(IP_OPTS * LEN_SELECTED));       
  	
  switch(alarm[4].input_pin)  /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
    case 4:  strcpy(s[3],SELECTED);  break;
	case 5:  strcpy(s[4],SELECTED);  break;
	case 6:  strcpy(s[5],SELECTED);  break;
	default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"a5_ip_pin\">\r\n");
  strcpy(tmp,str[0]);
  
  for(i = 1; i < (IP_OPTS + 1); i++)
  {
    if(gpio.direction[i-1] == 1)
	{
	  sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
        (i),s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
      strcat(tmp,str[i]);
	}
  }

  sprintf(str[IP_OPTS+1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[IP_OPTS+1]);

#elif(MODEL_TYPE == EIT_RS232_FULL)
  char tmp[6];

  sprintf(tmp,"PIN 1");            /* str[4]=</select */

#elif(MODEL_TYPE == EIT_RS485)
  int i;
  char tmp[(4 * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[4 + 2][LEN_OPTIONS];
  char s[4][LEN_SELECTED];
  char opt[4][5] = {"PIN1", "PIN2", "PIN3", "PIN4"};
  
  memset(s,NULL,(4 * LEN_SELECTED));       
  	
  switch(alarm[4].input_pin)  /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    case 3:  strcpy(s[2],SELECTED);  break;
    case 4:  strcpy(s[3],SELECTED);  break;
	default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"a5_ip_pin\">\r\n");
  strcpy(tmp,str[0]);
  
  for(i = 1; i < (4 + 1); i++)
  {
    if(gpio.direction[i-1] == 1)
	{
	  sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
        (i),s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
      strcat(tmp,str[i]);
	}
  }

  sprintf(str[4 + 1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[4 + 1]);
#endif


  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    ssi_a5_pin_cdn()
  Description: Output the pin condition selection menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 ssi_a5_pin_cdn(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(PC_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[PC_OPTS + 2][LEN_OPTIONS];
  char s[PC_OPTS][LEN_SELECTED];
  char opt[PC_OPTS][5] = {"LOW", "HIGH"};
  
  memset(s,NULL,(PC_OPTS * LEN_SELECTED));       
  	
  switch(alarm[4].pin_condition)  /* Selected Parity = "selected", else " " */
  {
    case 1:  strcpy(s[0],SELECTED);  break;
    case 2:  strcpy(s[1],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"a5_pin_cdn\">\r\n");
  strcpy(tmp,str[0]);
  
  for(i = 1; i < (PC_OPTS + 1); i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      (i),s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[PC_OPTS+1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[PC_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}

/*------------------------------------------------------------------------
 Function:    ssi_a5_e_ic()
 Description: SSI: Display whether email alarm for Input Pin Condition 
              is enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_e_ic(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[0].isvr_condition)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_t_ic()
 Description: SSI: Display whether TRAP alarm for Input Pin Condition is
              enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_t_ic(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[4].condition[1].isvr_condition)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_e_c()
 Description: SSI: Display whether email alarm for character Condition 
              is enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_e_c(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  switch(alarm[4].condition[0].chars)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_t_c()
 Description: SSI: Display whether TRAP alarm for Input Pin Condition is
              enabled or not?
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_t_c(byte **str_)
cmx_reentrant
{
  char str[10];
  
  switch(alarm[4].condition[1].chars)
  {
    case 0: strcpy(str," "); break;
    case 1: strcpy(str,"checked"); break;
    default: strcpy(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a5_char1()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_char1(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%X",alarm[4].char1);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a5_char1()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_char2(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%X",alarm[4].char2);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_a5_ri()
 Description: SSI: Display the Reminder Interval
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_ri(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  sprintf(str,"%d",alarm[4].reminder_interval);
  
  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_to()
 Description: SSI: Display the Recipients
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_to(byte **str_)
cmx_reentrant 
{
  char str[50 + 1];
  
  //sprintf(str,"%s",alarm[0].recipients);
  strcpy(str,alarm[4].recipients);  

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_a5_msg()
 Description: SSI: Display the Message
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_a5_msg(byte **str_)
cmx_reentrant 
{
  char str[30 + 1];
  
  //sprintf(str,"%s",alarm[0].msg);
  strcpy(str,alarm[4].msg);    

  *str_=(byte *)str;           
  return (strlen((char *)str));
}
