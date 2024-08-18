/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"
#include "np_includes.h"

#define HALF 1
#define FULL 2


/*-------------------------------------------------------------------------------------------------
  Function:     mn_server()
  Description:  Main MN_SERVER. This functions at the higher level recieves data packet and then
                calls subfunctions to process it or if neeeded exits
  Input:        None
  Output:       Exit Server
-------------------------------------------------------------------------------------------------*/
int mn_server(void)
cmx_reentrant 
{
  byte packet_type;
  PSOCKET_INFO socket_ptr;

  long sent;
  long recvd;
  int status;
  long udp_sent;

#if (MN_TCP && !MN_TCP_DELAYED_ACKS)
  PSOCKET_INFO check_socket_ptr;
#endif      /* (MN_TCP) */

  byte i;
  byte dhcp_state;
  SCHAR snmp_socket;
  word16 usNumChar;
  SCHAR interface_no;
  snmp_socket = 0;

  socket_ptr = &sock_info[0];
  sent = 0;
  recvd = 0;
  status = 0;

#if (MN_TCP && !MN_TCP_DELAYED_ACKS)
  check_socket_ptr = &sock_info[MN_NUM_SOCKETS];
#endif      /* (MN_TCP) */

  while (1)
  {  
    /* Loop until we get a TCP or UDP packet, will open a socket automatically, if required,
     for HTTP and SNMP */
    interface_no = 0;
    if (mn_find_socket(SNMP_PORT, 0, null_addr, PROTO_UDP, interface_no) == PTR_NULL)
    {
      snmp_socket = mn_open(null_addr, SNMP_PORT, 0, NO_OPEN, PROTO_UDP,\
               STD_TYPE, ucSnmpIn, MN_SNMP_IN_BUFF_SIZE, interface_no);
    }

    /*---------------------------------------------------------------------------------------------
                              Recieve Data: 
                              
            Start recieving data packets and break if we recieve one
    ---------------------------------------------------------------------------------------------*/
    while (1)
    {

#if(MN_DHCP)
      if(flag.dhcp == ENABLED)
      {
        dhcp_state = dhcp_lease.dhcp_state;
        if (dhcp_state == DHCP_DEAD)
          return (DHCP_LEASE_EXPIRED);

        if ((dhcp_state == DHCP_RENEWING) || (dhcp_state == DHCP_REBINDING))
        {
          /* renew with the previous lease time */
          (void)mn_dhcp_renew(dhcp_lease.org_lease_time);
        }
      }
#endif
      
      /* Recieve IP Packet and store in packet_type */
      packet_type = mn_ip_recv();
      
      /* Check to See if the packet is TCP_TYPE or not? */
      if (packet_type & TCP_TYPE)
      {
        /* We also increment the no of bytes we recieved of TCP type */
        network.tcp_rx_bytes++;
        break;
      }
      
      /* Check to See if the packet is UDP_TYPE or not? */
      if (packet_type & UDP_TYPE)
      {
        /* We also increment the no of bytes we recieved of UDP type */
        network.udp_rx_bytes++;	  //Ajay
		break;
      }

      if((packet_type & ICMP_TYPE) && (packet_type & ICMP_REPLY_TYPE))
      {
	    network.icmp_rx_bytes++; //Ajay
		break;
	  }

      check_http_parse(&socket_ptr); /* Parse the HTTP Packet */

#if(MN_TCP_DELAYED_ACKS)
      mn_resend_tcp();
#else
      /* Check if the socket pointed to by check_socket_ptr has bytes
      we need to resend. If it does, try a resend. Increment
      check_socket_ptr so another socket will be checked the next time around */
      check_socket_ptr++;
      if (check_socket_ptr >= &sock_info[MN_NUM_SOCKETS])
        check_socket_ptr = &sock_info[0];
      
      if ((check_socket_ptr->ip_proto == PROTO_TCP) && \
                TCP_TIMER_EXPIRED(check_socket_ptr) && \
               (check_socket_ptr->send_len || \
               (check_socket_ptr->tcp_state >= TCP_FIN_WAIT_1)))
      {
        socket_ptr = check_socket_ptr;
        goto send_data;
      }
#endif
      
      /* Finally Call the mn_app_server_idle(): this function allows us to call any other
      type of function. In that we can do something with the socket if need be. We can return
      NEED_TO_SEND in case we need to send data back or NEED_TO_EXIT if we need to close the 
      socket */
      status = mn_app_server_idle(&socket_ptr);
      if (status == NEED_TO_SEND)
        goto send_data;
      else if (status == NEED_TO_EXIT)
        goto server_exit;

      /* Port 2002 is the admin console. It allows you to setp parameters such as IP Address,
      Subnet Mask etc...using port 2002. */
      if(flag.internet_console == ENABLED)
	  {
	    port2002_handler(&socket_ptr);     /* Handle port2002 socket */
      }
    }  //Internal WHILE....

    /*---------------------------------------------------------------------------------------------
                             Process recieved packets. 
    
    This is where the packets are recieved and then based on their type
    we call a fuction to process them . iServer processes most of its data in the mn_app_idle()
    function when the MN_SERVER is idle. This is single big loop thats why this behaviour
    ---------------------------------------------------------------------------------------------*/
    /* process the packet, switch sockets if neccessary */
    if (packet_type & TCP_TYPE)
      recvd = mn_tcp_recv(&socket_ptr);

    /* recieve a UDP Packet */
    if (packet_type & UDP_TYPE)
      recvd = mn_udp_recv(&socket_ptr);

    if((packet_type & ICMP_TYPE) && (packet_type & ICMP_REPLY_TYPE))
    {
      recvd = mn_icmp_recv_reply(&socket_ptr);
    }

    /* if socket_ptr == PTR_NULL there was some sort of error. The TCP
    or UDP layer has thrown away the bad packet so we just wait for another packet */
    if (socket_ptr == PTR_NULL)
      continue;

    /* If we got a TCP packet, and something is there in the socket */
    if ((packet_type & TCP_TYPE) && \
       ((recvd >= 0) || (recvd == NEED_TO_LISTEN) || (recvd == TCP_NO_CONNECT)))
    {
      if (socket_ptr->socket_type & HTTP_TYPE)
      {   
        if (mn_http_server_recv(&socket_ptr) == NEED_TO_SEND)
          goto send_data;
      }
      else
      {
        if (socket_ptr->tcp_state == TCP_CLOSED)  /* other side closed */
        {
          (void)mn_abort(socket_ptr->socket_no);
          socket_ptr = (PSOCKET_INFO)PTR_NULL;
        }
      }
    }

    /* get another packet if error or NEED_TO_LISTEN */
    if (socket_ptr == PTR_NULL || recvd < 0)
      continue;
    
    /* EIT Application code: this is the part that handles the transfer of data from 
    the Ethernet to the Serial port. We pretty much empty serial buffer, get the length 
    of the data we got from the socket and start transmitting to the UART. We wait till the 
    entire transfer is done. */   
    /* Ethernet to Serial using the UDP Protocol */
    if(terminal_server.protocol == UDP_PROTOCOL)
    {
      /* Remote Tunneling SEQUENTIAL connections */
	  if((multihost.connection_type == SEQUENTIAL) ||\
	     (multihost.connection_type == SIMULTANEOUS))
      {
        if((socket_ptr->dest_port == multihost.rp[0])||\
		   (socket_ptr->dest_port == multihost.rp[1])||\
		   (socket_ptr->dest_port == multihost.rp[2])||\
		   (socket_ptr->dest_port == multihost.rp[3])||\
		   (socket_ptr->dest_port == multihost.rp[4])||\
		   (socket_ptr->dest_port == multihost.rp[5])||\
		   (socket_ptr->dest_port == multihost.rp[6])||\
		   (socket_ptr->dest_port == multihost.rp[7])||\
		   (socket_ptr->dest_port == multihost.rp[8])||\
		   (socket_ptr->dest_port == multihost.rp[9])||\
		   (socket_ptr->dest_port == multihost.rp[10])||\
		   (socket_ptr->dest_port == multihost.rp[11])&&\
		   (socket_ptr->recv_len))
		{
	      buff_pool.db_out = 0;  /* The total no of bytes of data out of the buffer is zero */
          buff_pool.len = socket_ptr->recv_len; /* Get the length of the data in the socket */
            buff_pool.xmit_busy = TRUE;  /* Make the busy flag high till the trasmisson on the */
		    start_xmit();                /* Finally Start Transmitting the data */
            while (buff_pool.xmit_busy); /* Wait until the UART is done. */
			 continue;
		}
		
	  }
	  else if((multihost.connection_type == 1) && (terminal_server.no_connections > 0))
	  {
	    /* LOCAL Bridge */
		/* If we got something through ethernet, send it out the UART. */
      	if ((socket_ptr->src_port == terminal_server.local_port) && \
             socket_ptr->recv_len)
        {
          buff_pool.db_out = 0;       /* The total no of bytes of data out of the buffer is zero */
          buff_pool.len = socket_ptr->recv_len;      /* Get the length of the data in the socket */
          buff_pool.xmit_busy = TRUE;         /* Make the busy flag high till the trasmisson on the 
                                            UART in not complete */
          start_xmit();
          while (buff_pool.xmit_busy);       /* Wait until the UART is done. */
            continue;                        /* Get the next packet. */
        }
      }
    }  
	else if(terminal_server.protocol == TCP_PROTOCOL)
 	{
	  if(multihost.connection_type == 1)
	  {
	    /* Ethernet to Serial using TCP Protocol */
	    /* If we got something through ethernet, send it out the UART. */
        if (((socket_ptr->src_port == terminal_server.local_port) ||\
            (socket_ptr->src_port == 1000)) &&\
			(socket_ptr->tcp_state == TCP_ESTABLISHED) &&\
			socket_ptr->recv_len)
        {
          buff_pool.db_out = 0;  /* The total no of bytes of data out of the buffer is zero */
          buff_pool.len = socket_ptr->recv_len; /* Get the length of the data in the socket */
          buff_pool.xmit_busy = TRUE;  /* Make the busy flag high till the trasmisson on the 
                                     UART in not complete */
          /*-----------------------------------------------------------------------------------------------
          To Make the LEDs of TX high
          -----------------------------------------------------------------------------------------------*/
          FIO1PIN3  = 0xbf;

          if((iserver.model == EIT_W_4) || (iserver.model == EIT_D_4) || (iserver.model == EIT_PCB_4))
 		  {
            if(flag.rs485type == HALF)
		    {
	          /* Conver to Master Mode/Transmite Mode */
              FIO0DIR0 = 0x13;                     /* P0.0,P0.1,P0.4 Output                 */
              FIO0MASK0= 0xec;                     /* P0.0,P0.1,P0.4 Mask                   */
              FIO0SET0 = 0x11;                     /* P0.0,P0.4 High                        */
              FIO0CLR0 = 0x02;                     /* P0.1 Low                              */
            }
          }
		  start_xmit();                /* Finally Start Transmitting the data */
          while (buff_pool.xmit_busy); /* Wait until the UART is done. */
			continue;
		  
		  if((iserver.model == EIT_W_4) || (iserver.model == EIT_D_4) || (iserver.model == EIT_PCB_4))
 		  {
            if(flag.rs485type == HALF)
		    {
		      enable_timer(1);
              delayMs(1,10);
              reset_timer(1);
	          disable_timer(1);

		      /* Conver to Slave Mode/Recieve Mode */
              FIO0DIR0 = 0x13;                     /* P0.0,P0.1,P0.4 Output                 */
              FIO0MASK0= 0xec;                     /* P0.0,P0.1,P0.4 Mask                   */
              FIO0SET0 = 0x01;                     /* P0.0,P0.4 High, RE = 0, H/F_ = 1, DE = 0 */
              FIO0CLR0 = 0x12;                     /* P0.1 RE_ = 0                              */
		    }
          }	  
	    }
	  } 
	  else if((multihost.connection_type == SEQUENTIAL) ||\
	          (multihost.connection_type == SIMULTANEOUS))
      {	
        if((socket_ptr->tcp_state == TCP_ESTABLISHED) &&\
             (socket_ptr->recv_len) &&\
			 ((socket_ptr->dest_port == multihost.rp[0]) ||\
			  (socket_ptr->dest_port == multihost.rp[1]) ||\
			  (socket_ptr->dest_port == multihost.rp[2]) ||\
			  (socket_ptr->dest_port == multihost.rp[3]) ||\
			  (socket_ptr->dest_port == multihost.rp[4]) ||\
			  (socket_ptr->dest_port == multihost.rp[5]) ||\
			  (socket_ptr->dest_port == multihost.rp[6]) ||\
			  (socket_ptr->dest_port == multihost.rp[7]) ||\
			  (socket_ptr->dest_port == multihost.rp[8]) ||\
			  (socket_ptr->dest_port == multihost.rp[9]) ||\
			  (socket_ptr->dest_port == multihost.rp[10])||\
			  (socket_ptr->dest_port == multihost.rp[11])))
		  {
	        buff_pool.db_out = 0;  /* The total no of bytes of data out of the buffer is zero */
            buff_pool.len = socket_ptr->recv_len; /* Get the length of the data in the socket */
            buff_pool.xmit_busy = TRUE;  /* Make the busy flag high till the trasmisson on the */
		    start_xmit();                /* Finally Start Transmitting the data */
              while (buff_pool.xmit_busy); /* Wait until the UART is done. */
		  }
	  }
	}
    /* Modbus protocol: Ethernet to Serial...ModbusTCP to Modbus RTU */
    else if(terminal_server.protocol == MODBUS_PROTOCOL)
	{
	  /* If we got something through ethernet, send it out the UART. */
      if ((socket_ptr->src_port == terminal_server.local_port) && \
          (socket_ptr->tcp_state == TCP_ESTABLISHED) && \
           socket_ptr->recv_len)
      { 
	    buff_pool.db_out = 0;
        buff_pool.len = socket_ptr->recv_len;
        modbus_send(buff_pool.data_buff, buff_pool.len);
	  } 
	}

    
    usNumChar = socket_ptr->recv_len;
    usNumChar = mn_SNMP(usNumChar);
    if (usNumChar > 0 )
    {
      socket_ptr->send_ptr = ucSnmpOut;
      socket_ptr->send_len = usNumChar;
    }
    else
    {
      (void)mn_abort(snmp_socket);
      continue;
    }

    /* callback function to possibly do something with the packet we just received. */
    if (mn_app_server_process_packet(socket_ptr) == NEED_TO_EXIT)
      goto server_exit;

/*-------------------------------------------------------------------------------------------------
                   S E N D    D A T A
 This Label is called to send data when a TCP/UDP/ICMP packet is recieved and we have to respond
 to it.
-------------------------------------------------------------------------------------------------*/
send_data:
    if (socket_ptr != PTR_NULL)
    {
      /*-------------------------------------------------------------------------------------------
       It seems that port 1000 works with httpget and so has to behave like a http server 
       it gets a reading an closes a connection after it sends it. The port 1000 is enabled when
       No of connections to the iServer is 0 from the Ethernet. Closing a connection is not here.
      -------------------------------------------------------------------------------------------*/
      if ((socket_ptr->socket_type & HTTP_TYPE)|| (socket_ptr->src_port == 1000))
        mn_http_server_set_flag(socket_ptr);

      if (socket_ptr->ip_proto == PROTO_TCP)
      {
        /* reply to the packet we just got */
        sent = mn_tcp_send(socket_ptr);
        network.tcp_tx_bytes++; //Ajay

#if (MN_TCP_DELAYED_ACKS)
        if ((sent != UNABLE_TO_SEND) && (sent != UNABLE_TO_TCP_SEND))
#else
        if (sent != UNABLE_TO_SEND)
#endif
        {
          /*---------------------------------------------------------------------------------------
          After the iServer Sends Data from the Serial Port to the Ethernet at port 1000 it closes 
          the connection. This is done below when the mn_http_server_send() is dont with sending 
          data it closes connection. So I added Port 1000 to it.
          ---------------------------------------------------------------------------------------*/
          if( (socket_ptr->socket_type & HTTP_TYPE)||\
              (socket_ptr->src_port == 1000))
          {
            mn_http_server_send(&socket_ptr,(int)sent);

#if (MN_TCP_DELAYED_ACKS)
            if ((!mn_recv_byte_present(socket_ptr->interface_no)) && \
                (sent > 0) && (socket_ptr->packet_count < 2) && \
                (socket_ptr->send_len > socket_ptr->tcp_unacked_bytes))
               {
                     /* If there is something left to send, a packet has
                        not been received and we have only sent one packet
                        so far, then send another packet without waiting for
                        an ACK.
                     */
                 goto send_data;
               }
#endif
          }
          else
          {
            if ((sent < 0 && sent != TCP_NO_CONNECT) || socket_ptr->tcp_state == TCP_CLOSED)
              (void)mn_abort(socket_ptr->socket_no);
          }
        }
      }

      if ((socket_ptr != PTR_NULL) && (socket_ptr->ip_proto == PROTO_UDP))
      {
        /* reply to the packet we just got */
        udp_sent = mn_udp_send(socket_ptr);
        network.udp_tx_bytes++;                         //Ajay
        if ((udp_sent != UNABLE_TO_SEND) && \
            (socket_ptr->socket_no == snmp_socket))
              (void)mn_abort(snmp_socket);
      }
    }
  }     /* while (1) */

/*-------------------------------------------------------------------------------------------------
                   S E R V E R      E X I T 
 This Label is called to exit the Main Server.
-------------------------------------------------------------------------------------------------*/
server_exit:
  /* shutdown all HTTP and FTP sockets before leaving */
  for (i = 0; i < MN_NUM_SOCKETS; i++)
  {
    if (sock_info[i].socket_type & (HTTP_TYPE | FTP_TYPE))
      mn_close(i);
  }
  return (0);
}
