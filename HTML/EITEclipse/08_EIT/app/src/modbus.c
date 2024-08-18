#include "micronet.h"
#include "np_includes.h"

TCP_MODBUS_HEADER modbus_head;
TIMER_INFO_T modbus_wait_timer;
byte modbus_wait=0;

/*------------------------------------------------------------------------
Function:     modbus_send()
Description:  Send data arrived on port 502 to Serial Port 
Input:        Data arriving on port 502 
Output:       none
------------------------------------------------------------------------*/
void modbus_send(byte *str,word16 count)
cmx_reentrant
{
  word16 crc, n;
  
  modbus_head.trans_id[0]    = *str++;
  modbus_head.trans_id[1]    = *str++;
  modbus_head.protocol_id[0] = *str++;
  modbus_head.protocol_id[1] = *str++;
  modbus_head.length[0]      = *str++;
  modbus_head.length[1]      = *str++;
  modbus_head.u_id           = *str++;
  modbus_head.f_code         = *str--;
  
  n = modbus_head.length[0] * 0x100 + modbus_head.length[1];
	
  if((n + 6) == count)
  {
    crc            = calc_crc(str, str + n - 1);
    *(str + n + 1) = (byte)(crc / 0x100);   // crc hi
    *(str + n)     = (byte)(crc & 0x00ff);	// crc low
    
	buff_pool.db_out = 0;  /* The total no of bytes of data out of the buffer is zero */
    buff_pool.len = 6 + n + 2; /* Get the length of the data in the socket */
    buff_pool.db_out = buff_pool.db_out + 6;      
	buff_pool.xmit_busy = TRUE;
    start_xmit();
    while (buff_pool.xmit_busy);     /* Wait until the UART is done. */
	uib_in = uib_out = 0;
	uib_count = 0;
  }
  else
  {
    modbus_exception(0x03);
  }
}

/*------------------------------------------------------------------------
Function:     modbus_recv()
Description:  Call a function to recieve data from serial port and send 
              to ethernet port 502 of modbus. 
Input:        None
Output:       None
------------------------------------------------------------------------*/
void modbus_recv(void)
cmx_reentrant
{
  //timer1_start((comm_tout*10)/25);
  //while(!timer1_flag){}
  delayMs(1,100);
  if(!modbus_return_package())
  {
    modbus_exception(0x01);
  }
}

/*------------------------------------------------------------------------
Function:     modbus_return_package()
Description:  Get Data on the serial port parse into modbus packet
              and send it to the Ethernet               
Input:        None
Output:       0 Failure
              1 Success
------------------------------------------------------------------------*/
byte modbus_return_package(void)
cmx_reentrant
{
  word16 crc,i;
  byte *str;
  byte *start;
  byte *end;

  DISABLE_INTERRUPTS;
  start = &uart_in_buff[uib_out];
  end   = &uart_in_buff[uib_in - 1];
  ENABLE_INTERRUPTS;

  if(start != end)
  {
    crc = calc_crc(start,end - 2);
	if((*(end - 1) + (*end) * 0x100) == crc)
    {
	  i   = 0;
      str = &eth_out_buff[0] + 6;
      while(start != end)
      {				
        *str++ = *start++;
        i++;
        if (start >= &uart_in_buff[BUFF_SIZE])
        {
          start = &uart_in_buff[0];
        }
      }
      uib_count = i-1;	
      eth_out_buff[0] = modbus_head.trans_id[0]; 
	  eth_out_buff[1] = modbus_head.trans_id[1];
      eth_out_buff[2] = modbus_head.protocol_id[0];
      eth_out_buff[3] = modbus_head.protocol_id[0];
      eth_out_buff[4] = (byte)(uib_count/0x100);
      eth_out_buff[5] = (byte)(uib_count&0x00ff);
      uib_count += 6;
    }
    else
    {
       modbus_exception(0x03);
    }
    return 1;
  }
  else
  {
    return 0;
  }
}

/*------------------------------------------------------------------------
Function:     modbus_exception()
Description:  throw modbus exception
Input:        error number
Output:       None
------------------------------------------------------------------------*/
void modbus_exception(byte error)
cmx_reentrant
{
  eth_out_buff[0]   = modbus_head.trans_id[0];
  eth_out_buff[1]   = modbus_head.trans_id[1];
  eth_out_buff[2]   = modbus_head.protocol_id[0];
  eth_out_buff[3]   = modbus_head.protocol_id[0];
  eth_out_buff[4]   = 0x00;
  eth_out_buff[5]   = 0x03;
  eth_out_buff[6]   = modbus_head.u_id;
  eth_out_buff[7]   = 0x80 + modbus_head.f_code;
  eth_out_buff[8]   = error;
  uib_count      = 9;
}


/*------------------------------------------------------------------------
Function:     calc_crc()
Description:  Calculate Modbus CRC
Input:        pointer to the start and end of the packet 
Output:       CRC
------------------------------------------------------------------------*/
word16 calc_crc(byte *start_of_packet, byte *end_of_packet)
cmx_reentrant
{
  word16 crc;
  byte bit_count;
  byte *char_ptr;

  char_ptr = start_of_packet;
  crc      = 0xffff;
  
  do
  {
    crc ^= (unsigned int)*char_ptr;
    bit_count = 0;
    do
    {
      if(crc & 0x0001)
      {
        crc >>= 1;
        crc ^= POLYNOMIAL;
      }
      else
      {
        crc >>= 1;
      }
    }while(bit_count ++< 7);
  }while(char_ptr ++< end_of_packet);
  
  return(crc);
}


