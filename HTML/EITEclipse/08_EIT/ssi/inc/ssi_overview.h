#ifndef MICRONET_SSI_OV
#define MICRONET_SSI_OV      1

word16 ssi_pswd_msg(byte **str_) cmx_reentrant;   
word16 model_func(byte **str_) cmx_reentrant;     /* Return Model No    */
word16 version_func(byte **str_) cmx_reentrant;   /* Return Firmware Ver*/
word16 getMac_func(byte *str) cmx_reentrant;      
word16 mac_func(byte **str_) cmx_reentrant;       /* Return MAC Address */
word16 port_func(byte **str_) cmx_reentrant;      /*Return Ethernet Info*/
word16 dhcp_func(byte **str_) cmx_reentrant;      /* Return DHCP Flag   */
word16 gw_ip_func(byte **str_) cmx_reentrant;      /* Return DHCP Flag   */
word16 snmp_func(byte **str_) cmx_reentrant;      /* Return SNMP Flag   */
word16 modbus_func(byte **str_) cmx_reentrant;    /* Return MODBUS Flag */
word16 get_IPAddr_func(byte *str, byte *ip_addr) cmx_reentrant;
word16 ip_func(byte **str_) cmx_reentrant;        /* Return IP Address  */
word16 submask_func(byte **str_) cmx_reentrant;   /* Return Subnet Mask */
word16 hname_func(byte **str_) cmx_reentrant;     /* Return Hostname    */
word16 serial_func(byte **str_) cmx_reentrant;    /* Return Serial Info */
word16 ssi_dns(byte **str_) cmx_reentrant; 

#endif
