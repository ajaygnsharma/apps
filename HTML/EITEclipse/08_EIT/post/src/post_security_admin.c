#include "micronet.h"
#include "np_includes.h"



/*------------------------------------------------------------------------
 Function:     admin_func()
 Description:  POST: main function to verify regular user password and 
               redirect to frame.htm
 Input:        Socket Pointer
 Output:       None
------------------------------------------------------------------------*/
void post_security_admin(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte password_buff[ADMIN_PASS_LEN+1];

  memset(password_buff,NULL,sizeof(password_buff));
  mn_http_find_value(BODYptr,(byte *)"PW",password_buff);   /* Password ? */
  
  if(password.admin[strlen(password.admin)] == 0x0d)
  {
    if(!( memcmp(password.admin,password_buff,(strlen(password.admin) - 1))) &&\
    (strlen((char *)password_buff) == (strlen(password.admin) - 1)))                                     
    /* Password OK?   */
    {
      strcpy((char *)password_buff,password.admin);                  /* YES */
      flag.isvr_accessed = TRUE;
	  flag.password_entered_incorrect = FALSE;
	  flag.admin_password_entered_correct = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("security.htm")); 
    }
    else
    {
      password_buff[0]='\0';  
	  flag.password_entered_incorrect = TRUE; 
	  flag.admin_password_entered_correct = FALSE; /* NO  */
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("s_admin.htm"));
    }
  }
  else
  {
    if(!( memcmp(password.admin,password_buff,strlen(password.admin))) &&\
    (strlen((char *)password_buff) == strlen(password.admin)))                                     
    /* Password OK?   */
    {
      strcpy((char *)password_buff,password.admin);                  /* YES */
      flag.isvr_accessed = TRUE;
	  flag.password_entered_incorrect = FALSE;
	  flag.admin_password_entered_correct = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("security.htm")); 
    }
    else
    {
      password_buff[0]='\0'; 
	  flag.password_entered_incorrect = TRUE;
	  flag.admin_password_entered_correct = FALSE; /* NO  */
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("s_admin.htm"));
    }  
  }     
}
