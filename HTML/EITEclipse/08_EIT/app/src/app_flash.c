#include "micronet.h"
#include "np_includes.h"

/**************************************************************************************************
 Function:    flash_write_default()
 Description: Write defaults to the flash
 Input:       None
 Output:      None				
**************************************************************************************************/
void flash_write_default(void)
{
  DISABLE_INTERRUPTS;
  assume_defaults();  /* Assumer defaults first */
  write_all();		  /* Then Write the defaults */
  ENABLE_INTERRUPTS;
}


/**************************************************************************************************
Function: 		assume_defaults()
Description: 	Assume defaults while booting.. NOTE: 
                MAC Addr is not reset to default.
Input:			None
Output:			None
**************************************************************************************************/
void assume_defaults(void)
{
  int i,j,k;
  
  DISABLE_INTERRUPTS;
  
  /* Iserver information */
  strcpy(iserver.version,FIRMWARE_VERSION);/* Copy the IServer Ver  No: */
  
  /* Ethernet MAC */
  for(i = 0; i < ETH_ADDR_LEN; i++)
    eth_src_hw_addr[i] = eth_src_hw_addr[i];
  
  /* IP address */
  eth_src_addr[0] = DEFAULT_IP1;
  eth_src_addr[1] = DEFAULT_IP2;
  eth_src_addr[2] = DEFAULT_IP3;
  eth_src_addr[3] = DEFAULT_IP4;

  /* Subnet Mask */
  subnet_mask[0]= DEFAULT_NETMASK1;				 
  subnet_mask[1]= DEFAULT_NETMASK2;
  subnet_mask[2]= DEFAULT_NETMASK3; //0x00;
  subnet_mask[3]= DEFAULT_NETMASK4;		

  /* Gateway IP Address */	
  gateway_ip_addr[0]= DEFAULT_GW1;              
  gateway_ip_addr[1]= DEFAULT_GW2;
  gateway_ip_addr[2]= DEFAULT_GW3;
  gateway_ip_addr[3]= DEFAULT_GW4;
 
  /* DNS IP address */
  ip_dns_addr[0] = DEFAULT_DNS1;                
  ip_dns_addr[1] = DEFAULT_DNS2;
  ip_dns_addr[2] = DEFAULT_DNS3;
  ip_dns_addr[3] = DEFAULT_DNS4;
  
  /* Hostname */
  strcpy(network.hostname,"eit");
  sprintf((char *)temp_buff,"%0.2X%0.2X",eth_src_hw_addr[4],eth_src_hw_addr[5]);
  strcat(network.hostname,(char *)temp_buff);  
  
  /* Passwords */
  strcpy(password.login,"12345678");
  password.login[LOGIN_PASS_LEN] = '\0';
  
  strcpy(password.admin,"00000000");
  password.admin[ADMIN_PASS_LEN]='\0';	

  memset(password.serial,NULL,SERIAL_PASS_LEN);
  password.serial[SERIAL_PASS_LEN]='\0';	

  /* Device checkbox */
  device.check=0x00;	          /* Default: All devices are unchecked */
  for(i=0;i<MAX_UNITS;i++)
    strcpy(device.name[i],"");
  
  /* Device Command */
  for(i=0;i<MAX_UNITS;i++)
   strcpy(device.command[i],"");
  
  /* Device Unit */
  for(i=0;i<MAX_UNITS;i++)
    strcpy(device.unit[i],"");
  
  /* Device ID */
  for(i=0; i< 2 ;i++)
    strcpy(device.uid[i],"0");
  
  /* Serial Settings */
  serial.baudrate                = DEFAULT_BAUD;
  serial.databits                = DEFAULT_DATA_BITS;									   
  serial.parity                  = DEFAULT_PARITY;	  
  serial.stopbits                = DEFAULT_STOP_BITS;	
  serial.flowcontrol             = DEFAULT_FLOW_CONTROL;	
  
  /* Ethernet to Serial Configurations */
  terminal_server.no_connections = DEFAULT_NO_CONNECTIONS;
  terminal_server.local_port     = DEFAULT_LOCAL_PORT;
  
  ///////////////////////////////////////////////////////
  tunneling.remote_enable        = DISABLED;
  tunneling.remote_ip[0]         = DEFAULT_REMOTE_IP1;
  tunneling.remote_ip[1]         = DEFAULT_REMOTE_IP2;
  tunneling.remote_ip[2]         = DEFAULT_REMOTE_IP3;
  tunneling.remote_ip[3]         = DEFAULT_REMOTE_IP4;
  tunneling.remote_port          = DEFAULT_REMOTE_PORT;
  //////////////////////////////////////////////////////////

  /* Packing techniques */
  serial.endchar                 = DEFAULT_END_CHAR;                        
  serial.fwdendchar              = DEFAULT_FWD_END_CHAR;		
  serial.buffering_time          = DEFAULT_BUFFERING_TIME;
  terminal_server.timeout        = MN_SOCKET_INACTIVITY_TIME;

  /* Different flags */
  flag.dhcp                      = DEFAULT_FLAG_DHCP;
  flag.autonegotiation           = DEFAULT_FLAG_AUTO;
  flag.speed                     = DEFAULT_FLAG_SPEED;                          
  flag.duplex                    = DEFAULT_FLAG_DUPLEX;                             
  flag.snmp                      = DEFAULT_FLAG_SNMP;  
  flag.smtp                      = DISABLED;                             
  flag.serial_console            = DISABLED;             
  flag.internet_console          = DISABLED;
  flag.webserver                 = ENABLED;
  flag.HTTP_PORT                 = DEFAULT_HTTP_PORT;		//wendy
  terminal_server.protocol       = TCP_PROTOCOL;
  
  /* SMTP Server IP */
  smtp.server_ip[0]              = SMTP_SVR_IP1;
  smtp.server_ip[1]              = SMTP_SVR_IP2;
  smtp.server_ip[2]              = SMTP_SVR_IP3;
  smtp.server_ip[3]              = SMTP_SVR_IP4;	
  
  /* SMTP Email Address Length */
  memset(smtp.from,EMAIL_LEN,NULL);
  /* SMTP Subject length */
  memset(smtp.subject,30,NULL);
  
  flag.trap_en                   = 0;
  
  /* SNMP Trap Svr IP */
  memset(snmp.trap_ip,IP_ADDR_LEN,NULL);
  
  /* Set All the alarms information */
  for(i = 0; i < 5; i++)
  {
    alarm[i].enable        = 0;
    alarm[i].input_pin     = 0;
    alarm[i].pin_condition = 0;
    alarm[i].char1         = 0;
    alarm[i].char2         = 0;
	
	for(j = 0; j < 2; j++)
	{
	  alarm[i].condition[j].power_reset     = 0;
      alarm[i].condition[j].ip_changed      = 0;
      alarm[i].condition[j].isvr_accessed   = 0;
      alarm[i].condition[j].isvr_disconnect = 0;
      alarm[i].condition[j].isvr_condition  = 0;
      alarm[i].condition[j].chars           = 0;
	}
    
	alarm[i].reminder_interval = DEFAULT_ALARM0_REMINDER_INTERVAL;
  
    for(k = 0; k < 50; k++)
    {
      alarm[i].recipients[k] = NULL;
    }

    for(k = 0; k < 30; k++)
    {
      alarm[i].msg[k] = NULL;
    }
  }

  /* RS485 type */
  flag.rs485type = DEFAULT_GPIO_RS232_RS485;
  
  /* GPIO type: forced or Conditional */
  flag.gpio_type = DEFAULT_GPIO_TYPE;
  
  /* GPIO information */
  for(i = 0; i < 8; i++)
  {
    gpio.direction[i] = 1;
  }
  
  for(i = 0; i < 8; i++)
  {
    gpio.op_level[i] = 1;
  }
  
  for(i = 0; i < 4; i++)
  {
	gpio.if_condition[i] = 1;
  }

  for(i = 0; i < 4; i++)
  {
	gpio.then_condition[i] = 1;
  }

  serial.pkt_len                        = DEFAULT_PACKET_LENGTH;
  flag.rs485txrx                        = DEFAULT_FLAG_RS485TXRX;
  serial.rs485timer                     = DEFAULT_RS485_TIMER;

  /* IP Security */
  for(i = 0; i < NO_EX_IPS; i++)
  {
    for(j = 0; j < IP_ADDR_LEN; j++)
    {
      exclusivity.ip[i][j] = 0x00;
    }
  }
  
  /* Multihost Connections */
  terminal_server.connect_on            = DEFAULT_CONNECT_ON;
  tunneling.connect_on                  = DEFAULT_CONNECT_ON;
  terminal_server.disconnect_by         = DEFAULT_DISCONNECT_BY;
  tunneling.retry_timer                 = DEFAULT_TUNNELING_RETRY_TIMER;
  multihost.connection_type             = DEFAULT_CONNECTION_TYPE;
  multihost.retry_counter               = DEFAULT_MULTIHOST_RETRY_COUNTER;
  multihost.retry_timeout               = DEFAULT_MULTIHOST_RETRY_TIMEOUT;

  for(i = 0; i < 12; i++)
  {
    for(j = 0; j < IP_ADDR_LEN; j++)
    {
      multihost.rip[i][j] = DEFAULT_MULTIHOST_RIP;
    }
  }

  for(i = 0; i < 12; i++)
  {
    multihost.rp[i]                     = DEFAULT_MULTIHOST_REMOTE_PORT;
  }

  /* SNMP Information */
  for(i = 0; i < SNMP_CONTACT_LEN; i++)
  {
    snmp.contact[i]                     = DEFAULT_SNMP_CONTACT;
  }

  for(i = 0; i < SNMP_COMMUNITY_LEN; i++)
  {
    snmp.community[i]                   = DEFAULT_SNMP_COMMUNITY;
  }
  
  for(i = 0; i < SNMP_LOCATION_LEN; i++)
  {
    snmp.location[i]                    = DEFAULT_SNMP_LOCATION;
  }
  
  /* Security enabled or disabled */
  flag.security = 0;

  ENABLE_INTERRUPTS;
}


/**************************************************************************************************
Function: 		read_flash()
Description: 	Read all the values from the Flash 
Input:			None
Output:			None
**************************************************************************************************/
void read_flash(void)
{
  char temp_buff[20];
  int i,j;
  unsigned char temp_buffer[TEMP_BUFFER_SIZE];

  DISABLE_INTERRUPTS;
  j=0;
  
  /* Clears the buffer */
  memset(temp_buff,NULL,sizeof(temp_buff));                       
  memset(temp_buffer,NULL,sizeof(temp_buffer));
  
  /* First assume everything as defaults */
  assume_defaults();
  
  /* then read from the flash to the temp_buffer */
  _fl_read(temp_buffer,FLASH_START_ADDR,TEMP_BUFFER_SIZE);/* Read all */
  
#if(DEBUG_LEVEL == HIGH)
  for(i = 0; i < TEMP_BUFFER_SIZE; i++)
  {
    printf("temp_buffer[%d]=%0.2X\n\r",i,temp_buffer[i]);
  }
#endif

  /* Just a quick and dirty sanity check to see if the contents read are all correct or not ? 
  If a lot of information read is 0xff then something is wrong with the flash */
  for(i = 0; i < 1024; i++)
  {
    if(temp_buffer[i] < 0xff)
	  j++;
	else
	  j--;
  }
  
  /* If atleast 500 bytes of information read is good then we continue to read all the variables */
  if(j > 500)
  {
    /* MAC Address */
	for(i = 0; i < ETH_ADDR_LEN; i++)
  	  eth_src_hw_addr[i] = temp_buffer[i + FLASH_ADDR_MAC];                   
  	
    /* IP Address */
	for(i = 0; i < IP_ADDR_LEN; i++)
  	  eth_src_addr[i] = temp_buffer[i + FLASH_ADDR_IP];                   
  	
    /* Subnet Mask */
	for(i = 0; i < IP_ADDR_LEN; i++)
	  subnet_mask[i] = temp_buffer[i + FLASH_ADDR_NETMASK];
  	
	/* Gateway IP */
    for(i = 0; i < IP_ADDR_LEN; i++)
	  gateway_ip_addr[i] = temp_buffer[i + FLASH_ADDR_GATEWAY];
  	
    /* DNS Address */
	for(i = 0; i < IP_ADDR_LEN; i++)
	  ip_dns_addr[i] = temp_buffer[i + FLASH_ADDR_DNS];
    
	/* Hostname */
    for(i = 0; i < HOSTNAME_LEN; i++)
     network.hostname[i] = temp_buffer[FLASH_ADDR_HOSTNAME + i];
    network.hostname[HOSTNAME_LEN] = '\0';
  
	/* Passwords */
    for(i = 0; i < LOGIN_PASS_LEN; i++)
      password.login[i] = temp_buffer[FLASH_ADDR_LOGIN_PASS + i];
    password.login[LOGIN_PASS_LEN] = '\0';
  
    for(i = 0; i < ADMIN_PASS_LEN; i++)
      password.admin[i] = temp_buffer[FLASH_ADDR_ADMIN_PASS + i];
    password.admin[SERIAL_PASS_LEN] = '\0';

    for(i = 0; i < SERIAL_PASS_LEN; i++)
      password.serial[i] = temp_buffer[FLASH_ADDR_SERIAL_PASS + i];
    password.serial[SERIAL_PASS_LEN] = '\0';
  
  
    /*--------------------Device Configuration --------------------------------------------------*/
    for(i=0;i<MAX_UNITS;i++)
    {
      for(j = 0; j < 8; j++)
	  {
        device.name[i][j] = temp_buffer[(i * 8) + j + FLASH_ADDR_DEVICE_NAME];
      }
    }

    for(i=0;i<MAX_UNITS;i++)
    {
      for(j = 0; j < 8; j++)
      {
        device.command[i][j] = temp_buffer[(i * 8) + j + FLASH_ADDR_DEVICE_COMMAND];
      }
    }

    for(i=0;i<MAX_UNITS;i++)
    {
      for(j = 0; j < 8; j++)
      {
        device.unit[i][j]=temp_buffer[(i * 8) + j + FLASH_ADDR_DEVICE_UNIT];
      }
    }

    for(i=0;i< 2 ;i++) //Should 2 be MAX_UNITS ? 
    {
      device.uid[i][0]=temp_buffer[j+270];	                     /* YES */
	  device.uid[i][1]=temp_buffer[j+270];
      j=j+2;
    }

    device.check=temp_buffer[FLASH_ADDR_DEVICE_CHECK];    /* Check Mark */
  
    for(i=0;i<MAX_UNITS;i++)
    {
      device.format[i] = temp_buffer[i + FLASH_ADDR_DEVICE_FORMAT];/* YES */
    }

    /*--------------------Serial Port -----------------------------------------------------------*/
    serial.baudrate    = temp_buffer[FLASH_ADDR_BAUD];         /* Baudrate */
    serial.databits    = temp_buffer[FLASH_ADDR_DATA_BITS];    /* Data bits */
    serial.parity      = temp_buffer[FLASH_ADDR_PARITY];       /* Parity */
    serial.stopbits    = temp_buffer[FLASH_ADDR_STOP_BITS];	/* Stop bits */
    serial.flowcontrol = temp_buffer[FLASH_ADDR_FLOW_CONTROL]; /* Flow Control */
  
    /*------------------------ Serial to Ethernet Connections ----------------------------------*/
    terminal_server.no_connections=temp_buffer[FLASH_ADDR_NO_CONNECTIONS];
    
	memset(temp_buff,NULL,20);                       /* Clears the buffer */
    memcpy(temp_buff,&temp_buffer[FLASH_ADDR_LOCAL_PORT],5);
    terminal_server.local_port = np_np_atoi(temp_buff);
    
	tunneling.remote_enable = temp_buffer[FLASH_ADDR_REMOTE_ENABLE];
  
    ///////////////////////////////////////////////////////////////////////////////////////////////
	/*---------------------- Remote Tunneling --------------------------------------------------*/
    tunneling.remote_ip[0] = temp_buffer[FLASH_ADDR_REMOTE_IP];
    tunneling.remote_ip[1] = temp_buffer[FLASH_ADDR_REMOTE_IP + 1];
    tunneling.remote_ip[2] = temp_buffer[FLASH_ADDR_REMOTE_IP + 2];
    tunneling.remote_ip[3] = temp_buffer[FLASH_ADDR_REMOTE_IP + 3];
  	///////////////////////////////////////////////////////////////////////////////////////////////
		
    memset(temp_buff,NULL,20);                     /* Clears the buffer */
	memcpy(temp_buff,&temp_buffer[FLASH_ADDR_REMOTE_PORT],5);
	tunneling.remote_port = np_np_atoi(temp_buff);
  
    serial.endchar = temp_buffer[FLASH_ADDR_END_CHAR]; /* End Character */
    serial.fwdendchar = temp_buffer[FLASH_ADDR_FWD_END_CHAR];
  
    memset(temp_buff,NULL,20);                     /* Clears the buffer */
	memcpy(temp_buff,&temp_buffer[FLASH_ADDR_BUFFERING_TIME],4);
	serial.buffering_time = np_np_atoi(temp_buff);

    memset(temp_buff,NULL,20);
	memcpy(temp_buff,&temp_buffer[FLASH_ADDR_INACTIVITY_TIMEOUT],4);
	terminal_server.timeout = np_np_atoi(temp_buff);
  
  
    /*---------------Flags ----------------------------------------------------------------------*/
    flag.dhcp             = temp_buffer[FLASH_ADDR_DHCP];
    flag.autonegotiation  = temp_buffer[FLASH_ADDR_AUTONEGOTIATION];
    flag.speed            = temp_buffer[FLASH_ADDR_SPEED];
    flag.duplex           = temp_buffer[FLASH_ADDR_DUPLEX];
    flag.snmp             = temp_buffer[FLASH_ADDR_SNMP];
    flag.smtp             = temp_buffer[FLASH_ADDR_SMTP];                      
    flag.serial_console   = temp_buffer[FLASH_ADDR_SERIAL_CONSOLE];     
    flag.internet_console = temp_buffer[FLASH_ADDR_INTERNET_CONSOLE];
    flag.webserver        = temp_buffer[FLASH_ADDR_WEBSERVER];    
    terminal_server.protocol = temp_buffer[FLASH_ADDR_PROTOCOL];  
	if(terminal_server.protocol == MODBUS_PROTOCOL)
    {
      terminal_server.local_port = 502;     /* Assign the Modbus Protocol Port 502 */
    }
	else
	{
	  memset(temp_buff,NULL,20);                       /* Clears the buffer */
      memcpy(temp_buff,&temp_buffer[FLASH_ADDR_LOCAL_PORT],5);
      terminal_server.local_port = np_np_atoi(temp_buff);
	}       

			   //wendy
	memset(temp_buff,NULL,20);                       /* Clears the buffer */
    memcpy(temp_buff,&temp_buffer[FLASH_HTTP_PORT],5);
    flag.HTTP_PORT = np_np_atoi(temp_buff);

	/* SMTP Information */
	//wendy
    smtp.server_ip[0] = temp_buffer[FLASH_ADDR_SMTP_IP + 0];
    smtp.server_ip[1] = temp_buffer[FLASH_ADDR_SMTP_IP + 1];
    smtp.server_ip[2] = temp_buffer[FLASH_ADDR_SMTP_IP + 2];
    smtp.server_ip[3] = temp_buffer[FLASH_ADDR_SMTP_IP + 3];

    for(i = 0; i < 30; i++)
	{
	  smtp.from[i] = temp_buffer[FLASH_ADDR_SMTP_FROM + i];
    }
	smtp.from[30] = '\0';
  
    for(i = 0; i < 30; i++)
	{
	  smtp.subject[i] = temp_buffer[FLASH_ADDR_SMTP_SUBJECT + i];
    }
	smtp.subject[30] = '\0';
  
    /*--------------SNMP Configuration ---------------------------------*/
    flag.trap_en = temp_buffer[FLASH_ADDR_TRAP_ENABLE];
  
    for(i = 0; i < ETH_ADDR_LEN; i++)
  	  snmp.trap_ip[i] = temp_buffer[FLASH_ADDR_TRAP_IP + i];
    
    for(i = 0; i < 5; i++)
	{
	  /*--------------Alarm[i] Configuration ---------------------------*/
      alarm[i].enable        = temp_buffer[FLASH_ADDR_ALARM0_ENABLE        + (i*5)];
      alarm[i].input_pin     = temp_buffer[FLASH_ADDR_ALARM0_INPUT_PIN     + (i*5)];
      alarm[i].pin_condition = temp_buffer[FLASH_ADDR_ALARM0_PIN_CONDITION + (i*5)];
      alarm[i].char1         = temp_buffer[FLASH_ADDR_ALARM0_CHAR1         + (i*5)];
      alarm[i].char2         = temp_buffer[FLASH_ADDR_ALARM0_CHAR2         + (i*5)];
  
      /* -----------------------Emails -------------------------------------*/
      alarm[i].condition[0].power_reset     = temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 0];
      alarm[i].condition[0].ip_changed      = temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 1];
      alarm[i].condition[0].isvr_accessed   = temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 2];
      alarm[i].condition[0].isvr_disconnect = temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 3];
      alarm[i].condition[0].isvr_condition  = temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 4];
      alarm[i].condition[0].chars           = temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 5];
  
      /* -----------------------Traps --------------------------------------*/
      alarm[i].condition[1].power_reset     = temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 0];
      alarm[i].condition[1].ip_changed      = temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 1];
      alarm[i].condition[1].isvr_accessed   = temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 2];
      alarm[i].condition[1].isvr_disconnect = temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 3];
      alarm[i].condition[1].isvr_condition  = temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 4];
      alarm[i].condition[1].chars           = temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 5];

	  memset(temp_buff,NULL,20);
	  memcpy(temp_buff,&temp_buffer[FLASH_ADDR_ALARM0_REMINDER_INTERVAL + (i * 4)],4);
	  alarm[i].reminder_interval = np_np_atoi(temp_buff);

      for(j = 0; j < EMAIL_ADDR_BUF_LEN; j++)
        alarm[i].recipients[j] = temp_buffer[FLASH_ADDR_ALARM0_RECIPIENTS + (i * EMAIL_ADDR_BUF_LEN) + j];
      alarm[i].recipients[j] = '\0';

      for(j = 0; j < MSG_BUF_LEN; j++)
        alarm[i].msg[j] = temp_buffer[FLASH_ADDR_ALARM0_MSG + (i * MSG_BUF_LEN) + j];
      alarm[i].msg[j] = '\0';
  	}
	


	/*-------------------------- GPIOs----------------------------------*/
    flag.rs485type = temp_buffer[FLASH_ADDR_RS232_485_GPIO];
    flag.gpio_type = temp_buffer[FLASH_ADDR_GPIO_TYPE];
  
    for(i = 0; i < 6; i++)
    {
      gpio.direction[i] = temp_buffer[FLASH_ADDR_GPIO_DIRECTION + i];
    }
  
    for(i = 0; i < 6; i++)
    {
      gpio.op_level[i] = temp_buffer[FLASH_ADDR_OP_LEVEL + i];
    }

	for(i = 0; i < 3; i++)
    {
      gpio.if_condition[i] = temp_buffer[FLASH_ADDR_IF_CONDITION + i];
    }

	for(i = 0; i < 3; i++)
    {
      gpio.then_condition[i] = temp_buffer[FLASH_ADDR_THEN_CONDITION + i];
    }
  
	memset(temp_buff,NULL,20);
	memcpy(temp_buff,&temp_buffer[FLASH_ADDR_PACKET_LENGTH],4);
	serial.pkt_len = np_np_atoi(temp_buff);
    
	flag.rs485txrx = temp_buffer[FLASH_ADDR_RS485_TX_RX];
	memset(temp_buff,NULL,20);
	memcpy(temp_buff,&temp_buffer[FLASH_ADDR_RS485_TIMER],4);
	serial.rs485timer = np_np_atoi(temp_buff);
    
	/* IP exclusivity */
    for(i = 0; i < NO_EX_IPS; i++)
    {
      for(j = 0; j < IP_ADDR_LEN; j++)
      {
        exclusivity.ip[i][j] = temp_buffer[FLASH_ADDR_IP_EX_1 + (IP_ADDR_LEN * i) + j];
      }
    }

    terminal_server.connect_on = temp_buffer[FLASH_ADDR_TERMINAL_SERVER_CONNECT_ON];
    tunneling.connect_on = temp_buffer[FLASH_ADDR_TUNNELING_CONNECT_ON];
    terminal_server.disconnect_by = temp_buffer[FLASH_ADDR_DISCONNECT_BY];

	/* Multihost Connections */
	memset(temp_buff,NULL,20);
	memcpy(temp_buff,&temp_buffer[FLASH_ADDR_TUNNELING_RETRY_TIMER],4);
	tunneling.retry_timer = np_np_atoi(temp_buff);
	
	multihost.connection_type = temp_buffer[FLASH_ADDR_CONNECTION_TYPE];
	
	multihost.retry_counter = temp_buffer[FLASH_ADDR_MULTIHOST_RETRY_COUNTER];
    
	memset(temp_buff,NULL,20);
    memcpy(temp_buff,&temp_buffer[FLASH_ADDR_MULTIHOST_RETRY_TIMEOUT],4);
    multihost.retry_timeout = np_np_atoi(temp_buff);

	for(i = 0; i < 12; i++)
    {
      for(j = 0; j < IP_ADDR_LEN; j++)
      {
        multihost.rip[i][j] = temp_buffer[FLASH_ADDR_MULTIHOST_RIP + (IP_ADDR_LEN * i) + j];
      } 
    }

	for(i = 0; i < 12; i++)
    {
      memset(temp_buff,NULL,20);
      memcpy(temp_buff,&temp_buffer[FLASH_ADDR_MULTIHOST_REMOTE_PORT + (i * 5)],5);
      multihost.rp[i] = np_np_atoi(temp_buff);
	}

	/* SNMP information */
    for(i = 0; i < SNMP_CONTACT_LEN; i++)
    {
      snmp.contact[i] = temp_buffer[FLASH_ADDR_SNMP_CONTACT + i];
    }

    for(i = 0; i < SNMP_COMMUNITY_LEN; i++)
    {
      snmp.community[i] = temp_buffer[FLASH_ADDR_SNMP_COMMUNITY + i]; 
    }

    for(i = 0; i < SNMP_LOCATION_LEN; i++)
    {
      snmp.location[i] = temp_buffer[FLASH_ADDR_SNMP_LOCATION + i]; 
    }

    flag.security = temp_buffer[FLASH_ADDR_SECURITY_FLAG];

    /* Remaining GPIOs */
    for(i = 0; i < 2; i++)
    {
      gpio.direction[6 + i] = temp_buffer[FLASH_ADDR_GPIO_DIRECTION_07 + i];
    }

    for(i = 0; i < 2; i++)
    {
      gpio.op_level[6 + i]  = temp_buffer[FLASH_ADDR_OP_LEVEL_07 + i];
    }

    gpio.if_condition[3]    = temp_buffer[FLASH_ADDR_IF_CONDITION_04];
  
    gpio.then_condition[3]  = temp_buffer[FLASH_ADDR_THEN_CONDITION_04];
  
	
	/* Read the VBflash table for SNMP */
    getVBTableFromFlash();
  }

  ENABLE_INTERRUPTS;
  
}

/**************************************************************************************************
Function:    write_all()
Description: Write all Values
Input:       None
Output:      None
**************************************************************************************************/
void write_all()
{
  unsigned long i,j;
  unsigned char temp_buffer[TEMP_BUFFER_SIZE];
  unsigned char temp_buff[20];
  
  DISABLE_INTERRUPTS;
  
  /* Clear all the buffers */
  memset(temp_buff,NULL,20);                       /* Clears the buffer */
  memset(temp_buffer,NULL,TEMP_BUFFER_SIZE);
  
  /* Set the VBflashtable for the SNMP */
  setVBTableToFlash();

  for(i = 0; i < ETH_ADDR_LEN; i++)
    temp_buffer[FLASH_ADDR_MAC + i] = eth_src_hw_addr[i];/*Ethernet MAC */
	
  for(i = 0; i < ETH_ADDR_LEN; i++)		   
  	temp_buffer[FLASH_ADDR_IP + i] = eth_src_addr[i];    /* IP Address  */
  	
  for(i = 0; i < ETH_ADDR_LEN; i++)
	temp_buffer[FLASH_ADDR_NETMASK + i] = subnet_mask[i];/* Subnet Mask */
  	
  for(i = 0; i < ETH_ADDR_LEN; i++)
    temp_buffer[FLASH_ADDR_GATEWAY + i] = gateway_ip_addr[i];/* Gateway */
  	
  for(i = 0; i < ETH_ADDR_LEN; i++)
    temp_buffer[FLASH_ADDR_DNS + i] = ip_dns_addr[i];  /* DNS Server IP */
  
  for(i = 0; i < HOSTNAME_LEN; i++)							  /*Hostname*/
  {
    temp_buffer[FLASH_ADDR_HOSTNAME + i] = network.hostname[i];
  }
  
  for(i = 0; i < LOGIN_PASS_LEN; i++)                 /* Login Password */
  {
    //if(password.login[i] == NULL)
    //  temp_buffer[FLASH_ADDR_LOGIN_PASS + i] = NULL;
    //else
      temp_buffer[FLASH_ADDR_LOGIN_PASS + i] = password.login[i];		  
  }
  
  for(i = 0; i < ADMIN_PASS_LEN; i++)				  /* Admin Password */
  {
    temp_buffer[FLASH_ADDR_ADMIN_PASS + i] = password.admin[i];		  
  }
  
  for(i = 0; i < SERIAL_PASS_LEN; i++)               /* Serial Password */
  {
    if(password.serial[i] == NULL)
      temp_buffer[FLASH_ADDR_SERIAL_PASS + i] = NULL;
    else
      temp_buffer[FLASH_ADDR_SERIAL_PASS + i] = password.serial[i];		 
  }
  
  /*-----------------------------------------------------------------------------------------------
  
  Device Names 

  -----------------------------------------------------------------------------------------------*/
  for(i=0; i<MAX_UNITS; i++)
  {
    for(j = 0; j < 8; j++)
    {
      temp_buffer[FLASH_ADDR_DEVICE_NAME + (i * 8) + j]  = device.name[i][j];
    }
  }

  for(i=0; i<MAX_UNITS; i++)
  {
    for(j = 0; j < 8; j++)
    {
      temp_buffer[FLASH_ADDR_DEVICE_COMMAND + (i * 8) + j] = device.command[i][j];  
    }
  }

  for(i=0; i<MAX_UNITS; i++)
  {
    for(j = 0; j < 8; j++)
    {
      temp_buffer[FLASH_ADDR_DEVICE_UNIT + (i * 8) + j] = device.unit[i][j];      
    }
  }
  
  for(i=0; i<MAX_UNITS; i++)
  {
    temp_buffer[FLASH_ADDR_DEVICE_FORMAT + i] = device.format[i];
  }

  temp_buffer[270] = device.uid[0][0];                    
  temp_buffer[270+1] = device.uid[0][1];
  temp_buffer[270+2] = device.uid[1][0];
  temp_buffer[270+3] = device.uid[1][1];
  
  temp_buffer[FLASH_ADDR_DEVICE_CHECK] = device.check;
  
  /* Serial port Settings */
  temp_buffer[FLASH_ADDR_BAUD]         = serial.baudrate;
  temp_buffer[FLASH_ADDR_DATA_BITS]    = serial.databits;
  temp_buffer[FLASH_ADDR_PARITY]       = serial.parity;
  temp_buffer[FLASH_ADDR_STOP_BITS]    = serial.stopbits;
  temp_buffer[FLASH_ADDR_FLOW_CONTROL] = serial.flowcontrol;

  /* Network to Serial */
  temp_buffer[FLASH_ADDR_NO_CONNECTIONS] = terminal_server.no_connections;
  sprintf((char *)temp_buff,"%d",terminal_server.local_port);
  if(strlen((char *)temp_buff) < 6)
  {
    memcpy(&temp_buffer[FLASH_ADDR_LOCAL_PORT],temp_buff,5);
  }

  temp_buffer[FLASH_ADDR_REMOTE_ENABLE]  = tunneling.remote_enable;
  
  temp_buffer[FLASH_ADDR_REMOTE_IP]      = tunneling.remote_ip[0];
  temp_buffer[FLASH_ADDR_REMOTE_IP+1]    = tunneling.remote_ip[1];
  temp_buffer[FLASH_ADDR_REMOTE_IP+2]    = tunneling.remote_ip[2];
  temp_buffer[FLASH_ADDR_REMOTE_IP+3]    = tunneling.remote_ip[3];	
  
  sprintf((char *)temp_buff,"%d",tunneling.remote_port);
  if(strlen((char *)temp_buff) < 6)
  {
    memcpy(&temp_buffer[FLASH_ADDR_REMOTE_PORT],temp_buff,5);
  }

  /* Packing technique */
  temp_buffer[FLASH_ADDR_END_CHAR]       = serial.endchar;	
  temp_buffer[FLASH_ADDR_FWD_END_CHAR]   = serial.fwdendchar;	
  
  sprintf((char *)temp_buff,"%d",serial.buffering_time);
  if(strlen((char *)temp_buff) < 5)
  {
    memcpy(&temp_buffer[FLASH_ADDR_BUFFERING_TIME],temp_buff,5);
  }

  sprintf((char *)temp_buff,"%d",terminal_server.timeout);
  if(strlen((char *)temp_buff) < 5)
  {
    memcpy(&temp_buffer[FLASH_ADDR_INACTIVITY_TIMEOUT],temp_buff,5);
  }
  
  /* Flags */
  temp_buffer[FLASH_ADDR_DHCP] = flag.dhcp;
  temp_buffer[FLASH_ADDR_AUTONEGOTIATION] = flag.autonegotiation; 
  temp_buffer[FLASH_ADDR_SPEED]           = flag.speed;          
  temp_buffer[FLASH_ADDR_DUPLEX]          = flag.duplex;
  temp_buffer[FLASH_ADDR_SNMP]            = flag.snmp;
  temp_buffer[FLASH_ADDR_SMTP]            = flag.smtp;
  temp_buffer[FLASH_ADDR_SERIAL_CONSOLE]  = flag.serial_console;  
  temp_buffer[FLASH_ADDR_INTERNET_CONSOLE]= flag.internet_console;
  temp_buffer[FLASH_ADDR_WEBSERVER]       = flag.webserver;                
  temp_buffer[FLASH_ADDR_PROTOCOL]        = terminal_server.protocol;
  if(terminal_server.protocol == MODBUS_PROTOCOL)
  {
    terminal_server.local_port = 502;     /* Assign the Modbus Protocol Port 502 */
  }
  else
  {
    memset(temp_buff,NULL,20);                       /* Clears the buffer */
    memcpy(temp_buff,&temp_buffer[FLASH_ADDR_LOCAL_PORT],5);
    terminal_server.local_port = np_np_atoi((char *)temp_buff);
    
	sprintf((char *)temp_buff,"%d",terminal_server.local_port);
    if(strlen((char *)temp_buff) < 6)
    {
      memcpy(&temp_buffer[FLASH_ADDR_LOCAL_PORT],temp_buff,5);
    }
  }       

  /* SMTP information */
  temp_buffer[FLASH_ADDR_SMTP_IP]         = smtp.server_ip[0];
  temp_buffer[FLASH_ADDR_SMTP_IP + 1]     = smtp.server_ip[1];
  temp_buffer[FLASH_ADDR_SMTP_IP + 2]     = smtp.server_ip[2];
  temp_buffer[FLASH_ADDR_SMTP_IP + 3]     = smtp.server_ip[3];
  
  
  for(i = 0; i < 30; i++)
  {
    temp_buffer[FLASH_ADDR_SMTP_FROM + i]    = smtp.from[i];                
	temp_buffer[FLASH_ADDR_SMTP_SUBJECT + i] = smtp.subject[i];
  }
  
  /* Trap IP */
  temp_buffer[FLASH_ADDR_TRAP_ENABLE] = flag.trap_en;
  temp_buffer[FLASH_ADDR_TRAP_IP]     = snmp.trap_ip[0]; 
  temp_buffer[FLASH_ADDR_TRAP_IP + 1] = snmp.trap_ip[1];
  temp_buffer[FLASH_ADDR_TRAP_IP + 2] = snmp.trap_ip[2];
  temp_buffer[FLASH_ADDR_TRAP_IP + 3] = snmp.trap_ip[3];

  /*--------------------ALARMS-------------------------------------------------------------------*/
  for(i = 0; i < 5; i++)
  {
    temp_buffer[FLASH_ADDR_ALARM0_ENABLE        + (i * 5)] = alarm[i].enable;
    temp_buffer[FLASH_ADDR_ALARM0_INPUT_PIN     + (i * 5)] = alarm[i].input_pin;
    temp_buffer[FLASH_ADDR_ALARM0_PIN_CONDITION + (i * 5)] = alarm[i].pin_condition;
    temp_buffer[FLASH_ADDR_ALARM0_CHAR1         + (i * 5)] = alarm[i].char1;
    temp_buffer[FLASH_ADDR_ALARM0_CHAR2         + (i * 5)] = alarm[i].char2;
  
    temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 0] = alarm[i].condition[0].power_reset;
    temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 1] = alarm[i].condition[0].ip_changed;
    temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 2] = alarm[i].condition[0].isvr_accessed;
    temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 3] = alarm[i].condition[0].isvr_disconnect;
    temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 4] = alarm[i].condition[0].isvr_condition;
    temp_buffer[FLASH_ADDR_ALARM0_EMAIL + (i * 12) + 5] = alarm[i].condition[0].chars;
  
    temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 0] = alarm[i].condition[1].power_reset;
    temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 1] = alarm[i].condition[1].ip_changed;
    temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 2] = alarm[i].condition[1].isvr_accessed;
    temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 3] = alarm[i].condition[1].isvr_disconnect;
    temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 4] = alarm[i].condition[1].isvr_condition;
    temp_buffer[FLASH_ADDR_ALARM0_TRAP + (i * 12) + 5] = alarm[i].condition[1].chars;
  
    sprintf((char *)temp_buff,"%d",alarm[i].reminder_interval);
    if(strlen((char *)temp_buff) <= 4)
      memcpy(&temp_buffer[FLASH_ADDR_ALARM0_REMINDER_INTERVAL + (i * 4)],temp_buff,4);
    
    for(j = 0; j < EMAIL_ADDR_BUF_LEN; j++)
      temp_buffer[FLASH_ADDR_ALARM0_RECIPIENTS + (i * EMAIL_ADDR_BUF_LEN) + j] = alarm[i].recipients[j];
  
    for(j = 0; j < MSG_BUF_LEN; j++)
      temp_buffer[FLASH_ADDR_ALARM0_MSG + (i * MSG_BUF_LEN) + j] = alarm[i].msg[j];
  }

  /* GPIO information */
  temp_buffer[FLASH_ADDR_RS232_485_GPIO] = flag.rs485type;
  temp_buffer[FLASH_ADDR_GPIO_TYPE] = flag.gpio_type;
  
  for(i = 0; i < 6; i++)
  {
    temp_buffer[FLASH_ADDR_GPIO_DIRECTION + i] = gpio.direction[i];
  }

  for(i = 0; i < 6; i++)
  {
    temp_buffer[FLASH_ADDR_OP_LEVEL + i] = gpio.op_level[i];
  }

  for(i = 0; i < 3; i++)
  {
    temp_buffer[FLASH_ADDR_IF_CONDITION + i] = gpio.if_condition[i];
  }

  for(i = 0; i < 3; i++)
  {
    temp_buffer[FLASH_ADDR_THEN_CONDITION + i] = gpio.then_condition[i];
  }


  sprintf((char *)temp_buff,"%d",serial.pkt_len);
  if(strlen((char *)temp_buff) < 5)
  {
    memcpy(&temp_buffer[FLASH_ADDR_PACKET_LENGTH],temp_buff,5);
  }
  
  temp_buffer[FLASH_ADDR_RS485_TX_RX] = flag.rs485txrx;
  
  sprintf((char *)temp_buff,"%d",serial.rs485timer);
  if(strlen((char *)temp_buff) < 5)
  {
    memcpy(&temp_buffer[FLASH_ADDR_RS485_TIMER],temp_buff,5);
  }
  
  /* IP Exclusivity */
  for(i = 0; i < NO_EX_IPS; i++)
  {
    for(j = 0; j < IP_ADDR_LEN; j++)
    {
      temp_buffer[FLASH_ADDR_IP_EX_1 + (IP_ADDR_LEN * i) + j] = exclusivity.ip[i][j];
    }
  }

  temp_buffer[FLASH_ADDR_TERMINAL_SERVER_CONNECT_ON] = terminal_server.connect_on;
  temp_buffer[FLASH_ADDR_TUNNELING_CONNECT_ON] = tunneling.connect_on;
  temp_buffer[FLASH_ADDR_DISCONNECT_BY] = terminal_server.disconnect_by;
  
  sprintf((char *)temp_buff,"%d",tunneling.retry_timer);
  if(strlen((char *)temp_buff) < 5)
    memcpy(&temp_buffer[FLASH_ADDR_TUNNELING_RETRY_TIMER],temp_buff,5);
  
  /* Multihost Information */
  temp_buffer[FLASH_ADDR_CONNECTION_TYPE] = multihost.connection_type ;
  
  temp_buffer[FLASH_ADDR_MULTIHOST_RETRY_COUNTER] = multihost.retry_counter;
  
  sprintf((char *)temp_buff,"%d",multihost.retry_timeout);
  if(strlen((char *)temp_buff) < 5)
    memcpy(&temp_buffer[FLASH_ADDR_MULTIHOST_RETRY_TIMEOUT],temp_buff,5);
  
  for(i = 0; i < 12; i++)
  {
    for(j = 0; j < IP_ADDR_LEN; j++)
    {
      temp_buffer[FLASH_ADDR_MULTIHOST_RIP + (4 * i) + j] = \
	    multihost.rip[i][j];
#if(DEBUG_LEVEL == HIGH)
	  printf("temp_buffer[%d] = %d\r\n",(FLASH_ADDR_MULTIHOST_RIP +\
	    (4 * i) + j),multihost.rip[i][j]);
#endif
    }
  }

  for(i = 0; i < 12; i++)
  {
    sprintf((char *)temp_buff,"%d",multihost.rp[i]);
#if(DEBUG_LEVEL == HIGH)
	printf("temp_buf = %s\r\n",(char *)temp_buff);
#endif

	for(j = 0; j < 5; j++)
	{
	  temp_buffer[FLASH_ADDR_MULTIHOST_REMOTE_PORT+(i*5)+j]=temp_buff[j];
#if(DEBUG_LEVEL == HIGH)
	  printf("temp_buffer[%d] = %c\r\n",\
	    (FLASH_ADDR_MULTIHOST_REMOTE_PORT + (i * 5) + j),\
	    temp_buffer[FLASH_ADDR_MULTIHOST_REMOTE_PORT + (i * 5) + j]);
#endif
	}
  }
  
  /* SNMP information */
  for(i = 0; i < SNMP_CONTACT_LEN; i++)
  {
    temp_buffer[FLASH_ADDR_SNMP_CONTACT + i] = snmp.contact[i]; 
  }

  for(i = 0; i < SNMP_COMMUNITY_LEN; i++)
  {
    temp_buffer[FLASH_ADDR_SNMP_COMMUNITY + i] = snmp.community[i]; 
  }

  for(i = 0; i < SNMP_LOCATION_LEN; i++)
  {
    temp_buffer[FLASH_ADDR_SNMP_LOCATION + i] = snmp.location[i]; 
  }

  temp_buffer[FLASH_ADDR_SECURITY_FLAG] = flag.security;
  
  for(i = 0; i < 2; i++)
  {
    temp_buffer[FLASH_ADDR_GPIO_DIRECTION_07 + i] = gpio.direction[6 + i];
  }

  for(i = 0; i < 2; i++)
  {
    temp_buffer[FLASH_ADDR_OP_LEVEL_07 + i] = gpio.op_level[6 + i];
  }

  temp_buffer[FLASH_ADDR_IF_CONDITION_04]   = gpio.if_condition[3];
  
  temp_buffer[FLASH_ADDR_THEN_CONDITION_04] = gpio.then_condition[3];
  //wendy
  sprintf((char *)temp_buff,"%d",flag.HTTP_PORT);
  if(strlen((char *)temp_buff) < 6)
  {
    memcpy(&temp_buffer[FLASH_HTTP_PORT],temp_buff,5);
  }		
  //wendy
  ENABLE_INTERRUPTS;
  
  _fl_write(temp_buffer);	             /* Write Settings to the Flash */
}
