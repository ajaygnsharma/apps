#include "micronet.h"
#include "np_includes.h"
#include "U2e_http.h"


/*-------------------------------------------------------------------------------------------------
Function: SerialToEthernet(Socket Ptr)
Description: Send Data from the Serial Port to the Ethernet Port.
             We are having two scenarios here, first if the data comes and is less than the buffer
             size we simply send it accross, but lets say that its more than the buffer size, We 
             adjust the pointer accordingly.
Input:       Socket Ptr
Output:      NEED TO SEND if we need to reply to the ethernet
-------------------------------------------------------------------------------------------------*/
SCHAR SerialToEthernet(PSOCKET_INFO socket_ptr)
cmx_reentrant
{
  word16 count, cnt1, cnt2;
  SCHAR ret_val;

  ret_val = 0;
  count   = 0;

  /*-----------------------------------U D P only -----------------------------------------------*/
  if(terminal_server.protocol == UDP_PROTOCOL)
  {
    /* Since buffering time is used, timer is used here, this is match based timer function
    The while loop stays there polling for the max size of the serial data till the timer 
    matches the serial buffering time. */
    if(serial.buffering_time > 0) /* Execute this only if buffering time of serial data is 
    enabled */
    {
      /*
      * setup timer #1 for delay
      */
      T1TCR = 0x02;		/* reset timer */
      T1PR  = 0x00;		/* set prescaler to zero */
      T1MR0 = serial.buffering_time * TIME_INTERVAL;/* Set the timer to match buffering time */
      T1IR  = 0xff;		/* reset all interrrupts */
      T1MCR = 0x04;		/* stop timer on match */
      T1TCR = 0x01;		/* start timer */
  
      while (T1TCR & 0x01) /* Loop in this while loop till timer matches the buffering time */
      {
        DISABLE_INTERRUPTS;
        count = uib_count;    /* Get the No of bytes in the Serial Port */
        /* Cut down the no of bytes recieved from the serial
        port to max size of the TCP window as that is the max that can be sent in one packet */
        if (count > MN_ETH_TCP_WINDOW) /* If the data in the serial buffer > size of TCP buffer */
          count = MN_ETH_TCP_WINDOW;   /* Restrict the count to the size of ethernet packet len */
        ENABLE_INTERRUPTS;
      }
      T1TCR = 0;		     /* Disable Timer */
    }
    else /* Execute this part of code only if the buffering time is not set */
    {
      DISABLE_INTERRUPTS;
      count = uib_count;    /* Get the No of bytes in the Serial Port */
      if (count > MN_ETH_TCP_WINDOW)
       count = MN_ETH_TCP_WINDOW;
      ENABLE_INTERRUPTS;    
    }
  }
  /*-----------------------------------TCP only -------------------------------------------------*/
  else if(terminal_server.protocol == TCP_PROTOCOL)
  {
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
        if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
 		{
          U1THR = XON;   /* U1THR is the HW 16 Bytes register that transmits data on wire 
                         and triggers an Interrupt */  
        }
		else
		{
		  U0THR = XON;        /* If less character recvd send XON      */
		}
      }
	}
    
    if (uib_in > uib_out)         /* In Ring buff: Overlap Occured? NO */
    {
      if((serial.endchar != NULL) || (serial.pkt_len != NULL))
      {
        /* End Char recvd?*/
        if((flag.endchar_recvd == TRUE) || (flag.pkt_len_recvd == TRUE))
        {
          memcpy(eth_out_buff, &uart_in_buff[uib_out], count);
          uib_count -= count;       /* Reset the Count for UART Bytes */
          uib_out += count;         /* Start Pointer = Curr Pointer   */
          socket_ptr->send_ptr = eth_out_buff;/* point to send buffer */
          if((serial.fwdendchar == ENABLED) || (serial.pkt_len != NULL))
            socket_ptr->send_len = count;/*noof bytes to send + endch */
          else
            socket_ptr->send_len = (count - 1);/*noof bytes to send-ec*/
          flag.endchar_recvd    = FALSE;
          flag.pkt_len_recvd    = FALSE;
        }
      }
      else
      {
        memcpy(eth_out_buff,&uart_in_buff[uib_out],count);/* Send data*/
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
          memcpy(eth_out_buff, &uart_in_buff[uib_out], cnt1); 
          uib_count = (uib_count - (cnt1 + cnt2));   /* Reset "no:of bytes in serial port" cntr  */
          
          /* If there is some data at the start of the ring buffer then copy that too the 
          ethernet buffer, but append that...to the previous data */
          if (cnt2)
            memcpy(&eth_out_buff[cnt1], &uart_in_buff[0], cnt2);
          
          /* Pointer now points to the new data end */
          uib_out = cnt2;
      
          socket_ptr->send_ptr = eth_out_buff;  /* point to send buffer */
          if((serial.fwdendchar == ENABLED) || (serial.pkt_len != NULL))
            socket_ptr->send_len = count;       /* noof bytes to send + end char */
          else
            socket_ptr->send_len = (count - 1); /* noof bytes to send - end char */
        }
      }
      else /* No Packaging technique will execute this section of code */
      {
        cnt1 = BUFF_SIZE - uib_out; 
        cnt2 = count - cnt1;
        memcpy(eth_out_buff, &uart_in_buff[uib_out], cnt1);
        uib_count = (uib_count - (cnt1 + cnt2)); /* Reset "no:of bytes in serial port" cntr  */
        if (cnt2)
          memcpy(&eth_out_buff[cnt1], &uart_in_buff[0], cnt2);
          uib_out = cnt2;
      
        socket_ptr->send_ptr = eth_out_buff;    /* point to send buffer */
        socket_ptr->send_len = count;     /* Get the noof bytes to send */
      }
    }
    ret_val = NEED_TO_SEND;  
  }
  
  /* Since we call this function when atleast one byte is recieved we always return a 
  NEED_TO_SEND so we HAVE to send the data to the ethernet */
  return(ret_val);
}


/*-------------------------------------------------------------------------------------------------
  Function:    EthernetToSerial_BufferingTime(void)
  Description: Send Data Ethernet to Serial. There is no packaging technique applied here as i used 
               to think before. Instead we are always sending data from the ethernet to the serial
               as it arrives.
  Input:       None
  Output:      Not being used....What do we do to make it better.
--------------------------------------------------------------------------------------------------*/
SCHAR EthernetToSerial(void)
cmx_reentrant
{
  if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
  {
    if(terminal_server.connect_on == 2)
    {
      U1MCR |= (1 << 0);       //Rise DTR 
    }
    else if(terminal_server.connect_on == 3)
    {
      U1MCR |= (0 << 0);      /* Drop DTR */
    }

    U1THR = buff_pool.data_buff[buff_pool.db_out];/* send first char 	*/
    serial.tx_bytes++;                  /* Inc UART TX bytes cntr */
    buff_pool.db_out++;                   /* Increment Sent Pointer     */
    if (buff_pool.db_out < buff_pool.len)/* There more chars to send?   */
      U1IER |= 0x02;
    else
      buff_pool.xmit_busy = FALSE;	/* No,flag that sending is finished */
  }
  else
  {
    U0THR = buff_pool.data_buff[buff_pool.db_out];/* send first char 	*/
    serial.tx_bytes++;                  /* Inc UART TX bytes cntr */
    buff_pool.db_out++;                   /* Increment Sent Pointer     */
    if (buff_pool.db_out < buff_pool.len)/* There more chars to send?   */
      U0IER |= 0x02;				     /* Yes, enable xmit interrupt. */
    else
      buff_pool.xmit_busy = FALSE;	/* No,flag that sending is finished */
  }
  return 0;
}
