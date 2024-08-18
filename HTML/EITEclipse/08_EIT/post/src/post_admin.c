#include "micronet.h"
#include "np_includes.h"



/*------------------------------------------------------------------------
 Function:     admin_func()
 Description:  POST: main function to verify regular user password and 
               redirect to frame.htm
 Input:        Socket Pointer
 Output:       None
------------------------------------------------------------------------*/
void post_admin(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte password_buff[ADMIN_PASS_LEN+1];
  char password_len;
  char http_password_len;

  memset(password_buff,NULL,sizeof(password_buff));
  mn_http_find_value(BODYptr,(byte *)"PW",password_buff);   /* Password ? */
  
  password_len      = strlen(password.admin);
  http_password_len = strlen((char *)password_buff);
  
  if(password.admin[password_len - 1] == 0x0d)
  {
    if(!( memcmp(password.admin,password_buff,(password_len - 1))) &&\
    (http_password_len == (password_len - 1)))                                     
    /* Password OK?   */
    {
      strcpy((char *)password_buff,password.admin);                  /* YES */
      flag.isvr_accessed = TRUE;
	  flag.password_entered_incorrect = FALSE;
	  flag.admin_password_entered_correct = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("network.htm")); 
    }
    else
    {
      password_buff[0]='\0';                                         /* NO  */
	  flag.password_entered_incorrect = TRUE; 
	  flag.admin_password_entered_correct = FALSE;                                       /* NO  */
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("admin.htm"));
    }
  }
  else
  {
    if(!(memcmp(password.admin,password_buff,password_len)) &&\
    (http_password_len == password_len))                                     
    /* Password OK?   */
    {
      strcpy((char *)password_buff,password.admin);                  /* YES */
      flag.isvr_accessed = TRUE;
	  flag.password_entered_incorrect = FALSE;
	  flag.admin_password_entered_correct = TRUE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("network.htm")); 
    }
    else
    {
      password_buff[0]='\0';                                         /* NO  */
	  flag.password_entered_incorrect = TRUE;
	  flag.admin_password_entered_correct = FALSE;
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("admin.htm"));
    }  
  }     
}
