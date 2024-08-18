#include "micronet.h"
#include "np_includes.h"

/**************************************************************************************************
 Function:    power_on_alarm()
 Description: Send SMTP Message/Email
 Input:       None
 Outout:      None
**************************************************************************************************/
void power_on_alarm(void)
cmx_reentrant 
{
  int i;

  for(i = 0; i < 5; i++)
  {
    /* If any of the alarm is enabled */
	if(alarm[i].enable == TRUE)
	{
	  /* If that alarm's email is enabled for power reset ? */
	  if(alarm[i].condition[0].power_reset == ENABLED)
        send_mail(i + 1); /* Send the actual email */
	  
	  /* If that alarm's trap is enabled for power reset ? */
	  if(alarm[i].condition[1].power_reset == ENABLED)
        mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0); /* Send the actual trap */
	}
  }
}


/**************************************************************************************************
 Function:    ip_changed_alarm()
 Description: Send an Alarm when IP is changed.
 Input:       None
 Outout:      None
**************************************************************************************************/
void ip_changed_alarm(void)
cmx_reentrant
{
  int i;

  for(i = 0; i < 5; i++)
  {
    if(alarm[i].enable == TRUE)
	{
	  if(alarm[i].condition[0].ip_changed == ENABLED)
        send_mail(i + 1);
	  if(alarm[i].condition[1].ip_changed == ENABLED)
        mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0);
	}
  }
  flag.ip_changed = FALSE;
}


/**************************************************************************************************
 Function:    isvr_accessed_alarm()
 Description: Send an Alarm when Iserver is accessed.
 Input:       None
 Outout:      None
**************************************************************************************************/
void isvr_accessed_alarm(void)
cmx_reentrant
{
  int i;

  for(i = 0; i < 5; i++)
  {
    if(alarm[i].enable == TRUE)
	{
	  if(alarm[i].condition[0].isvr_accessed == ENABLED)
        send_mail(i + 1);
	  if(alarm[i].condition[1].isvr_accessed == ENABLED)
        mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0);
	}
  }
  flag.isvr_accessed = FALSE;
}

/**************************************************************************************************
 Function:    isvr_disconnect_alarm()
 Description: Send an Alarm when iServer is disconnected
 Input:       None
 Outout:      None
**************************************************************************************************/
void isvr_disconnect_alarm(void)
cmx_reentrant
{
  int i;

  for(i = 0; i < 5; i++)
  {
    if(alarm[i].enable == TRUE)
	{
	  if(alarm[i].condition[0].isvr_disconnect == ENABLED)
	  {
        send_mail(i + 1);
		alarm[i].reminder_interval_counter = 0;
	  }
	  if(alarm[i].condition[1].isvr_disconnect == ENABLED)
      {
	    mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0);
		alarm[i].reminder_interval_counter = 0;
	  }
	}
  }
  flag.isvr_disconnect = FALSE;
}


byte test_pin_condition(byte pin_no,byte alarm_no)
{
  byte ret_val;
  byte ip;
  
  ip = 0;
  ret_val = 0;
  
  ip = test_bit(2,6); 
      
  /* Alarm Pin condition is Low and the present GPIO is Low too */ 
	if((alarm[alarm_no].pin_condition == 0) && (ip == 0))
    {
      ret_val = 1;
	}
	/* Alarm Pin condition is High and the present GPIO is High too */ 
	if((alarm[alarm_no].pin_condition == 1) && (ip == 1))
	{
	  ret_val = 1;
	}

  return ret_val;
}

/**************************************************************************************************
 Function:    isvr_condition_alarm()
 Description: Send an Alarm when iServer condition is met.
 Input:       None
 Outout:      None
**************************************************************************************************/
void isvr_condition_alarm(void)
cmx_reentrant
{
  int i;
  int gpio_list[6] = {6,3,5,4,2,7};

  if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
  {
    for(i = 0; i < 5; i++)
    {
      if(alarm[i].enable == TRUE)
	  {
	    /* Only if the direction of GPIO 1 is input and alarm enabled */
        if(gpio.direction[0] == 1)
        {
          if(test_pin_condition(6,i) > 0)
	      {
	        if(alarm[i].condition[0].isvr_condition == ENABLED)
		    {
		      send_mail(i + 1); /* Send an alarm */
			  alarm[i].reminder_interval_counter = 0;
		    }
		    if(alarm[i].condition[1].isvr_condition == ENABLED)
		    {
		      mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0);
			  alarm[i].reminder_interval_counter = 0;
		    }
		    flag.isvr_condition = FALSE;
	      }
	    }
      }  
    }
  }
  else if((iserver.model == EIT_PCB_T))
  {
    /* Flag=TRUE when we need to send email/trap, Flag=FALSE when message/trap already sent */ 
    if(flag.isvr_condition == TRUE)
    {
      for(i = 0; i < 5; i++)
      {
        if(alarm[i].enable == ENABLED)
        {
          /* Only if the direction of GPIO is input and alarm enabled */
          if(gpio.direction[(alarm[i].input_pin - 1)] == 1)
          {
            if(test_pin_condition(gpio_list[(alarm[i].input_pin - 1)],i) > 0)
	        {
	          if(alarm[i].condition[0].isvr_condition == ENABLED)
			  {
			    send_mail(i + 1); /* Send an alarm */
		      }
			  if(alarm[i].condition[1].isvr_condition == ENABLED)
			  {
			    mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0);
			  }
			  flag.isvr_condition = FALSE;
	        }
          }
	    }
      }
    }
  }
}

/**************************************************************************************************
 Function:    isvr_char_alarm()
 Description: Send an Alarm when Iserver is accessed.
 Input:       None
 Outout:      None
**************************************************************************************************/
void isvr_char_alarm(void)
cmx_reentrant
{
  int i;

  for(i = 0; i < 5; i++)
  {
    if(alarm[i].enable == TRUE)
	{
	  if(alarm[i].condition[0].chars == ENABLED)
        send_mail(i + 1);
	  if(alarm[i].condition[1].chars == ENABLED)
        mn_snmp_trap(sysDescr_OID0,0,0,1,snmp.trap_ip,0);
	}
  }
  flag.isvr_char1 = FALSE;
  flag.isvr_char2 = FALSE;
}

