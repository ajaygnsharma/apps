/*------------------------------------------------------------------------ 
					POST_MANAGEMENT.c

Description:	Post for management.htm page
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
 Function:    post_smtpsnmp()
 Description: POST FUNCTION: Update SMTP and SNMP Settings.
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_smtpsnmp(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"smtp",temp_buff))/* SMTP En?   */
  {
    flag.smtp = ENABLED;
  }
  else
  {
    flag.smtp = DISABLED;  
  }

  if(mn_http_find_value(BODYptr,(byte *)"snmp",temp_buff))/* SNMP En?   */
  {
    flag.snmp = ENABLED;
  }
  else
  {
    flag.snmp = DISABLED;  
  }

  /*--------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"trap",temp_buff))/* SMTP En?   */
  {
    flag.trap_en = ENABLED;
  }
  else
  {
    flag.trap_en = DISABLED;  
  }


  if(mn_http_find_value(BODYptr,(byte *)"trap_ip",temp_buff))/* TRAP IP */
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(snmp.trap_ip,temp_buff,IP_ADDR_LEN);             /* TRAP IP  */
    }
    else
    {
      temp_buff[0]=0x00;
      temp_buff[1]=0x00;
      temp_buff[2]=0x00;
      temp_buff[3]=0x00;
      memcpy(snmp.trap_ip,temp_buff,IP_ADDR_LEN);   
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"Contact",temp_buff))/* Contact */
  {
    mn_setFlashVariable(temp_buff,sysContact0,SNMP_CONTACT_LEN);
    //strcpy(snmp.contact,(char *)temp_buff);
  }

  if(mn_http_find_value(BODYptr,(byte *)"Community",temp_buff))
  {
    strcpy(snmp.community,(char *)temp_buff);
  }

  if(mn_http_find_value(BODYptr,(byte *)"Location",temp_buff))/* Location */
  {
    mn_setFlashVariable(temp_buff,sysLocation0,SNMP_LOCATION_LEN);
    //strcpy(snmp.location,(char *)temp_buff);    
  }

  if(mn_http_find_value(BODYptr,(byte *)"smtpip",temp_buff)) /* SMTP IP */
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(smtp.server_ip,temp_buff,IP_ADDR_LEN);         /* SMTP IP  */
    }
    else
    {
      temp_buff[0]=0x00;
      temp_buff[1]=0x00;
      temp_buff[2]=0x00;
      temp_buff[3]=0x00;
      memcpy(smtp.server_ip,temp_buff,IP_ADDR_LEN);   
    }
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"from",temp_buff))
  {
    strcpy(smtp.from,(char *)temp_buff);            /* SMTP Sender info */
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"sub",temp_buff))
  {
    strcpy(smtp.subject,(char *)temp_buff);         /* SMTP Subject     */
  }	
  
  write_all();       /* Write all this updated information to the flash */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("snmp_email.htm"));
}


