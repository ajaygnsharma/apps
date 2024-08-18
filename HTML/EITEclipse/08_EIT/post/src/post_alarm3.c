/*------------------------------------------------------------------------ 
					POST_ALARM3.c

Description:	Post for alarm3.htm page
				1. 
                2. 
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
 Function:    post_alarm3()
 Description: POST FUNCTION: Update Alarm2 Settings.
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_alarm3(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"alm_3",temp_buff))/*Alarm1 En?*/
    alarm[2].enable = ENABLED;
  else
    alarm[2].enable = DISABLED;
  
  /*--------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"a3_e_pr",temp_buff))/*A1 Eml pr*/
    alarm[2].condition[0].power_reset = ENABLED;
  else
    alarm[2].condition[0].power_reset = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_e_ipc",temp_buff))/*A2 Eml pr*/
    alarm[2].condition[0].ip_changed = ENABLED;
  else
    alarm[2].condition[0].ip_changed = DISABLED;
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_e_isa",temp_buff))/*a3 Eml pr*/
    alarm[2].condition[0].isvr_accessed = ENABLED;
  else
    alarm[2].condition[0].isvr_accessed = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_e_spd",temp_buff))/*a3 Eml pr*/
    alarm[2].condition[0].isvr_disconnect = ENABLED;
  else
    alarm[2].condition[0].isvr_disconnect = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_e_ic",temp_buff))/*a3 Eml pr*/
    alarm[2].condition[0].isvr_condition = ENABLED;
  else
    alarm[2].condition[0].isvr_condition = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_e_c",temp_buff))/*a3 Eml pr*/
    alarm[2].condition[0].chars = ENABLED;
  else
    alarm[2].condition[0].chars = DISABLED;  
  
  /*--------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"a3_t_pr",temp_buff))/*A1 Trp Pr*/
    alarm[2].condition[1].power_reset = ENABLED;
  else
    alarm[2].condition[1].power_reset = DISABLED;  

  if(mn_http_find_value(BODYptr,(byte *)"a3_t_ipc",temp_buff))/*A1 Trp Pr*/
    alarm[2].condition[1].ip_changed = ENABLED;
  else
    alarm[2].condition[1].ip_changed = DISABLED;  

  if(mn_http_find_value(BODYptr,(byte *)"a3_t_isa",temp_buff))/*a3 Trp Pr*/
    alarm[2].condition[1].isvr_accessed = ENABLED;
  else
    alarm[2].condition[1].isvr_accessed = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_t_spd",temp_buff))/*a3 Eml pr*/
    alarm[2].condition[1].isvr_disconnect = ENABLED;
  else
    alarm[2].condition[1].isvr_disconnect = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_t_ic",temp_buff))/*a3 Eml pr*/
    alarm[2].condition[1].isvr_condition = ENABLED;
  else
    alarm[2].condition[1].isvr_condition = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_t_c",temp_buff))/* a3 Eml pr*/
    alarm[2].condition[1].chars = ENABLED;
  else
    alarm[2].condition[1].chars = DISABLED;  
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_ip_pin",temp_buff))/* A3 Eml pr*/
  {
    alarm[2].input_pin = temp_buff[0] - '0';
//	switch(alarm[0].input_pin)
//	{
//	  case 1: FIO2MASK0 = 0xfb; break;       /* P2.2 will only be read  */
//	  case 2: FIO2MASK0 = 0xf7; break;       /* P2.3 will only be read  */
//	  case 3: FIO2MASK0 = 0xe7; break;       /* P2.4 will only be read  */
//	  case 4: FIO2MASK0 = 0xdf; break;       /* P2.5 will only be read  */
//	  case 5: FIO2MASK0 = 0xbf; break;       /* P2.6 will only be read  */
//	  case 6: FIO2MASK0 = 0x7f; break;       /* P2.7 will only be read  */
//	  default:FIO2MASK0 = 0xfb; break;       /* P2.2 will only be read  */
//	}
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_pin_cdn",temp_buff))/* A3 Eml pr*/
  {
    alarm[2].pin_condition = temp_buff[0] - '0';
	switch(alarm[2].pin_condition)
	{
	  case 1: alarm[2].gpio_bit_mask = 0x04; break; /* GPIO 1 be read   */
	  case 2: alarm[2].gpio_bit_mask = 0x08; break; /* GPIO 2 be read   */
	  case 3: alarm[2].gpio_bit_mask = 0x10; break; /* GPIO 3 be read   */
	  case 4: alarm[2].gpio_bit_mask = 0x20; break; /* GPIO 4 be read   */
	  case 5: alarm[2].gpio_bit_mask = 0x40; break; /* GPIO 5 be read   */
	  case 6: alarm[2].gpio_bit_mask = 0x80; break; /* GPIO 6 be read   */
	  default:alarm[2].gpio_bit_mask = 0x04; break; /* GPIO 1 be read   */
	}
  }
  
  /*-------------------------Alarm 3 Char1 -----------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"a3_c1",temp_buff))
  {
    sscanf((char *)temp_buff,"%x",&alarm[2].char1);         /* Endchar  */
    //alarm[2].char1 = np_atoi((char *)temp_buff);              /* Rem Int   */
    //strcpy(smtp.from,(char *)temp_buff);          /* SMTP Sender info */
  }

  /*-------------------------Alarm 3 Char2 -----------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"a3_c2",temp_buff))
  {
    sscanf((char *)temp_buff,"%x",&alarm[2].char2);         /* Endchar  */
    //alarm[2].char2 = np_atoi((char *)temp_buff);              /* Rem Int   */
    //strcpy(smtp.from,(char *)temp_buff);          /* SMTP Sender info */
  }
  
  /*-------------------------Alarm 1 Information -----------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"a3_ri",temp_buff))
    alarm[2].reminder_interval = np_atoi((char *)temp_buff);  /* Rem Int   */
    //strcpy(smtp.from,(char *)temp_buff);          /* SMTP Sender info */
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_to",temp_buff))
    strcpy(alarm[2].recipients,(char *)temp_buff); /* Alarm0 Recipients */
  
  if(mn_http_find_value(BODYptr,(byte *)"a3_msg",temp_buff))
    strcpy(alarm[2].msg,(char *)temp_buff);           /* Alarm0 Message */
  	
  write_all();       /* Write all this updated information to the flash */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("alarm3.htm"));
}
