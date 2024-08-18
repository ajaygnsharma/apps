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

/**************************************************************************************************
 Function:    get_no_of_io()
 Description: SSI: Get the no of ios
 Input:       None
 Output:      no of ios
**************************************************************************************************/
char get_no_of_io(void)
{
  int no_of_io = 0;

  switch(iserver.model)
  {
    case EIT_W_2F:   no_of_io = 1; break;
	case EIT_W_4:    no_of_io = 4; break;
    case EIT_D_2F:   no_of_io = 0; break;
    case EIT_D_4:    no_of_io = 3; break;
    case EIT_PCB_2F: no_of_io = 3; break;
    case EIT_PCB_4:  no_of_io = 8; break;
    case EIT_PCB_T:  no_of_io = 3; break;
    default:         no_of_io = 1; break;
  }

  return (no_of_io);
}

/**************************************************************************************************
 Function:    ssi_fio()
 Description: SSI: Display whether conditional GPIO is checked or 
                   not?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_cio(byte **str_)
cmx_reentrant 
{
  int no_of_io = 0;
  char str[200];
  
  no_of_io = get_no_of_io();
  
  if(no_of_io < 2) /* Dont show this option for the RS232 Full models */
  {
    strcpy(str," ");
  }
  else
  {
    switch(flag.cio)
    {
      case DISABLED: 
	    strcpy(str,"document.writeln(\"<input type=checkbox name=cio id=cio> Conditional I/O Pins\");"); 
		break;
	  
	  case ENABLED:  
	    strcpy(str,"document.writeln(\"<input type=checkbox name=cio id=cio checked=checked> Conditional I/O Pins\");"); 
		break;

      default:       
	    strcpy(str,"document.writeln(\"<input type=checkbox name=cio id=cio> Conditional I/O Pins\");"); 
		break;
    }
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_s_no()
 Description: SSI: Display the Serial No on the IO pins page
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_s_no(byte **str_)
cmx_reentrant 
{
  char str[200];

  strcpy((char *)str," ");

  switch(iserver.model)
  {
    case EIT_W_2F:   strcat(str,"\"1\""); break;
	case EIT_W_4:    strcat(str,"\"1\",\"2\",\"3\",\"4\""); break;
	case EIT_D_2F: 	 strcat(str," "); break;
    case EIT_D_4:    strcat(str,"\"1\",\"2\""); break;
    case EIT_PCB_2F: strcat(str,"\"1\",\"2\",\"3\""); break;
    case EIT_PCB_4:  strcat(str,"\"1\",\"2\",\"3\",\"4\",\"5\",\"6\""); break;
    case EIT_PCB_T:  strcat(str,"\"1\",\"2\",\"3\""); break;
    default:         strcat(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}



/**************************************************************************************************
 Function:    ssi_fio_status()
 Description: SSI: Display is ....
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_cio_status(byte **str_)
cmx_reentrant 
{
  char str[200];
  
  strcpy((char *)str," ");

  switch(iserver.model)
  {
    case EIT_W_2F:   strcat(str," "); break;
	
	case EIT_W_4:    
	  switch(flag.cio)
	  {
	    case ENABLED: strcat((char *)str,"\"\",\"\",\"\",\"\""); break;
		case DISABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\""); 
		  break;
		default: strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\""); 
		  break;  
	  }
	  break;
	
	case EIT_D_2F: 	 strcat(str," "); break;
    
	case EIT_D_4:    
	  switch(flag.cio)
	  {
	    case ENABLED: strcat((char *)str,"\"\",\"\",\"\""); break;
		case DISABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\""); 
		  break;
		default: strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\""); 
		  break;  
	  }
	  break;
    
	case EIT_PCB_2F: 
	  switch(flag.cio)
	  {
	    case ENABLED: strcat((char *)str,"\"\",\"\",\"\",\"\""); break;
		case DISABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\""); 
		  break;
		default: strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\""); 
		  break;  
	  }
	  break;

    case EIT_PCB_4:
	  switch(flag.cio)
	  {
	    case ENABLED: strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\""); break;
		case DISABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
		  break;
		default: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
		  break;  
	  }
	  break;

    case EIT_PCB_T:
	  switch(flag.cio)
	  {
	    case ENABLED: strcat((char *)str,"\"\",\"\",\"\""); break;
		case DISABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\"");
		  break;
		default: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\"");
		  break;  
	  }
	  break;

    default: strcat(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/**************************************************************************************************
 Function:    ssi_cio_status()
 Description: SSI: Display if Conditional IOs are disabled
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_fio_status(byte **str_)
cmx_reentrant 
{
  char str[200];
  
  strcpy((char *)str," ");

  switch(iserver.model)
  {
    case EIT_W_2F:   strcat(str," "); break;
	
	case EIT_W_4:    
	  switch(flag.cio)
	  {
	    case ENABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\""); 
		  break;
		
		case DISABLED: 
		  strcat((char *)str,"\"\",\"\",\"\",\"\""); 
		  break;
		
		default: 
		  strcat((char *)str,"\"\",\"\",\"\",\"\""); 
		  break;  
	  }
	  break;
	
	case EIT_D_2F: 	 strcat(str," "); break;
    
	case EIT_D_4:    
	  switch(flag.cio)
	  {
	    case ENABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\""); 
		  break;
		
		case DISABLED: 
		  strcat((char *)str,"\"\",\"\",\"\""); 
		  break;
		
		default: 
		  strcat((char *)str,"\"\",\"\",\"\"");  
		  break;  
	  }
	  break;
    
	case EIT_PCB_2F: 
	  switch(flag.cio)
	  {
	    case ENABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\""); 
		  break;
		
	    case DISABLED: 
		  strcat((char *)str,"\"\",\"\",\"\",\"\""); 
		  break;
		
		default: 
		  strcat((char *)str,"\"\",\"\",\"\",\"\""); 
		  break;  
	  }
	  break;

    case EIT_PCB_4:
	  switch(flag.cio)
	  {
	    case ENABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\",\"disabled\"");
		  break;

		case DISABLED: 
		  strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\""); 
		  break;
		
		default: 
		  strcat((char *)str,"\"\",\"\",\"\",\"\",\"\",\"\""); 
		  break;  
	  }
	  break;

    case EIT_PCB_T:
	  switch(flag.cio)
	  {
	    case ENABLED: 
		  strcat((char *)str,"\"disabled\",\"disabled\",\"disabled\"");
		  break;
		
		case DISABLED: 
		  strcat((char *)str,"\"\",\"\",\"\""); 
		  break;
		
		default: 
		  strcat((char *)str,"\"\",\"\",\"\""); 
		  break;  
	  }
	  break;

    default: strcat(str," "); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/**************************************************************************************************
 Function:    ssi_i_chk()
 Description: SSI: Display which Inputs are Checked ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_i_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  strcpy((char *)str," ");
  
  no_of_io = get_no_of_io();
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/**************************************************************************************************
 Function:    ssi_o_chk()
 Description: SSI: Display which Outputs are Checked ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_o_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  strcpy((char *)str," ");
  
  no_of_io = get_no_of_io();
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_h_chk()
 Description: SSI: Display which Outputs are High or Low ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_h_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  strcpy((char *)str," ");
  
  no_of_io = get_no_of_io();
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/**************************************************************************************************
 Function:    ssi_l_chk()
 Description: SSI: Display which Outputs are low or not?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_l_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  strcpy((char *)str," ");
  
  no_of_io = get_no_of_io();
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_image_source()
 Description: SSI: Display the image source
 Input:       Pointer to the string that will contain the image names
 Output:      Length of the string returned
**************************************************************************************************/
word16 ssi_image_source(byte **str_)
cmx_reentrant 
{
  char str[200];
  int no_of_io = 0;
  int i;
  char gpio_bit_no[8] = {6,3,5,4,2,7,8,9};
  char ret_val = 0;

  strcpy((char *)str," ");
  
  no_of_io = get_no_of_io();
  
  for(i = 0; i < no_of_io; i++)
  {
    if(i != 0)
      strcat((char *)str,",");
	
	if(gpio.direction[i] == 1)
	{
	  ret_val = test_bit(2,gpio_bit_no[i]);
	  
	  switch(ret_val)
	  {
	    case 0: strcat((char *)str,"\"grey.jpg\""); break;
        default:strcat((char *)str,"\"green.jpg\""); break;
	  }
	}
	else
	{
	  strcat((char *)str,"\"white.jpg\"");
	}      
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_ci_i_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_ci_i_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  no_of_io = get_no_of_io();
  
  no_of_io = no_of_io / 2;

  strcpy((char *)str," ");
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_ci_o_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_ci_o_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  no_of_io = get_no_of_io();
  no_of_io /= 2;

  strcpy((char *)str," ");
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_ct_i_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_ct_i_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  no_of_io = get_no_of_io();
  no_of_io /= 2;

  strcpy((char *)str," ");
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/**************************************************************************************************
 Function:    ssi_ct_o_chk()
 Description: SSI: Display which Conditional Ifs are Checked ?
 Input:       None
 Output:      String containing the status
**************************************************************************************************/
word16 ssi_ct_o_chk(byte **str_)
cmx_reentrant 
{
  char str[200];
  int i;  
  int no_of_io = 0;

  no_of_io = get_no_of_io();
  no_of_io /= 2;

  strcpy((char *)str," ");
  
  for(i = 0; i < no_of_io; i++)
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

  *str_=(byte *)str;           
  return (strlen((char *)str));
}
