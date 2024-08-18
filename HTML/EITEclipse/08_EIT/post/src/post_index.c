#include "micronet.h"
#include "np_includes.h"

byte psword_buff[ADMIN_PASS_LEN+1];


int np_np_atoi(const char* ch)
{
  int number = 0;
  int *nPtr = &number; 
  unsigned char bIsNegative = FALSE;
 
  if (ch == NULL)
    return 0;
  else if (*ch == 43)
    *ch++;
  else if(*ch == 45)
  {
    bIsNegative = TRUE;
    *ch++;
  } 

  while (*ch != '\0' && *ch != ' ' && *ch >= 48 && *ch <= 57)
  {
    *nPtr = (*nPtr * 10);
    *nPtr += (*ch - 48);
    *ch++;
  }
 
  if (bIsNegative)
    number *= -1;
 
  return  number;
}


//int np_np_atoi(char *str_to_convert, int limit)
//{
//  byte str[][27] = {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16",\
//    "17","18","19","20","21","22","23","24","25","26","27"};
//  int i,ret_val = 0;
//  int converted_i = 0;
//
//  if(limit <= 27)
//  {
//    for(i = 0; i < limit; i++)
//    {
//      ret_val = mn_stricmp((byte *)str_to_convert,str[i]);
//	  if(ret_val == 0)
//	  {
//	    converted_i = i + 1;
//	  }  
//    }
//  }
//
//  return(converted_i);
//}

/*------------------------------------------------------------------------
 Function:    check_password()
 Description: Check the password in flash with the one passed as arg
 Input:       Flash Password String, String called to the Function
 Output:      0: Password not matched 
              1: Passwords matched.
------------------------------------------------------------------------*/
byte check_password(byte *flash_pswd, byte *tmp_pswd)
cmx_reentrant 
{
	int i=0;
	byte flag=1;
	
	if(flash_pswd=='\0')
	return flag;

	while(flash_pswd[i]==' '&&i<(ADMIN_PASS_LEN))
	{
		i++;
	}

	for(;i<(ADMIN_PASS_LEN);i++)
	{
		if(*tmp_pswd==flash_pswd[i])
		{			
			flag=1;
		}
		else
		{
			flag=0;
			return flag;
		}
		tmp_pswd++;
	}
	if(i>=(ADMIN_PASS_LEN))
	{
		if(*tmp_pswd)
		flag=0;
	}
	return flag;
}		

/*------------------------------------------------------------------------
 Function:     check_password_func()
 Description:  Check Password entered with one from flash               
 Input:        String
 Output:       0: Password Incorrect
               1: Password OK
------------------------------------------------------------------------*/
byte check_password_func(byte *str)
cmx_reentrant 
{
	return (check_password((byte *)password.login,str));
}



/*------------------------------------------------------------------------
 Function:     main_func()
 Description:  POST: main function to verify regular user password and 
               redirect to frame.htm
 Input:        Socket Pointer
 Output:       None
------------------------------------------------------------------------*/
void main_func(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte password_buff[ADMIN_PASS_LEN+1];
  char password_len;
  char http_password_len;

  memset(password_buff,NULL,sizeof(password_buff));
  mn_http_find_value(BODYptr,(byte *)"PW",password_buff);   /* Password ? */
  flag.admin_password_entered_correct = FALSE; /* NO  */
  
  password_len      = strlen(password.login);
  http_password_len = strlen((char *)password_buff);
   	  
  if(password.login[password_len - 1] == 0x0d)
  {
    if(!( memcmp(password.login,password_buff,(password_len - 1))) &&\
    (http_password_len == (password_len - 1)))                                     
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
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("index.htm"));
    }
  }
  else
  {
    if(!(memcmp(password.login,password_buff,password_len)) &&\
    (http_password_len == password_len))                                     
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
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("index.htm"));
    }  
  }     
}


/*------------------------------------------------------------------------
 Function:     post_load()
 Description:  POST: load function to verify the password and page no:
 Input:        Socket Pointer
 Output:       None
------------------------------------------------------------------------*/
void post_load(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  //byte password_buff[ADMIN_PASS_LEN+1];
  byte page_no_buff[10];
  int page_no;

  page_no = 1;

  mn_http_find_value(BODYptr,(byte *)"PAGENO",page_no_buff);
  page_no = np_np_atoi((char*)page_no_buff,27);
  
  if(flag.password_entered_incorrect == FALSE)
  {
    if(page_no != NULL)
	{
      switch(page_no)
		{
		  case 1:  mn_http_set_file(socket_ptr, mn_vf_get_entry("overview.htm")); break;
		  case 2:  if((flag.admin_password_entered_correct == TRUE) || (password.admin[0] == NULL))
		           {
				     mn_http_set_file(socket_ptr, mn_vf_get_entry("network.htm"));
				   }
				   else
				   {
				   	 mn_http_set_file(socket_ptr, mn_vf_get_entry("admin.htm"));
				   } 
				   break;
		  case 3:  mn_http_set_file(socket_ptr, mn_vf_get_entry("serial.htm")); break;
		  case 4:  mn_http_set_file(socket_ptr, mn_vf_get_entry("iopins.htm")); break;
		  case 5:  mn_http_set_file(socket_ptr, mn_vf_get_entry("snmp_email.htm")); break;
		  case 6:  if((flag.admin_password_entered_correct == TRUE) || (password.admin[0] == NULL))
		           {
				     mn_http_set_file(socket_ptr, mn_vf_get_entry("security.htm"));
				   }
				   else
				   {
				   	 mn_http_set_file(socket_ptr, mn_vf_get_entry("s_admin.htm"));
				   } 
				   break;
		  case 7:  mn_http_set_file(socket_ptr, mn_vf_get_entry("device_query.htm")); break;
		  case 8:  mn_http_set_file(socket_ptr, mn_vf_get_entry("device_setup.htm")); break;
		  case 9:  mn_http_set_file(socket_ptr, mn_vf_get_entry("read_device.htm")); break;
		  case 10: mn_http_set_file(socket_ptr, mn_vf_get_entry("t_wait.htm")); break;
		  case 11: if((flag.admin_password_entered_correct == TRUE) || (password.admin[0] == NULL))
		           {
				     mn_http_set_file(socket_ptr, mn_vf_get_entry("reboot.htm"));
				   }
				   else
				   {
				   	 mn_http_set_file(socket_ptr, mn_vf_get_entry("system_admin.htm"));
				   } 
				   break;
		  case 12: mn_http_set_file(socket_ptr, mn_vf_get_entry("diagnostics.htm")); break;
		  case 15: mn_http_set_file(socket_ptr, mn_vf_get_entry("bridge.htm")); break;
		  case 16: mn_http_set_file(socket_ptr, mn_vf_get_entry("packing.htm")); break;
		  case 17: mn_http_set_file(socket_ptr, mn_vf_get_entry("multihost.htm")); break;
		  case 20: mn_http_set_file(socket_ptr, mn_vf_get_entry("alarm1.htm")); break;
		  case 21: mn_http_set_file(socket_ptr, mn_vf_get_entry("alarm2.htm")); break;
          case 22: mn_http_set_file(socket_ptr, mn_vf_get_entry("alarm3.htm")); break;
          case 23: mn_http_set_file(socket_ptr, mn_vf_get_entry("alarm4.htm")); break;
          case 24: mn_http_set_file(socket_ptr, mn_vf_get_entry("alarm5.htm")); break;
		  case 25: mn_http_set_file(socket_ptr, mn_vf_get_entry("defaults.htm")); break;
		  case 26: mn_http_set_file(socket_ptr, mn_vf_get_entry("upgrade.htm")); break;
		  default: mn_http_set_file(socket_ptr, mn_vf_get_entry("overview.htm")); break;
		}
	}
	else
	{
	  mn_http_set_file(socket_ptr, mn_vf_get_entry("overview.htm"));
	}
  }
  else
  {
    //password_buff[0]='\0';                                         /* NO  */
    flag.password_entered_incorrect = TRUE;
	mn_http_set_file(socket_ptr, mn_vf_get_entry("index.htm"));
  }  
}
