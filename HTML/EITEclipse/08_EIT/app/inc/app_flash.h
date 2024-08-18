#ifndef app_flash
#define app_flash	1

/*----------------------Function Prototypes ----------------------------*/
void flash_write_default(void);             /* Write Defaults to Flash  */
void write_all(void);						/* Write All Conf to Flash  */
void read_flash(void);						/* Read Flash               */
void assume_defaults(void); 

/*----------------------Macros -----------------------------------------*/
#define FLASH_START_ADDR                 0x0007D000  /* Starting Address of Last Sector of Flash */
#define FLASH_HOSTNAME_LEN	             7	
#define FLASH_ADDR_MAC                   1
#define FLASH_ADDR_IP                    7
#define FLASH_ADDR_NETMASK               11
#define FLASH_ADDR_GATEWAY               15
#define FLASH_ADDR_DNS                   19
#define FLASH_ADDR_HOSTNAME              23

#define FLASH_ADDR_LOGIN_PASS            30
#define FLASH_ADDR_ADMIN_PASS            46
#define FLASH_ADDR_SERIAL_PASS           62
#define FLASH_ADDR_DEVICE_NAME           78
#define FLASH_ADDR_DEVICE_COMMAND        142
#define FLASH_ADDR_DEVICE_UNIT           206
#define FLASH_ADDR_DEVICE_CHECK          274
#define FLASH_ADDR_DEVICE_FORMAT         275
#define FLASH_ADDR_BAUD                  283
#define FLASH_ADDR_DATA_BITS             284
#define FLASH_ADDR_PARITY                285
#define FLASH_ADDR_STOP_BITS             286
#define FLASH_ADDR_FLOW_CONTROL          287
#define FLASH_ADDR_NO_CONNECTIONS        288
#define FLASH_ADDR_LOCAL_PORT            289
#define FLASH_ADDR_REMOTE_ENABLE         294
#define FLASH_ADDR_REMOTE_IP             295
#define FLASH_ADDR_REMOTE_PORT           299
#define FLASH_ADDR_END_CHAR              304
#define FLASH_ADDR_FWD_END_CHAR          305
#define FLASH_ADDR_BUFFERING_TIME        306
#define FLASH_ADDR_INACTIVITY_TIMEOUT    310
#define FLASH_ADDR_DHCP                  314
#define FLASH_ADDR_AUTONEGOTIATION       315
#define FLASH_ADDR_SPEED             	 316
#define FLASH_ADDR_DUPLEX                317
#define FLASH_ADDR_SNMP                  318
#define FLASH_ADDR_SMTP                  319
#define FLASH_ADDR_SERIAL_CONSOLE        320
#define FLASH_ADDR_INTERNET_CONSOLE      321
#define FLASH_ADDR_WEBSERVER             322
#define FLASH_ADDR_PROTOCOL              323
#define FLASH_ADDR_SMTP_IP	             324
#define FLASH_ADDR_SMTP_FROM             328
#define FLASH_ADDR_SMTP_SUBJECT          358
#define FLASH_ADDR_TRAP_ENABLE           388
#define FLASH_ADDR_TRAP_IP				 389
#define FLASH_ADDR_ALARM0_ENABLE		 393
#define FLASH_ADDR_ALARM0_INPUT_PIN		 394
#define FLASH_ADDR_ALARM0_PIN_CONDITION	 395
#define FLASH_ADDR_ALARM0_CHAR1			 396
#define FLASH_ADDR_ALARM0_CHAR2			 397
#define FLASH_ADDR_ALARM0_EMAIL			 418
#define FLASH_ADDR_ALARM0_TRAP			 424
#define FLASH_ADDR_ALARM0_REMINDER_INTERVAL 478
#define FLASH_ADDR_ALARM0_RECIPIENTS	 498
#define FLASH_ADDR_ALARM0_MSG			 748
#define FLASH_ADDR_RS232_485_GPIO		 898
#define FLASH_ADDR_GPIO_TYPE			 899
#define FLASH_ADDR_GPIO_DIRECTION		 900
#define FLASH_ADDR_OP_LEVEL				 906
#define FLASH_ADDR_IF_CONDITION          912
#define FLASH_ADDR_THEN_CONDITION        915
#define FLASH_ADDR_PACKET_LENGTH		 918
#define FLASH_ADDR_RS485_TX_RX			 922
#define FLASH_ADDR_RS485_TIMER			 923
#define FLASH_ADDR_IP_EX_1				 927
#define FLASH_ADDR_TERMINAL_SERVER_CONNECT_ON 975
#define FLASH_ADDR_TUNNELING_CONNECT_ON	 976
#define FLASH_ADDR_DISCONNECT_BY		 977
#define FLASH_ADDR_TUNNELING_RETRY_TIMER 978
#define FLASH_ADDR_CONNECTION_TYPE       982

#define FLASH_HTTP_PORT                  983   //wendy
#define FLASH_ADDR_MULTIHOST_RETRY_COUNTER 988
#define FLASH_ADDR_MULTIHOST_RETRY_TIMEOUT 989
#define FLASH_ADDR_MULTIHOST_RIP         993
#define FLASH_ADDR_MULTIHOST_REMOTE_PORT 1041
#define FLASH_ADDR_SNMP_CONTACT          1101
#define FLASH_ADDR_SNMP_COMMUNITY        1112
#define FLASH_ADDR_SNMP_LOCATION         1123
#define FLASH_ADDR_SECURITY_FLAG         1134

#define FLASH_ADDR_GPIO_DIRECTION_07     1135
#define FLASH_ADDR_OP_LEVEL_07           1137
#define FLASH_ADDR_IF_CONDITION_04       1139
#define FLASH_ADDR_THEN_CONDITION_04     1140

#endif
