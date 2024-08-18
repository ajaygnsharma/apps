/*------------------------------------------------------------------------ 
					post_device_query.c

Description:	Post Settings to the iopins.htm Page
				1. Post the Device Query Data
                2. 
				3. 
				4. 
				5. 
                6. 

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

#define HALF 1
#define FULL 2


#define MASTER  1
#define SLAVE   2

/*------------------------------------------------------------------------
 Function:    post_command()
 Description: POST FUNCTION: Take Data from Internet (Command)
              and Send it to UART and when get it back sent it to a 
              buffer.
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_command(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  unsigned long cmd_len;
  word16 count, cnt1, cnt2;
  byte temp_buff[33];
  char *cr_ptr;
  byte temp_resp[50];

  count = 0;
  strcpy((char *)temp_buff,(char *)cmd.response_buff);
  memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
  
  /*-----------------------------------------------------------------------------------------------
   RS485 Half Duplex
  -----------------------------------------------------------------------------------------------*/
  if((iserver.model == EIT_W_4) || (iserver.model == EIT_D_4) || (iserver.model == EIT_PCB_4))
  {
    if(flag.rs485type == FULL)
    {
      if(mn_http_find_value(BODYptr,(byte *)"cmd",cmd.cmd_buff))
      {
        memset(buff_pool.data_buff,NULL,sizeof(buff_pool.data_buff));
        cmd_len = strlen((char *)cmd.cmd_buff);
        memcpy(buff_pool.data_buff,cmd.cmd_buff,cmd_len);

        buff_pool.db_out = 0;
        buff_pool.len = cmd_len + 1;
        buff_pool.data_buff[cmd_len] = 0x0d;

        buff_pool.xmit_busy = TRUE;
        start_xmit();
        while (buff_pool.xmit_busy);     /* Wait until the UART is done. */

        if(serial.buffering_time != NULL)
        {
          /* If there are some unacknowledged bytes then we need to reset the local counter of 
          Serial port and take a count the next time we come in this loop. This is only for TCP
          not UDP as there is no acknowledgment */
          if (socket_ptr->tcp_unacked_bytes) 
            count = 0;            /* No Ack recieved,Wait. Not Sending Data */
          else                    /* Yes Ack recieved . We can Send Data    */
          {
            T1TCR = 0x02;		/* reset timer */
            T1PR  = 0x00;		/* set prescaler to zero */
            T1MR0 = serial.buffering_time * TIME_INTERVAL; /* timer match at buffering time */
            T1IR  = 0xff;		/* reset all interrrupts */
            T1MCR = 0x04;		/* stop timer on match */
            T1TCR = 0x01;		/* start timer */
  
            while ((T1TCR & 0x01))
            {
              DISABLE_INTERRUPTS;
              count = uib_count;    /* Get the No of bytes in the Serial Port */
              if (count > MN_ETH_TCP_WINDOW)
                count = MN_ETH_TCP_WINDOW;  
              if(serial.endchar != NULL)
              {
                if(flag.endchar_recvd == TRUE)
                  break;
              }
              if(serial.pkt_len != NULL)
              {
                if(flag.pkt_len_recvd == TRUE)
                  break;
              }
              ENABLE_INTERRUPTS;
            }
            T1TCR = 0;		/* Disable Timer */
          }
        }
        else
        {
          if (socket_ptr->tcp_unacked_bytes) 
            count = 0;            /* No Ack recieved,Wait. Not Sending Data */
          else                    /* Yes Ack recieved . We can Send Data    */
          {
            DISABLE_INTERRUPTS;
            count = uib_count;    /* Get the No of bytes in the Serial Port */
            if (count > MN_ETH_TCP_WINDOW)
              count = MN_ETH_TCP_WINDOW;
            ENABLE_INTERRUPTS;    
          }
        }
  
       /* If local count of serial data is more than zero, execute this loop otherwise dont waste the 
       precious CPU cycles */
       if (count > 0)   
       {
    
         if (uib_in > uib_out)         /* In Ring buff: Overlap Occured? NO */
         { 
           if((serial.endchar != NULL) || (serial.pkt_len != NULL))
           {
             /* End Char recvd?*/
             if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE))
             {
               if((serial.fwdendchar == ENABLED) || (serial.pkt_len != NULL))
                 memcpy(cmd.response_buff, &uart_in_buff[uib_out], count);
               else
                 memcpy(cmd.response_buff, &uart_in_buff[uib_out], (count - 1));
          
               uib_count -= count;       /* Reset the Count for UART Bytes */
               uib_out += count;         /* Start Pointer = Curr Pointer   */
               flag.endchar_recvd    = FALSE;
               flag.pkt_len_recvd    = FALSE;
             }
           }
           else
           {  
             memcpy(cmd.response_buff,&uart_in_buff[uib_out],count);/* Send data*/
             uib_count -= count; /* Reset "no:of bytes in serial port" cntr  */
             uib_out += count;    /* Start ptr = Current Ptr               */
             socket_ptr->send_ptr = eth_out_buff;    /* point to send buffer */
             socket_ptr->send_len = count;     /* Get the noof bytes to send */	
           }
         }
         else                  /* In UART Buffer: Overlap Occured? YES   */
         {
           /* This section of code only for Packaging technique */
           if((serial.endchar != NULL) || (serial.pkt_len != NULL))
           {
             /* End Char recvd?*/
             if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE)) 
             {
               /* cnt1 is size of data that is present till the end of buffer */
               cnt1 = BUFF_SIZE - uib_out; 
          
               /* cnt2 is the size of data that is present from the starting of the buffer */
               cnt2 = count - cnt1; 
          
               /* First we copy the data that is present till the end of serial buff to Ethernet buff */
               memcpy(cmd.response_buff, &uart_in_buff[uib_out], cnt1); 
               uib_count = (uib_count - (cnt1 + cnt2));   /* Reset "no:of bytes in serial port" cntr  */
          
               /* If there is some data at the start of the ring buffer then copy that too the 
               ethernet buffer, but append that...to the previous data */
               if (cnt2)
                 memcpy(&cmd.response_buff[cnt1], &uart_in_buff[0], cnt2);
          
               /* Pointer now points to the new data end */
               uib_out = cnt2;
             }
           }
           else /* No Packaging technique will execute this section of code */
           {
             cnt1 = BUFF_SIZE - uib_out; 
             cnt2 = count - cnt1;
             memcpy(cmd.response_buff, &uart_in_buff[uib_out], cnt1);
             uib_count = (uib_count - (cnt1 + cnt2)); /* Reset "no:of bytes in serial port" cntr  */
             if (cnt2)
               memcpy(&cmd.response_buff[cnt1], &uart_in_buff[0], cnt2);
             uib_out = cnt2;
           }
         }
       } //if(count > 0)
     }
   }
   else if(flag.rs485type == HALF)
   {
     if(mn_http_find_value(BODYptr,(byte *)"cmd",cmd.cmd_buff))
     {
        memset(buff_pool.data_buff,NULL,sizeof(buff_pool.data_buff));
        cmd_len = strlen((char *)cmd.cmd_buff);
        memcpy(buff_pool.data_buff,cmd.cmd_buff,cmd_len);

        buff_pool.db_out = 0;
        buff_pool.len = cmd_len + 1;
        buff_pool.data_buff[cmd_len] = 0x0d;

        buff_pool.xmit_busy = TRUE;
        
        /* Conver to Master Mode/Transmite Mode */
        FIO0DIR0 = 0x13;                     /* P0.0,P0.1,P0.4 Output                 */
        FIO0MASK0= 0xec;                     /* P0.0,P0.1,P0.4 Mask                   */
        FIO0SET0 = 0x11;                     /* P0.0,P0.4 High                        */
        FIO0CLR0 = 0x02;                     /* P0.1 Low                              */
        
        start_xmit();

        while (buff_pool.xmit_busy);     /* Wait until the UART is done. */
        
        enable_timer(1);
        delayMs(1,10);
        reset_timer(1);
	    disable_timer(1);

        /* Conver to Slave Mode/Recieve Mode */
        FIO0DIR0 = 0x13;                     /* P0.0,P0.1,P0.4 Output                 */
        FIO0MASK0= 0xec;                     /* P0.0,P0.1,P0.4 Mask                   */
        FIO0SET0 = 0x01;                     /* P0.0,P0.4 High, RE = 0, H/F_ = 1, DE = 0 */
        FIO0CLR0 = 0x12;                     /* P0.1 RE_ = 0                              */
    
        if(serial.buffering_time != NULL)
        {
          /* If there are some unacknowledged bytes then we need to reset the local counter of 
          Serial port and take a count the next time we come in this loop. This is only for TCP
          not UDP as there is no acknowledgment */
          if (socket_ptr->tcp_unacked_bytes) 
            count = 0;            /* No Ack recieved,Wait. Not Sending Data */
          else                    /* Yes Ack recieved . We can Send Data    */
          {
            T1TCR = 0x02;		/* reset timer */
            T1PR  = 0x00;		/* set prescaler to zero */
            T1MR0 = serial.buffering_time * TIME_INTERVAL; /* timer match at buffering time */
            T1IR  = 0xff;		/* reset all interrrupts */
            T1MCR = 0x04;		/* stop timer on match */
            T1TCR = 0x01;		/* start timer */
  
            while ((T1TCR & 0x01))
            {
              DISABLE_INTERRUPTS;
              count = uib_count;    /* Get the No of bytes in the Serial Port */
              if (count > MN_ETH_TCP_WINDOW)
                count = MN_ETH_TCP_WINDOW;  
              if(serial.endchar != NULL)
              {
                if(flag.endchar_recvd == TRUE)
                  break;
              }
              if(serial.pkt_len != NULL)
              {
                if(flag.pkt_len_recvd == TRUE)
                  break;
              }
              ENABLE_INTERRUPTS;
            }
            T1TCR = 0;		/* Disable Timer */
          }
        }
        else
        {
          if (socket_ptr->tcp_unacked_bytes) 
            count = 0;            /* No Ack recieved,Wait. Not Sending Data */
          else                    /* Yes Ack recieved . We can Send Data    */
          {
            DISABLE_INTERRUPTS;
            count = uib_count;    /* Get the No of bytes in the Serial Port */
            if (count > MN_ETH_TCP_WINDOW)
              count = MN_ETH_TCP_WINDOW;
            ENABLE_INTERRUPTS;    
          }
        }
  
       /* If local count of serial data is more than zero, execute this loop otherwise dont waste the 
       precious CPU cycles */
       if (count > 0)   
       {
    
         if (uib_in > uib_out)         /* In Ring buff: Overlap Occured? NO */
         { 
           if((serial.endchar != NULL) || (serial.pkt_len != NULL))
           {
             /* End Char recvd?*/
             if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE))
             {
               if((serial.fwdendchar == ENABLED) || (serial.pkt_len != NULL))
                 memcpy(cmd.response_buff, &uart_in_buff[uib_out], count);
               else
                 memcpy(cmd.response_buff, &uart_in_buff[uib_out], (count - 1));
          
               cr_ptr = strchr((char *)cmd.response_buff,0x0d);
			   memset(temp_resp,NULL,sizeof(temp_resp));
			   memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
			   memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
			   memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

			   uib_count -= count;       /* Reset the Count for UART Bytes */
               uib_out += count;         /* Start Pointer = Curr Pointer   */
               flag.endchar_recvd    = FALSE;
               flag.pkt_len_recvd    = FALSE;
             }
           }
           else
           {  
             memcpy(cmd.response_buff,&uart_in_buff[uib_out],count);/* Send data*/
             cr_ptr = strchr((char *)cmd.response_buff,0x0d);
			 memset(temp_resp,NULL,sizeof(temp_resp));
			 memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
			 memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
			 memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

			 uib_count -= count; /* Reset "no:of bytes in serial port" cntr  */
             uib_out += count;    /* Start ptr = Current Ptr               */
           }
         }
         else                  /* In UART Buffer: Overlap Occured? YES   */
         {
           /* This section of code only for Packaging technique */
           if((serial.endchar != NULL) || (serial.pkt_len != NULL))
           {
             /* End Char recvd?*/
             if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE)) 
             {
               /* cnt1 is size of data that is present till the end of buffer */
               cnt1 = BUFF_SIZE - uib_out; 
          
               /* cnt2 is the size of data that is present from the starting of the buffer */
               cnt2 = count - cnt1; 
          
               /* First we copy the data that is present till the end of serial buff to Ethernet buff */
               memcpy(cmd.response_buff, &uart_in_buff[uib_out], cnt1); 
               uib_count = (uib_count - (cnt1 + cnt2));   /* Reset "no:of bytes in serial port" cntr  */
          
               /* If there is some data at the start of the ring buffer then copy that too the 
               ethernet buffer, but append that...to the previous data */
               if (cnt2)
                 memcpy(&cmd.response_buff[cnt1], &uart_in_buff[0], cnt2);
          
			   cr_ptr = strchr((char *)cmd.response_buff,0x0d);
			   memset(temp_resp,NULL,sizeof(temp_resp));
			   memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
			   memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
			   memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

               /* Pointer now points to the new data end */
               uib_out = cnt2;
             }
           }
           else /* No Packaging technique will execute this section of code */
           {
             cnt1 = BUFF_SIZE - uib_out; 
             cnt2 = count - cnt1;
             memcpy(cmd.response_buff, &uart_in_buff[uib_out], cnt1);
             uib_count = (uib_count - (cnt1 + cnt2)); /* Reset "no:of bytes in serial port" cntr  */
             if (cnt2)
               memcpy(&cmd.response_buff[cnt1], &uart_in_buff[0], cnt2);
             uib_out = cnt2;

			 cr_ptr = strchr((char *)cmd.response_buff,0x0d);
			 memset(temp_resp,NULL,sizeof(temp_resp));
			 memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
			 memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
			 memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

           }
         }
       } //if(count > 0)
     }
   }
  }
  else
  {
    if(mn_http_find_value(BODYptr,(byte *)"cmd",cmd.cmd_buff))
    {
    memset(buff_pool.data_buff,NULL,sizeof(buff_pool.data_buff));
    cmd_len = strlen((char *)cmd.cmd_buff);
    memcpy(buff_pool.data_buff,cmd.cmd_buff,cmd_len);

    buff_pool.db_out = 0;
    buff_pool.len = cmd_len + 1;
    buff_pool.data_buff[cmd_len] = 0x0d;

    buff_pool.xmit_busy = TRUE;
    start_xmit();
    while (buff_pool.xmit_busy);     /* Wait until the UART is done. */

    if(serial.buffering_time > 0)
    {
      /* If there are some unacknowledged bytes then we need to reset the local counter of 
      Serial port and take a count the next time we come in this loop. This is only for TCP
      not UDP as there is no acknowledgment */
      if (socket_ptr->tcp_unacked_bytes) 
        count = 0;            /* No Ack recieved,Wait. Not Sending Data */
      else                    /* Yes Ack recieved . We can Send Data    */
      {
        T1TCR = 0x02;		/* reset timer */
        T1PR  = 0x00;		/* set prescaler to zero */
        T1MR0 = serial.buffering_time * TIME_INTERVAL; /* timer match at buffering time */
        T1IR  = 0xff;		/* reset all interrrupts */
        T1MCR = 0x04;		/* stop timer on match */
        T1TCR = 0x01;		/* start timer */
  
        while ((T1TCR & 0x01))
        {
          DISABLE_INTERRUPTS;
          count = uib_count;    /* Get the No of bytes in the Serial Port */
          if (count > MN_ETH_TCP_WINDOW)
            count = MN_ETH_TCP_WINDOW;  
          if(serial.endchar != NULL)
          {
            if(flag.endchar_recvd == TRUE)
              break;
          }
          if(serial.pkt_len != NULL)
          {
             if(flag.pkt_len_recvd == TRUE)
               break;
          }
          ENABLE_INTERRUPTS;
        }
        T1TCR = 0;		/* Disable Timer */
      }
    }
    else
    {
      if (socket_ptr->tcp_unacked_bytes) 
        count = 0;            /* No Ack recieved,Wait. Not Sending Data */
      else                    /* Yes Ack recieved . We can Send Data    */
      {
        DISABLE_INTERRUPTS;
        count = uib_count;    /* Get the No of bytes in the Serial Port */
        if (count > MN_ETH_TCP_WINDOW)
          count = MN_ETH_TCP_WINDOW;
        ENABLE_INTERRUPTS;    
      }
    }
  
    /* If local count of serial data is more than zero, execute this loop otherwise dont waste the 
    precious CPU cycles */
    if (count > 0)   
    {
      /* Software Flow Control: Part 3: If the no of bytes recieved in the serial buffer are less
      than 500 or so, then XON is sent immediately and the remote side will start sending
      Data to iServer */
      if(serial.flowcontrol == SOFTWARE_FLOW_CONTROL)/*This part of code execute only if flow control
      enabled */
	  {
	    if(count < 500 && count > 0) /* Data in the serial buffer is less than 500, more data can 
        come in, so send XON */
        {
#if(MODEL_TYPE == EIT_RS232_FULL)
          U1THR = XON;   /* U1THR is the HW 16 Bytes register that transmits data on wire 
          and triggers an Interrupt */  
#elif((MODEL_TYPE == EIT_RS232_GPIO) || (MODEL_TYPE == EIT_RS232_TTL) || (MODEL_TYPE == EIT_RS485))
          U0THR = XON;        /* If less character recvd send XON      */
#endif
        }
	  }
    
      if (uib_in > uib_out)         /* In Ring buff: Overlap Occured? NO */
      { 
        if((serial.endchar != NULL) || (serial.pkt_len != NULL))
        {
          /* End Char recvd?*/
          if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE))
          {
            if((serial.fwdendchar == ENABLED) || (serial.pkt_len != NULL))
              memcpy(cmd.response_buff, &uart_in_buff[uib_out], count);
            else
              memcpy(cmd.response_buff, &uart_in_buff[uib_out], (count - 1));
          
            uib_count -= count;       /* Reset the Count for UART Bytes */
            uib_out += count;         /* Start Pointer = Curr Pointer   */
            flag.endchar_recvd    = FALSE;
            flag.pkt_len_recvd    = FALSE;
          }
        }
        else
        {  
          memcpy(cmd.response_buff,&uart_in_buff[uib_out],count);/* Send data*/
          uib_count -= count; /* Reset "no:of bytes in serial port" cntr  */
          uib_out += count;    /* Start ptr = Current Ptr               */
          socket_ptr->send_ptr = eth_out_buff;    /* point to send buffer */
          socket_ptr->send_len = count;     /* Get the noof bytes to send */	
        }
      }
      else                  /* In UART Buffer: Overlap Occured? YES   */
      {
        /* This section of code only for Packaging technique */
        if((serial.endchar != NULL) || (serial.pkt_len != NULL))
        {
          /* End Char recvd?*/
          if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE)) 
          {
            /* cnt1 is size of data that is present till the end of buffer */
            cnt1 = BUFF_SIZE - uib_out; 
          
            /* cnt2 is the size of data that is present from the starting of the buffer */
            cnt2 = count - cnt1; 
          
            /* First we copy the data that is present till the end of serial buff to Ethernet buff */
            memcpy(cmd.response_buff, &uart_in_buff[uib_out], cnt1); 
            uib_count = (uib_count - (cnt1 + cnt2));   /* Reset "no:of bytes in serial port" cntr  */
          
            /* If there is some data at the start of the ring buffer then copy that too the 
            ethernet buffer, but append that...to the previous data */
            if (cnt2)
              memcpy(&cmd.response_buff[cnt1], &uart_in_buff[0], cnt2);
          
            /* Pointer now points to the new data end */
            uib_out = cnt2;
          }
        }
        else /* No Packaging technique will execute this section of code */
        {
          cnt1 = BUFF_SIZE - uib_out; 
          cnt2 = count - cnt1;
          memcpy(cmd.response_buff, &uart_in_buff[uib_out], cnt1);
          uib_count = (uib_count - (cnt1 + cnt2)); /* Reset "no:of bytes in serial port" cntr  */
          if (cnt2)
            memcpy(&cmd.response_buff[cnt1], &uart_in_buff[0], cnt2);
          uib_out = cnt2;
        }
      }
    } //if(count > 0)
  }
  mn_http_set_file(socket_ptr, mn_vf_get_entry("device_query.htm")); 
 }
}
