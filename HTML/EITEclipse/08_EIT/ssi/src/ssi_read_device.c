#include "micronet.h"
#include "np_includes.h"

#define HALF    1
#define FULL    2

#define MASTER  1
#define SLAVE   2

byte read_buff[200];

/*------------------------------------------------------------------------
 Function:    dev_read_func()
 Description: SSI: To Set up a table that reads continously the reading 
              of each device.
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_read_func(byte **str_)
cmx_reentrant 
{
  int i;
  byte vars[5][2] = {"a","b","c","d","e"};  
  i = 0;  

  strcpy((char *)read_buff," ");
  
  for (i=0;i<4;i++)
  {
    strcat((char *)read_buff,"var dev_n");
    strcat((char *)read_buff,(char *)vars[i]);
    strcat((char *)read_buff,"= new Array(");
	read_column(i);
    strcat((char *)read_buff,");\n");
  }
  
  *str_=(byte *)read_buff;           
  return (strlen((char *)read_buff));  
}



/*------------------------------------------------------------------------
 Function:    dev_rcall_func()
 Description: SSI: To Set up a table that reads continously the reading 
              of each device.
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
void read_column(int i)
cmx_reentrant 
{
  switch(i)
  {
    case 0:
	  read_sno();                                /* Get the Serial No   */
	  break;
	case 1:
	  read_dev_name();               /* Show No: to left of each device */
	  break;
	case 2:
	  get_reading();                                 /* Get the Reading */
	  break;
	case 3:
	  read_unit();                       /* Read the unit of the device */
	  break;
	default:
	  break;
  }
}


/*------------------------------------------------------------------------
 Function:    read_sno()
 Description: Get the Serial Number of the devices
 Input:       None
 Output:      Zero
------------------------------------------------------------------------*/
void read_sno(void)
cmx_reentrant 
{
  int i;
  byte temp;
  char sno[MAX_UNITS][2] = {"1", "2", "3", "4", "5", "6", "7", "8"};

  temp = device.check;
  
  for(i=0; i<MAX_UNITS; i++)
  {
    if(temp & 0x01)
	{
	  if(i != 0)
        strcat((char *)read_buff,",");        
      strcat((char *)read_buff,"\"");
      strcat((char *)read_buff,sno[i]);
      strcat((char *)read_buff,"\"");
	}
	temp = temp >> 1;
  }
}



/*------------------------------------------------------------------------
 Function:    show_item_func()
 Description: Show the Device No: 
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 read_dev_name(void)
cmx_reentrant 
{
  int i;
  
  temp_buff[0] = device.check;            /* Device check to temp Buff  */

  for(i = 0; i < MAX_UNITS; i++)
  {
    if(temp_buff[0]&0x01)
	{
	  if(i != 0)
        strcat((char *)read_buff,",");        
      strcat((char *)read_buff,"\"");
      strcat((char *)read_buff,device.name[i]);
      strcat((char *)read_buff,"\"");
    }
    temp_buff[0]=temp_buff[0]>>1;/* Right Shift to get the BIT we need*/
  }

  return 0;

}


/*------------------------------------------------------------------------
 Function:    get_reading()
 Description: Send command to the unit and get the reading
 Input:       Command and the Device No
 Output:      string
------------------------------------------------------------------------*/
void get_reading(void)
cmx_reentrant 
{
  int i,c;
  unsigned long cmd_len,response_len;
  word16 count, cnt1, cnt2;
  byte temp[33];
  char *cr_ptr;
  byte temp_resp[50];

  count = 0;
  temp_buff[0] = device.check;

  for(i=0;i<MAX_UNITS;i++)
  {
      
#if((MODEL_TYPE == EIT_RS232_FULL) || (MODEL_TYPE == EIT_RS232_GPIO) || (MODEL_TYPE == EIT_RS232_TTL))
    if(temp_buff[0]&0x01)
	{
      if(i != 0)
        strcat((char *)read_buff,",");        
      strcat((char *)read_buff,"\"");
    
      strcpy((char *)temp,(char *)cmd.response_buff);
      memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));		           
	  memcpy(cmd.cmd_buff,device.command[i],sizeof(device.command[i]));  
		               /* Copy the command from Flash to the cmd_buffer */
    
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
      else
      {
        DISABLE_INTERRUPTS;
        count = uib_count;    /* Get the No of bytes in the Serial Port */
        if (count > MN_ETH_TCP_WINDOW)
          count = MN_ETH_TCP_WINDOW;
        ENABLE_INTERRUPTS;    
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
      response_len = strlen((char*)cmd.response_buff);

      if(cmd.response_buff[response_len - 1] == 0x0d)
      {
        cmd.response_buff[response_len - 1] = 0x00;
      }
           
      strcat((char *)read_buff,(char *)cmd.response_buff);
      strcat((char *)read_buff,"\"");
  
    }
  	temp_buff[0]=temp_buff[0]>>1;

#elif(MODEL_TYPE == EIT_RS485)
    if(flag.rs485type == FULL)
    {
      if(temp_buff[0]&0x01)
	  {
        if(i != 0)
          strcat((char *)read_buff,",");        
        strcat((char *)read_buff,"\"");
     
        strcpy((char *)temp,(char *)cmd.response_buff);
        memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));		           
	    memcpy(cmd.cmd_buff,device.command[i],sizeof(device.command[i]));  
		               /* Copy the command from Flash to the cmd_buffer */
    
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
        else
        {
          DISABLE_INTERRUPTS;
          count = uib_count;    /* Get the No of bytes in the Serial Port */
          if (count > MN_ETH_TCP_WINDOW)
            count = MN_ETH_TCP_WINDOW;
          ENABLE_INTERRUPTS;    
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
        response_len = strlen((char*)cmd.response_buff);

        if(cmd.response_buff[response_len - 1] == 0x0d)
        {
          cmd.response_buff[response_len - 1] = 0x00;
        }
           
        strcat((char *)read_buff,(char *)cmd.response_buff);
        strcat((char *)read_buff,"\"");  
      }
  	  temp_buff[0]=temp_buff[0]>>1;
    }
    else if(flag.rs485type == HALF)
    {
      /*------------------------------------------------------------------------------------------
      Enter this loop only if the device is selected 
      ------------------------------------------------------------------------------------------*/
      if(temp_buff[0]&0x01)
	  {
        if(i != 0)
          strcat((char *)read_buff,",");        
        strcat((char *)read_buff,"\"");
     
        strcpy((char *)temp,(char *)cmd.response_buff);
        memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));		           
	    memcpy(cmd.cmd_buff,device.command[i],sizeof(device.command[i]));  
		               /* Copy the command from Flash to the cmd_buffer */
    
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
        else
        {
          DISABLE_INTERRUPTS;
          count = uib_count;    /* Get the No of bytes in the Serial Port */
          if (count > MN_ETH_TCP_WINDOW)
            count = MN_ETH_TCP_WINDOW;
          ENABLE_INTERRUPTS;    
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

				cr_ptr = strchr((char *)cmd.response_buff,0x0d);
     			memset(temp_resp,NULL,sizeof(temp_resp));
	     		memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
		   	    memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
		 	    memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

              }
            }
            else
            {  
              memcpy(cmd.response_buff,&uart_in_buff[uib_out],count);/* Send data*/
              
			  uib_count -= count; /* Reset "no:of bytes in serial port" cntr  */
              uib_out += count;    /* Start ptr = Current Ptr               */

			  cr_ptr = strchr((char *)cmd.response_buff,0x0d);
			  memset(temp_resp,NULL,sizeof(temp_resp));
			  memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
			  memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
			  memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

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

				cr_ptr = strchr((char *)cmd.response_buff,0x0d);
			    memset(temp_resp,NULL,sizeof(temp_resp));
			    memcpy(temp_resp,cr_ptr,strlen(cr_ptr));
			    memset(cmd.response_buff,NULL,sizeof(cmd.response_buff));
			    memcpy(cmd.response_buff,temp_resp,strlen((char *)temp_resp));/* Send data*/

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
        response_len = strlen((char*)cmd.response_buff);

        for(c = 0; c < response_len; c++)
        {
          if(cmd.response_buff[c] == 0x0d)
          {
            cmd.response_buff[c] = 0x20;
          }
        }
           
        strcat((char *)read_buff,(char *)cmd.response_buff);
        strcat((char *)read_buff,"\"");  
      }
  	  temp_buff[0]=temp_buff[0]>>1;
    }

#endif
  }
}


/*------------------------------------------------------------------------
 Function:    read_unit()
 Description: Show the unit no
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 read_unit(void)
cmx_reentrant 
{
  int i;

  temp_buff[0] = device.check;            /* Device check to temp Buff  */

  for(i = 0; i < MAX_UNITS; i++)
  {
    if(temp_buff[0]&0x01)
	{
	  if(i != 0)
        strcat((char *)read_buff,",");        
      strcat((char *)read_buff,"\"");
      strcat((char *)read_buff,device.unit[i]);
      strcat((char *)read_buff,"\"");
    }
    temp_buff[0]=temp_buff[0]>>1;/* Right Shift to get the BIT we need*/
  }

  return 0;

}



/*------------------------------------------------------------------------
 Function:    ssi_chkd_devs()
 Description: Show the total no:of checked devices
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 ssi_chkd_devs(byte **str_)
cmx_reentrant 
{
  unsigned long i, chkd_devs;
  char str[10];

  chkd_devs = 0;
  temp_buff[0] = device.check;            /* Device check to temp Buff  */

  for(i = 0; i < MAX_UNITS; i++)
  {
    if(temp_buff[0] & 0x01)
	{
	  chkd_devs = chkd_devs + 1;   
    }
    temp_buff[0]=temp_buff[0]>>1; /* Right Shift to get the BIT we need */
  }

  sprintf(str,"%d",chkd_devs);  
  *str_=(byte *)str;           
  
  return (strlen((char *)str));

}


/*------------------------------------------------------------------------
 Function:    ssi_timeout_select()
 Description: Show input to enter timeout if it is zero or Show input 
              to stop autoupdate.
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 ssi_timeout_select(byte **str_)
cmx_reentrant 
{
  word16 i;
  byte *str;
  
  *str_ = cgi_buff;
  str = cgi_buff;

  //sprintf(timeout_buff,"%d",(char *)timeout_buff);
  if(np_atoi((char *)timeout_buff) > 0)
  {
    strcpy((char *)str,"<input type=submit value=\"Stop auto update\">");
	i = ((word16)strlen((char *)str));
  }
  else
  {
    strcpy((char *)str,"<input type=text name=T size=3 value=");
	
    memset(timeout_buff,'0',3);
    timeout_buff[3] = '\0';
    if((np_atoi((char *)timeout_buff)<5) && (np_atoi((char *)timeout_buff)>0))
	{
      timeout_buff[0]='5';
	  timeout_buff[1]='\0';
	}
	strcat((char *)str,(char *)timeout_buff);
    
    strcat((char *)str,"><input type=submit value=\"Auto update\">");
	i = ((word16)strlen((char *)str));		
  }
  return i;
}

/*------------------------------------------------------------------------
 Function:    ssi_load_timeout()
 Description: Show present timeout value              
 Input:       string
 Output:      size of timeout string
------------------------------------------------------------------------*/
word16 ssi_load_timeout(byte **str)
cmx_reentrant 
{
  if((np_atoi((char *)timeout_buff)) < 5 && (np_atoi((char *)timeout_buff) > 0))
  {
    timeout_buff[0]='5';
    timeout_buff[1]='\0';
  }
  
  *str = timeout_buff;
  return ((word16)strlen((char *)timeout_buff));
}


/*------------------------------------------------------------------------
 Function:    ssi_load_on()
 Description: Show string "setTimeout("AutoReload()",xxxxx) if timeout 
              present(>0) else dont show anything..
 Input:       string
 Output:      size of output
------------------------------------------------------------------------*/
word16 ssi_load_on(byte **str_)
cmx_reentrant
{
  word16 i;
  byte *str;
 
  *str_=cgi_buff;
  str=cgi_buff;
  i=0;

  if(np_atoi((char *)timeout_buff) > 0)
  {
    strcpy((char *)str,"setTimeout(\"AutoReload()\",");
	if((np_atoi((char *)timeout_buff)<5) && (np_atoi((char *)timeout_buff)>0))
	{
      timeout_buff[0]='5';
      timeout_buff[1]='\0';
    }
    strcat((char *)str,(char *)timeout_buff);
    strcat((char *)str,(char *)"000);");
    i = ((word16)strlen((char *)str));
  }
  else
  {
    i=0;
  }
  memset(timeout_buff,'0',3);
  timeout_buff[3] = '\0';
  return i;
}
