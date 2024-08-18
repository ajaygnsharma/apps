/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

/************************************************************
* CMX recommends making a copy of this file before changing *
* any of the routines below.                                *
*************************************************************/

#include "micronet.h"
#include "np_includes.h"
#include "u2e_http.h"


word16 data_length;
int tsr_socket[5]={-1,-1,-1,-1,-1};

int socket_nfind(byte n) cmx_reentrant;
int socket_get(byte n) cmx_reentrant;

unsigned long reboot_counter;

#if (MN_SMTP)
/* replace the ip address below with the ip address of your SMTP server */
byte ip_smtp_addr[IP_ADDR_LEN] = {SMTP_SVR_IP1,SMTP_SVR_IP2,SMTP_SVR_IP3,SMTP_SVR_IP4};
#endif      /* (MN_SMTP) */

#if MN_ETHERNET
/************************************************************
   if using a chip with EEPROM you may need to write a routine
   to take the value of the hw_addr in EEPROM and put it into
   the array below on startup, otherwise replace eth_src_hw_addr
   below with the proper ethernet hardware address.
*************************************************************/
byte eth_src_hw_addr[ETH_ADDR_LEN];// = { DEFAULT_MAC1,DEFAULT_MAC2,DEFAULT_MAC3,DEFAULT_MAC4,DEFAULT_MAC5,DEFAULT_MAC6 };

/************************************************************
   If ARP is used the array below is used as a temporary holder
   for the destination hardware address. It does not have to be
   changed.

   If ARP is not being used replace the hardware address below
   with the hardware address of the destination. The hardware
   address used MUST be the correct one.
*************************************************************/
byte eth_dest_hw_addr[ETH_ADDR_LEN] = { 0x00,0x11,0x11,0x1F,0x31,0x14 };

/************************************************************
   If a gateway is being used set the gateway IP address and
   subnet mask below.

   If a gateway is not being used:
      set the gateway IP address to { 255,255,255,255 }
      set the subnet mask to        { 255,255,255,  0 }
*************************************************************/
byte gateway_ip_addr[IP_ADDR_LEN] = { DEFAULT_GW1,DEFAULT_GW2,DEFAULT_GW3,DEFAULT_GW4 };
byte subnet_mask[IP_ADDR_LEN]     = { DEFAULT_NETMASK1,DEFAULT_NETMASK2,DEFAULT_NETMASK3,  DEFAULT_NETMASK4 };

#endif      /* MN_ETHERNET */

/*------------------------------------------------------------------------
Function: 		s_ip_process()
Description: 	Check the sanity of the IP Address
Input:			?
Output: 		?
------------------------------------------------------------------------*/ 
byte s_ip_process(byte * ip_buff,byte position)
cmx_reentrant {
	byte temp_ip[4];
	byte i=position;
	byte j=0;
	byte k=0;
	byte temp=0;

	while(ip_buff[i]!='\0'&&ip_buff[i]!='\r'&&k<4)
	{
		j=0;
		while(ip_buff[i]!='.'&&ip_buff[i]!='\0'&&ip_buff[i]!='\r'&&j<3)
		{
			if(ip_buff[i]>='0'&&ip_buff[i]<='9')
			{
				temp=temp*10+ip_buff[i]-'0';
			}
			else
			{
				return 0;
			}
			j++;
			i++;
		}
		if(ip_buff[i]!='.'&&ip_buff[i]!='\0'&&ip_buff[i]!='\n'&&ip_buff[i]!='\r')
		{
			return 0;
		}
		i++;
		if(j==0)
		{
			return 0;
		}
		temp_ip[k]=temp;
		k++;
		temp=0;
	}

	if(k==4)
	{
		for(i=0;i<4;i++)
		{
			ip_buff[i]=temp_ip[i];
		}
		ip_buff[i]='\0';
		return 1;
	}
	else
	{
		return 0;
	}
}

/*------------------------------------------------------------------------
 Function:    moddev_supplement()
 Description: To fill spaces in a buffer if the buffer doesnot meet it max
              value
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
void moddev_supplement(byte *buff,byte number)
cmx_reentrant
{
	int i=0;
	int j=number;
	
	*(buff+number)='\0'; // force to Maxim number data in the buff
	
    for(i=number;i>=0;i--)
	{
		if(!(*(buff+i)))
		{
			j=number;
		}	
		*(buff+j)=*(buff+i);
		j--;
	}
	for(;j>=0;j--)
	{
		*(buff+j)=' ';
	}
}


/*------------------------------------------------------------------------
Function: 		s_ip_process()
Description: 	Check the sanity of the MAC Address
Input:			?
Output: 		?
------------------------------------------------------------------------*/ 
byte s_mac_process(byte *mac_buff)
cmx_reentrant
{
  byte i=0;
  byte j=0;
  byte k=0;
  byte temp=0;
  byte temp_mac[6];

  while(mac_buff[i]!='\0'&&mac_buff[i]!='\r'&&k<6)
  {
    for(j=0;j<3;j++)
	{
	  if(mac_buff[i]==':'||mac_buff[i]=='\0'||mac_buff[i]=='\r')
	  {				
        j = 3;
      }
	  if(mac_buff[i]>='0'&&mac_buff[i]<='9')
      {
        temp=temp*0x10+mac_buff[i]-'0';				
      }
      if(mac_buff[i]>='a'&&mac_buff[i]<='f')
      {
        temp=temp*0x10+mac_buff[i]-'a'+10;	
      }
      if(mac_buff[i]>='A'&&mac_buff[i]<='F')
      {
        temp=temp*0x10+mac_buff[i]-'A'+10;				
      }			
      i++;
    }
    temp_mac[k]=temp;
    k++;
    temp=0;		
  }
  if(k==6)
  {
    for(i=0;i<6;i++)
    {
      mac_buff[i]=temp_mac[i];
    }
	mac_buff[i]='\0';
	return 1;
  }
  else
  {
    return 0;
  }
}


/* --------------------------------------------------------------------- */
#if (MN_TCP)
/* return number of bytes to send */
/* Called from mn_tcp_send */
word16 mn_app_get_send_size(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   SEND_LEN_T bytes_to_send;
#if (MN_HTTP && MN_SERVER_SIDE_INCLUDES)
   word16 num_bytes;
   VF_PTR vf_ptr;
#endif      /* (MN_HTTP && MN_SERVER_SIDE_INCLUDES) */
#if (MN_VIRTUAL_FILE && (MN_HTTP || (MN_FTP_SERVER || MN_FTP_CLIENT) || MN_SMTP))
   SEND_LEN_T non_header_bytes;
   PAGE_SEND_PTR ps_ptr;

   ps_ptr = &page_send_info[socket_ptr->socket_no];
#endif      /* (MN_VIRTUAL_FILE) */

   if (socket_ptr->send_ptr == PTR_NULL)
      bytes_to_send = 0;
   else
      bytes_to_send = socket_ptr->send_len;
#if (MN_HTTP || (MN_FTP_SERVER || MN_FTP_CLIENT) || MN_SMTP)
   if (bytes_to_send)
      {
      if (socket_ptr->socket_type & (HTTP_TYPE|SMTP_TYPE|FTP_TYPE))
         {
         if (bytes_to_send > socket_ptr->send_tcp_window)
            bytes_to_send = socket_ptr->send_tcp_window ;

#if (MN_VIRTUAL_FILE)
#if (MN_HTTP && MN_SERVER_SIDE_INCLUDES)
         vf_ptr = (VF_PTR)socket_ptr->user_data;
         if ((socket_ptr->socket_type & HTTP_TYPE) && \
               (vf_ptr != PTR_NULL) && \
               (vf_ptr->page_type & VF_PTYPE_HTML))
            {
            num_bytes = mn_http_process_includes(socket_ptr,(word16)bytes_to_send);
            if (num_bytes)
               bytes_to_send = num_bytes;
            else
               bytes_to_send = socket_ptr->send_len;
            }
         else
#endif      /* (MN_HTTP && MN_SERVER_SIDE_INCLUDES) */
            {
            if (bytes_to_send > ps_ptr->header_bytes)
               non_header_bytes = bytes_to_send - ps_ptr->header_bytes;
            else
               non_header_bytes = 0;

            if (non_header_bytes)
               {
#if (MN_FTP_SERVER)
               if ((socket_ptr->socket_type & FTP_TYPE) && \
                     (socket_ptr->src_port == FTP_DATA_PORT) && \
                     (ftp_transfer_mode & FTP_LIST_ACTIVE))
                  bytes_to_send = ftp_list(socket_ptr);
               else
#endif      /* (MN_FTP_SERVER) */
               /* copy bytes to send from ROM to RAM buffer if necessary */
               if (ps_ptr->page_ptr != PTR_NULL)
                  {
                  (void)mn_memcpy_cb((socket_ptr->send_ptr+ps_ptr->header_bytes), \
                     ps_ptr->page_ptr, (word16)(non_header_bytes));
                  }
#if (CMX_FFS_USED)
               else if (ps_ptr->vf_ptr != PTR_NULL)
                  {
                  if (vf_set_filepos(ps_ptr->vf_ptr) >= 0)
                     {
                     bytes_to_send = (SEND_LEN_T)F_READ_LEN((socket_ptr->send_ptr+ps_ptr->header_bytes), \
                        non_header_bytes, 1, ps_ptr->vf_ptr->effs_file_ptr);
                     }
                  else
                     bytes_to_send = 0;
                  }
#endif      /* (CMX_FFS_USED) */
               else if (ps_ptr->header_bytes)
                  {
                  /* Copy first part of ram page to HTTPBuffer after the
                     header info.
                  */
                  (void)memcpy((void *)(socket_ptr->send_ptr + ps_ptr->header_bytes), \
                     (void *)ps_ptr->ram_page_ptr, (word16)(non_header_bytes));
                  }
               }
            }
#endif      /* (MN_VIRTUAL_FILE) */
         }
      }
#endif      /* (MN_HTTP || (MN_FTP_SERVER || MN_FTP_CLIENT) || MN_SMTP) */

   return (word16)(bytes_to_send);
}

/* initialization before receiving a packet */
/* Called from mn_tcp_recv */
/* num = number of bytes to be received */
void mn_app_init_recv(word16 num,PSOCKET_INFO socket_ptr)
cmx_reentrant {
#if (!MN_FTP_SERVER)
   num = num;
#endif      /* (!MN_FTP_SERVER) */

#if MN_HTTP
   if (socket_ptr->socket_type & HTTP_TYPE)
      mn_http_init_recv(socket_ptr);
   else
#endif
#if (MN_FTP_SERVER)
   if (socket_ptr->socket_type & FTP_TYPE)
      mn_ftp_server_init_recv(num,socket_ptr);
   else
#endif
      {
      /* put your code here */
      socket_ptr = socket_ptr;
      }
}

/* process received byte. Called from mn_tcp_recv */
void mn_app_recv_byte(byte c,PSOCKET_INFO socket_ptr)
cmx_reentrant {
#if (!MN_HTTP && !MN_FTP_SERVER)
   c=c;
   socket_ptr=socket_ptr;
#endif
#if MN_HTTP
   if (socket_ptr->socket_type & HTTP_TYPE)
      mn_http_recv_byte(c,socket_ptr);
#endif
#if (MN_HTTP && MN_FTP_SERVER)
   else
#endif
#if (MN_FTP_SERVER)
   if (socket_ptr->src_port == FTP_CONTROL_PORT)
      mn_ftp_server_recv_byte(c);
#endif
}



/*-------------------------------------------------------------------------------------------------
 mn_app_process_packet(socket ptr)
 Description: Process received packet. Called from mn_tcp_recv. 
 Input:  Socket Ptr
 Output: Pointer to the Socket
-------------------------------------------------------------------------------------------------*/
SCHAR mn_app_process_packet(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
   	/* for TCP packets if this function returns NEED_IGNORE_PACKET then
      the packet will be not be ACKed. UDP packets are not effected by
      the return code */
   if (socket_ptr->socket_type & HTTP_TYPE)
      return (mn_http_process_packet(socket_ptr));
   else
   {
     /* remove the next line and put your code here */
     if(flag.internet_console == ENABLED)
	 {
	   if(socket_ptr->src_port==2002)
	   {
	     mn_port2002_process_packet(socket_ptr);/* Process Port 2002 */
	   }
	 }
     
	 /* remove the next line and put your code here */
     socket_ptr = socket_ptr;
     ;
   }
   return (1);
}
/*-----------------------------------------------------------------------------------------------*/


/*
   Do what needs to be done after successfully sending a packet.
   Called from mn_tcp_recv().
*/
void mn_app_send_complete(word16 data_len,PSOCKET_INFO socket_ptr)
cmx_reentrant {
#if (MN_HTTP && MN_SERVER_SIDE_INCLUDES)
   VF_PTR vf_ptr;
#endif      /* (MN_HTTP && MN_SERVER_SIDE_INCLUDES) */
#if (MN_VIRTUAL_FILE)
   PAGE_SEND_PTR ps_ptr;

   ps_ptr = &page_send_info[socket_ptr->socket_no];
#endif      /* (MN_VIRTUAL_FILE) */

#if (MN_HTTP && MN_SERVER_SIDE_INCLUDES)
   /* When using server-side includes the size of the data sent may
      not be the same as the size of the data read from the web page.
      The mn_http_process_includes function keeps track of the bytes
      read from the page in the page_send_info structure bytes_read member.
   */
   vf_ptr = (VF_PTR)socket_ptr->user_data;
   if ((socket_ptr->socket_type & HTTP_TYPE) && \
         (vf_ptr != PTR_NULL) && \
         (vf_ptr->page_type & VF_PTYPE_HTML))
      data_len = ps_ptr->bytes_read;
#endif      /* (MN_HTTP && MN_SERVER_SIDE_INCLUDES) */

   /* data_len should never be greater than socket_ptr->send_len but
      just in case we check it.
   */
   if (data_len > socket_ptr->send_len)
      data_len = socket_ptr->send_len;

   /* subtract bytes already sent */
   if (socket_ptr->send_len > 0)
      socket_ptr->send_len -= data_len;

   /* move pointer to next spot to start sending if anything left to send,
      or set it to PTR_NULL otherwise.
   */
   if (socket_ptr->send_len > 0)
      {
#if (MN_VIRTUAL_FILE)
      if (ps_ptr->header_bytes <= data_len)
         {
         data_len = data_len - ps_ptr->header_bytes;

         if (ps_ptr->page_ptr != PTR_NULL)            /* page in ROM */
            ps_ptr->page_ptr += data_len;
         else if (ps_ptr->ram_page_ptr != PTR_NULL)   /* page in RAM */
            {
            ps_ptr->ram_page_ptr += data_len;
            if (ps_ptr->header_bytes)
               socket_ptr->send_ptr = ps_ptr->ram_page_ptr;
            }
#if (CMX_FFS_USED)
         else if (ps_ptr->vf_ptr != PTR_NULL)
            ps_ptr->vf_ptr->filepos += data_len;
#endif      /* (CMX_FFS_USED) */
         else
            socket_ptr->send_ptr += data_len;

         ps_ptr->header_bytes = 0;
         }
      else
         {
         /* ps_ptr->header_bytes is greater than data_len. In other words,
            we didn't send all the header the first time.
         */
         ps_ptr->header_bytes = ps_ptr->header_bytes - data_len;
         /* Move the remaining header bytes to the start of the buffer. */
         (void)memcpy((void *)socket_ptr->send_ptr, \
            (void *)(socket_ptr->send_ptr + data_len), ps_ptr->header_bytes);
         }
#else
      socket_ptr->send_ptr += data_len;
#endif      /* (MN_VIRTUAL_FILE) */
      }
   else     /* socket_ptr->send_len == 0 */
      {
      socket_ptr->send_ptr = PTR_NULL;
#if (MN_VIRTUAL_FILE)
      page_send_close(socket_ptr->socket_no);
#endif      /* (MN_VIRTUAL_FILE) */
      }
}

#endif      /* #if MN_TCP */





/**************************************************************************************************
  Function Name: mn_app_server_idle(socket_ptr)
  Description:   called from mn_server or mn_http_server_task when the server is idle 
  Input:         Socket Ptr
  Output:        NEED_TO_SEND 
**************************************************************************************************/
SCHAR mn_app_server_idle(PSOCKET_INFO *psocket_ptr)
cmx_reentrant 
{
  int j;
  SCHAR i;
  SCHAR ret_val = 0;
  int a;
  byte np_msg;/*auto 4.2a*/
  byte DU_send_buff[10];/*auto 4.2a*/
  byte bcast_addr[IP_ADDR_LEN]={255,255,255,255};/*auto 4.2a*/
  PSOCKET_INFO socket_ptr  = PTR_NULL;
  PSOCKET_INFO m_socket_ptr[12];
  unsigned int k = 0;
  unsigned int l = 0;
    
  /* This initiates a reboot. I do it here because I dont want show a black out at any page
  once a reboot is given */
  if(flag.reboot == TRUE)
  {
    reboot_counter++;
	if(reboot_counter == 10)
	{
	  reboot_counter = 0;
	  flag.reboot = FALSE;
	  WDTInit();
	}
  }
   
  FIO1PIN3     |= 0x40; //////////////////////////////?

  /* If the Flag for SNMP Write is set write to the flash the value of Contact and Location */
  if(flag.snmp_var_set == TRUE)
  {
    DISABLE_INTERRUPTS;
    write_all();
    flag.snmp_var_set = FALSE;
    ENABLE_INTERRUPTS;
  }

  /*-----------------------------------------------------------------------------------------------
                                          Alarms: 
  These alarms are executed only when the smtp flag is set(when we enable SMTP) on the webpage   
  -----------------------------------------------------------------------------------------------*/
  if((flag.smtp == ENABLED) || ((flag.snmp == ENABLED) && (flag.trap_en == ENABLED)))
  {
    /* Condition 1: is in main.c(main_task()) as that is only executed once */
	
	/* Condition 2:
	IP Changed: When one changes the IP address, A flag is set , this checks and sends email  */
    if(flag.ip_changed == TRUE)
      ip_changed_alarm(); 	                              /* Send Alarm */

	/* Condition 3: iServer accessed is in this function */

    /* Condition 4: Serial Port disconnected only applies to full RS232 model */
    /* Serial Port disconnected: DCD will go low when the Serial port is disconnected            */
    if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
    {
	  if(flag.isvr_disconnect == TRUE)
      { 
        /* Complement of DCD is stored in this register U1MSR. The status of this register is:
        Bit 8 - 1 when Serial Port is connected , Bit 8 - 0 when serial port is disconnected       */
        if(!(U1MSR & 0x80))        
        {
          isvr_disconnect_alarm();
        }
      }
    }

    /* Condition 5: Alarm for Input Condition */
	if(flag.isvr_condition == TRUE)
	  isvr_condition_alarm();
	
	/* Condition 6: Input character on serial port matches with a character on the webpage */    
	if((flag.isvr_char1 == TRUE) || (flag.isvr_char2 == TRUE))
      isvr_char_alarm();
  }   

  /*-----------------------------------------------------------------------------------------------
   Discover Utility: first find a socket with the port no: and bcast address as the destination
  address, and then see if the socket is NULL or not and that there is some data in it 
  -----------------------------------------------------------------------------------------------*/
  socket_ptr = mn_find_socket(7011, 7011, bcast_addr, PROTO_UDP, 0);
  if(socket_ptr != PTR_NULL && socket_ptr->recv_len && (!strncmp((char *)socket_ptr->recv_ptr,"AFFIRM",6)))
  {
      /* Along with the reply to the socket I am sending the dhcp Status and the 
      hostname */
      np_msg                    = (flag.dhcp + 0x30);
	  DU_send_buff[0]              = np_msg; 		    //send_buff[0]='0'/'1' 
	  /* Append the buffer with the hostname */
      memcpy(&DU_send_buff[1],network.hostname,strlen((char *)network.hostname));
      //strcat((char *)DU_send_buff,network.hostname); 	//send_buff[1]=e; send_buff[2]=i s62AD
	  DU_send_buff[(strlen((char *)network.hostname)) + 1] ='\0';
	  socket_ptr->send_ptr      = DU_send_buff; 
	  socket_ptr->send_len      = strlen((char *)DU_send_buff);
	  mn_udp_send(socket_ptr);
      socket_ptr -> recv_len    = NULL;
      ret_val = NEED_TO_SEND;	
  }

  /*-----------------------------------------------------------------------------------------------
                               Multihost Connection 
  -----------------------------------------------------------------------------------------------*/
  if(multihost.connection_type == SIMULTANEOUS)
  {
    if(uib_count)
    {
      for(a = 0; a < multihost.conns_to_make; a++)
      {
        if(terminal_server.protocol == TCP_PROTOCOL)
		{
          m_socket_ptr[a] = find_remote_socket(3000, multihost.rp[a], multihost.rip[a]);
		}
		else if(terminal_server.protocol == UDP_PROTOCOL)
		{
		  m_socket_ptr[a] = find_remote_UDP_socket(3000,multihost.rp[a],multihost.rip[a]);
		}
		
		if(m_socket_ptr[a] == PTR_NULL)
        {
          if(flag.retry == TRUE)
		  {
		    for(l = 0; l < multihost.retry_counter; l++)
		    {
              m_socket_ptr[a]->tcp_unacked_bytes = 0;
			   
			  /* Multihost tunneling Connection */
	          multihost.r_sock_no[a] = mn_open(multihost.rip[a], 3000, multihost.rp[a],\
                ACTIVE_OPEN,PROTO_TCP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);		  

		      if(multihost.r_sock_no[a] >= 0) 
              {
                printf("Retry: %d, Opened conn: %d.%d.%d.%d\r\n",k,multihost.rip[a][0],\
		           multihost.rip[a][1],multihost.rip[a][2],multihost.rip[a][3]);
		        multihost.conn_estbld[k] = a;
                flag.retry = TRUE;
				break;
		      }
		      else
		      {
			    printf("Retry: %d, Cant connect: %d.%d.%d.%d Trying again.\r\n",k,multihost.rip[a][0],\
		          multihost.rip[a][1],multihost.rip[a][2],multihost.rip[a][3]);
		        delayMs(1,multihost.retry_timeout);
				flag.retry = FALSE;
              }
			}
          }
        }
		else
		{
		  multihost.conn_estbld[k] = a;
		  k++;
		}
	  }

	  for(a = 0; a < k; a++)
	  {	
        if(a == 0)
        {
          ret_val = SerialToEthernet(m_socket_ptr[multihost.conn_estbld[0]]);
          //*psocket_ptr = m_socket_ptr[multihost.conn_estbld[0]]; /* Set the Pointer to current Socket */ 
          //m_socket_ptr[multihost.conn_estbld[0]]; /* Set the Pointer to current Socket */ 
          if(terminal_server.protocol == TCP_PROTOCOL)
		  {
		    if(ret_val == NEED_TO_SEND)
			{
			  mn_tcp_send(m_socket_ptr[multihost.conn_estbld[0]]);
			}
		  }
		  else if(terminal_server.protocol == UDP_PROTOCOL)
		  {
		    mn_udp_send(m_socket_ptr[multihost.conn_estbld[0]]);
		  }
        }
        else
        {
          m_socket_ptr[multihost.conn_estbld[a]]->send_len = m_socket_ptr[multihost.conn_estbld[0]]->send_len;
          m_socket_ptr[multihost.conn_estbld[a]]->send_ptr = m_socket_ptr[multihost.conn_estbld[0]]->send_ptr;  /* point to send buffer */
          //*psocket_ptr = m_socket_ptr[multihost.conn_estbld[a]]; /* Set the Pointer to current Socket */ 
		  //m_socket_ptr[multihost.conn_estbld[a]]; /* Set the Pointer to current Socket */ 
          if(terminal_server.protocol == TCP_PROTOCOL)
		  {
		    if(ret_val == NEED_TO_SEND)
			{
			  mn_tcp_send(m_socket_ptr[multihost.conn_estbld[a]]);
			}
		  }
		  else if(terminal_server.protocol == UDP_PROTOCOL)
		  {
		    mn_udp_send(m_socket_ptr[multihost.conn_estbld[a]]);
		  }
        }
      }
    }        
  }
  //flag.retry = FALSE;  
  else if(multihost.connection_type == SEQUENTIAL)
  {
    /*---------------------------------------------------------------------------------------------
	 Multihost_connection == SEQUENTIAL
	---------------------------------------------------------------------------------------------*/
	/* If we got some data on the serial port */
	if(uib_count)
    {
      /* Basically circle around with all the sockets and find out which one is active.
	  If there is no active socket, open one after this loop otherwise simply make a ptr to 
	  the socket found and send data to that and exit */
	  for(a = 0; a < multihost.conns_to_make; a++)
      {
        if(terminal_server.protocol == TCP_PROTOCOL)
		{
          /* find a remote socket */
		  socket_ptr = find_remote_socket(3000, multihost.rp[a], multihost.rip[a]);
		}
		else if(terminal_server.protocol == UDP_PROTOCOL)
		{
		  socket_ptr = find_remote_UDP_socket(3000,multihost.rp[a],multihost.rip[a]);
		}
		
		/* A socket found we can send data to that and exit */
		if(socket_ptr != PTR_NULL)
		{
		  m_socket_ptr[a] = socket_ptr;
		  ret_val = SerialToEthernet(m_socket_ptr[a]);/* Collect serial data for this socket */
		  *psocket_ptr = m_socket_ptr[a];
		  //mn_tcp_send(m_socket_ptr[a]);
	      break; /* send data only to this socket */
		}
      }
      
	  if(socket_ptr == PTR_NULL)
      {
	    printf("No sockets active now..\r\n");
		/* Since no socket is found we need to start again and try to open sockets to 
		inidividual hosts and if one is done make a pointer to that and break */
        for(a = 0; a < multihost.conns_to_make; a++)
        {
          if(terminal_server.protocol == TCP_PROTOCOL)
		  {
			if(flag.retry == TRUE)
		    {
		      for(l = 0; l < multihost.retry_counter; l++)
		      {
                multihost.r_sock_no[a] = mn_open(multihost.rip[a], 3000, multihost.rp[a],\
                  ACTIVE_OPEN, PROTO_TCP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff,\
				  MN_ETH_TCP_WINDOW, 0);
			  }
			}
          }
		  else if(terminal_server.protocol == UDP_PROTOCOL)
		  {
		    multihost.r_sock_no[a] = mn_open(multihost.rip[a], 3000, multihost.rp[a],\
			    ACTIVE_OPEN, PROTO_UDP, STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff,\
				MN_ETH_TCP_WINDOW, 0);
		  }
		      	
	      if(multihost.r_sock_no[a] >= 0) 
          {
            printf("Retry: %d, Opened conn: %d.%d.%d.%d\r\n",l,multihost.rip[a][0],\
		           multihost.rip[a][1],multihost.rip[a][2],multihost.rip[a][3]);
		    m_socket_ptr[a] =  MK_SOCKET_PTR(multihost.r_sock_no[a]);
			flag.retry = FALSE;
			break;
		  }
		  else
		  {
		    printf("Retry: %d, Cant connect: %d.%d.%d.%d Trying again.\r\n",l,multihost.rip[a][0],\
		          multihost.rip[a][1],multihost.rip[a][2],multihost.rip[a][3]);
		    printf("Delaying %d msecs\r\n",multihost.retry_timeout);
			delayMs(1,multihost.retry_timeout);
			flag.retry = TRUE;
		  }		    
		}
	  }  
	}
  }
  else if((multihost.connection_type == 1) && (terminal_server.no_connections == 0))
  {
    /*---------------------------------------------------------------------------------------------
                  P O R T          1 0 0 0  
      
      Since this is a Single Loop TCP/IP Stack, We have to make sure that we keep these sockets 
      listening all the time for connections after they are closed by the client or by themselves. 
      This is the Port 1000 Socket Server. Socket No: 1000 is fixed and does not change 
    ---------------------------------------------------------------------------------------------*/
    socket_ptr = find_my_socket();
        
    if (socket_ptr != PTR_NULL)
    {
      if(uib_count)
      {
        ret_val = SerialToEthernet(socket_ptr);
      }
      else
      {
        ret_val = NEED_TO_SEND;
      }
      *psocket_ptr = socket_ptr; /* Set the Pointer to current Socket */
    }

    /* Open a server socket in case it is closed...Well we dont listen in this stack, we simply
    open/close/send/recv data in this TCP/IP Stack */
    if (mn_find_socket(1000, 0, null_addr, PROTO_TCP, 0) == PTR_NULL)
    {
      (void)mn_open(null_addr, 1000, 0, NO_OPEN, PROTO_TCP, STD_TYPE,\
        buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
    }
  }
  else if((multihost.connection_type == 1) && (terminal_server.no_connections != 0))    
  {
    /*-------------------------------------------------------------------------------------------
                  P O R T       2 0 0 0 
      
      (terminal_server.local_port) bridging...We restrict no of connections to a max of 5
    -------------------------------------------------------------------------------------------*/
    if(terminal_server.protocol == TCP_PROTOCOL) 
	{
      /* Make sure that we have a socket ready for Local port ex: port 2000 */
      if (mn_find_socket(terminal_server.local_port, 0, null_addr, PROTO_TCP, 0) == PTR_NULL) 
      {
        (void)mn_open(null_addr, terminal_server.local_port, 0, NO_OPEN,\
		  PROTO_TCP,STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
      }
    }
	else if(terminal_server.protocol == UDP_PROTOCOL)
	{
      socket_ptr = find_UDP_socket(terminal_server.local_port);
	  /* Make sure that we have a socket ready. */
      if (socket_ptr == PTR_NULL)
      {
        (void)mn_open(null_addr, terminal_server.local_port, 0, NO_OPEN, PROTO_UDP,\
	          STD_TYPE | INACTIVITY_TIMER, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
      }
    }
	else if(terminal_server.protocol == MODBUS_PROTOCOL)
	{
	  socket_ptr = find_my_socket();
	  /* Make sure that we have a socket ready. */
      if (socket_ptr == PTR_NULL)
      {
        (void)mn_open(null_addr, 502, 0, NO_OPEN, PROTO_TCP,\
	          STD_TYPE, buff_pool.data_buff, MN_ETH_TCP_WINDOW, 0);
      }
	}
      
	if(terminal_server.protocol == TCP_PROTOCOL) 
    {
      for (i=0;i<MN_NUM_SOCKETS;i++)
      {
        if (SOCKET_ACTIVE(i))
        {
          if ((sock_info[i].src_port == terminal_server.local_port) && \
          (sock_info[i].tcp_state == TCP_ESTABLISHED))
          {
            if((flag.smtp == ENABLED) || ((flag.snmp == ENABLED) && (flag.trap_en == ENABLED)))
			{
			  if(flag.isvr_accessed != FALSE)
                isvr_accessed_alarm(); 	                              /* Send Alarm */
			}

			socket_ptr = MK_SOCKET_PTR(i);
            
            if(flag.security == ENABLED)
            {
              if(memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[0],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[1],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[2],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[3],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[4],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[5],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[6],IP_ADDR_LEN) &&\
                 memcmp((void*)socket_ptr->ip_dest_addr,(void *)exclusivity.ip[7],IP_ADDR_LEN))
               {
			     mn_tcp_close(socket_ptr);
               }
			}
			  
		    if(socket_nfind(i)<0) /* Restrict only sockets=tsrstatus to make connections 
            by  looking inside a record. If a socket is found we get -1 here*/
            {
	          j=socket_get(i);                          /* What no is the socket in the record */
		      if(j<0)
                mn_tcp_close(socket_ptr); /* If no socket found close the socket pointer, else
                make that tsr_socket = 1*/
              else
                tsr_socket[j]=i;					
            }
	      }
        }
      }
    }
	
    if (socket_ptr != PTR_NULL)
    {
      /*-----------------------------------------------------------------------------------------
           Get the Data from the Serial Port only if there is ANY? otherwise dont even bother. 
          Otherwise it slows down the whole system as this is in a total loop.
          This is like a semaphore where we wait for a character to come from the serial port and then
          only then call the function of sending data to the ethernet 
      -----------------------------------------------------------------------------------------*/
      if(uib_count)
      {
        if(terminal_server.protocol == MODBUS_PROTOCOL)
		{
		  modbus_recv();
		  socket_ptr->send_ptr = eth_out_buff;  /* point to send buffer */
          socket_ptr->send_len = uib_count;     /* noof bytes to send + end char */
		  //*psocket_ptr = socket_ptr; /* Set the Pointer to current Socket */
		  if (!(socket_ptr->tcp_unacked_bytes))
		  {  
			mn_tcp_send(socket_ptr);
			uib_count = 0;
		  }
		}
		else
		{
		  if(tunneling.connect_on == 1)
          { 
            ret_val = SerialToEthernet(socket_ptr);
          }
          else if(tunneling.connect_on == 3)
          {
            if(U1MSR & 0x20)
            {
              ret_val = SerialToEthernet(socket_ptr);
            }
          }
          else if(tunneling.connect_on == 4)
          {
            if(!(U1MSR & 0x20))
            {
              ret_val =  SerialToEthernet(socket_ptr);
            }
          }    
          *psocket_ptr = socket_ptr; /* Set the Pointer to current Socket */
		}
      }
    }
  } /*-----------Port 2000 Bridging.....-----------------------------------------------------*/
   return (ret_val);
}
/*-----------------------------------------------------------------------------------------------*/

#if (RTOS_USED == RTOS_NONE)
/* called from mn_recv */
SCHAR mn_app_recv_idle(void)
cmx_reentrant {
   /* put your code here.

      return NEED_TO_EXIT if mn_recv must be exited immediately.
   */

   return (0);
}

/*-------------------------------------------------------------------------------------------------
 Function:    mn_app_server_process_packet()
 Description: Called from "mn_server" after a packet has been received
 Input:       Socket Ptr
 Output:      NEED_TO_EXIT if mn_server needs to exit
-------------------------------------------------------------------------------------------------*/
SCHAR mn_app_server_process_packet(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
   /* remove the next line and put your code here */
   socket_ptr = socket_ptr;

   return (0);
}

#endif      /* (RTOS_USED == RTOS_NONE) */



/*------------------------------------------------------------------
 Function: 		socket_nfind(byte n)
 Description: 	find if the socket already in the record. 
 
 Input: 		Socket Number 
 Output:		if find, then return the number, if fail, return -1
-------------------------------------------------------------------*/
int socket_nfind(byte n)
cmx_reentrant
{
	byte j;
	
	for(j=0;j<terminal_server.no_connections;j++)
	{
		if(tsr_socket[j]==n)
		return n;
	}
	return -1;
}


/*-----------------------------------------------------------
 Function:		socket_get( no of the socket )
 
 Description: 	get the blank record where can store the new socket. 
 Input:			Socket Number
 Output:		if full, return 1.
---------------------------------------------------------------------*/ 				 
int socket_get(byte n)
cmx_reentrant
{
	byte j;
	for(j=0;j<terminal_server.no_connections;j++)
	{
		if(tsr_socket[j]==-1)
			return j;
		//else
		//{
		/*	if((!SOCKET_ACTIVE(tsr_socket[j])) || (sock_info[tsr_socket[j]].src_port != 2000)
			||(tsr_socket[j]==n && sock_info[tsr_socket[j]].src_port == 2000)
			||(sock_info[tsr_socket[j]].src_port == 2000 && sock_info[tsr_socket[j]].dest_port==0))
				return j;*/
		//}	
	}
	return -1;
}



/*------------------------------------------------------------------------
 Function:		rmt_find_socket(source port,destination port)
 Description: 	get the blank record where can store the new socket. 
 Input:			Socket Number
 Output:		if full, return 1.
------------------------------------------------------------------------*/
PSOCKET_INFO find_remote_socket(word16 src_port, word16 dest_port, byte *ip_dest_addr)
cmx_reentrant 
{
  int i;
  PSOCKET_INFO socket_ptr;

  socket_ptr = PTR_NULL;

  for (i=0;i<MN_NUM_SOCKETS;i++)
  {
    if (SOCKET_ACTIVE(i) &&\
	   (sock_info[i].src_port == src_port) &&\
	   (sock_info[i].dest_port == dest_port) &&
       !(memcmp((void *)sock_info[i].ip_dest_addr, (void *)ip_dest_addr,IP_ADDR_LEN)) &&\
       (sock_info[i].tcp_state == TCP_ESTABLISHED))
    {
      socket_ptr = &sock_info[i];
	  tunneling.remote_socket_number = i;
      break;
    }
   }
   return (socket_ptr);
}

/*------------------------------------------------------------------------
 Function:		rmt_find_socket(source port,destination port)
 Description: 	get the blank record where can store the new socket. 
 Input:			Socket Number
 Output:		if full, return 1.
------------------------------------------------------------------------*/
PSOCKET_INFO mh_tcp_state(unsigned int socket_no)
cmx_reentrant 
{
  PSOCKET_INFO socket_ptr;

  socket_ptr = PTR_NULL;

  if (SOCKET_ACTIVE(socket_no) && (sock_info[socket_no].tcp_state == TCP_ESTABLISHED))
  {
    socket_ptr = &sock_info[socket_no];
  }
  
  return (socket_ptr);
}


/*-------------------------------------------------------------------------------------------------
 Function:    find_my_socket()
 Description: Look for a socket to send the UART data out of. 
 Input:       None
 Output:      Ptr to the Socket
-------------------------------------------------------------------------------------------------*/
static PSOCKET_INFO find_my_socket(void)
cmx_reentrant 
{
  SCHAR i;
  PSOCKET_INFO socket_ptr;

  socket_ptr = PTR_NULL;
   
  for (i=0;i<MN_NUM_SOCKETS;i++)
  {
    if (SOCKET_ACTIVE(i))
    {
      if (((sock_info[i].src_port == 502) || (sock_info[i].src_port == 1000)) &&\
	      (sock_info[i].tcp_state == TCP_ESTABLISHED))
      {
        socket_ptr = MK_SOCKET_PTR(i);
        break;
      }
    }
  }
  return (socket_ptr);
}


/*-------------------------------------------------------------------------------------------------
 Function:    find_UDP_socket()
 Description: Look for a socket to send the UART data out of. 
 Input:       None
 Output:      Ptr to the Socket
-------------------------------------------------------------------------------------------------*/
static PSOCKET_INFO find_UDP_socket(unsigned long local_port)
cmx_reentrant 
{
  SCHAR i;
  PSOCKET_INFO socket_ptr;

  socket_ptr = PTR_NULL;
   
  for (i=0;i<MN_NUM_SOCKETS;i++)
  {
    if (SOCKET_ACTIVE(i))
    {
      if ((sock_info[i].src_port == local_port))
      {
        socket_ptr = MK_SOCKET_PTR(i);
        break;
      }
    }
  }
  return (socket_ptr);
}


/*-------------------------------------------------------------------------------------------------
 Function:    find_UDP_socket()
 Description: Look for a socket to send the UART data out of. 
 Input:       None
 Output:      Ptr to the Socket
-------------------------------------------------------------------------------------------------*/
static PSOCKET_INFO find_remote_UDP_socket(word16 src_port, unsigned long dest_port,byte *ip_dest_addr)
cmx_reentrant 
{
  SCHAR i;
  PSOCKET_INFO socket_ptr;

  socket_ptr = PTR_NULL;
   
  for (i=0;i<MN_NUM_SOCKETS;i++)
  {
    if (SOCKET_ACTIVE(i) &&\
	   (sock_info[i].src_port == src_port) &&\
	   (sock_info[i].dest_port == dest_port) &&
       !(memcmp((void *)sock_info[i].ip_dest_addr, (void *)ip_dest_addr,IP_ADDR_LEN)))
    {
      socket_ptr = MK_SOCKET_PTR(i);
      break;
    }
  }
  return (socket_ptr);
}
