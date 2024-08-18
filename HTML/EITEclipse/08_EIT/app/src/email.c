#include "micronet.h"
#include "np_includes.h"


/*------------------------------------------------------------------------
 Function:    send_mail(Alarm_number)
 Description: Send SMTP Message/Email
 Input:       None
 Outout:      None
------------------------------------------------------------------------*/
void send_mail(byte alarm_number)
cmx_reentrant 
{
  SCHAR socket_no = 0;
  int retval;
  SMTP_INFO_T mail_info;
  int i;

  memcpy(ip_smtp_addr,smtp.server_ip,IP_ADDR_LEN);  
  
  if(socket_no <= 0)
  {
    for(i = 0; i < 3; i++)
	{
	  socket_no = mn_smtp_start_session(DEFAULT_PORT, eth_interface_no, 0, 0);
	  /* I put this delay here because atleast at the newport server the session is 
	  not started the moment i open one connection, so i try again for three times */
	  delayMs(1,2000); /* Delays is put here in between trying to make connections 
	  and it seems to be working */
  	  if(socket_no >= 0)
	  {
	    break;	/* If a socket is assigned which means a good connection is made, i exit */
	  }
	  else
	  {
		printf("SMTP session failed initialization ...trying again..\r\n");
	  }
	}
  }

  if (socket_no >= 0)
  {
//#if(DEBUG_LEVEL == MEDIUM)
      printf("Started the SMTP Session ...\r\n");
//#endif
  
	mail_info.from       = (byte *)smtp.from;
    switch(alarm_number)
	{
	  case 1: mail_info.to         = (byte *)alarm[0].recipients; break;
	  case 2: mail_info.to         = (byte *)alarm[1].recipients; break;
	  case 3: mail_info.to         = (byte *)alarm[2].recipients; break;
	  case 4: mail_info.to         = (byte *)alarm[3].recipients; break;
	  case 5: mail_info.to         = (byte *)alarm[4].recipients; break;
	}

    mail_info.subject    = (byte *)smtp.subject;
    
	switch(alarm_number)
	{
	  case 1: mail_info.message    = (byte *)alarm[0].msg; break;
	  case 2: mail_info.message    = (byte *)alarm[1].msg; break;
	  case 3: mail_info.message    = (byte *)alarm[2].msg; break;
	  case 4: mail_info.message    = (byte *)alarm[3].msg; break;
	  case 5: mail_info.message    = (byte *)alarm[4].msg; break;
	}

	mail_info.attachment = PTR_NULL;
    mail_info.filename   = PTR_NULL;

    retval = mn_smtp_send_mail(socket_no, &mail_info);
      
    mn_smtp_end_session(socket_no);
    
    if (retval < 0)           /* there was an error sending the message */
    {
//#if(DEBUG_LEVEL == MEDIUM)
        printf("Error Sending Email...\r\n"); 
//#endif
      //break;              
    }
    else
    {
//#if(DEBUG_LEVEL == MEDIUM)
        printf("Message Sent Successfully...\r\n");
//#endif
    }
  }

}
