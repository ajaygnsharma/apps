#include "micronet.h"
#include "np_includes.h"


/*------------------------------------------------------------------------
 Function:     main_func()
 Description:  POST: main function to verify regular user password and 
               redirect to frame.htm
 Input:        Socket Pointer
 Output:       None
------------------------------------------------------------------------*/
void post_chk_login_passwd(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte password_buff[ADMIN_PASS_LEN+1];

  memset(password_buff,NULL,sizeof(password_buff));
  mn_http_find_value(BODYptr,(byte *)"PW",password_buff);   /* Password ? */
  
  if(password.login[strlen(password.login)] == 0x0d)
  {
    if(!( memcmp(password.login,password_buff,(strlen(password.login) - 1))) &&\
    (strlen((char *)password_buff) == (strlen(password.login) - 1)))                                     
    /* Password OK?   */
    {
      strcpy((char *)password_buff,password.login);                  /* YES */
      flag.isvr_accessed = TRUE;
	  flag.password_entered_incorrect = FALSE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("overview.htm")); 
    }
    else
    {
      password_buff[0]='\0'; 
	  flag.password_entered_incorrect = TRUE;                                        /* NO  */
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("login.htm"));
    }
  }
  else
  {
    if(!( memcmp(password.login,password_buff,strlen(password.login))) &&\
    (strlen((char *)password_buff) == strlen(password.login)))                                     
    /* Password OK?   */
    {
      strcpy((char *)password_buff,password.login);                  /* YES */
      flag.isvr_accessed = TRUE;
	  flag.password_entered_incorrect = FALSE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("overview.htm")); 
    }
    else
    {
      password_buff[0]='\0';                                         /* NO  */
	  flag.password_entered_incorrect = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("login.htm"));
    }  
  }     
}
