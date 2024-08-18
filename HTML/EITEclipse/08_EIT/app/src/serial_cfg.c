#include "micronet.h"
#include "np_includes.h"

#define 	MSG_CHARS 	30
#define     HALF        1
#define     FULL        2

byte temp;

/**************************************************************************************************
Function: 		initUART()
Description:  	Initialize UART1 to baudrate etc..
Comment:		Also Set CTS and RTS to low
Input:			None
Output:			None
**************************************************************************************************/
int initUART()
cmx_reentrant 
{
  DISABLE_INTERRUPTS;
  
 if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
 {
   rs232_full_init_uart();
 }
 else if((iserver.model == EIT_W_4) || (iserver.model == EIT_D_4) || (iserver.model == EIT_PCB_4))
 {
   rs485_init_uart();
 }
 else if((iserver.model == EIT_PCB_T))
 {
   rs232_gpio_ttl_init_uart();
 }

  ENABLE_INTERRUPTS;

  return 0;
}


/**************************************************************************************************
Function: 		start_xmit()
Description:  	Starts transmission out of the UART. 
				Assumes there is always at least one character to send 
				when this function is called.
Input:			None
Output:			None
**************************************************************************************************/
void start_xmit(void)
cmx_reentrant 
{
  EthernetToSerial();
}

/**************************************************************************************************
Function: 		uart1_handler()
Description:  	ISR of UART
Input:			None
Output:			None
**************************************************************************************************/
void uart1_handler(void)
cmx_reentrant 
{
  volatile byte inter;
  volatile byte status;
  byte num;
  byte c;

  while (!((inter = U1IIR) & 0x01))
  {
    switch (inter & 0x0F)
    {
      case 0x0C:                 			  /* character timeout 	    */
      case 0x04:          					  /* RECIEVED		        */       				
        status = U1LSR;	
        if(status & 0x01)      				  /* A BYTE RECIEVED 	    */
		{
          c = U1RBR;
          serial.rx_bytes++;                  /* Inc UART RX bytes cntr */
		  if((c == alarm[0].char1) || (c == alarm[0].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[1].char1) || (c == alarm[1].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[2].char1) || (c == alarm[2].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[3].char1) || (c == alarm[3].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[4].char1) || (c == alarm[4].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }

          if (uib_count < BUFF_SIZE)
          {
            uart_in_buff[uib_in++] = c;
            if (uib_in == BUFF_SIZE)
              uib_in = 0;
            uib_count++;

            if(serial.pkt_len != NULL)
            {
              if(uib_count == serial.pkt_len)
                flag.pkt_len_recvd = TRUE;
            }
            if(serial.endchar != NULL)
            {
              if(c == serial.endchar)
                flag.endchar_recvd = TRUE;
            }

            if(serial.flowcontrol == SOFTWARE_FLOW_CONTROL)
            {
              if(uib_count > 400)
              {
                U1THR = XOFF;
              }
              else if(uart_in_buff[uib_in] == XON)
              {
                U1TER = 0x80;
              }
              else if(uart_in_buff[uib_in] == XOFF)
              {
                U1TER = 0x00;              
              }
              else
              {
                U1TER = 0x80;
              }
            }
          }
        }
        break;

      case 0x02:                 /* xmit */
        if (U1LSR & 0x20)    /* THR empty */
        {
          num = serial.fifo_size;
          while (num)
          {
            num--;
			serial.tx_bytes++;                  /* Inc UART TX bytes cntr */
            
            /* Are there more chars to send? */
            if (buff_pool.db_out < buff_pool.len)
            {
              /* Yes, send it */
              U1THR = buff_pool.data_buff[buff_pool.db_out];
              buff_pool.db_out++;
            }
            else
            {
              /* No, disable xmit interrupt and flag send is done. */
              U1IER &= (~0x02);
              buff_pool.xmit_busy = FALSE;
              break;
            }
          }
        }
        break;

//        if (U1LSR & 0x20)    /* THR empty */
//        {
//          num = serial.fifo_size;
//          while (num)
//          {
//            num--;
//			serial.tx_bytes++;                  /* Inc UART TX bytes cntr */
//            if(serial.endchar != NULL)
//            {
//              /* Are there more chars to send? */
//              if(serial.fwdendchar == ENABLED)
//              {
//                if (buff_pool.count < buff_pool.db_in)
//                {
//                  /* Yes, send it */
//                  U1THR = buff_pool.uart_out_buff[buff_pool.count];
//                  buff_pool.count++;
//                }
//                else
//                {
//                  /* No, disable xmit interrupt and flag send is done. */
//                  U1IER &= (~0x02);
//                  buff_pool.xmit_busy = FALSE;
//                  memset(buff_pool.uart_out_buff,NULL,sizeof(buff_pool.uart_out_buff));
//                  break;
//                }
//              }
//              else
//              {
//                if (buff_pool.count < (buff_pool.db_in - 1))
//                {
//                  /* Yes, send it */
//                  U1THR = buff_pool.uart_out_buff[buff_pool.count];
//                  buff_pool.count++;
//                }
//                else
//                {
//                  /* No, disable xmit interrupt and flag send is done. */
//                  U1IER &= (~0x02);
//                  buff_pool.xmit_busy = FALSE;
//                  memset(buff_pool.uart_out_buff,NULL,sizeof(buff_pool.uart_out_buff));
//                  break;
//                }
//              }
//            }
//            else if(serial.pkt_len != NULL)
//            {
//              if (buff_pool.db_out < buff_pool.db_in)
//              {
//                /* Yes, send it */
//                U1THR = buff_pool.uart_out_buff[buff_pool.db_out];
//                buff_pool.db_out++;
//              }
//              else
//              {
//                /* No, disable xmit interrupt and flag send is done. */
//                U1IER &= (~0x02);
//                buff_pool.xmit_busy = FALSE;
//                memset(buff_pool.uart_out_buff,NULL,sizeof(buff_pool.uart_out_buff));
//                buff_pool.db_out = 0;
//                buff_pool.db_in = 0;
//                break;
//              }
//            }    
//            else
//            {
//              /* Are there more chars to send? */
//              if (buff_pool.db_out < buff_pool.len)
//              {
//                /* Yes, send it */
//                U1THR = buff_pool.data_buff[buff_pool.db_out];
//                buff_pool.db_out++;
//              }
//              else
//              {
//                /* No, disable xmit interrupt and flag send is done. */
//                U1IER &= (~0x02);
//                buff_pool.xmit_busy = FALSE;
//                break;
//              }
//            }
//          }
//
//        }
//        break;

      case 0x06:                 /* line status */
        status = U1LSR;
        break;

      case 0x00:                 /* modem status */
        break;

      default:
        break;  
	}
  }
}

/**************************************************************************************************
Function: 		uart2_handler()
Description:  	ISR of UART
Input:			None
Output:			None
**************************************************************************************************/
void uart0_handler(void)
cmx_reentrant 
{
  volatile byte inter;
  volatile byte status;
  byte num;
  byte c;
  // DWORD regVal;

  //FIO1CLR3 = 0xC0;
  //init_timer();
  //regVal = T3TCR;
  //regVal |= 0x02;
  //T3TCR = regVal;
  
  
  //T3TCR = 1;              /* Timer1 Enable              */

  while (!((inter = U0IIR) & 0x01))
  {
    switch (inter & 0x0F)
    {
      case 0x0C:                 			/* character timeout 	*/
      case 0x04:          						/* RECIEVED		*/       				
        status = U0LSR;	
        if(status & 0x01)      				/* A BYTE RECIEVED 	*/
		{
          c = U0RBR;
          serial.rx_bytes++;                  /* Inc UART RX bytes cntr */
		  if((c == alarm[0].char1) || (c == alarm[0].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[1].char1) || (c == alarm[1].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[2].char1) || (c == alarm[2].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[3].char1) || (c == alarm[3].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          else if((c == alarm[4].char1) || (c == alarm[4].char2))
          {
            flag.isvr_char1 = TRUE;  
            flag.isvr_char2 = TRUE;  
          }
          if((iserver.model == EIT_W_4)||(iserver.model == EIT_D_4)||(iserver.model == EIT_PCB_4))
		  {
		    if(flag.rs485type == HALF)
            {
              if(!(FIO0PIN0 & 0x10))     /* P0.4 High? Recv IFF DE485 is High */
              {
                if (uib_count < BUFF_SIZE)
                {
                  uart_in_buff[uib_in++] = c;
                  if (uib_in == BUFF_SIZE)
                    uib_in = 0;
                  uib_count++;
                  if(serial.pkt_len != NULL)
                  {
                    if(uib_count == serial.pkt_len)
                      flag.pkt_len_recvd = TRUE;
                  }
                  if(serial.endchar != NULL)
                  {
                    if(c == serial.endchar)
                      flag.endchar_recvd = TRUE;
                  }
                }
              }
            }
            else if(flag.rs485type == FULL)
            {
              if (uib_count < BUFF_SIZE)
              {
                uart_in_buff[uib_in++] = c;
                if (uib_in == BUFF_SIZE)
                  uib_in = 0;
                uib_count++;
                if(serial.pkt_len != NULL)
                {
                  if(uib_count == serial.pkt_len)
                    flag.pkt_len_recvd = TRUE;
                }
                if(serial.endchar != NULL)
                {
                  if(c == serial.endchar)
                    flag.endchar_recvd = TRUE;
                }
              }
            }
		  }
		  else
		  {
            if (uib_count < BUFF_SIZE)
            {
              uart_in_buff[uib_in++] = c;
              if (uib_in == BUFF_SIZE)
                uib_in = 0;
              uib_count++;
              if(serial.pkt_len != NULL)
              {
                if(uib_count == serial.pkt_len)
                  flag.pkt_len_recvd = TRUE;
              }
              if(serial.endchar != NULL)
              {
                if(c == serial.endchar)
                  flag.endchar_recvd = TRUE;
              }

              if(serial.flowcontrol == SOFTWARE_FLOW_CONTROL)
              {
                if(uib_count > 400)
                {
                  U0THR = XOFF;
                }
                else if(uart_in_buff[uib_in] == XON)
                {
                  U0TER = 0x80;
                }
                else if(uart_in_buff[uib_in] == XOFF)
                {
                  U0TER = 0x00;              
                }
                else
                {
                  U0TER = 0x80;
                }
              }
            }
          }
		}
        break;

      case 0x02:                 /* xmit */
        if (U0LSR & 0x20)    /* THR empty */
        {
          num = serial.fifo_size;
          while (num)
          {
            num--;
			serial.tx_bytes++;                  /* Inc UART TX bytes cntr */
            
            /* Are there more chars to send? */
            if (buff_pool.db_out < buff_pool.len)
            {
              /* Yes, send it */
              U0THR = buff_pool.data_buff[buff_pool.db_out];
              buff_pool.db_out++;
            }
            else
            {
              /* No, disable xmit interrupt and flag send is done. */
              U0IER &= (~0x02);
              buff_pool.xmit_busy = FALSE;
              break;
            }
          }
        }
        break;

      case 0x06:                 /* line status */
        status = U0LSR;
        break;

      case 0x00:                 /* modem status */
        break;

      default:
        break;  
	}
  }
  //FIO1SET3 = 0xC0;
}


/**************************************************************************************************
Function:    show_all_settings()
Description: Show all the settings on the serial port
Input:       void
Output:      void
**************************************************************************************************/
void show_settings_mode(void)
cmx_reentrant
{
  char sp_Buffer[20];
  char web_serial_password_len;
  char serial_port_password_len;
  char password_retry_cntr = 0; 

  /* Get the length of the Serial Password */
  web_serial_password_len = strlen(password.serial);

  /* If the password starts with NULL or CR , Dont prompt for password and show all settings */
  if((password.serial[0] == NULL) || (password.serial[0] == 0x0D))
    print_all_settings();
  else
  {
    /* If the password is something else not NULL */
	while(password_retry_cntr < 3) /* Only allow three times to ask for password */
    {
      /* Clear the buffer to get and reply response */
	  memset(sp_Buffer,NULL,strlen(sp_Buffer));
      /* Ask for password */
	  printf("Password?");
      /* Get the password */
	  gets(sp_Buffer);
      /* Get the password length */
	  serial_port_password_len = strlen(sp_Buffer);
      
	  /* Compare the password with the webpassword. Compare content as well as length */
	  /* If the password was set from the SERIAL PORT it will have the CR in the end, 
	  if that is the case do this routine */
      if(password.serial[web_serial_password_len - 1] == 0x0d)
      {
        if(!( memcmp(password.serial,sp_Buffer,(web_serial_password_len - 1))) &&\
            (web_serial_password_len == serial_port_password_len)) 
        {
   	      print_all_settings();
		  break;
        }   
        else
        {
          /* Incorrect password */
		  printf("incorrect password\r\n");
	      password_retry_cntr++;
        }
      }
      else
	  {
	    /* Password set from the WEB so there is no CR in the end */
		if(!( memcmp(password.serial,sp_Buffer,web_serial_password_len)) &&\
              (web_serial_password_len == (serial_port_password_len - 1))) 
        {
   	      print_all_settings();
		  break;
        }   
        else
        {
          printf("incorrect password\r\n");
		  password_retry_cntr++;
        }
      }  
	}
	if(password_retry_cntr > 2)
	{
      printf("Too many failed attempts...Rebooting...");
      WDTInit();                             /* Perform Reboot */
	}
  }    
}

/**************************************************************************************************
Function:    print_all_settings()
Description: print all the settings on the serial port
Input:       void
Output:      void
**************************************************************************************************/
void print_all_settings(void)
cmx_reentrant
{
  char BD[12][7]={"300","600","1200","2400","4800","9600","19200",\
    "38400","57600","115200","230400","460800"};
  char DT[4][10]={"5","6","7","8"};  
  char PT[3][10]={"None","Even","Odd"};
  char ST[2][10]={"1","2"};
  char FC[3][10]={"None","Hardware","Software"};
  char DP[2][9] = {"Disabled", "Enabled"};
  char IC[2][9] = {"Disabled", "Enabled"};
  char SC[2][9] = {"Disabled", "Enabled"};
  char model_buf[11];
  
  switch(iserver.model)
  {
    case EIT_W_2F:   strcpy(model_buf,"EIT_W_2F"); break;
	case EIT_W_4:    strcpy(model_buf,"EIT_W_4"); break;
    case EIT_D_2F:   strcpy(model_buf,"EIT_D_2F"); break;
    case EIT_D_4:    strcpy(model_buf,"EIT_D_4"); break;
    case EIT_PCB_2F: strcpy(model_buf,"EIT_PCB_2F"); break;
    case EIT_PCB_4:  strcpy(model_buf,"EIT_PCB_4"); break;
    case EIT_PCB_T:  strcpy(model_buf,"EIT_PCB_T"); break;
    default:         strcpy(model_buf,"EIT_PCB_T"); break;
  }

  printf("\r\n");
  printf("\r\n");
  
  printf("Model: \t\t%s\r\nFirmware Ver: \t%s\r\n",model_buf,\
    iserver.version);
  printf("MAC Address:\t%0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\r\n",eth_src_hw_addr[0],
    eth_src_hw_addr[1],eth_src_hw_addr[2],eth_src_hw_addr[3],\
	eth_src_hw_addr[4],eth_src_hw_addr[5]);
  
  printf("\r\n");
  printf("\r\n");
  
  printf("BD: %s (%d)\r\n",BD[serial.baudrate - 1],serial.baudrate);
  printf("DT: %s bits (%d)\r\n",DT[serial.databits - 1],serial.databits);
  printf("PT: %s (%d)\r\n",PT[serial.parity - 1],serial.parity);
  printf("ST: %s bit (%d)\r\n",ST[serial.stopbits - 1],serial.stopbits);
  printf("FC: %s (%d)\r\n",FC[serial.flowcontrol - 1],serial.flowcontrol);
  printf("DP: %s (%d)\r\n",DP[flag.dhcp],flag.dhcp);
  printf("IP: %d.%d.%d.%d\r\n",eth_src_addr[0],\
    eth_src_addr[1],eth_src_addr[2],eth_src_addr[3]);
  printf("SM: %d.%d.%d.%d\r\n",subnet_mask[0],\
    subnet_mask[1],subnet_mask[2],subnet_mask[3]);
  printf("GW: %d.%d.%d.%d\r\n",gateway_ip_addr[0],\
    gateway_ip_addr[1],gateway_ip_addr[2],gateway_ip_addr[3]);
  //DNS?
  printf("LP: %s\r\n",password.login);
  printf("SP: %s\r\n",password.admin);
  printf("CP: %s\r\n",password.serial);
  printf("IC: %s (%d)\r\n", IC[flag.internet_console],\
    flag.internet_console);
  printf("SC: %s (%d)\r\n", SC[flag.serial_console],\
    flag.serial_console);
  printf("WS: %s (%d)\r\n", IC[flag.webserver],\
    flag.webserver);
}


/**************************************************************************************************
Function:    serial_console()
Description: Show the serial console
Input:       void
Output:      void
**************************************************************************************************/
void serial_console(void)
cmx_reentrant
{
  char sp_Buffer[20];
  char web_serial_password_len;
  char serial_port_password_len;
  char password_retry_cntr = 0; 

  /* Get the length of the Serial Password */
  web_serial_password_len = strlen(password.serial);
  
  /* If the password starts with NULL or CR , Dont prompt for password and show all settings */
  if((password.serial[0] == NULL) || (password.serial[0] == 0x0D))
    show_prompt();
  else
  {
    while(password_retry_cntr < 3)
    {
      memset(sp_Buffer,NULL,strlen(sp_Buffer));
      printf("Password?");
      gets(sp_Buffer);
      serial_port_password_len = strlen(sp_Buffer);
		  
      if(password.serial[web_serial_password_len - 1] == 0x0d)
      {
        if(!( memcmp(password.serial,sp_Buffer,(web_serial_password_len - 1))) &&\
            (web_serial_password_len == serial_port_password_len)) 
        {
   	      show_prompt();
		  break;
        }   
        else
        {
          printf("incorrect password\r\n");
	      password_retry_cntr++;
        }
      }
      else
      {
	    if(!( memcmp(password.serial,sp_Buffer,web_serial_password_len)) &&\
            (web_serial_password_len == (serial_port_password_len - 1))) 
        {
   	      show_prompt();
		  break;
        }   
        else
        {
          printf("incorrect password\r\n");
	      password_retry_cntr++;
        }
      }  
	}
	if(password_retry_cntr > 2)
	{
      printf("Too many failed attempts...Rebooting...");
      WDTInit();                             /* Perform Reboot */
	}
  }
}

/**************************************************************************************************
Function:    show_prompt()
Description: Show the prompt on serial console
Input:       void
Output:      void
**************************************************************************************************/
void show_prompt(void)
cmx_reentrant
{
  char option[20];
  char *cmd_ptr; 
  byte i = 0;
  char *token;
  char model_buf[11];
  
  token = NULL;
  cmd_ptr = 0;
  
  switch(iserver.model)
  {
    case EIT_W_2F:   strcpy(model_buf,"EIT_W_2F"); break;
	case EIT_W_4:    strcpy(model_buf,"EIT_W_4"); break;
    case EIT_D_2F:   strcpy(model_buf,"EIT_D_2F"); break;
    case EIT_D_4:    strcpy(model_buf,"EIT_D_4"); break;
    case EIT_PCB_2F: strcpy(model_buf,"EIT_PCB_2F"); break;
    case EIT_PCB_4:  strcpy(model_buf,"EIT_PCB_4"); break;
    case EIT_PCB_T:  strcpy(model_buf,"EIT_PCB_T"); break;
    default:         strcpy(model_buf,"EIT_PCB_T"); break;
  }

  while(1)
  {
    /* Print the iServer Model */
	printf("\n\r\n\r%s>",model_buf);
	
	/* get the command */
	gets(option);
	cmd_ptr = option;

    /* Find the command */
	if(option[0] == 'q' || option[0] == 'Q')
	{
      /* Command: QUIT */
	  printf("Save Changes before leaving? 'Y' for YES\r\n");
	  gets(option);
	  if((option[0] == 'y') || (option[0] == 'Y'))
	  {
	    write_all();
		printf("Changed saved...\r\n");
	    printf("Starting iServer...\r\n");
	  }
	  else
	  {
	    printf("Changes not saved...\r\n");
	    printf("Starting iServer...\r\n");
	  }
	  break;
	}
    else
    {
	  /* Switch to the command */
	  switch(*cmd_ptr)
      {
        /* Command: SET Parameters */
	  	case 's':
	    case 'S':
	      if((token = strtok(option," -")) == NULL)
	      {
	        printf("Invalid option\r\n");  
		  }
	      else
	      {
	        while(token != NULL)
	        {
	          if(!( (strncmp(token,"?",1)) ))
			  {
			    show_set_help();
			  }
			  else if(!( (strncmp(token,"BD",2)) ))                 /*------------------- Baudrate */
			  {
			    token += 2;
			    i = np_np_atoi(token);
			    if(i < 13)
	            {
	              serial.baudrate = i;
	              printf("Success\r\n");
					//retval = SUCCESS;
			    }
			    else
			      printf("Invalid option\r\n");
					//retval = WRONG_PARAMS;
			  }
              else if(!( strncmp(token,"DT",2) ))                 /*------------------- Databits */
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 5)
	            {
	              serial.databits = i;
	              printf("Success\r\n");
				  //retval = SUCCESS;
			    }
			    else
			      printf("Invalid option\r\n");
        	  }
        	  else if(!( strncmp(token,"PT",2) ))               /*------------------- Parity --*/
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 4)
        	    {
                  serial.parity = i;
                }
        	    else
                  printf("Invalid option\r\n");		
        	  }
        	  else if(!( strncmp(token,"ST",2) ))              /*------------------- Stop bits */
              {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 3)
        	    {
                  serial.stopbits = i;
        	    }
        	    else
                  printf("Invalid option\r\n");		
        	  }
              else if(!( strncmp(token,"FC",2) ))              /*-----------------Flow Control */
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 4)
        	    { 
                  serial.flowcontrol = i;
        	    }
        	    else
                  printf("Invalid option\r\n");		
        	  }
         	  else if(! ( (strncmp(token,"MC",2)) ))                /*------------------- MAC Addr */
		      {
		        token += 2;
		        if(s_mac_process((byte *)token))
                {
                  memcpy(eth_src_hw_addr,token,ETH_ADDR_LEN); 
                  printf("Success\r\n");
                  //retval = SUCCESS;
		        }
		        else
		          printf("Invalid option\r\n");
		      }
              else if(!( strncmp(token,"DP",2) ))              /*----------------DHCP Option----*/
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 2)
        	    {
                  flag.dhcp = i;
        	    }
        	    else
                  printf("Invalid option\r\n");		
        	  }
              else if(! ( (strncmp(token,"IP",2)) ))   /*------------------- Set the IP Address -*/
		      {
		        token += 2;
		        if(s_ip_process((byte *)token,0))
			    {
                  memcpy(eth_src_addr,token,IP_ADDR_LEN);
			  	  printf("Success \r\n");
		        }
			    else
		          printf("Invalid option\r\n");
              }
              else if(! ( (strncmp(token,"SM",2)) ))   /*------------------- Set the Subnet Mask -*/
		      {                                                                                     
		        token += 2;
		        if(s_ip_process((byte *)token,0))
			    {
                  memcpy(subnet_mask,token,IP_ADDR_LEN);
				  printf("Success \r\n");
			    }
                else
		          printf("Invalid option\r\n");
		      }
              else if(! ( (strncmp(token,"GW",2)) ))   /*------------ Set the Gateway IP Address --*/
		      {
		        token += 2;
		        if(s_ip_process((byte *)token,0))
                {
			      memcpy(gateway_ip_addr,token,IP_ADDR_LEN);
				  printf("Success \r\n");
			    }
		        else
		          printf("Invalid option\r\n");
		      }
              else if(! ( (strncmp(token,"LP",2)) ))   /*-------------- Set the Login Password ----*/
		      {
		        token += 2;
		        if(token != NULL && (strlen(token) <= (PASSWORD_LEN + 1)))
			    { 
                  memcpy(password.login,token,(PASSWORD_LEN + 1));
				  printf("Success \r\n");
			    }
                else
		         printf("Invalid option\r\n");
		      }	  
              else if(! ( (strncmp(token,"SP",2)) ))   /*-------------- Set the Admin Password ----*/
		      {
		        token += 2;
		        if(token != NULL && (strlen(token) <= (PASSWORD_LEN + 1)))
			    {
                  memcpy(password.admin,token,(PASSWORD_LEN + 1));
				  printf("Success \r\n");
			    }
                else
		          printf("Invalid option\r\n");
		      }
              else if(! ( (strncmp(token,"CP",2)) ))   /*-------------- Set the Serial Password ----*/
		      {
		        token += 2;
		        if(token != NULL && (strlen(token) <= (PASSWORD_LEN + 1)))
			    {
                  memcpy(password.serial,token,(PASSWORD_LEN + 1));
				  printf("Success \r\n");
			    }
                else
		          printf("Invalid option\r\n");
		      }
              else if(!( strncmp(token,"IC",2) ))          /*----------------Internet Console------*/
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 2)
			    {
        	      flag.internet_console = i;
				  printf("Success \r\n");
        	    }
			    else
                  printf("Invalid option\r\n");		
              }
              else if(!( strncmp(token,"SC",2) ))              /*----------------Serial Console----*/
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 2)
			    {
        	      flag.serial_console = i;
				  printf("Success \r\n");
			    }
        	    else
                  printf("Invalid option\r\n");		
        	  }
              else if(!( strncmp(token,"WS",2) ))              /*----------------Webserver----*/
        	  {
        	    token += 2;
			    i = np_np_atoi(token);
			    if(i < 2)
			    {
                  flag.webserver = i;
				  printf("Success \r\n");
        	    }
			    else
                  printf("Invalid option\r\n");		
        	  }  
#if(DEBUG_LEVEL == HIGH)			
			printf("token = %s\n\r",token);
#endif
		      token = strtok(NULL," -");
		    }
          }
		  break;
	  
	    case 'r':
        case 'R':
          if((!( (strncmp(cmd_ptr,"reboot",6)) )) || (!( (strncmp(cmd_ptr,"REBOOT",6)) )))
		  {
            printf("Rebooting.........\r\n");
            WDTInit();                             /* Perform Reboot */
		  }
          break;

        case 'f':
        case 'F':
          if((!( (strncmp(cmd_ptr,"factory",7)) )) || (!( (strncmp(cmd_ptr,"FACTORY",7)) )))
		  {
            flash_write_default();                              /* Write Defaults */
            printf("Written Defaults......\r\n\r\n");
		  }
		  break;
        
	    case '?':	 
	      show_help(); 
		  break;

	    case 'p':
	    case 'P':
	       print_all_settings();
		   break;

	    case 'c':
	    case 'C':  
		  //WriteMacToFlash();
          write_all();
		  printf("Changes Saved \r\n");
	      break;
      
        case 0x0d:
	      break;

	    default: 
          printf("Invalid Command \r\n");
      }
	}
  }/* While */
}

/**************************************************************************************************
Function:    show_help()
Description: Show help on serial port
Input:       void
Output:      void
**************************************************************************************************/
void show_help(void)
cmx_reentrant
{
  printf("\r\n\r\niServer Configuration Commands:\r\n\r\n");
													    
  printf("?:       Show this help \r\n");
  printf("p:       Print all the settings\r\n");
  printf("s:       Change settings, \"s ?\": for more information \r\n");
  printf("q:       Quit\r\n");
  printf("REBOOT:  Reboot the device\r\n");
  printf("FACTORY: Reset factory defaults\r\n");
}

/**************************************************************************************************
Function:    show_set_help()
Description: Show change settings help on serial port
Input:       void
Output:      void
**************************************************************************************************/
void show_set_help(void)
cmx_reentrant
{
  printf("\r\n\r\nCmd Description:       Parameters\r\n");
  printf("---------------------------------------------------------\r\n");
  printf("BD  Baudrate           1-300, 2-600, 3-1200, 4-2400, 5-4800, 6-9600\r\n");
  printf("                       7-19200, 8-38400, 9-57600, 10-115200, 11-230400, \r\n"); 
  printf("                       12-460800\r\n");
  printf("DT  Data bits          1- 5 bits, 2- 6 bits, 3- 7 bits, 4- 8 bits\r\n");
  printf("PT  Parity             1- None, 2- Even, 3- Odd\r\n");
  printf("ST  Stop bit           1- 1 bit, 2- 2 bits\r\n");
  printf("FC  Flow Control       1- None, 2- Hardware, 3- Software\r\n");
  printf("MC  MAC address\r\n");
  printf("DP  DHCP               0-Disable, 1-Enable\r\n");
  printf("IP  IP address\r\n");
  printf("SM  Subnet mask\r\n");
  printf("GW  Gateway\r\n");
  printf("PP  Login password     0-16 Alphanumeric characters\r\n");
  printf("SP  Admin password     0-16 Alphanumeric characters\r\n");
  printf("CP  Cnfg password      0-16 Alphanumeric characters\r\n");
  printf("IC  Internet Console   0-Disable, 1-Enable\r\n");
  printf("SC  Serial Console     0-Disable, 1-Enable\r\n");
  printf("WS  Webserver Access   0-Disable, 1-Enable\r\n");
}
