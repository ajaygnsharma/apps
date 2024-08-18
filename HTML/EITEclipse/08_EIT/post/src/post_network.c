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


byte ip_status=0;
byte ip_update_flag=0;
//byte rst_status=0;
//byte code error_ip[]="Bad Ip";
////byte code ip_message[]="<font color=red>Reset or Power Off/On the device to active new IP or Host Name</font>";
//
////extern byte temp_buff[20];
////extern byte psword_buff[7];

void remove_space(byte *str)
cmx_reentrant{
	int i,j;
	i=(word16)strlen((char *)str);
	while(i)
	{
		if(*str==' ')
		{
			for(j=0;j<i;j++)
			*(str+j)=*(str+j+1);
		}
		else
		{
			str++;
		}
		i--;
	}
	*str='\0';
}


/*------------------------------------------------------------------------
 Function:    post_network()
 Description: POST FUNCTION: To decide whether to load admin.htm
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_network(PSOCKET_INFO socket_ptr)
cmx_reentrant
{

   mn_http_set_file(socket_ptr, mn_vf_get_entry("network.htm")); 
}


/*------------------------------------------------------------------------
 Function:    network_func()
 Description: POST FUNCTION: Update network.htm Settings. 
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void network_func(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  
  if(mn_http_find_value(BODYptr,(byte *)"dhcp",temp_buff))/*--DHCP Flag-*/
  {
    flag.dhcp = 1;
  }
  else
  {
    flag.dhcp = 0;
  }

  if(mn_http_find_value(BODYptr,(byte *)"protocol",temp_buff))
  {
    terminal_server.protocol = temp_buff[0]-'0'; /*1:TCP 2:UDP 3:Modbus */
  }

  if(mn_http_find_value(BODYptr,(byte *)"ip",temp_buff))/*----IP Addr---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  if(strcmp((char *)temp_buff,(char *)eth_src_addr))
	  {
	    flag.ip_changed = TRUE;   /* Diff IP posted than in the system */
	  }
	  memcpy(eth_src_addr,temp_buff,IP_ADDR_LEN); /* YES: Set IP Addr   */
	}
	else
	{
	  ip_status=1;                                     /* NO: Set Flag  */
	}
  }

  if(mn_http_find_value(BODYptr,(byte *)"dns",temp_buff))/*----DNS Addr--*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(ip_dns_addr,temp_buff,IP_ADDR_LEN); /* YES: Set IP Addr   */
	}
  }

  if(mn_http_find_value(BODYptr,(byte *)"gw",temp_buff)) /*---Gateway---*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(gateway_ip_addr,temp_buff,IP_ADDR_LEN);  /* YES:Set Gateway*/
	}
  }

  if(mn_http_find_value(BODYptr,(byte *)"netmask",temp_buff))/*-Netmask-*/
  {
    if(s_ip_process(temp_buff,0))                /* Proper IP Address ? */
	{
	  memcpy(subnet_mask,temp_buff,IP_ADDR_LEN);  /* YES:Set Subnet Mask*/
	}
  }

  //ip_status=0;
  ip_update_flag=0; 
  		
  if(mn_http_find_value(BODYptr,(byte *)"hname",temp_buff))/*--Hostname-*/
  {
    ip_update_flag=1;
	hname_supplement(temp_buff,FLASH_HOSTNAME_LEN);/* Empty hname buff=0*/      
    memcpy(network.hostname,temp_buff,FLASH_HOSTNAME_LEN+1);/* Set Hname*/
  }

  if(mn_http_find_value(BODYptr,(byte *)"au",temp_buff))/*AutoNegotiate */
  {
    flag.autonegotiation = 1;
  }
  else
  {
    flag.autonegotiation = 0;
  }

  if(mn_http_find_value(BODYptr,(byte *)"speed",temp_buff))/*Speed Flag */
  {
    if(temp_buff[0] == '0')
    {
      flag.speed = 0;                                /* Speed = 10 Mbps */
    }
    else
    {                                               /* Speed = 100 Mbps */
      flag.speed = 1;
    }
  }
  
  if(mn_http_find_value(BODYptr,(byte *)"duplex",temp_buff))/* Duplex   */
  {
    if(temp_buff[0] == '0')
    {
      flag.duplex = 0;                                 /* Duplex = Half */
    }
    else
    {
      flag.duplex = 1;                                 /* Duplex = Full */
    }
  }
  if(mn_http_find_value(BODYptr,(byte *)"port",temp_buff))
  {
    
	unsigned long temp_port = np_atoi((char *)temp_buff);
	if((temp_port !=  terminal_server.local_port) && (temp_port != 2002)&&(temp_port > 0) &&(temp_port <= 65535))
	   {
	   	 flag.HTTP_PORT=np_atoi((char *)temp_buff); 
	   }  
  }
  write_all();                   /* Write all updates settings to Flash */
  mn_http_set_file(socket_ptr, mn_vf_get_entry("network.htm")); 
}

/*------------------------------------------------------------------------
 Function:    spswd_func()
 Description: SSI: Return Admin Password 
 Input:       String Pointer
 Output:      String Pointer
------------------------------------------------------------------------*/ 
word16 spswd_func(byte **str)
cmx_reentrant 
{
  remove_space((byte *)password.admin);
  *str=(byte *)password.admin;
  return (strlen(password.admin));
}

///*------------------------------------------------------------------------
// Function: 	  pswd0_func()
// Description: Read Login Password into String from the Flash
// Input:		  String Pointer
// Output:	  String Pointer
//------------------------------------------------------------------------*/ 
//word16 pswd0_func(byte **str)
//cmx_reentrant 
//{
//  remove_space((byte *)password.login);
//  *str=(byte *)password.login;
//  return (strlen(password.login));
//}


/*------------------------------------------------------------------------
 Function: 	  hname_supplement()
 Description: Read Hostname 
 Input:		  String Pointer
 Output:	  String Pointer
------------------------------------------------------------------------*/ 
void hname_supplement(byte * buff,byte number)
cmx_reentrant 
{
  int i=0;
  int j=number;
	
  *(buff+number)='\0'; // force to Maxim number data in the buff
  for(i=number;i>=0;i--)
  {
    if((*(buff+i))=='\0')
	{
	  j=number;
	}
	if((*(buff+i))!=' ')
	{
	  *(buff+j)=*(buff+i);
	  j--;
	}
  }
	
  for(;j>=0;j--)
  {
    *(buff+j)=' ';
  }
}
