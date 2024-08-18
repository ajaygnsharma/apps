/*------------------------------------------------------------------------ 
					POST_DEVICE_SETUP.c

Description:	Post for device_setup.htm page
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
 Function:    update_dev_func()
 Description: POST FUNCTION: Update_dev_func(socket pointer)
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
void post_updatedevparam(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte i,number;
  byte temp_buff[30];
  number = 0;

  if(mn_http_find_value(BODYptr,(byte *)"N",temp_buff))  /* Unit Number */
  {
    number=temp_buff[0]-'0';
  }

  if(mn_http_find_value(BODYptr,(byte *)"ID",temp_buff))   /* Device ID */
  {
	i=number-1;
	//moddev_supplement(temp_buff,1*2);
	strcpy(device.uid[i],(char *)temp_buff);      
  }

  if(mn_http_find_value(BODYptr,(byte *)"NM",temp_buff)) /* Device Name */ 
  {
    i=number-1;
	//moddev_supplement(temp_buff,DEVICE_NAME_LEN); 
	strcpy(device.name[i],(char *)temp_buff);
  }

  if(mn_http_find_value(BODYptr,(byte *)"CMD",temp_buff))    /* Command */
  {	
    i=number-1;
	//moddev_supplement(temp_buff,DEVICE_CMD_LEN);
	strcpy(device.command[i],(char *)temp_buff);
  }

  if(mn_http_find_value(BODYptr,(byte *)"UNT",temp_buff))/*Response Unit*/
  {
    i=number-1;
	//moddev_supplement(temp_buff,DEVICE_UNIT_LEN);
    strcpy(device.unit[i],(char *)temp_buff);	
  }

  write_all();                               /* Update all the Settings */

  mn_http_set_file(socket_ptr, mn_vf_get_entry("device_setup.htm"));
}



/*------------------------------------------------------------------------
 Function:    post_setdeviceparam()
 Description: POST FUNCTION: 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_setdeviceparam(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  mn_http_set_file(socket_ptr,mn_vf_get_entry("setdeviceparam.htm"));
}

/*------------------------------------------------------------------------
 Function:    post_read_dev()
 Description: POST FUNCTION: Load the page to read devices
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_read_dev(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  mn_http_set_file(socket_ptr,mn_vf_get_entry("read_device.htm"));
}


/*------------------------------------------------------------------------
 Function:    post_check()
 Description: POST: Set the checkmarks on the npdevice.htm page
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
void post_check(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte temp;
  temp=0;
    
  if(mn_http_find_value(BODYptr,(byte *)"ck1",temp_buff))
  {
    temp |=0x01;
  }

  if(mn_http_find_value(BODYptr,(byte *)"ck2",temp_buff))
  {
    temp |=0x02;
  }

  if(mn_http_find_value(BODYptr,(byte *)"ck3",temp_buff))
  {
    temp |=0x04;
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"ck4",temp_buff))
  {
    temp |=0x08;
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"ck5",temp_buff))
  {
    temp |=0x10;
  }

  if(mn_http_find_value(BODYptr,(byte *)"ck6",temp_buff))
  {
    temp |=0x20;
  }

  if(mn_http_find_value(BODYptr,(byte *)"ck7",temp_buff))
  {
    temp |=0x40;
  }

  if(mn_http_find_value(BODYptr,(byte *)"ck8",temp_buff))
  {
    temp |=0x80;
  }
   
  device.check=temp;                             /* Device Check update */
  write_all();                                   /* Update the Settings */
  mn_http_set_file(socket_ptr, mn_vf_get_entry("device_setup.htm"));
}
