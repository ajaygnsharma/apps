#include "micronet.h"
#include "np_includes.h"

#define PASSWORD_OK             1
#define INVALID_PASSWORD        2

#define WRONG_CMD               100 
#define WRONG_FORMAT            101 
#define WRONG_HEADER            102 
#define WRONG_PARAMS            103 
#define UNKNOWN_CHAR_IN_PARAMS  104
#define PROMPT                  105

#define HELP                    1
#define PRINT_SETTINGS          2
#define SUCCESS                 3
#define WRITE_FLASH             15
#define QUIT                    16
#define RESET                   17
#define FACTORY                 18

#define CMD_EXE_OK              "Command Executed\n\rEIS32-W>"
#define RST_MSG                 "\r\nThe unit will reset now.\r\n"
#define FACTORY_MSG             "\r\nFactory default is written.\
 Send RESET command to activate the new settings.\r\nEIS32-W>"
#define WROTE_FLASH_MSG         "\r\nWrote to Flash\r\n\r\nEIS32-W>"



#define HELP_MSG "iServer Configuration Commands:\r\n\r\n\
Cmd Description:       Parameters\r\n\
---------------------------------------------------------------------\r\n\
BD  BaudRate           0-300, 1-600, 2-1200, 3-2400, 4-4800, 5-9600,\r\n\
                       6-19200, 7-38400, 8-57600,9-115200,10-230400,\r\n\
                       11-460800\r\n\
DT  Data Bits          0-7 bits, 1 - 8 bits\r\n\
PT  Parity             0-none, 1-Odd, 2-even\r\n\
ST  Stop Bit           0-1 bit,1-2 bits\r\n\
FC  FlowControl        0-none, 1-hardware, 2-software\r\n\
MD  Interface          1-RS232 with Modem Signals,\r\n\
                       2-RS232 (TX/RX) with 6GPIOs,\r\n\
                       3-RS485 2 wire with 6 GPIOs,\r\n\
                       4-RS485 4 wire with 4 GPIOs\r\n\
4T  RS485 Timer        0-9999\r\n\
4M  RS485 Type         1-Master, 2-Slave\r\n\
NC  No of Conns        0-5\r\n\
LP  Local Port         0-9999\r\n\
RE  Remote Access      0-Disable,1-Enable\r\n\
RI  Remote IP          Valid IP Address\r\n\
RP  Remote Port        0-9999\r\n\
EC  End Character      0-128\r\n\
EE  Fwd End Char       0-Disable, 1-Enable\r\n\
BT  Buffering time     0-9999\r\n\
PL  Packet length      0-9999\r\n\
IT  Inactivity timeout 0-99999\r\n\
DP  DHCP               0-Disable, 1-Enable\r\n\
MC  MAC address\r\n\
IP  IP address\r\n\
SM  Subnet mask\r\n\
GW  Gateway\r\n\
HN  Hostname           0-7 Alphanumeric characters\r\n\
PR  Protocol           1-TCP, 2-UDP, 3-Modbus TCP\r\n\
PP  Login password     0-16 Alphanumeric characters\r\n\
SP  Admin password     0-16 Alphanumeric characters\r\n\
CP  Cnfg password      0-16 Alphanumeric characters\r\n\
IC  Internet Console   0-Disable, 1-Enable\r\n\
SC  Serial Console     0-Disable, 1-Enable\r\n\
WS  Webserver Access   0-Disable, 1-Enable\r\n\r\n\
Example:\r\n\
To configure Baudrate 9600, 1 stop bit, Odd Parity, and RS232 mode.\r\n\
s -BD5 -PT1 -ST1 -MD0\r\n\
\r\n\
EIS-32W>"

byte p2k2_timeout=0;

byte cmd_list[PARA_NUM][2]=
{
  {'B','D'}, // 00
  {'D','T'}, // 01
  {'P','T'}, // 02
  {'S','T'}, // 03
  {'F','C'}, // 04
  {'M','D'}, // 05
  {'4','T'}, // 06
  {'4','M'}, // 07
  {'N','C'}, // 08
  {'L','P'}, // 09
  {'R','E'}, // 10
  {'R','I'}, // 11
  {'R','P'}, // 12
  {'E','C'}, // 13
  {'E','E'}, // 14
  {'B','T'}, // 15
  {'P','L'}, // 16
  {'I','T'}, // 17
  {'D','P'}, // 18
  {'M','C'}, // 19
  {'I','P'}, // 20
  {'S','M'}, // 21
  {'G','W'}, // 22
  {'H','N'}, // 23
  {'P','R'}, // 24
  {'P','P'}, // 25
  {'S','P'}, // 26
  {'C','P'}, // 27
  {'I','C'}, // 28
  {'S','C'}, // 29
  {'W','S'}, // 30
};


/*------------------------------------------------------------------------
 Function:    init_port2002.cmd_buff(void)
 Description: Initialize the command buffers
 Input:       none
 Output:      none
------------------------------------------------------------------------*/
void init_cmd_buff(void)
cmx_reentrant
{
  port2002.cmd_in_ptr = port2002.cmd_out_ptr = &port2002.cmd_buff[0];
  port2002.cmd_count=0;
}

/*------------------------------------------------------------------------
 Function:    check_spassword_func(void)
 Description: Check the admin password
 Input:       none
 Output:      none
------------------------------------------------------------------------*/
byte check_spassword_func(byte *str)
cmx_reentrant 
{
  return (check_password((byte *)password.serial,str));
}


/*------------------------------------------------------------------------
 Function:    check_cpswd()
 Description: Check password entered on the Port 2002 
 Input:       String Pointer 
 Output:      1: Password is matched
              2: Password is different
------------------------------------------------------------------------*/
byte check_cpswd(byte *in_ptr)
cmx_reentrant
{
  byte retval=0;
  
  while(*in_ptr!='\0' && *in_ptr!=0x0a && *in_ptr!=0x0d)
  {
    *port2002.cmd_in_ptr++ = *in_ptr++;
  }
  *port2002.cmd_in_ptr='\0';
  if(*in_ptr==0x0a||*in_ptr==0x0d)
  {
    //if(check_spassword_func(port2002.cmd_out_ptr))
	/*---------------------------------------------------------------------------------------------
     Check the password entered on the console and the admin password 
    also check the lengths to make sure they are equal 
    ---------------------------------------------------------------------------------------------*/
    if(!( memcmp(password.admin,port2002.cmd_out_ptr,strlen(password.admin)) ) &&\
         (strlen((char *)port2002.cmd_out_ptr) == strlen(password.admin)))
    {
	  retval=PASSWORD_OK;
	}
	else
	{
	  retval=INVALID_PASSWORD;
	}
  }
  return (retval);		// wait for command.
}

/*------------------------------------------------------------------------
Function:     mn_port2002_packet(Socket Pointer)
Description:  Process the packet 
Input:        Socket Pointer
Output:       None
------------------------------------------------------------------------*/
char mn_port2002_process_packet(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  byte ret;

  //p2k2_start_timer();

  if(port2002.counter > 2)
  {
	ret = check_cmd(port2002.socket_buff);
	

    //if(!( memcmp(password.admin,port2002.socket_buff,strlen(password.admin)) ))
        //{
   	if(ret)
    {
    //  ret = PASSWORD_OK;
          init_cmd_buff(); // clear command buff, ready for new command
    }
    
    if(ret)
	{
	  init_cmd_buff(); // clear command buff, ready for new command
	}
	if(socket_ptr != PTR_NULL)
	{
	  switch(ret)
	  {
	    case 0:
		  break;
		
		case HELP:                            /* Display Help Message   */
          socket_ptr->send_ptr = HELP_MSG;
          socket_ptr->send_len = (word16)strlen(HELP_MSG);
		  break;
		
		case PRINT_SETTINGS:		        /* Display Current Settings */
		  print_read();              
		  socket_ptr->send_ptr = port2002.print_buff;
		  socket_ptr->send_len=(word16)strlen((char*)port2002.print_buff);
		  break;
        
		case SUCCESS:
		  socket_ptr->send_ptr= CMD_EXE_OK;
          socket_ptr->send_len = (word16)strlen(CMD_EXE_OK);
          break;
        
		case 4:
		  //s_getMac_func(port2002.print_buff);
		  break;
	    
		case WRITE_FLASH:
		  socket_ptr->send_ptr=WROTE_FLASH_MSG;
		  socket_ptr->send_len=(word16)strlen(WROTE_FLASH_MSG);
		  write_all();						 /* Write to Flash Settings */
		  break;
		
		case QUIT:
		  port2002.queue = 1;
		  socket_ptr->send_ptr="\r\nQuit\r\n";
		  socket_ptr->send_len=(word16)strlen("\r\nQuit\r\n");
		  break;
		
		case RESET:
		  socket_ptr->send_ptr=RST_MSG;
		  socket_ptr->send_len=(word16)strlen(RST_MSG);
		  WDTInit();                                          /* Reboot */
		  break;

		case FACTORY:
		  flash_write_default();
		  socket_ptr->send_ptr=FACTORY_MSG;
		  socket_ptr->send_len=(word16)strlen(FACTORY_MSG);
		  break;
        
		case WRONG_CMD:
		  socket_ptr->send_ptr = "wrong command\r\n";
		  socket_ptr->send_len = (word16)strlen("wrong command\r\n");
		  break;
	    
		case WRONG_PARAMS:
		  socket_ptr->send_ptr = "wrong parameter\r\n";
		  socket_ptr->send_len = (word16)strlen("wrong parameter\r\n");
		
		case PROMPT:
		  socket_ptr->send_ptr = "\r\nEIS32-W>";
		  socket_ptr->send_len = (word16)strlen("\r\nEIS32-W>");
		
		default:
		  break;
		}
	}
  }
  else  // login on name and password
  {
    switch(port2002.counter)
	{
	  case 0:
	    break;

	  case 1:
	    ret = check_cpswd(port2002.socket_buff);
   	    if(ret)
		{
          init_cmd_buff(); // clear command buff, ready for new command
		}

        switch(ret)
		{
		  case PASSWORD_OK:
		    port2002.counter++;
			break;
		  case INVALID_PASSWORD:
			socket_ptr->send_ptr = "\r\nInvalid Admin Password!!! Try Again.";
		    socket_ptr->send_len = (word16)strlen("\r\nInvalid Admin Password!!! Try Again.");
			port2002.counter--;
			break;
		  default:
			break;
		}
	    break;
	  
	  case 3:
		break;
	  
	  default:
		break;
	}
  }
  return (0);
}

/*------------------------------------------------------------------------
Function:    print_read()
Description: Print the Present Settings 
Input:       None
Output:      None
------------------------------------------------------------------------*/
void print_read(void)
cmx_reentrant
{
  char temp_buff[50];
  char BD[12][7]={"300","600","1200","2400","4800","9600","19200",\
    "38400","57600","115200","230400","460800"};
  char DT[4][10]={"5","6","7","8"};  
  char PT[3][10]={"None","Even","Odd"};
  char ST[2][10]={"1","2"};
  char FC[3][10]={"None","Hardware","Software"};
  char R4M[2][10] = {"Master","Slave"};
  char NC[6][10]={"0","1","2","3","4","5"};
  char RE[2][9] = {"Disabled","Enabled"};
  char EE[2][9] = {"Disabled", "Enabled"};
  char DP[2][9] = {"Disabled", "Enabled"};
  char IC[2][9] = {"Disabled", "Enabled"};
  char SC[2][9] = {"Disabled", "Enabled"};
  char PR[3][10]= {"TCP","UDP","ModbusTCP"};  
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

  sprintf(temp_buff,"\r\nModel:\t\t%s\r\nFirmware Ver:\t%s\r\n\r\n\r\n",\
    model_buf,iserver.version);
  strcpy((char *)port2002.print_buff,temp_buff);
  
  sprintf(temp_buff,"BD: %s (%d)\r\n",BD[serial.baudrate - 1],\
    serial.baudrate);
  strcat((char *)port2002.print_buff,temp_buff);
  
  sprintf(temp_buff,"DT: %s bits (%d)\r\n",DT[serial.databits - 1],\
    serial.databits);
  strcat((char *)port2002.print_buff,temp_buff);
  
  sprintf(temp_buff,"PT: %s (%d)\r\n",PT[serial.parity - 1],\
    serial.parity);
  strcat((char *)port2002.print_buff,temp_buff);  
  
  sprintf(temp_buff,"ST: %s bit (%d)\r\n",ST[serial.stopbits - 1],\
    serial.stopbits);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"FC: %s (%d)\r\n",FC[serial.flowcontrol - 1],\
    serial.flowcontrol);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"4T: %d \r\n",serial.rs485timer);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"4M: %s (%d)\r\n",R4M[flag.rs485txrx - 1],\
    flag.rs485txrx);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"NC: %s (%d)\r\n",NC[terminal_server.no_connections],
    terminal_server.no_connections);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"LP: %d \r\n",terminal_server.local_port);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"RE: %s (%d)\r\n",RE[tunneling.remote_enable],\
    tunneling.remote_enable);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"RI: %d.%d.%d.%d\r\n",tunneling.remote_ip[0],\
    tunneling.remote_ip[1],tunneling.remote_ip[2],\
	tunneling.remote_ip[3]);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"RP: %d\r\n",tunneling.remote_port);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"EC: %X\r\n",serial.endchar);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"EE: %s (%d)\r\n",EE[serial.fwdendchar],\
    serial.fwdendchar);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"BT: %d\r\n",serial.buffering_time);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"PL: %d\r\n",serial.pkt_len);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"IT: %d\r\n",terminal_server.timeout);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"DP: %s (%d)\r\n",DP[flag.dhcp],flag.dhcp);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"MC: %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\r\n",\
    eth_src_hw_addr[0], eth_src_hw_addr[1], eth_src_hw_addr[2],\
	eth_src_hw_addr[3], eth_src_hw_addr[4], eth_src_hw_addr[5]);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"IP: %d.%d.%d.%d\r\n",eth_src_addr[0],\
    eth_src_addr[1],eth_src_addr[2],eth_src_addr[3]);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"SM: %d.%d.%d.%d\r\n",subnet_mask[0],\
    subnet_mask[1],subnet_mask[2],subnet_mask[3]);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"GW: %d.%d.%d.%d\r\n",gateway_ip_addr[0],\
    gateway_ip_addr[1],gateway_ip_addr[2],gateway_ip_addr[3]);
  strcat((char *)port2002.print_buff,temp_buff);
  //DNS?
  
  sprintf(temp_buff,"HN: %s\r\n",network.hostname);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"PR: %s (%d)\r\n",PR[terminal_server.protocol - 1],\
    terminal_server.protocol);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"PP: %s\r\n",password.login);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"SP: %s\r\n",password.admin);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"CP: %s\r\n",password.serial);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"IC: %s (%d)\r\n", IC[flag.internet_console],\
    flag.internet_console);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"SC: %s (%d)\r\n", SC[flag.serial_console],\
    flag.serial_console);
  strcat((char *)port2002.print_buff,temp_buff);

  sprintf(temp_buff,"WS: %s (%d)\r\n\r\n", IC[flag.webserver],\
    flag.webserver);
  strcat((char *)port2002.print_buff,temp_buff);
  strcat((char *)port2002.print_buff,"EIS-32W>");
}



/*------------------------------------------------------------------------
  Function:    check_cmd()
  Description: Check the Command entered on the Console and return a no:
  Input:       Pointer to the buffer that contains the command

  Output:      SUCCESS:
               1  - when input="?":            Help Menu
               2  - when input="p/P":          Print the current settings
			   3  - when input="s/S":          Setting Parameters
			   4  - when input="mac":          Setting mac address
			   10 - when input="r CTS"         Reading CTS
			   11 - when input="r RTS"         Reading RTS
			   12 - when input="r DTR"         Reading DTR
			   13 - when input="r DCD/DSR"     Reading DCD/DSR
			   14 - when input="w RTS=1/0"     Setting RTS 1/0
			   15 - when input="RESET/reset"   Resetting the device
			   16 - when input="q"             Need to Quit
			   17 - when input="s NW..."       Tell user to reboot device
			   18 - when input="FACTORY"       Setting factory defaults

			   FAILURE:
			   100 - Wrong Command
			   101 - Wrong Format
			   102 - Wrong Header
			   103 - Wrong Parameters
			   104 - Unknown Character in Parameter
------------------------------------------------------------------------*/               
byte check_cmd(byte *in_ptr)
cmx_reentrant
{
  byte retval=0;
  byte i = 0;
  unsigned long j = 0;
  char *token;
  char temp_buff[10];

  memset(temp_buff,NULL,sizeof(temp_buff));

  //byte j=0;
  //byte para_buff[CMD_BUFF_SIZE];

  //char cmd_limit[PARA_NUM]={9,2,1,1,1,4,1,6,18,15,16,16,6,1,15,5,15,15,2,\
  //                          5,2,1,1,13,5,1,1,16,1};
  //char cmd_len_limit[PARA_NUM]={1,1,1,1,1,4,1,1,18,15,16,16,1,1,15,5,15,\
  //                              15,2,5,1,1,1,1,5,1,1,16,1};
  
  while(*in_ptr!='\0' &&\
        *in_ptr!=0x0a &&\
		*in_ptr!=0x0d)                     /* Cmd Not NULL or CR or LF  */
  {
    *port2002.cmd_in_ptr++ = *in_ptr++;
  }
  *port2002.cmd_in_ptr='\0';
  
  if(*in_ptr == 0x0a || *in_ptr == 0x0d)
  {
    switch(*port2002.cmd_out_ptr++)       /*---Switch Command Entered---*/
	{
      case '?':	retval = HELP; break;	       /* Return a HELP Menu "OK"   */
      
	  /*------------------SET parameter --------------------------------*/
	                                            /* Ex: s -BD5 -PT1 -ST1 */
	  case 's':                                         /* s/S accepted */
      case 'S': 
		if((token = strtok((char *)port2002.cmd_out_ptr," -")) == NULL)
		{
		  retval = WRONG_PARAMS;
		  break;
		}
		else
		{
		  while(token != NULL)
		  {
		    if(!( (strncmp(token,"BD",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 13))
              {
                serial.baudrate = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"DT",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 5))
              {
                serial.databits = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"PT",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 4))
              {
                serial.parity = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"ST",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 3))
              {
                serial.stopbits = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"FC",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 4))
              {
                serial.flowcontrol = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"MD",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 3))
              {
                flag.rs485type = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"4T",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 9999))
              {
                serial.rs485timer = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"4M",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 3))
              {
                flag.rs485txrx = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"NC",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 6))
              {
                terminal_server.no_connections = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"LP",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 9999))
              {
                terminal_server.local_port = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"RE",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if(i < 2)
              {
                tunneling.remote_enable = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"RI",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if(s_ip_process((byte *)token,0))
              {
                memcpy(tunneling.remote_ip,token,IP_ADDR_LEN);/* IP Addr*/
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"RP",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 9999))
              {
                tunneling.remote_port = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"EC",2)) ))
		    {
		      token += 2;
              strcpy(temp_buff,token); 
		      sscanf(temp_buff,"%x",&j);                    /* Endchar  */

			  if((j > 0) && (j < 0xff))
              {
                serial.endchar = j;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"EE",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);               /* Enable Fwd char Endchar */
			  if(i < 2)
              {
                serial.fwdendchar = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"BT",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 9999))
              {
                serial.buffering_time = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"PL",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 1024))
              {
                serial.pkt_len = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"IT",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 9999))
              {
                terminal_server.timeout = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"DP",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);                           /* Enable DHCP */
			  if(i < 2)
              {
                flag.dhcp = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"MC",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if(s_mac_process((byte *)token))
              {
                memcpy(eth_src_hw_addr,token,ETH_ADDR_LEN);  /* MAC Addr*/
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"IP",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if(s_ip_process((byte *)token,0))
              {
                memcpy(eth_src_addr,token,IP_ADDR_LEN);       /* IP Addr*/
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"SM",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if(s_ip_process((byte *)token,0))
              {
                memcpy(subnet_mask,token,IP_ADDR_LEN);   /* Subnet Mask */
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"GW",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if(s_ip_process((byte *)token,0))
              {
                memcpy(gateway_ip_addr,token,IP_ADDR_LEN);    /* IP Addr*/
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"HN",2)) ))
		    {
		      token += 2;
		      if(strlen(token) <= HOSTNAME_LEN)
			  {
			    hname_supplement((byte *)token,HOSTNAME_LEN);
                memcpy(network.hostname,token,HOSTNAME_LEN);/* Hostname */
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"PR",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);
		      if((i > 0) && (i < 4))
              {
                terminal_server.protocol = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"PP",2)) ))
		    {
		      token += 2;
		      if(strlen(token) <= LOGIN_PASS_LEN)
			  {
			    //moddev_supplement((byte *)token,LOGIN_PASS_LEN);
                memcpy(password.login,token,LOGIN_PASS_LEN);
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"SP",2)) ))
		    {
		      token += 2;
		      if(strlen(token) <= ADMIN_PASS_LEN)
			  {
			    //moddev_supplement((byte *)token,ADMIN_PASS_LEN);
                memcpy(password.admin,token,ADMIN_PASS_LEN);
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
            else if(! ( (strncmp(token,"CP",2)) ))
		    {
		      token += 2;
		      if(strlen(token) <= SERIAL_PASS_LEN)
			  {
			    moddev_supplement((byte *)token,SERIAL_PASS_LEN);
                memcpy(password.serial,token,SERIAL_PASS_LEN);
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"IC",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);                      /* Internet console */
			  if(i < 2)
              {
                flag.internet_console = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"SC",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);                        /* Serial Console */
			  if(i < 2)
              {
                flag.serial_console = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else if(! ( (strncmp(token,"WS",2)) ))
		    {
		      token += 2;
		      i = np_np_atoi(token);                             /* Webserver */
			  if(i < 2)
              {
                flag.webserver = i;
                retval = SUCCESS;
		      }
		      else
		        retval = WRONG_PARAMS;
		    }
			else
		    {
		      retval = WRONG_PARAMS;
		    }


#ifdef DEBUG		    
			printf("token = %s\n\r",token);
#endif
		    token = strtok(NULL," -");
		  }
        }
		break;
      
      case 'p':
      case 'P':
        retval = PRINT_SETTINGS;
	    break;
	  
	  case 'F':
	    if((*port2002.cmd_out_ptr)  =='A' &&\
		   *(port2002.cmd_out_ptr+1)=='C' &&\
		   *(port2002.cmd_out_ptr+2)=='T' &&\
		   *(port2002.cmd_out_ptr+3)=='O' &&\
		   *(port2002.cmd_out_ptr+4)=='R' &&\
		   *(port2002.cmd_out_ptr+5)=='Y')
		 {
		   retval = FACTORY;
		 }
		 else
		 {
		   retval = WRONG_FORMAT;
		 }  
		 break;

      case 'R':
	    if((*port2002.cmd_out_ptr  =='E' &&\
		   *(port2002.cmd_out_ptr+1)=='S' &&\
		   *(port2002.cmd_out_ptr+2)=='E' &&\
		   *(port2002.cmd_out_ptr+3)=='T'))
	    { 
		 retval = RESET; 
	   	}
		else
		{
          retval = WRONG_FORMAT;                        /* Wrong Format */
		}
        break;

	  case 'c':
	  case 'C':
	    retval = WRITE_FLASH;
		break;

	  case 'q':	
	  case 'Q': 
	    retval = QUIT; 
		break;
	  
	  default:
		retval = PROMPT;                               /* Return Prompt */
		break;
    }
  }
  
  return (retval);		                            /* wait for command */
}


/*------------------------------------------------------------------------
Function:    mn_port2002_handler()
Description: Handles the connection on port 2002
Input:       socket pointer
Output:      none
------------------------------------------------------------------------*/
void port2002_handler(PSOCKET_INFO *psocket_ptr)
cmx_reentrant 
{
  PSOCKET_INFO socket_ptr = PTR_NULL;
  byte i;
  
  if ( (mn_find_socket(2002, 0, null_addr, PROTO_TCP, 0)) == PTR_NULL )
  {
    mn_open(null_addr, 2002, 0, NO_OPEN, PROTO_TCP,STD_TYPE,\
	  port2002.socket_buff, MN_ETH_TCP_WINDOW, 0);/* Socket always open */
  }

  if(port2002.socket_num >= 0)	/* is 0/1/3 when need to check password */
  {
    port2002.found=TRUE;
    socket_ptr = &sock_info[port2002.socket_num];
    if (socket_ptr != PTR_NULL &&\
	    !socket_ptr->tcp_unacked_bytes) /* if no bytes need to be ACK   */
    {
      switch(port2002.counter) 
      {
	    case 0:				          /* 0: if version needs to be sent */
	      port2002.queue=0;
		  if(((strlen(password.admin)) == NULL) || (password.admin[0] == 0x0d))
		  {
            port2002.counter += 2;
		  }
		  else
		  {
		    socket_ptr->send_ptr = "\r\nPassword?";
            socket_ptr->send_len = (word16)strlen("\r\nPassword?");
	        port2002.counter++;
		  }
		  break;

	    case 1:
	      break;
	    case 2:					          /* 3: if login is successful  */
	      socket_ptr->send_ptr = "\r\nEIS32-W>";
 	      socket_ptr->send_len = (word16)strlen("\r\nEIS32-W>");
	      port2002.counter++;
	      break;
	    default:
	      break;
      }
      
	  *psocket_ptr = socket_ptr; 
      if(mn_tcp_send(socket_ptr) < 0)         /* Failed to send Data ?  */
      {
        mn_tcp_close(socket_ptr);
	    port2002.socket_num = -1;
       	port2002.found      = FALSE;
	    port2002.counter    = 0;
      }
    }
  }

  port2002.found = FALSE;

  for (i = 0;i < MN_NUM_SOCKETS; i++)
  {
    if (SOCKET_ACTIVE(i)&&\
        sock_info[i].src_port == 2002 &&\
        sock_info[i].dest_port != 0)
    {
      port2002.found = TRUE;
      if(port2002.socket_num < 0)
      {
        port2002.socket_num = i;
        //p2k2_start_timer();
      }
      else if(i != port2002.socket_num)
      {
        socket_ptr = &sock_info[i];
	    mn_tcp_close(socket_ptr);
      }
    }
  }

  if(port2002.found == TRUE)
  {
    //p2k2_check_timeout();
    if((p2k2_timeout== 1) || (p2k2_timeout == 3) || (port2002.queue == 1))
    {
      socket_ptr = &sock_info[port2002.socket_num];
      if(p2k2_timeout == 1)
      {
        socket_ptr->send_ptr = "\r\nThe socket will close in 2 seconds.\r\n";
        socket_ptr->send_len = (word16)strlen("\r\nThe socket will close in 2 seconds.\r\n");
	    mn_tcp_send(socket_ptr);
      }
      else
      {
        mn_tcp_close(socket_ptr);
	    p2k2_timeout=0;
	    port2002.queue=0;
      }
    }
  }

  if(port2002.found==FALSE)
  {
    port2002.socket_num=-1;
    port2002.counter=0;
  }
}
