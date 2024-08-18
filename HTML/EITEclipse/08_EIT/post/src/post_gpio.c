/*------------------------------------------------------------------------------------------------
					post_gpio.c

Description:	Post Settings to the iopins.htm Page
				
----------------------------------------------------
GPIO            PIN                  PORT
----------------------------------------------------
1               PIN2     (J17A)      P2.6
2               PIN7/DCD (J16A)      P2.3
3               PIN8/DTR (J16A)      P2.5
4               PIN1/DSR (J17A)      P2.4
5               PIN5/CTS (J16A)      P2.2
6               PIN6/RTS (J16A)      P2.7
----------------------------------------------------

Author:			Ajay Sharma

Rev:			0

History:					  
-------------------------------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

/**************************************************************************************************
  Function: io_dir()
  Description: Set the direction of a gpio input/output
  Input: IO no and the direction
  Output: None
**************************************************************************************************/
void io_dir(byte port_no, byte io_no, byte dir)
{
  if(port_no == PORT_0)
  {
    if(dir == INPUT)
    {
      FIO0DIRL &= ~(1 << io_no);		 /* Make that direction input */
    }
    else if(dir == OUTPUT)
    {
      FIO0DIRL |= (1 << io_no);		 /* Make that direction output */
    }
  }
  else if(port_no == PORT_2)
  {
    if(dir == INPUT)
    {
      FIO2DIRL &= ~(1 << io_no);		 /* Make that direction input */
    }
    else if(dir == OUTPUT)
    {
      FIO2DIRL |= (1 << io_no);		 /* Make that direction output */
    }
  }
}

/**************************************************************************************************
  Function: io_set()
  Description: Set the Value of a gpio LOGIC_HIGH/LOGIC_LOW
  Input: IO no and the direction
  Output: None
**************************************************************************************************/
void io_set(byte port_no, byte io_no, byte value)
{
  if(port_no == PORT_0)
  {
    FIO0MASKL = 0xFFFF;                     /* Mask all bit */
    FIO0MASKL &= !(1 << io_no);           /* Make that bit 0 in the mask */
      
    if(value == LOGIC_LOW)
    {
	  FIO0CLRL = (1 << io_no);            /* Make that bit LOGIC_LOW */
    }
    else if(value == LOGIC_HIGH)
    {
      FIO0SETL = (1 << io_no);  			/* Make that bit LOGIC_HIGH */
    }
  }
  else if(port_no == PORT_2)
  {
    FIO2MASKL = 0xFFFF;                     /* Mask all bit */
    FIO2MASKL &= !(1 << io_no);           /* Make that bit 0 in the mask */
      
    if(value == LOGIC_LOW)
    {
	  FIO2CLRL = (1 << io_no);            /* Make that bit LOGIC_LOW */
    }
    else if(value == LOGIC_HIGH)
    {
      FIO2SETL = (1 << io_no);  			/* Make that bit LOGIC_HIGH */
    }
  }
}

byte test_bit(byte port_no, byte io_no)
{
  byte ret_val = 0;

  if(port_no == PORT_0)
  {
    FIO0MASKL = 0xFFFF;                   /* Mask all bit */
    FIO0MASKL &= !(1 << io_no);           /* Make that bit 0 in the mask */
    
	if(FIO0PINL & (1 << io_no))
	{
	  ret_val = LOGIC_HIGH;
	}
	else
	{
	  ret_val = LOGIC_LOW;
	}
  }
  else if(port_no == PORT_2)
  {
    FIO2MASKL = 0xFFFF;                   /* Mask all bit */
    FIO2MASKL &= !(1 << io_no);           /* Make that bit 0 in the mask */
    
	if(FIO2PINL & (1 << io_no))
	{
	  ret_val = LOGIC_HIGH;
	}
	else
	{
	  ret_val = LOGIC_LOW;
	}
  }
  
  return ret_val;
}

/*-------------------------------------------------------------------------------------------------
 Function:    post_gpio()
 Description: POST FUNCTION: Update iopins.htm Settings. 
 Input:       Socket Pointer 
 Output:      None
-------------------------------------------------------------------------------------------------*/
void post_gpio(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  
  /*-----------------------------------------------------------------------------------------------
                 F O R C E D / C O N D I T I O N A L     GPIOs
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"cio",temp_buff))
  {
    //switch(temp_buff[0])
    //{
      /*-------------------------------------------------------------------------------------------
      FIO2DIR0  =  1 1 1 1   1 1 0 0 (P2.2 to P2.7 is Output)
      FIO2CLR0  =  1 1 1 1   1 1 0 0 (P2.2 to P2.7 is Cleared)
      FIO2MASK0 =  0 0 0 0   0 0 1 1 (P2.2 to P2.7 is Masked and only they will be changed)
      -------------------------------------------------------------------------------------------*/
      //case '1': 
      //  flag.gpio_type = 1;
      //  break;       /* Forced GPIO                */
      
      /*-------------------------------------------------------------------------------------------
      FIO2DIR0  = 1 0 0 1    0 1 0 0 (P2.5,P2.3,P2.6 is I/P, P2.2,4,7 is O/P)
      FIO2CLR0  = 1 0 0 1    0 1 0 0 (Clear only the O/Ps)
      FIO2MASK0 = 0 1 1 0    1 0 1 1 (Only O/Ps are Masked)
      -------------------------------------------------------------------------------------------*/
      //case '2': 
        flag.cio = ENABLED;  
      //  FIO2DIR0       = 0x94;
      //  FIO2CLR0       = 0x94;
      //  FIO2MASK0      = 0x6b;
      //  break;       /* Conditional GPIOs          */
      
      //default : 
      //  flag.gpio_type = 1;  break;       /* Forced GPIO                */
    //}
  }
  else
  {
    flag.cio = DISABLED;
  }
  

  /*===============================================================================================
                 F O R C E D          G P I O
  ===============================================================================================*/


  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      1 
                 
                 is P2.6 
   io1 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"io1",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1':
	    gpio.direction[0] = 1;  io_dir(2,6,INPUT); break;  
      case '2': 						  				  /* P2.6 = I/P */
        gpio.direction[0] = 2;  io_dir(2,6,OUTPUT); break;
      default : 										  /* P2.6 = I/P */
        gpio.direction[0] = 1;  io_dir(2,6,INPUT); break;
    }
  }

  /*------------------p1c sets the GPIO1's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"p1c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[0] = 1;  
		if(gpio.direction[0] == 2)
		{  
		  io_set(2,6,LOGIC_HIGH);
		}
		break;

	  case '2': 						  
        gpio.op_level[0] = 2;  
		if(gpio.direction[0] == 2)
		{
		  io_set(2,6,LOGIC_LOW);
		}  
		break;

      default : 
        gpio.op_level[0] = 1;
		if(gpio.direction[0] == 2)
		{
		  io_set(2,6,LOGIC_LOW);
		}
		break;
    }
  }

  
  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      2 
                 
                 is P2.3 
   io2 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"io2",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1':                                           /* P2.3 = I/P */
        gpio.direction[1] = 1; io_dir(2,3,INPUT); break;  
      case '2': 						                  /* P2.3 = O/P */
        gpio.direction[1] = 2; io_dir(2,3,OUTPUT); break; 
      default :                                           /* P2.3 = O/P */
        gpio.direction[1] = 1; io_dir(2,3,INPUT); break; 
    }
  }

  /*------------------p2c sets the GPIO2's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"p2c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[1] = 1;  
		if(gpio.direction[1] == 2)
		{  
          io_set(2,3,LOGIC_HIGH);
		}
		break;                          
      
      case '2': 						  
        gpio.op_level[1] = 2;  
		if(gpio.direction[1] == 2)
		{
          io_set(2,3,LOGIC_LOW);
		}
		break;

      default : 
        gpio.op_level[1] = 1;  
		if(gpio.direction[1] == 2)
		{
          io_set(2,3,LOGIC_LOW);
		}
		break;
	}
  }



  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      3 
                 
                 is P2.5 
   io3 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"io3",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1':                                           /* P2.5 = I/P */
        gpio.direction[2] = 1; io_dir(2,5,INPUT); break; 
      case '2': 						  				  /* P2.5 = O/P */
        gpio.direction[2] = 2; io_dir(2,5,OUTPUT); break;
      default : 										  /* P2.5 = O/P */
        gpio.direction[2] = 1; io_dir(2,5,INPUT); break;
    }
  }
  
  /*------------------p3c sets the GPIO3's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"p3c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[2] = 1;
		if(gpio.direction[2] == 2)
		{
		  io_set(2,5,LOGIC_HIGH);
		}
		break;                          
      
      case '2': 						  
        gpio.op_level[2] = 2;
		if(gpio.direction[2] == 2)
		{
		  io_set(2,5,LOGIC_LOW);
		}
		break;

      default : 
        gpio.op_level[2] = 1;
		if(gpio.direction[2] == 2)
		{
		  io_set(2,5,LOGIC_LOW);
		}
		break;
    }
  }


  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      4 
                 
                 is P2.4 
   io4 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"io4",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 										  /* P2.4 = I/P */
        gpio.direction[3] = 1; io_dir(2,4,INPUT); break; 
      case '2': 						  				  /* P2.4 = O/P */
        gpio.direction[3] = 2; io_dir(2,4,OUTPUT); break; 
      default :                                           /* P2.4 = O/P */
        gpio.direction[3] = 1; io_dir(2,4,INPUT); break; 
    }
  }

  /*------------------p4c sets the GPIO4's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"p4c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[3] = 1;
		if(gpio.direction[3] == 2)
		{
		  io_set(2,4,LOGIC_HIGH);
		}
		break;                          
      
      case '2': 						  
        gpio.op_level[3] = 2;
		if(gpio.direction[3] == 2)
		{
		  io_set(2,4,LOGIC_LOW);
		}  
		break;

      default : 
        gpio.op_level[3] = 1;
		if(gpio.direction[3] == 2)
		{  
		  io_set(2,4,LOGIC_LOW);
		}
		break;
    }
  }


  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      5 
                 
                 is P2.2 
   io5 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"io5",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1':                                           /* P2.2 = I/P */
        gpio.direction[4] = 1; io_dir(2,2,INPUT); break;  
      case '2': 						                  /* P2.2 = O/P */
        gpio.direction[4] = 2; io_dir(2,2,OUTPUT); break;
      default :                                           /* P2.2 = O/P */
        gpio.direction[4] = 1; io_dir(2,2,INPUT); break;
    }
  }

  /*------------------p5c sets the GPIO5's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"p5c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[4] = 1;
		if(gpio.direction[4] == 2)
		{
		  io_set(2,2,LOGIC_HIGH);
		}
		break;                          
      
      case '2': 						  
        gpio.op_level[4] = 2;
		if(gpio.direction[4] == 2)
		{  
		  io_set(2,2,LOGIC_LOW);
		}
		break;

      default : 
        gpio.op_level[4] = 1;
		if(gpio.direction[4] == 2)
		{  
		  io_set(2,2,LOGIC_LOW);
		}
		break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      6 
                 
                 is P2.7 
   io6 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"io6",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 										  /* P2.7 = I/P */
        gpio.direction[5] = 1; io_dir(2,7,INPUT); break;   
      case '2': 						  				  /* P2.7 = O/P */
        gpio.direction[5] = 2; io_dir(2,7,OUTPUT); break;  
      default : 										  /* P2.7 = O/P */
        gpio.direction[5] = 1; io_dir(2,7,INPUT); break;
    }
  }

  /*------------------p6c sets the GPIO6's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"p6c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[5] = 1;
		if(gpio.direction[5] == 2)
		{  
		  io_set(2,7,LOGIC_HIGH);
		}
		break;                          
      
      case '2': 						  
        gpio.op_level[5] = 2;
		if(gpio.direction[5] == 2)
		{  
		  io_set(2,7,LOGIC_LOW);
		}
		break;

      default : 
        gpio.op_level[5] = 1;
		if(gpio.direction[5] == 2)
		{  
		  io_set(2,7,LOGIC_LOW);
		}
		break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      7 
                 
                 is P2.8 
   io7 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"io7",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 										  /* P2.8 = I/P */
        gpio.direction[6] = 1; io_dir(2,8,INPUT); break;  
      case '2': 						  				  /* P2.8 = O/P */
        gpio.direction[6] = 2; io_dir(2,8,OUTPUT); break; 
      default : 										  /* P2.8 = O/P */
        gpio.direction[6] = 1; io_dir(2,8,INPUT); break; 
    }
  }

  /*------------------p7c sets the GPIO7's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"p7c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[6] = 1;
		if(gpio.direction[6] == 2)
		  io_set(2,8,LOGIC_HIGH);
		break;                          
      
      case '2': 						  
        gpio.op_level[6] = 2;
		if(gpio.direction[6] == 2)
		  io_set(2,8,LOGIC_LOW);
		break;

      default : 
        gpio.op_level[6] = 1;
		if(gpio.direction[6] == 2)
		  io_set(2,8,LOGIC_LOW);
		break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 G   P   I   O      8 
                 
                 is P2.9 
   io8 sets the Direction of the GPIO 
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"io8",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 										  /* P2.9 = I/P */
        gpio.direction[7] = 1; io_dir(2,9,INPUT); break;
      case '2': 						  				  /* P2.9 = O/P */
        gpio.direction[7] = 2; io_dir(2,9,OUTPUT); break;
      default : 										  /* P2.9 = O/P */
        gpio.direction[7] = 1; io_dir(2,9,INPUT); break;
    }
  }

  /*------------------p8c sets the GPIO8's LOGIC_HIGH of LOGIC_LOW value-------------------------------------*/  
  if(mn_http_find_value(BODYptr,(byte *)"p8c",temp_buff))
  {
    switch(temp_buff[0])
    {
      case '1': 
        gpio.op_level[7] = 1;
		if(gpio.direction[7] == 2)
		  io_set(2,9,LOGIC_HIGH);
		break;                          
      
      case '2': 						  
        gpio.op_level[7] = 2;
		if(gpio.direction[7] == 2)
		  io_set(2,9,LOGIC_LOW);
		break;

      default : 
        gpio.op_level[7] = 1;
		if(gpio.direction[7] == 2)
		  io_set(2,9,LOGIC_LOW);
		break;
    }
  }


  /*===============================================================================================
                 C O N D I T I O N A L          G P I O s
  ===============================================================================================*/
  /*-----------------------------------------------------------------------------------------------
                 I  F   1 
                 
           Set the IF Condition[0]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_i1",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.if_condition[0] = 1; break;
	  case '2': gpio.if_condition[0] = 2;	break;
	  default: gpio.if_condition[0] = 1; break;
    }

  }

  /*-----------------------------------------------------------------------------------------------
                 I  F   2 
                 
           Set the IF Condition[1]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_i2",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.if_condition[1] = 1; break;
	  case '2': gpio.if_condition[1] = 2;	break;
	  default: gpio.if_condition[1] = 1; break;
    }
  }
  
  /*-----------------------------------------------------------------------------------------------
                 I  F   3 
                 
           Set the IF Condition[2]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_i3",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.if_condition[2] = 1; break;
	  case '2': gpio.if_condition[2] = 2;	break;
	  default: gpio.if_condition[2] = 1; break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 I  F   4 
                 
           Set the IF Condition[3]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_i4",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.if_condition[3] = 1; break;
	  case '2': gpio.if_condition[3] = 2;	break;
	  default: gpio.if_condition[3] = 1; break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 T H E N   1 
                 
           Set the THEN Condition[0]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_t1",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.then_condition[0] = 1; break;
	  case '2': gpio.then_condition[0] = 2;	break;
	  default: gpio.then_condition[0] = 1; break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 T H E N   2 
                 
           Set the THEN Condition[1]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_t2",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.then_condition[1] = 1; break;
	  case '2': gpio.then_condition[1] = 2;	break;
	  default: gpio.then_condition[1] = 1; break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 T H E N   3 
                 
           Set the THEN Condition[2]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_t3",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.then_condition[2] = 1; break;
	  case '2': gpio.then_condition[2] = 2;	break;
	  default: gpio.then_condition[2] = 1; break;
    }
  }

  /*-----------------------------------------------------------------------------------------------
                 T H E N   4 
                 
           Set the THEN Condition[3]
  -----------------------------------------------------------------------------------------------*/
  if(mn_http_find_value(BODYptr,(byte *)"c_t4",temp_buff))
  {
  	switch(temp_buff[0])
    {
	  case '1': gpio.then_condition[3] = 1; break;
	  case '2': gpio.then_condition[3] = 2;	break;
	  default: gpio.then_condition[3] = 1; break;
    }
  }

  write_all();                                           /* Write all updates settings to Flash  */
  mn_http_set_file(socket_ptr, mn_vf_get_entry("iopins.htm")); 
}
