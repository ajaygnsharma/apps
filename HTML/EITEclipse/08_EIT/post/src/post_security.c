/*------------------------------------------------------------------------ 
					POST_SECURITY.c

Description:	Post for security.htm page
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
 Function:    add_space()
 Description: Go to the Max size of buffer , with spaces.
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void add_space(unsigned long *buff, byte number)
cmx_reentrant
{
  int i = 0;
  int j = number;

  *(buff + number) = '\0';

  for(i = number; i >= 0; i--)
  {
	if(!(*(buff + i)))
	{
	  j = number;
	}
	*(buff + j) = *(buff + i);
	j--;
  }

  for( ;j >= 0; j--)
  {
    *(buff + j) = ' ';
  }
}


/*------------------------------------------------------------------------
 Function:    post_security()
 Description: POST FUNCTION: Update security.htm Page settings 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_security(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"ica",temp_buff))/* Internet Con*/
  {
    flag.internet_console = ENABLED;
  }
  else
  {
    flag.internet_console = DISABLED;  
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"spca",temp_buff))/* Serial Cons*/
  {
    flag.serial_console = ENABLED;
  }
  else
  {
    flag.serial_console = DISABLED;  
  }

  if(mn_http_find_value(BODYptr,(byte *)"wsa",temp_buff)) /* Webserver  */
  {
    flag.webserver = ENABLED;
  }
  else
  {
    flag.webserver = DISABLED;  
  } 
  
  if(mn_http_find_value(BODYptr,(byte *)"ipx",temp_buff)) /* IP Exclusivity */
  {
    flag.security = ENABLED;
  }
  else
  {
    flag.security = DISABLED;  
  } 
  
  if(mn_http_find_value(BODYptr,(byte *)"ip1",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[0],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip2",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[1],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip3",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[2],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip4",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[3],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip5",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[4],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip6",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[5],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip7",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[6],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
  
  
  if(mn_http_find_value(BODYptr,(byte *)"ip8",temp_buff))/*---IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(exclusivity.ip[7],temp_buff,IP_ADDR_LEN);/* Set IP Addr    */
	}
  }
  
    	
  write_all();       /* Write all this updated information to the flash */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("security.htm"));
}

/*------------------------------------------------------------------------
 Function:    post_change_login()
 Description: POST FUNCTION: Update security.htm Login Password
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_change_login(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  char login_pswd_buff[20];
  char confirm_pswd_buff[20];

  if(mn_http_find_value(BODYptr,(byte *)"login",(byte *)login_pswd_buff) && 
     mn_http_find_value(BODYptr,(byte *)"confirm",(byte *)confirm_pswd_buff))
  {
//    add_space(temp_buffer,LOGIN_PASS_LEN);
	if(!( memcmp(login_pswd_buff,confirm_pswd_buff,(strlen(login_pswd_buff))) ))
	{
	  memcpy(password.login,login_pswd_buff,LOGIN_PASS_LEN);       /* Login Pass*/
	  flag.confirm_pswd_entered_incorrect = 2;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("change_login.htm"));
	}
	else
	{
	  flag.confirm_pswd_entered_incorrect = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("change_login.htm"));
	}
  }
  else
  {
    flag.confirm_pswd_entered_incorrect = TRUE;
	mn_http_set_file(socket_ptr, mn_vf_get_entry("change_login.htm"));
  }
  
  write_all();
  
}


/*------------------------------------------------------------------------
 Function:    post_change_admin()
 Description: POST FUNCTION: Update security.htm Login Password
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_change_admin(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  char admin_pswd_buff[20];
  char confirm_pswd_buff[20];

  if(mn_http_find_value(BODYptr,(byte *)"admin",(byte *)admin_pswd_buff) && 
     mn_http_find_value(BODYptr,(byte *)"confirm",(byte *)confirm_pswd_buff))
  {
//    add_space(temp_buffer,LOGIN_PASS_LEN);
	if(!( memcmp(admin_pswd_buff,confirm_pswd_buff,(strlen(admin_pswd_buff))) ))
	{
	  memcpy(password.admin,admin_pswd_buff,LOGIN_PASS_LEN);       /* Login Pass*/
	  flag.confirm_pswd_entered_incorrect = 2;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("change_admin.htm"));
	}
	else
	{
	  flag.confirm_pswd_entered_incorrect = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("change_admin.htm"));
	}
  }
  else
  {
    flag.confirm_pswd_entered_incorrect = TRUE;
	mn_http_set_file(socket_ptr, mn_vf_get_entry("change_admin.htm"));
  }
  
  write_all();
  
}


/*------------------------------------------------------------------------
 Function:    post_change_admin()
 Description: POST FUNCTION: Update security.htm Login Password
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_change_serial(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  char serial_pswd_buff[20];
  char confirm_pswd_buff[20];

  if(mn_http_find_value(BODYptr,(byte *)"serial",(byte *)serial_pswd_buff) && 
     mn_http_find_value(BODYptr,(byte *)"confirm",(byte *)confirm_pswd_buff))
  {
//    add_space(temp_buffer,LOGIN_PASS_LEN);
	if(!( memcmp(serial_pswd_buff,confirm_pswd_buff,(strlen(serial_pswd_buff))) ))
	{
	  memcpy(password.serial,serial_pswd_buff,SERIAL_PASS_LEN);       /* Login Pass*/
	  flag.confirm_pswd_entered_incorrect = 2;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("change_serial.htm"));
	}
	else
	{
	  flag.confirm_pswd_entered_incorrect = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("change_serial.htm"));
	}
  }
  else
  {
    flag.confirm_pswd_entered_incorrect = TRUE;
	mn_http_set_file(socket_ptr, mn_vf_get_entry("change_serial.htm"));
  }
  
  write_all();
}
