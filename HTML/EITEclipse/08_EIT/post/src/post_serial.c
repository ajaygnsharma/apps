/*------------------------------------------------------------------------ 
					post_network.c

Description:	Return SSIs for network.htm page
				1. DHCP Flag Enabled/Disabled
                2. Autonegotiation Enabled/Disabled
				3. 10 Mbps Enabled / Disabled
				4. 100 Mbps Enabled / Disabled
				5. Full Duplex Enabled/Disabled
                6. Half Duplex Enabled/Disabled

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

/*------------------------------------------------------------------------
 Function:    serial_cfg_func()
 Description: POST FUNCTION: Update np_serial.htm Page settings 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_serial(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"B",temp_buff))
    serial.baudrate=np_atoi((char *)temp_buff);                /* Baudrate */
  		
  if(mn_http_find_value(BODYptr,(byte *)"D",temp_buff))
    serial.databits=temp_buff[0]-'0';                       /* Databits */
  	
  if(mn_http_find_value(BODYptr,(byte *)"P",temp_buff))
    serial.parity=temp_buff[0]-'0';                           /* Parity */
    
  if(mn_http_find_value(BODYptr,(byte *)"S",temp_buff))
    serial.stopbits=temp_buff[0]-'0';                     /* Stop bits  */
  	
  if(mn_http_find_value(BODYptr,(byte *)"F",temp_buff))
    serial.flowcontrol=temp_buff[0]-'0';                /* Flow Control */
  
  if(mn_http_find_value(BODYptr,(byte *)"R4",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        flag.rs485type = 1;  break;  
      case '2': 
        flag.rs485type = 2;  break;  
      default : 
        flag.rs485type = 1;  break;
    }
  }


  if(mn_http_find_value(BODYptr,(byte *)"rs485timer",temp_buff))
    serial.rs485timer = np_atoi((char *)temp_buff);
  

  if(mn_http_find_value(BODYptr,(byte *)"rs485txrx",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        flag.rs485txrx = 1;  break;       /* RS485 Transmit             */
      case '2': 
        flag.rs485txrx = 2;  break;       /* RS485 Recieve             */
      default : 
        flag.rs485txrx = 1;  break;       /* RS485 Transmit            */
    }
  }
    
  write_all();       /* Write all this updated information to the flash */

  if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
  { 
    rs232_full_init_uart();
  }
  else if((iserver.model == EIT_W_4) || (iserver.model == EIT_D_4) || (iserver.model == EIT_PCB_4))
  {
    rs485_init_uart();
  }
  else
  {
    rs232_gpio_ttl_init_uart();
  }

  mn_http_set_file(socket_ptr, mn_vf_get_entry("serial.htm"));
}


/*------------------------------------------------------------------------
 Function:    serial_cfg_func()
 Description: POST FUNCTION: Update np_serial.htm Page settings 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_bridge(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"N",temp_buff))
  {
    terminal_server.no_connections= (temp_buff[0]-'0') - 1;      /* No of Conns*/
  }			

  if(mn_http_find_value(BODYptr,(byte *)"lp",temp_buff))
  {
    terminal_server.local_port=np_atoi((char *)temp_buff);   /* Local Port */
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"connect_on",temp_buff))
  {
    terminal_server.connect_on = np_atoi((char *)temp_buff);   /* TErminal Server Connect on */
  }
  
  
  /* Remote Host Information */
  if(mn_http_find_value(BODYptr,(byte *)"ren",temp_buff))
  {
    tunneling.remote_enable = 1;            /* Tunneling Remote Disable */
  }
  else
  {
    tunneling.remote_enable = 0;             /* Tunneling Remote Enable */
  }	
  
  if(mn_http_find_value(BODYptr,(byte *)"RIP",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(tunneling.remote_ip,temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      temp_buff[0]=0x00;
      temp_buff[1]=0x00;
      temp_buff[2]=0x00;
      temp_buff[3]=0x00;
      memcpy(tunneling.remote_ip,temp_buff,IP_ADDR_LEN);   
    }
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"RP",temp_buff))
  {
    tunneling.remote_port=np_atoi((char *)temp_buff);       /* Remote Port */
  }

  if(mn_http_find_value(BODYptr,(byte *)"retry_timer",temp_buff))
  {
    tunneling.retry_timer = np_atoi((char *)temp_buff);    /* Remote Timer */
  }

  if(mn_http_find_value(BODYptr,(byte *)"remote_connect_on",temp_buff))
  {
    tunneling.connect_on = temp_buff[0] - '0';
  }

  write_all();       /* Write all this updated information to the flash */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("bridge.htm"));
}



/*------------------------------------------------------------------------
 Function:    post_packaging()
 Description: POST FUNCTION: Update packaging.htm Page settings 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_packing(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"E",temp_buff))
  {
    sscanf((char *)temp_buff,"%x",&serial.endchar);         /* Endchar  */
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"FW",temp_buff))
  {
    serial.fwdendchar = 1;                                /* Fwdendchar */
  }
  else
  {
    serial.fwdendchar = 0;                                /* Fwdendchar */
  }
	
  if(mn_http_find_value(BODYptr,(byte *)"bt",temp_buff))
  {
    serial.buffering_time = np_atoi((char *)temp_buff);         /* Timeout */
  }

  if(mn_http_find_value(BODYptr,(byte *)"pl",temp_buff))
  {
    serial.pkt_len = np_atoi((char *)temp_buff);          /* Packet Length */
  }
		
  if(mn_http_find_value(BODYptr,(byte *)"it",temp_buff))
  {
    terminal_server.timeout=np_atoi((char *)temp_buff);/* Inactivity Tmout */
  }

  if(mn_http_find_value(BODYptr,(byte *)"disconnect_by",temp_buff))
  {
    terminal_server.disconnect_by = temp_buff[0] - '0';
  }

  write_all();       /* Write all this updated information to the flash */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("packing.htm"));
}


/*------------------------------------------------------------------------
 Function:    post_multihost)
 Description: POST FUNCTION: Update multihost.htm Page settings 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_multihost(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  unsigned int i;

  if(mn_http_find_value(BODYptr,(byte *)"connection_type",temp_buff))
  {
    multihost.connection_type = temp_buff[0] - '0';
  }

  for(i = 0; i < 12; i++)
  {
    if(mn_http_find_value(BODYptr,(byte *)"retry_counter",temp_buff))
    {
      multihost.retry_counter = np_atoi((char *)temp_buff);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"retry_timeout",temp_buff))
  {
    multihost.retry_timeout = np_atoi((char *)temp_buff);
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip1",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[0],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
#if(DEBUG_LEVEL == HIGH)	  
	  printf("multihost.rip[0] = %d.%d.%d.%d\r\n",multihost.rip[0][0],\
	    multihost.rip[0][1],multihost.rip[0][2],multihost.rip[0][3]);
#endif
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
	  memcpy(multihost.rip[0],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip2",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[1],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
	  memcpy(multihost.rip[1],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip3",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[2],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[2],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip4",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[3],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[3],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip5",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[4],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[4],temp_buff,IP_ADDR_LEN);
    }
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"mip6",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[5],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[5],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip7",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[6],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[6],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip8",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[7],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[7],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip9",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[8],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[8],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip10",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[9],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[9],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip11",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[10],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[10],temp_buff,IP_ADDR_LEN);
    }
  }

  if(mn_http_find_value(BODYptr,(byte *)"mip12",temp_buff))
  {
    if(s_ip_process(temp_buff,0))
    {
      memcpy(multihost.rip[11],temp_buff,IP_ADDR_LEN);   /* Remote IP  */
    }
    else
    {
      memset(temp_buff,NULL,IP_ADDR_LEN);
      memcpy(multihost.rip[11],temp_buff,IP_ADDR_LEN);
    }
  }
  
  /*----------------Remote Ports------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"mp1",temp_buff))
    multihost.rp[0] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp2",temp_buff))
    multihost.rp[1] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp3",temp_buff))
    multihost.rp[2] = np_atoi((char *)temp_buff);       
  
  if(mn_http_find_value(BODYptr,(byte *)"mp4",temp_buff))
    multihost.rp[3] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp5",temp_buff))
    multihost.rp[4] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp6",temp_buff))
    multihost.rp[5] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp7",temp_buff))
    multihost.rp[6] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp8",temp_buff))
    multihost.rp[7] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp9",temp_buff))
    multihost.rp[8] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp10",temp_buff))
    multihost.rp[9] = np_atoi((char *)temp_buff);       

  if(mn_http_find_value(BODYptr,(byte *)"mp11",temp_buff))
    multihost.rp[10] = np_atoi((char *)temp_buff);      
  
  if(mn_http_find_value(BODYptr,(byte *)"mp12",temp_buff))
    multihost.rp[11] = np_atoi((char *)temp_buff);      

  if((multihost.connection_type == SIMULTANEOUS) || (multihost.connection_type == SEQUENTIAL))
  {
    for(i = 0; i < 12; i++)
    {
      if(multihost.rip[i][0] != NULL)
      {
        multihost.conns_to_make++;
      }  
    }
  }

  write_all();       /* Write all this updated information to the flash */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("multihost.htm"));
}
