/* Refer to the Readme.txt */


#include "micronet.h"
#include "u2e_http.h"
#include "np_includes.h"

void lcd_msg_func(PSOCKET_INFO) cmx_reentrant;
word16 getMsg_func(byte **) cmx_reentrant;
void main_task(void) cmx_reentrant;
int hardware_init(void);

unsigned long HTTP_PORT;
byte eth_src_addr[IP_ADDR_LEN];

byte default_page1[20];// = "index.htm"; 
byte default_page2[20];// = "index.html";								    

byte ip_dns_addr[IP_ADDR_LEN]  = {DEFAULT_DNS1,\
                                  DEFAULT_DNS2,\
								  DEFAULT_DNS3,\
								  DEFAULT_DNS4}; /* DNS IP */

byte dhcp_status;
byte temp_buff[MSG_BUFF_SIZE];
byte cgi_buff[CGI_MAX];

/* buffer variables for incoming UART chars. */
byte uart_in_buff[BUFF_SIZE];
volatile word16 uib_in;
volatile word16 uib_count;
word16 uib_out;
byte eth_out_buff[MN_ETH_TCP_WINDOW];
byte buff_2002[200];

struct passwd password;
struct nw network;
struct s serial;
struct t terminal_server;
struct t1 tunneling;
struct d device;
struct m iserver;
struct f flag; 
struct e smtp;
struct n snmp;
struct a alarm[5];
struct g gpio;
struct dc cmd;
struct ex exclusivity;
struct p2k2 port2002;
struct mh multihost;

/* buffer variables used by incoming ethernet chars and outgoing UART chars */
BUFF_POOL_T buff_pool;

/**************************************************************************************************
 Function:    main()
 Description: Get the ball Rolling
 Input:       None
 Output:      None				
**************************************************************************************************/
int main(void)
{
  SCHAR retval;
  
  ////////////////////////////////////MODEL HERE
  iserver.model = detect_model();
  if(iserver.model == NULL)
  {
    retval = -1;
  }

  if(hardware_init() < 0) /* Initialize the Hardware */
  {
    retval = -1;
  }

  data_or_conf_mode();    /* Do we need to go to data or configuration mode */
  
  while(1)
  {
    main_task();		  /* Start the Main Task */
  }
  
  return (retval);		 
  
}

/**************************************************************************************************
 Function:    main_task()
 Description: Main Task
 Input:       None
 Output:      None
**************************************************************************************************/
void main_task(void) 
cmx_reentrant
{
  SCHAR retval;
  PINTERFACE_INFO interface_ptr;
  byte bcast_addr[IP_ADDR_LEN]={255,255,255,255};/*auto 4.2a*/
  byte DU_socket_buff[10];
  byte set_ip_socket_buff[64];
  int i;
  unsigned int j = 0;
  unsigned int k = 0;

  /* Create a pointer to the interface */
  interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

  /* If there is no login password the default web page must be changed to frame.htm
  otherwise, it should be  index.htm which will ask for password. So otherwise we
  should not ask for password */
  if(password.login[0] == NULL)
  {
    /* This is fix from the http.c file. I have removed these default pages from there
	and moved over here. These were constants before, not any more. Also they are
	moved to np_includes.h */
	strcpy((char *)default_page1,"overview.htm"); 
  }
  else
  {
	strcpy((char *)default_page1,"index.htm");
  }

  /* Call mn_init before using any other MicroNet API functions. */
  if (mn_init() < 0) 
  {
    MN_TASK_STOP;
    return;
  }

  retval = -1;

  /* Open interface. mn_assign_interface and mn_open_interface must be called before doing anything 
  with an interface. */
  retval = mn_assign_interface(0, eth_src_addr, PTR_NULL, ip_dns_addr);

  if (retval < 0)
  {
    MN_TASK_STOP;
    return;
  }

  /* Open Interface */
  if (mn_open_interface(0) < 0) 
  {
    MN_TASK_STOP;
    return;
  }
  
  if(flag.webserver == ENABLED)
  {
    /* Initialize the Webpages */
    webpages_init();  
  }

#if(MN_DHCP)    			
  if(flag.dhcp == FALSE)		/* DHCP Disabled */
  {
    bootpMode = BOOTP_INACTIVE;
    dhcp_lease.infinite_lease = TRUE;
    dhcp_lease.dhcp_state = DHCP_OK;
  }
  else if(flag.dhcp == TRUE)	/* DHCP Enabled */
  {
    #define LEASE_TIME 1000
    flag.dhcp = 1;
#if (DEBUG_LEVEL == MEDIUM)
    printf("DHCP looking for IP....\r\n");
#endif
    while(mn_dhcp_start(PTR_NULL, LEASE_TIME) != 1)
    {
      mn_wait_ticks(10);
      printf(".");
    }
  }
#endif
  
  /* Got the IP from DHCP. Assign it to the working IP */
  memcpy(eth_src_addr,interface_ptr->ip_src_addr,IP_ADDR_LEN); 
  /* Also assign the DNS Address */
  memcpy(ip_dns_addr,interface_ptr->ip_dns_addr,IP_ADDR_LEN);

#if (DEBUG_LEVEL == MEDIUM)
  printf("IP = %d.%d.%d.%d...\n\r",eth_src_addr[0],\
          eth_src_addr[1],eth_src_addr[2],eth_src_addr[3]);
#endif
  
  /* Start Opening Sockets */
  /* Discover Utility UDP Socket */
  mn_open(bcast_addr, 7011, 7011, NO_OPEN, PROTO_UDP, STD_TYPE, DU_socket_buff, 64, 0);

  /* Port 1 for Setting the IP Address using the iConnect Software */
  mn_open(null_addr,IPPORT, 0, NO_OPEN, PROTO_TCP, STD_TYPE, set_ip_socket_buff, 64, 0);

  /*---------------------------------------------------------------------------------------------
	          SIMULTANEOUS Connection
  ---------------------------------------------------------------------------------------------*/
  if(multihost.connection_type == SIMULTANEOUS)
  {
    for(i = 0; i < 12; i++)
    {
      /* If the IP address is not starting with 0 then they are valid */
	  if(multihost.rip[i][0] != NULL)
      {
        multihost.conns_to_make++;	/* Make a counter to the no of connections to make */
      }  
    }
    
	for(i = 0; i < multihost.conns_to_make; i++)
    {
      /* ------------T C P -----------------------------------*/
      if(terminal_server.protocol == TCP_PROTOCOL)   
      {
	    /* Multihost tunneling Connection */
	    multihost.r_sock_no[i] = mn_open(multihost.rip[i], 3000, multihost.rp[i],\
          ACTIVE_OPEN,PROTO_TCP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
      }
	  else if(terminal_server.protocol == UDP_PROTOCOL)
	  { 
	    /*-------------UDP---------------------------------------*/
	    multihost.r_sock_no[i] = mn_open(multihost.rip[i], 3000, multihost.rp[i],\
          ACTIVE_OPEN,PROTO_UDP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff,\
		  MN_ETH_TCP_WINDOW, 0);
      }

	  if(multihost.r_sock_no[i] >= 0) 
      {
        printf("PR..Opened conn: %d.%d.%d.%d\r\n",multihost.rip[i][0],\
		multihost.rip[i][1],multihost.rip[i][2],multihost.rip[i][3]);
        multihost.conn_estbld[j] = i;
		j++;
      }   
      else
      {
        for(k = 0; k < multihost.retry_counter; k++)
		{
          /* Multihost tunneling Connection */
	      multihost.r_sock_no[i] = mn_open(multihost.rip[i], 3000, multihost.rp[i],\
            ACTIVE_OPEN,PROTO_TCP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);		  

		  if(multihost.r_sock_no[i] >= 0) 
          {
            printf("Retry: %d, Opened conn: %d.%d.%d.%d\r\n",k,multihost.rip[i][0],\
		      multihost.rip[i][1],multihost.rip[i][2],multihost.rip[i][3]);
		    multihost.conn_estbld[j] = i;
			flag.retry = TRUE;
            break;
		  }
		  else
		  {
			printf("Retry: %d, Cant connect: %d.%d.%d.%d Trying again.\r\n",k,multihost.rip[i][0],\
		      multihost.rip[i][1],multihost.rip[i][2],multihost.rip[i][3]);
		    delayMs(1,multihost.retry_timeout);
			flag.retry = FALSE;
          }
        }   
      }  
    }
    //flag.retry = FALSE; 
  }
  /*---------------------------------------------------------------------------------------------
	                                SEQUENTIAL CONNECTIONS
  ---------------------------------------------------------------------------------------------*/
  else if(multihost.connection_type == SEQUENTIAL)
  {
    for(i = 0; i < 12; i++)
    {
      if(multihost.rip[i][0] != NULL)
      {
        multihost.conns_to_make++;
      }  
    }
    
	for(i = 0; i < multihost.conns_to_make; i++)
    {
      for(k = 0; k < multihost.retry_counter; k++)
      {
        /* ------------T C P -----------------------------------*/
        if(terminal_server.protocol == TCP_PROTOCOL)   
        {
	      multihost.r_sock_no[i] = mn_open(multihost.rip[i], 3000, multihost.rp[i], ACTIVE_OPEN,\
            PROTO_TCP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
        }
	    else if(terminal_server.protocol == UDP_PROTOCOL)
	    { 
	      /*-------------UDP---------------------------------------*/
	      multihost.r_sock_no[i] = mn_open(multihost.rip[i], 3000, multihost.rp[i], ACTIVE_OPEN,\
		  PROTO_UDP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
        }
 
        if(multihost.r_sock_no[i] > 0) 
        {
          multihost.conn_estd_num = i;
		  printf("PR...Open conn..\r\n");
          flag.retry = TRUE;
          break;                                      /* Connection established Dont try anymore */
        }   
        else
        {
          delayMs(1,multihost.retry_timeout);
		  flag.retry = FALSE;
          printf("PR...Can't open conn...\r\n");
        }
	  }	  
    }
  }
  /*--------------------------------------------------------------------------------------------- 
	                             Local Tunneling Socket 
  ---------------------------------------------------------------------------------------------*/
  else if((multihost.connection_type == 1) && terminal_server.no_connections > 0) 
  {
    /* ------------T C P -----------------------------------*/
    if(terminal_server.protocol == TCP_PROTOCOL)   
    {
	  (void)mn_open(null_addr, terminal_server.local_port, 0, NO_OPEN,\
        PROTO_TCP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
    }
	else if(terminal_server.protocol == UDP_PROTOCOL)
    {
	/*-------------UDP---------------------------------------*/
	  (void)mn_open(null_addr, terminal_server.local_port, 0, NO_OPEN,\
        PROTO_UDP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0); 
	}
	else if(terminal_server.protocol == MODBUS_PROTOCOL)
	{
	  (void)mn_open(null_addr, 502, 0, NO_OPEN,\
        PROTO_TCP, STD_TYPE, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
	}
  }
  else if((multihost.connection_type == 1) && (terminal_server.no_connections == 0))
  {
    /*------------ Port 1000 Socket -------------------------*/
    (void)mn_open(null_addr, 1000, 0, NO_OPEN, PROTO_TCP, STD_TYPE, buff_pool.data_buff,\
      MN_ETH_TCP_WINDOW, 0);
  }   
  
  /* Internet Console Socket */
  if(flag.internet_console == ENABLED)
  {
    mn_open(null_addr, 2002, 0, NO_OPEN, PROTO_TCP, STD_TYPE ,\
      port2002.socket_buff, MN_ETH_TCP_WINDOW, 0); /* Inet Console Port */
    init_cmd_buff();
  }

#if(DEBUG_LEVEL == MEDIUM)
  read_flash();	         /* Read all settings from Flash */
  printVBTable();
#endif


  /* Set the Mask for the IO lines meant to Drive the TX/RX lines P1.30(TX) and P1.31(RX) */
  FIO1DIR3  = 0xC0; /* Set P1.30 and P1.31 as Output */
  FIO1MASK3 = 0x3f; /* Set the Mask for P1.30,P1.31 */
  
  /* Condition 1: Send the Power On Alarm if enabled */
  if((flag.smtp == ENABLED) || ((flag.snmp == ENABLED) && (flag.trap_en == ENABLED)))
    power_on_alarm();       
  
  /* Finally Start the main server but donot return unless something went wrong */
  mn_server();           /* Initialize main Server */

  mn_close_interface(0); /* If mn_server exited..close the interface */
  MN_TASK_STOP;
}


/**************************************************************************************************
 Function:    hardware_init()
 Description: Initialize the Hardware.
 Input:       None
 Output:      None
**************************************************************************************************/
int hardware_init(void)
{
  extern void fnInitFlashDriver(void);
  int ret_val = 0;
  
#ifdef _POST_FILES
  fnInitFlashDriver();				                    // initialise FLASH driver once on start up
  fnClearUploadSpace();
#endif

  /*-----------------------------------------------------------------------------------------------
   Read all settings from Flash 
  -----------------------------------------------------------------------------------------------*/
  read_flash();
  
  /*-----------------------------------------------------------------------------------------------
   Set the HTTP Port 
  -----------------------------------------------------------------------------------------------*/
  HTTP_PORT=flag.HTTP_PORT;//wendy                
  
  /*-----------------------------------------------------------------------------------------------
  Make the P0.26 as Input
  -----------------------------------------------------------------------------------------------*/
  FIO0DIR3  = 0xfb;                                     /* Making P0.26 as Input,P0.25 as Output */
  FIO0MASK3 = 0xf9;
  FIO0SET3 = 0x02;										/* Make P0.25 High */
  
  /* Write defaults to the flash if the reset defaults button is pressed */
  if(!(FIO0PIN3 & 0x04))
  {
    flash_write_default();           /* Flag raised, write the defaults */
	FIO0CLR3 = 0x02;
  }
  

  /*-----------------------------------------------------------------------------------------------
  Reset all the counters 
  -----------------------------------------------------------------------------------------------*/
  uib_in = uib_out      = uib_count             = 0;
  serial.rx_bytes       = serial.tx_bytes       = 0;
  network.tcp_rx_bytes  = network.tcp_tx_bytes  = 0;
  network.udp_rx_bytes  = network.udp_tx_bytes  = 0;
  network.icmp_rx_bytes = network.icmp_tx_bytes = 0;
  flag.isvr_condition   = TRUE;
  port2002.socket_num   = -1;
  port2002.found        = FALSE; /* Flag : whether port 2002 is found   */
  port2002.counter      = 0;     /* counter on telnet on port 2002      */
  port2002.queue        = 0;
  flag.isvr_accessed    = TRUE;
  flag.isvr_disconnect  = TRUE;
  flag.isvr_condition   = TRUE;
  flag.write_defaults   = DISABLED;
  flag.endchar_recvd    = FALSE;
  flag.pkt_len_recvd    = FALSE;
  flag.snmp_var_set     = FALSE;
  flag.retry            = TRUE;
  multihost.retry_count	= 0;
  multihost.conns_to_make = 0;
  buff_pool.db_out      = 0;
  buff_pool.len         = 0;
  buff_pool.xmit_busy   = FALSE;
  flag.password_entered_incorrect = FALSE;
  flag.confirm_pswd_entered_incorrect = FALSE;
  flag.reboot           = FALSE; 
  flag.admin_password_entered_correct = FALSE;
  flag.dont_try_remote_conn = FALSE;
  
  /*-----------------------------------------------------------------------------------------------
  Initialize UART 
  -----------------------------------------------------------------------------------------------*/
  initUART(); 

  return (ret_val);
}


/**************************************************************************************************
  Function:       GPIOResetInit
  Descriptions:	  Initialize the target board before running the main() 
                  function; User may change it as needed, but may not 
                  deleted it.

  Input: None
  Returned value: None
**************************************************************************************************/
void GPIOResetInit( void )
{
  /*-----------------------------------------------------------------------------------------------
   Reset all GPIO pins to default: primary function 
  -----------------------------------------------------------------------------------------------*/
  PINSEL0 = 0x00000000;
  PINSEL1 = 0x00000000;
  PINSEL2 = 0x00000000;
  PINSEL3 = 0x00000000;
  
  PINSEL4 = 0x00000000;
  //PINSEL4 = 0x0000AAAA; Ajay Uncomment only if hardware flow control is not working.

  PINSEL5 = 0x00000000;
  PINSEL6 = 0x00000000;
  PINSEL7 = 0x00000000;
  PINSEL8 = 0x00000000;
  PINSEL9 = 0x00000000;
  PINSEL10 = 0x00000000;
  /*-----------------------------------------------------------------------------------------------
   The 6 GPIOs(P2.2-P2.7) should all be with no pull ups and pull down 
  -----------------------------------------------------------------------------------------------*/
  PINMODE4 |= 0x0000AAA0; 
    
  IODIR0 = 0x00000000;
  IODIR1 = 0x00000000;
  IOSET0 = 0x00000000;
  IOSET1 = 0x00000000;
    
  FIO0DIR = 0x00000000;
  FIO1DIR = 0x00000000;
  FIO2DIR = 0x00000000;
  FIO3DIR = 0x00000000;
  FIO4DIR = 0x00000000;
    
  FIO0SET = 0x00000000;
  FIO1SET = 0x00000000;
  FIO2SET = 0x00000000;
  FIO3SET = 0x00000000;
  FIO4SET = 0x00000000;

  /*-----------------------------------------------------------------------------------------------
   Turn on FIO on GPIO 1: Bit 0: 1 = High speed GPIO enabled on Port 0, 0 - disabled.
  -----------------------------------------------------------------------------------------------*/
  SCS |= 0x00000001;                        
  
  /*-----------------------------------------------------------------------------------------------
  Turn on Timer 2 and Timer 3: PCONP.22 = Timer Control 2, PCONP.23 = Timer Control 3 
  -----------------------------------------------------------------------------------------------*/
  PCONP |= 0x00C00000;

  return;        
}

/**************************************************************************************************
  Function:       detect_model(void)
  Descriptions:	  detect the model using the solder jumps. 
  Input:          None
  Returned value: 1: EIT-W-2F
                  2: EIT-W-4
				  3: EIT-D-2F
				  4: EIT-D-4
				  5: EIT-PCB-2F
				  6: EIT-PCB-4
				  7: EIT-PCB-T
**************************************************************************************************/
char detect_model(void)
{
  char ret_val = 0;
  unsigned char j_7;
  unsigned char ss_P;
  unsigned char ss_N;
  unsigned char ss_R;

  io_dir(0, 1, INPUT);
  io_dir(0, 5, INPUT);
  io_dir(0, 7, INPUT);
  io_dir(0, 8, INPUT);

  ss_P = test_bit(0,1); 
  j_7  = test_bit(0,5);
  ss_N = test_bit(0,7);
  ss_R = test_bit(0,8);

  //printf(" ss_P = %d\r\n j_7 = %d\r\n ss_N = %d\r\n ss_R = %d\r\n",ss_P,j_7,ss_N,ss_R);
  if((ss_P == 1) && (j_7 == 1) && (ss_N == 0) && (ss_R == 0))
  {
    ret_val = EIT_W_2F;
  }
  else if((ss_P == 0) && (j_7 == 0) && (ss_N == 0) && (ss_R == 0))
  {
    ret_val = EIT_W_4;
  }
  else if((ss_P == 1) && (j_7 == 1) && (ss_N == 1) && (ss_R == 0))
  {
    ret_val = EIT_D_2F;
  }
  else if((ss_P == 0) && (j_7 == 0) && (ss_N == 1) && (ss_R == 0))
  {
    ret_val = EIT_D_4;
  }
  else if((ss_P == 1) && (j_7 == 1) && (ss_N == 0) && (ss_R == 1))
  {
    ret_val = EIT_PCB_2F;
  }
  else if((ss_P == 0) && (j_7 == 0) && (ss_N == 0) && (ss_R == 1))
  {
    ret_val = EIT_PCB_4;
  }
  else if((ss_P == 1) && (j_7 == 0) && (ss_N == 0) && (ss_R == 1))
  {
    ret_val = EIT_PCB_T;
  }

  return ret_val;
}
