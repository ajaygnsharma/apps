#ifndef MICRONET_My_includes
#define MICRONET_My_includes		1

/*----------------------- SSI and POST Header Files --------------------*/
#include "ssi_overview.h"
#include "ssi_network.h"
#include "ssi_serial.h"
#include "ssi_security.h"
#include "ssi_management.h"
#include "ssi_alarm1.h"
#include "ssi_alarm2.h"
#include "ssi_alarm3.h"
#include "ssi_alarm4.h"
#include "ssi_alarm5.h"
#include "ssi_device_setup.h"
#include "ssi_gpio.h"
#include "ssi_device_query.h"
#include "ssi_read_device.h"
#include "ssi_diagnostics.h"
#include "ssi_bridge.h"
#include "ssi_packing.h"
#include "ssi_multihost.h"

#include "post_admin.h"
#include "post_network.h"
#include "post_serial.h"
#include "post_security.h"
#include "post_security_admin.h"
#include "post_management.h"
#include "post_alarm1.h"
#include "post_alarm2.h"
#include "post_alarm3.h"
#include "post_alarm4.h"
#include "post_alarm5.h"
#include "post_index.h"
#include "post_device_setup.h"
#include "post_system.h"
#include "post_system_admin.h"
#include "post_gpio.h"
#include "post_device_query.h"
#include "post_read_device.h"
#include "post_diagnostics.h"
#include "post_login.h"

#include "webpages.h"
#include "serial_cfg.h"
#include "u2e_http.h"
#include "email.h"
#include "alarm.h"
#include "modbus.h"
#include "data_conf_mode.h"
#include "port2002_handler.h"
#include "rs485_snn.h"
#include "rs232_full_snn.h"
#include "rs232_gpio_snn.h"
#include "tcp_local_bridge.h"
//#include "ipport.h"

/*----------------- Driver Header --------------------------------------*/
#include "app_flash.h"
#include "wdt.h"
#include "flash.H"          
#include "timer.h"
#include "irq.h"

//#include "LPC23xx.h"		/* LPC23xx/24xx Peripheral Registers	*/
//#include "type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRMWARE_VERSION  "1.1g"

/*-------------------------------------------------------------------------------------------------
*      M O D E L      T Y P E 
* Basically 7 types of Models are present:
*-------------------------------------------------------------------------------------------------*/
#define EIT_W_2F   1
#define EIT_W_4    2
#define EIT_D_2F   3
#define EIT_D_4    4
#define EIT_PCB_2F 5
#define EIT_PCB_4  6
#define EIT_PCB_T  7

//#define TIME_INTERVAL	14999
#define INPUT  0 
#define OUTPUT 1

#define LOGIC_LOW  0 
#define LOGIC_HIGH 1

#define PORT_0 0
#define PORT_2 2

/*-------------------------------------------------------------------------------------------------
DEBUG Level
--------------------------------------------------------------------------------------------------*/
#define LOW                    1
#define MEDIUM                 2
#define HIGH                   3

/*--------------------Macros -------------------------------------------*/
#define Fcclk	               48000000     /* 48 MHz Main Clock        */
#define Fpclk	               (Fcclk / 4)	/* 12 MHz Periphrl Clk      */
#define DEBUG_LEVEL	           DISABLED   	/* Debug/Not                */
#define PARA_NUM               31           // TELNET CONFIGURATION PARAMETER ?
#define CNFG_PORT              2002         /* Configuration Port       */
#define LEASE_TIME             1000         /* DHCP lease time          */
#define TCP_PROTOCOL           1            /* TCP 0; UDP 1: MODBUS 2   */
#define UDP_PROTOCOL           2            /* TCP 0; UDP 1: MODBUS 2   */
#define MODBUS_PROTOCOL        3            /* TCP 0; UDP 1: MODBUS 2   */


#define TRUE                   1            /* TRUE or YES              */
#define FALSE                  0            /* FALSE or YES             */
#define ENABLED                1            /* ENABLED or TRUE or YES   */
#define DISABLED               0            /* ENABLED or FALSE or YES  */
#define ESTABLISHED            1            /* Connection established ? */
#define FAILED                 0            /* Connection established ? */


/*------------------------------------------------------------------------
                        BUFFER LENGTHS
------------------------------------------------------------------------*/
#define EMAIL_LEN              30+1         /* Length of Email Address  */
#define SUB_LEN                30+1         /* Length of Subject        */
#define LOGIN_PASS_LEN 		   16				
#define ADMIN_PASS_LEN 		   16
#define PASSWORD_LEN           16				
#define SERIAL_PASS_LEN        16           
#define HOSTNAME_LEN           7            /* Length of Hostname       */
#define EMAIL_ADDR_BUF_LEN     50           /* Email Address Buffer Len */
#define MSG_BUF_LEN            30           /* Msg Buffer Length        */
#define DEVICE_NAME_LEN        8            /* Length of Name of Devices*/
#define DEVICE_CMD_LEN         8            /* Length of Cmd of Devices */
#define DEVICE_UNIT_LEN        8            /* Length of Response Unit  */
#define MSG_BUFF_SIZE	       200          /* Size of Message          */
#define PNT_BUFF_SIZE          400
#define TEMP_BUFFER_SIZE       1500         /* Size of temp buffer      */
#define CGI_MAX                1500			/* CGI Buffer Limit         */
//#define MSG_BUFF_SIZE	       50           /* Size of Message          */


#define MAX_UNITS              8            /* Total No of Units Present*/
#define MAX_READ               2            /* Total No of Units read   */
#define NO_EX_IPS              8			/* No of Exclusivity IPs    */
#define ONE_GPIO               1
#define SIX_GPIO               6            /* Max GPIOs                */
#define FOUR_GPIO              4            /* Max GPIOs                */

#define SOFTWARE_FLOW_CONTROL  0x02
#define HARDWARE_FLOW_CONTROL  0x03

#define XON                    0x11
#define XOFF                   0x13

#define RS485_HDPLX            0x01
#define RS485_FDPLX            0x02

#define HALF_DUPLEX_RECV       1
#define HALF_DUPLEX_TRANS      0

#define IPPORT                 1


/*------------------------------------------------------------------------
						 DEFAULTS
------------------------------------------------------------------------*/
#define DEFAULT_DEV_CHK                0x00
#define DEFAULT_DATA_BITS              0x04
#define DEFAULT_PARITY                 0x01
#define DEFAULT_STOP_BITS              0x01
#define DEFAULT_FLOW_CONTROL           0x01
#define DEFAULT_BAUD                   0x06
#define DEFAULT_LOCAL_PORT             2000
#define DEFAULT_NO_CONNECTIONS         0x05
#define DEFAULT_REMOTE_ACCESS          0x00
#define DEFAULT_REMOTE_IP1             0x00
#define DEFAULT_REMOTE_IP2             0x00
#define DEFAULT_REMOTE_IP3             0x00
#define DEFAULT_REMOTE_IP4             0x00
#define DEFAULT_REMOTE_PORT            0x00
#define DEFAULT_END_CHAR               0x00
#define DEFAULT_FWD_END_CHAR           0x00
#define DEFAULT_BUFFERING_TIME         0x00
#define DEFAULT_INACTIVITY_TIMEOUT     0x00
#define DEFAULT_FLAG_DHCP              0x00
#define DEFAULT_FLAG_AUTO              0x01
#define DEFAULT_FLAG_SPEED             0x00

#define DEFAULT_FLAG_DUPLEX            0x00
#define DEFAULT_FLAG_SNMP              0x00
#define DEFAULT_FLAG_SMTP              0x00
#define DEFAULT_FLAG_SERIAL_CONSOLE    0x00
#define DEFAULT_FLAG_INTERNET_CONSOLE  0x01
#define DEFAULT_WEB_SERVER             0x01
#define DEFAULT_GPIO_RS232_RS485       0x01
#define DEFAULT_GPIO_TYPE              0x01
#define DEFAULT_INPUT_DIR1             0x01   
#define DEFAULT_INPUT_DIR2             0x01   
#define DEFAULT_INPUT_DIR3             0x01   
#define DEFAULT_INPUT_DIR4             0x01                           
#define DEFAULT_INPUT_DIR5             0x01                           
#define DEFAULT_INPUT_DIR6             0x01                           
#define DEFAULT_INPUT_DIR7             0x01                           
#define DEFAULT_INPUT_DIR8             0x01                           
#define DEFAULT_OUTPUT_LVL1            0x01                           
#define DEFAULT_OUTPUT_LVL2            0x01                           
#define DEFAULT_OUTPUT_LVL3            0x01                           
#define DEFAULT_OUTPUT_LVL4            0x01                           
#define DEFAULT_OUTPUT_LVL5            0x01                           
#define DEFAULT_OUTPUT_LVL6            0x01                           
#define DEFAULT_OUTPUT_LVL7            0x01                           
#define DEFAULT_OUTPUT_LVL8            0x01                           
#define DEFAULT_IF0                    0x01                           
#define DEFAULT_IF1                    0x01                           
#define DEFAULT_IF2                    0x01                           
#define DEFAULT_THEN0                  0x01                           
#define DEFAULT_THEN1                  0x01                           
#define DEFAULT_THEN2                  0x01                           
#define DEFAULT_PACKET_LENGTH          0
#define DEFAULT_RS485_TIMER		       0
#define DEFAULT_FLAG_RS485TXRX         1
#define DEFAULT_IP_EXCLUSIVITY_00      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_01      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_02      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_03      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_10      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_11      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_12      0x00						   
#define DEFAULT_IP_EXCLUSIVITY_13      0x00						   
#define DEFAULT_MSK_EXCLUSIVITY_00     0xff						   
#define DEFAULT_MSK_EXCLUSIVITY_01     0xff						   
#define DEFAULT_MSK_EXCLUSIVITY_02     0xff						   
#define DEFAULT_MSK_EXCLUSIVITY_03     0x00						   
#define DEFAULT_MSK_EXCLUSIVITY_10     0xff						   
#define DEFAULT_MSK_EXCLUSIVITY_11     0xff						   
#define DEFAULT_MSK_EXCLUSIVITY_12     0xff						   
#define DEFAULT_MSK_EXCLUSIVITY_13     0x00						   
#define DEFAULT_LOCAL_PORT_CH1         '0'
#define DEFAULT_LOCAL_PORT_CH2         '2'
#define DEFAULT_LOCAL_PORT_CH3         '0'
#define DEFAULT_LOCAL_PORT_CH4         '0'
#define DEFAULT_LOCAL_PORT_CH5         '0'
#define DEFAULT_LOCAL_PORT_LOW_BYTE    0xD0
#define DEFAULT_CONNECT_ON             0x01
#define DEFAULT_DISCONNECT_BY          0x01
#define DEFAULT_TUNNELING_RETRY_TIMER  0x00
#define DEFAULT_ALARM0_REMINDER_INTERVAL 0x00
#define DEFAULT_CONNECTION_TYPE        0x01
#define DEFAULT_MULTIHOST_RETRY_COUNTER 0x02
#define DEFAULT_MULTIHOST_RETRY_TIMEOUT 200
#define DEFAULT_MULTIHOST_RIP          0x00
#define DEFAULT_MULTIHOST_REMOTE_PORT  0x00

#define DEFAULT_IP1                    192
#define DEFAULT_IP2                    168
#define DEFAULT_IP3                    1
#define DEFAULT_IP4                    200

#define DEFAULT_MAC1                   0x00
#define DEFAULT_MAC2                   0x03
#define DEFAULT_MAC3                   0x34
#define DEFAULT_MAC4                   0x00
#define DEFAULT_MAC5                   0xA5
#define DEFAULT_MAC6                   0x41

#define DEFAULT_HOSTNAME1              0x00
#define DEFAULT_HOSTNAME2              0x00
#define DEFAULT_HOSTNAME3              0x00
#define DEFAULT_HOSTNAME4              0x00
#define DEFAULT_HOSTNAME5              0x00
#define DEFAULT_HOSTNAME6              0x00
#define DEFAULT_HOSTNAME7              0x00

#define DEFAULT_GW1                    0
#define DEFAULT_GW2                    0
#define DEFAULT_GW3                    0
#define DEFAULT_GW4                    0

#define DEFAULT_NETMASK1               255
#define DEFAULT_NETMASK2               255
#define DEFAULT_NETMASK3               255
#define DEFAULT_NETMASK4               0

#define DEFAULT_DNS1                   0
#define DEFAULT_DNS2                   0
#define DEFAULT_DNS3                   0
#define DEFAULT_DNS4                   0

#define DEFAULT_SNMP_LOCATION          0
#define DEFAULT_SNMP_COMMUNITY         0
#define DEFAULT_SNMP_CONTACT           0

#define SMTP_SVR_IP1                   0
#define SMTP_SVR_IP2                   0
#define SMTP_SVR_IP3                   0
#define SMTP_SVR_IP4                   0

#define SIMULTANEOUS                   2
#define SEQUENTIAL 	                   3

#define SNMP_CONTACT_LEN               11
#define SNMP_COMMUNITY_LEN             11
#define SNMP_LOCATION_LEN              11
#define SNMP_DESCRIPTION_LEN           30

/*--------------------Network Information ------------------------------*/
extern struct nw
{
  char hostname[HOSTNAME_LEN + 1];          /* Hostname of Iserver      */
  byte webserver_enable;					/* Webserver ON/OFF         */
  unsigned long tcp_rx_bytes;               /* No of TCP bytes recvd    */
  unsigned long tcp_tx_bytes;               /* No of TCP bytes sent     */
  unsigned long udp_rx_bytes;               /* No of UDP bytes recvd    */
  unsigned long udp_tx_bytes;				/* No of UDP bytes sent     */
  unsigned long icmp_rx_bytes;				/* No of ICMP bytes recvd   */
  unsigned long icmp_tx_bytes;				/* No of ICMP bytes sent    */
}network;

/*--------------------Passwords ----------------------------------------*/
extern struct passwd
{
  char login[PASSWORD_LEN + 1];                         /* Login Password           */
  char admin[ADMIN_PASS_LEN+1];							/* Admin Password           */
  char serial[PASSWORD_LEN + 1];						/* Serial Console Pswd      */
}password;

/*--------------------Serial Port Configuration ------------------------*/
extern struct s
{												   
  byte baudrate;							/* Baudrate                 */
  byte databits;							/* Databits                 */
  byte parity;								/* Parity                   */
  byte stopbits;							/* Stop Bits                */
  byte flowcontrol;							/* Flow Control 0-N,1-H,2-S */
  byte transciever;							/* RS232/RS485              */
  byte modbus_tcp;							/* Enable/Disable           */
  unsigned long endchar;					/* Endchar in HEX           */
  byte fwdendchar;							/* Forward End char         */
  unsigned long buffering_time;             /* Buffering Timeout        */
  byte en_serial_pass;						/* Enable Serial Pswd       */
  unsigned long pkt_len;                    /* Packet Length of Data    */
  unsigned long rx_bytes;                   /* No:of UART bytes recvd   */
  unsigned long tx_bytes;                   /* No:of UART bytes sent    */
  unsigned long rs485timer;                 /* RS485 timer value        */
  byte fifo_size;							/* Serial Port fifo size    */
}serial;


/*--------------------Terminal Server ----------------------------------*/
extern struct t
{
  byte protocol;							/* TCP/UDP                  */
  byte svr_or_clt;							/* Server/Client            */
  byte no_connections;						/* No of Connections        */
  unsigned long local_port;                 /* TSR Port                 */
  byte connect_on;                          /* Connection Control       */
  unsigned long timeout;					/* Connection Timeout       */
  byte device_number;						/* Device No                */
  byte disconnect_by;                       /* Disconnect by            */
}terminal_server;

/*--------------------Remote Tunneling----------------------------------*/
extern struct t1
{
	byte remote_enable;						/* Remote Enable            */
	byte remote_ip[IP_ADDR_LEN];			/* Remote IP                */
	unsigned long remote_port;				/* Remote Port              */
	int remote_socket_number;               /* Remote Socket Number     */
	int remote_connection_status;           /* 0: Failed, 1: Established*/
	byte connect_on;                        /* Connect on Remote Bridge */
	unsigned long retry_timer;              /* Retry Timer              */
}tunneling;

/*--------------------Device Setup -------------------------------------*/
extern struct d
{
  byte check;                               /* Check Mark               */
  char uid[MAX_UNITS][2];                   /* Id of the Unit           */
  char name[MAX_UNITS][DEVICE_NAME_LEN + 1];/* Device Name              */
  char command[MAX_UNITS][DEVICE_CMD_LEN +1];/* Device Command          */
  char unit[MAX_UNITS][DEVICE_UNIT_LEN + 1];/* Device Unit              */
  char format[DEVICE_UNIT_LEN + 1];         /* Format of Reading        */
}device;                                           

/*-------------------Model information----------------------------------*/
extern struct m
{
  char model;                            /* Model Name               */
  char version[5];                          /* Version Number           */
}iserver;


/*-------------------Important Flags------------------------------------*/
extern struct f
{
  int autonegotiation;                      /* Autonegotiation: ON/OFF  */
  int speed;                                /* Speed 0: 10/1: 100 Mbps  */
  int duplex;                               /* Duplex 0: Half/1: Full   */
  unsigned long	HTTP_PORT; 				//wendy
  int dhcp;                                 /* DHCP   0: EN;   1: DIS   */
  int snmp;                                 /* SNMP   0: EN;   1: DIS   */ 
  int trap_en;                              /* SNMP Trap0:EN   1: DIS   */
  int modbus;                               /* MODBUS 0: EN;   1: DIS   */ 
  int serial_console;                       /* Console0: EN;   1: DIS   */
  int webserver;                            /* Webserver 0: EN;1: DIS   */
  int upgrade;                              /* Upgrade 0: EN   1: DIS   */
  int smtp;                                 /* SMTP    0: EN   1: DIS   */
  int internet_console;                     /* Inet Console 0:EN 1:DIS  */
  int ip_changed;                           /* IP Changed 0: EN 1: DIS  */
  int rs485type;                            /* 1: RS485 2 Wire; 
                                               2: RS485 4 Wire */                                       
  int gpio_type;                            /* 1: Forced; 2: Conditional*/
  int rs485txrx;                            /* 1: TX;  2: RX            */
  int ip_exclusivity;                       /* 0: DIS; 1: ENabled       */
  int isvr_accessed;                        /* Accessed 1: Not Access: 0*/
  int isvr_condition;                       /* change 1: no change: 0   */
  int isvr_disconnect;
  int isvr_char1;
  int isvr_char2;
  int port_1000_pkt;
  int write_defaults;
  int endchar_recvd;
  int pkt_len_recvd;
  int snmp_var_set;
  int retry;
  int security;
  int password_entered_incorrect;
  int confirm_pswd_entered_incorrect;
  int admin_password_entered_correct;
  int reboot;
  int dont_try_remote_conn;
  char cio; /* if conditional GPIO is enabled or not ? */
}flag;

/*-------------------Email add SNMP case -------------------------------*/
extern struct e
{
  byte server_ip[IP_ADDR_LEN];              /* From address             */
  char from[EMAIL_LEN];                     /* Message length           */
  char subject[SUB_LEN];                    /* Subject length           */
}smtp;


/*-------------------SNMP Info -----------------------------------------*/
extern struct n
{
  byte trap_ip[IP_ADDR_LEN];
  byte contact[SNMP_CONTACT_LEN + 1];
  char community[SNMP_COMMUNITY_LEN + 1];
  char location[SNMP_LOCATION_LEN + 1];
}snmp;

extern struct c                             /*----Conditions------------*/
{
    int  power_reset;	                    /* Power Reset  EN/DI       */ 
    int  ip_changed;                        /* IP Changed   EN/DI       */
    int  isvr_accessed;                     /* Iserver Accessed  EN/DI  */
    int  isvr_disconnect;                   /* Iserver Disconnect EN/DI */
    int  isvr_condition;                    /* Iserver Condition EN/DI  */
    int  chars;                             /* Chars        EN/DI       */
}condition[2];

/*-------------------Alarm Structure -----------------------------------*/
extern struct a
{
  int    enable;                            /* Enable Alarm1/2/3/4/5    */
  struct c condition[2];
  char   input_pin;                         /* Which Input pin to check */
  char   pin_condition;                     /* What Condition L/H ?     */
  unsigned long char1;                      /* Char 1 in i/p stream     */
  unsigned long char2;                      /* Char 2 in i/p stream     */
  int    reminder_interval;                 /* Alarm's Reminder Interval*/
  int    reminder_interval_flag;			/* Reminder interval flag */
  int 	 reminder_interval_counter;         /* Reminder interval counter */
  char   priority;                          /* Alarm's Priority H/L     */
  char   recipients[EMAIL_ADDR_BUF_LEN+1];  /* Alarm's Recipients       */
  char   msg[MSG_BUF_LEN+1];                /* Alarm's Message          */
  char   gpio_bit_mask;                     /* Alarm 1 GPIO bit mask    */
}alarm[5];


/*------------------- GPIO RS232 RS485 ---------------------------------*/
extern struct g
{
  int direction[8];                  /* Dir GPIO: 1 I/P or 2 O/P */
  int op_level[8];                   /* Level GPIO: 1 High/2 Low */
  int if_condition[8 - 4];           /* IF Cond : 1 High/2 Low   */
  int then_condition[8 - 4];         /* Then Cond : 1 High/2 Low */
}gpio;

/*------------------- Device Command -----------------------------------*/
extern struct dc
{
  byte cmd_buff[33];                        /* Device Command Buffer    */
  byte response_buff[33];                   /* Response Buffer          */
}cmd;


/*--------------------IP Exclusivity -----------------------------------*/
extern struct ex
{
  byte ip[NO_EX_IPS][IP_ADDR_LEN];          /* IP Exclusivity           */
}exclusivity;

/*-------------------Multihost Connection-------------------------------*/
extern struct mh
{
  unsigned char connection_type;            /* Connection type          */
  unsigned char retry_counter;          /* Retry Counter            */
  unsigned long retry_timeout;              /* Retry Timeout            */
  unsigned char rip[12][IP_ADDR_LEN];       /* Remote IP Address        */
  unsigned long rp[12];                     /* Remote Port              */
  int r_sock_no[12];                 /* Socket Number            */
  unsigned long retry_count;                /* Retry Count              */
  unsigned char conns_to_make;  /* Counter for No of Conns to make */
  int conn_estd_num;
  unsigned char conn_estbld[12]; /* Established Connections */
}multihost;

extern struct p2k2
{
  int socket_num;
  char found;	                /* Flag : whether port 2002 is found    */
  byte counter;    /* counter to  steps while doing telnet on port 2002 */
  byte queue;                                 /* Queue for the Commands */
  byte print_buff[PNT_BUFF_SIZE];             /* Buffer for print       */
  byte cmd_buff[CMD_BUFF_SIZE];     /* Buffer for Commands on port 2002 */
  byte *cmd_in_ptr;	     /* Pointer to the data coming in to the buffer */
  byte *cmd_out_ptr;     /* Pointer to the data going out of the buffer */
  word16 cmd_count;	   /* Count of the data that comes in to the buffer */
  byte socket_buff[200]; 
}port2002;

typedef unsigned char U8;
typedef unsigned long U32;
typedef char S8;
typedef long S32;
	
extern byte ip_dns_addr[IP_ADDR_LEN];       /* DNS Server IP Address    */
extern byte timeout_buff[5];
extern byte cgi_buff [CGI_MAX];			    /* CGI Includes Buff:1000 B */
extern byte temp_buff [MSG_BUFF_SIZE];      /* Temp Buff   1:    50   B */
//extern unsigned long  temp_buffer[TEMP_BUFFER_SIZE];/* Temp Buffer 150B */
extern byte eth_src_addr[IP_ADDR_LEN];      /* Iserver IP Addres 4    B */
extern unsigned long elapse_timer;
extern unsigned long elapse_size;
extern byte buff_2002[200];
extern unsigned long HTTP_PORT; //wendy

/* these are the web pages the server looks for when the client requests
   the default main page. Customarily this is index.htm or index.html but
   they can be changed if desired.
*/
//cmx_const byte default_page1[] = "index.htm"; Ajay 
extern byte default_page1[20];// = "index.htm"; 
//cmx_const byte default_page2[] = "index.html";  Ajay
extern byte default_page2[20];// = "index.html";

int init_iserver(void) cmx_reentrant;
void webpages_init(void);
void read_dip_switches( void );
byte s_ip_process(byte * ip_buff,byte position) cmx_reentrant;
char detect_model(void);

extern unsigned char *fnClearUploadSpace(void);
extern void fnInitFlashDriver(void);

#endif
