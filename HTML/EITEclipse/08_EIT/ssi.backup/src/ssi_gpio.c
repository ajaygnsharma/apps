/*------------------------------------------------------------------------ 
					ssi_gpio.c

Description:	Return SSIs for network.htm page
				1. 
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


/*------------------------------------------------------------------------
 Function:    ssi_fio()
 Description: SSI: Display whether forced GPIO is checked or 
                   not?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_fio(byte **str_)
cmx_reentrant 
{
#if(MODEL_TYPE == EIT_RS232_FULL)
  char str[2];  
  
  strcpy(str," ");
#else
  char str[200];
  
  switch(flag.gpio_type)
  {
    case 1: strcpy(str,"<input type=\"radio\" name=\"gpiotype\" id=\"gpiotype\" value=\"1\" checked=\"checked\"> Forced GPIO"); break;
	case 2: strcpy(str,"<input type=\"radio\" name=\"gpiotype\" id=\"gpiotype\" value=\"1\" > Forced GPIO"); break;
    default: strcpy(str,"<input type=\"radio\" name=\"gpiotype\" id=\"gpiotype\" value=\"1\" checked=\"checked\" > Forced GPIO"); break;
  }
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_cio()
 Description: SSI: Display whether Conditional GPIO is checked or 
                   not?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_cio(byte **str_)
cmx_reentrant 
{
  
#if(MODEL_TYPE == EIT_RS232_FULL)  
  char str[2];  
  
  strcpy(str," ");
#else
  char str[200];

  switch(flag.gpio_type)
  {
    case 1: strcpy(str,"<input type=\"radio\" name=\"gpiotype\" id=\"gpiotype\" value=\"2\"> Conditional GPIO"); break;
    case 2: strcpy(str,"<input type=\"radio\" name=\"gpiotype\" id=\"gpiotype\" value=\"2\" checked=\"checked\"> Conditional GPIO"); break;
    default: strcpy(str,"<input type=\"radio\" name=\"gpiotype\" id=\"gpiotype\" value=\"2\"> Conditional GPIO"); break;
  }
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_s_no()
 Description: SSI: Display the Serial No on the IO pins page
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_s_no(byte **str_)
cmx_reentrant 
{
  char str[120];
  
  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
  strcat(str,"\"1\"");

#elif((MODEL_TYPE == EIT_RS232_GPIO)) 
  strcat(str,"\"1\",\"2\",\"3\",\"4\",\"5\",\"6\"");
	  
#elif(MODEL_TYPE == EIT_RS485)
  strcat(str,"\"1\",\"2\",\"3\",\"4\"");
    
#elif(MODEL_TYPE == EIT_RS232_TTL)
  strcat(str,"\"1\",\"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\"");

#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}



/*------------------------------------------------------------------------
 Function:    ssi_fio_status()
 Description: SSI: Display is ....
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_fio_status(byte **str_)
cmx_reentrant 
{
  char str[180];
  
  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"\"");
    }
    else
    {
      strcat((char *)str,"\"disabled\"");
    }
#elif((MODEL_TYPE == EIT_RS232_GPIO))
    if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\"");
    }
    else
    {
      strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
    }  
#elif(MODEL_TYPE == EIT_RS485)
	if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"\",\"\",\"\",\"\"");
    }
    else
    {
      strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
    }
#elif(MODEL_TYPE == EIT_RS232_TTL)
    if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"");
    }
    else
    {
      strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
    }
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_cio_status()
 Description: SSI: Display if Conditional IOs are disabled
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_cio_status(byte **str_)
cmx_reentrant 
{
  char str[120];
  
  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"disabled\"");
    }
    else
    {
      strcat((char *)str,"\"\"");
    }
#elif((MODEL_TYPE == EIT_RS232_GPIO))
    if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
    }
    else
    {
      strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\"");
    }  
#elif(MODEL_TYPE == EIT_RS485)
	if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
    }
    else
    {
      strcat((char *)str,"\"\",\"\",\"\",\"\"");
    }
#elif(MODEL_TYPE == EIT_RS232_TTL)
    if(flag.gpio_type == 1)
    {
      strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
    }
    else
    {
      strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"");
    }
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_i_chk()
 Description: SSI: Display which Inputs are Checked ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_i_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < ONE_GPIO; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.direction[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif((MODEL_TYPE == EIT_RS232_GPIO))
    for(i = 0; i < SIX_GPIO; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.direction[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
	  
#elif(MODEL_TYPE == EIT_RS485)
	for(i = 0; i < FOUR_GPIO; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.direction[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 8; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.direction[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_i_chk()
 Description: SSI: Display which Outputs are Checked ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_o_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_GPIO)
  for(i = 0; i < 6; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	  
	switch(gpio.direction[i])
	{
	  case 1: strcat((char *)str,"\" \""); break;
	  case 2: strcat((char *)str,"\"checked\""); break;
      default:strcat((char *)str,"\"checked\""); break;
	}  
  }
#elif(MODEL_TYPE == EIT_RS485)
  for(i = 0; i < 4; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	  
	switch(gpio.direction[i])
	{
	  case 1: strcat((char *)str,"\" \""); break;
	  case 2: strcat((char *)str,"\"checked\""); break;
      default:strcat((char *)str,"\"checked\""); break;
	}  
  }

#elif(MODEL_TYPE == EIT_RS232_TTL)
  for(i = 0; i < 8; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	  
	switch(gpio.direction[i])
	{
	  case 1: strcat((char *)str,"\" \""); break;
	  case 2: strcat((char *)str,"\"checked\""); break;
      default:strcat((char *)str,"\"checked\""); break;
	}  
  }

#elif(MODEL_TYPE == EIT_RS232_FULL)
  for(i = 0; i < 1; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	  
	switch(gpio.direction[i])
	{
	  case 1: strcat((char *)str,"\" \""); break;
	  case 2: strcat((char *)str,"\"checked\""); break;
      default:strcat((char *)str,"\"checked\""); break;
	}  
  }

#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_h_chk()
 Description: SSI: Display which Outputs are High or Low ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_h_chk(byte **str_)
cmx_reentrant 
{
  char str[180];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < 1; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#elif(MODEL_TYPE == EIT_RS232_GPIO)
    for(i = 0; i < SIX_GPIO; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS485)
   for(i = 0; i < 4; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 8; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#endif


  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_l_chk()
 Description: SSI: Display which Outputs are low or not?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_l_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < 1; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\" \""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#elif(MODEL_TYPE == EIT_RS232_GPIO)
    for(i = 0; i < SIX_GPIO; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\" \""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS485)
   for(i = 0; i < 4; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\" \""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 8; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.op_level[i])
	  {
	    case 1: strcat((char *)str,"\" \""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#endif  
  
  *str_=(byte *)str;           
  return (strlen((char *)str));

}


/*------------------------------------------------------------------------
 Function:    ssi_image_source()
 Description: SSI: Display the image source
 Input:       Pointer to the string that will contain the image names
 Output:      Length of the string returned
------------------------------------------------------------------------*/
word16 ssi_image_source(byte **str_)
cmx_reentrant 
{
  char str[180];
  
#if(MODEL_TYPE == EIT_RS232_FULL)
  char i_p;
  i_p = 0x00;
  strcpy((char *)str," ");
  
  if(gpio.direction[0] == 1)
  {
    FIO2MASK0 = 0xbf;           /* P2.(i+2) will only be read */
	  i_p = ((FIO2PIN0 & (~FIO2MASK0)) & 0x40) >> 6;
	  switch(i_p)
	  {
	    case 0: strcat((char *)str,"\"grey.jpg\""); break;
	    case 1: strcat((char *)str,"\"green.jpg\""); break;
        default:strcat((char *)str,"\"grey.jpg\""); break;
	  }
  }
  else
  {
    strcat((char *)str,"\"white.jpg\"");
  }

#elif(MODEL_TYPE == EIT_RS232_GPIO)
  int i;
  char i_p[6];
  char gpio_bit_mask[6] = {0x40, 0x08, 0x20, 0x10, 0x04, 0x80};
  char fio2mask0[6] =     {0xbf, 0xf7, 0xdf, 0xef, 0xfb, 0x7f};

  memset(i_p,NULL,sizeof(i_p));            /* Clear the i_p Array       */
  strcpy((char *)str," ");

  for(i = 0; i < SIX_GPIO; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	if(gpio.direction[i] == 1)
	{
	  FIO2MASK0 = fio2mask0[i];           /* P2.(i+2) will only be read */
	  i_p[i] = ((FIO2PIN0 & (~FIO2MASK0)) & gpio_bit_mask[i]) >> (i + 2);
	  switch(i_p[i])
	  {
	    case 0: strcat((char *)str,"\"grey.jpg\""); break;
	    case 1: strcat((char *)str,"\"green.jpg\""); break;
        default:strcat((char *)str,"\"grey.jpg\""); break;
	  }
	}
	else
	{
	  strcat((char *)str,"\"white.jpg\"");
	}      
  }

#elif(MODEL_TYPE == EIT_RS485)
  int i;
  char i_p[4];
  char gpio_bit_mask[4] = {0x40, 0x08, 0x20, 0x10};
  char fio2mask0[4] =     {0xbf, 0xf7, 0xdf, 0xef};

  memset(i_p,NULL,sizeof(i_p));            /* Clear the i_p Array       */
  strcpy((char *)str," ");

  for(i = 0; i < 4; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	if(gpio.direction[i] == 1)
	{
	  FIO2MASK0 = fio2mask0[i];           /* P2.(i+2) will only be read */
	  i_p[i] = ((FIO2PIN0 & (~FIO2MASK0)) & gpio_bit_mask[i]) >> (i + 2);
	  switch(i_p[i])
	  {
	    case 0: strcat((char *)str,"\"grey.jpg\""); break;
	    case 1: strcat((char *)str,"\"green.jpg\""); break;
        default:strcat((char *)str,"\"grey.jpg\""); break;
	  }
	}
	else
	{
	  strcat((char *)str,"\"white.jpg\"");
	}      
  }

#elif(MODEL_TYPE == EIT_RS232_TTL)
  int i;
  char i_p[8];
  char gpio_bit_mask[8] = {0x40, 0x08, 0x20, 0x10, 0x04, 0x80, 0x01, 0x02};
  char fio2mask0[8] =     {0xbf, 0xf7, 0xdf, 0xef, 0xfb, 0x7f, 0xfe, 0xfd};

  memset(i_p,NULL,sizeof(i_p));            /* Clear the i_p Array       */
  strcpy((char *)str," ");

  for(i = 0; i < 6; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	if(gpio.direction[i] == 1)
	{
	  FIO2MASK0 = fio2mask0[i];           /* P2.(i+2) will only be read */
	  i_p[i] = ((FIO2PIN0 & (~FIO2MASK0)) & gpio_bit_mask[i]) >> (i + 2);
	  switch(i_p[i])
	  {
	    case 0: strcat((char *)str,"\"grey.jpg\""); break;
	    case 1: strcat((char *)str,"\"green.jpg\""); break;
        default:strcat((char *)str,"\"grey.jpg\""); break;
	  }
	}
	else
	{
	  strcat((char *)str,"\"white.jpg\"");
	}      
  }
  for(i = 6; i < 8; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	if(gpio.direction[i] == 1)
	{
	  FIO2MASK1 = fio2mask0[i];           /* P2.(i+2) will only be read */
	  i_p[i] = ((FIO2PIN1 & (~FIO2MASK1)) & gpio_bit_mask[i]) >> (i + 2);
	  switch(i_p[i])
	  {
	    case 0: strcat((char *)str,"\"grey.jpg\""); break;
	    case 1: strcat((char *)str,"\"green.jpg\""); break;
        default:strcat((char *)str,"\"grey.jpg\""); break;
	  }
	}
	else
	{
	  strcat((char *)str,"\"white.jpg\"");
	}      
  }

#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_ci_i_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_ci_i_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < 1; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str," "); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_GPIO)
    for(i = 0; i < 3; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 4; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
	  
#elif(MODEL_TYPE == EIT_RS485)
	for(i = 0; i < 2; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#endif

#if(DEBUG_LEVEL == HIGH)
  printf("%s\n\r",str);
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_ci_o_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_ci_o_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < 1; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str," "); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_GPIO)
    for(i = 0; i < 3; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str,"\"\""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\"checked\""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 4; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str,"\"\""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\"checked\""); break; 
	  }  
    }	  
#elif(MODEL_TYPE == EIT_RS485)
	for(i = 0; i < 2; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.if_condition[i])
	  {
	    case 1: strcat((char *)str,"\" \""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\"checked\""); break; 
	  }  
    }
#endif

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",str);
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_ct_i_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_ct_i_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < 1; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str," "); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_GPIO)
    for(i = 0; i < 3; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 4; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
	  
#elif(MODEL_TYPE == EIT_RS485)
	for(i = 0; i < 2; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str,"\"checked\""); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }
#endif

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",str);
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_ct_o_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
------------------------------------------------------------------------*/
word16 ssi_ct_o_chk(byte **str_)
cmx_reentrant 
{
  char str[120];
  int i;  

  strcpy((char *)str," ");

#if(MODEL_TYPE == EIT_RS232_FULL)
    for(i = 0; i < 1; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str," "); break;
	    case 2: strcat((char *)str,"\" \""); break;     
        default:strcat((char *)str,"\" \""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_GPIO)
    for(i = 0; i < 3; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str,"\"\""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\"checked\""); break; 
	  }  
    }

#elif(MODEL_TYPE == EIT_RS232_TTL)
    for(i = 0; i < 4; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str,"\"\""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\"checked\""); break; 
	  }  
    }
	  
#elif(MODEL_TYPE == EIT_RS485)
	for(i = 0; i < 2; i++)
    {
      if(i != 0)
        strcat((char *)str,",");
	  
	  switch(gpio.then_condition[i])
	  {
	    case 1: strcat((char *)str,"\" \""); break;
	    case 2: strcat((char *)str,"\"checked\""); break;     
        default:strcat((char *)str,"\"checked\""); break; 
	  }  
    }
#endif

#if (DEBUG_LEVEL == HIGH)
  printf("%s\n\r",str);
#endif

  *str_=(byte *)str;           
  return (strlen((char *)str));
}
