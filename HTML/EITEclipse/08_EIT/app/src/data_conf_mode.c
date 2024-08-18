#include "micronet.h"
#include "np_includes.h"

/*------------------------------------------------------------------------
 Function:    data_or_conf_mode()
 Description: Decide whether to enter data or configuration mode
 Input:       None
 Outout:      None
------------------------------------------------------------------------*/
void data_or_conf_mode(void)
cmx_reentrant
{
  unsigned long option[4];
  unsigned long i;
  
  memset(option,NULL,4);
  i = 0;  
  
  flag.rs485type = 2;
  
  /* Make the pins 1.20 - 23 as Inputs                                  */
  //FIO1DIR2  = 0x0f;
  
  T1TCR = 1;         /* Enable Timer 1 */
  T1TCR = 0x02;		/* reset timer */
  T1PR  = 0x00;		/* set prescaler to zero */
  T1MR0 = 5 * 1000 * TIME_INTERVAL;               /* wait for 5 seconds */
  T1IR  = 0xff;		/* reset all interrrupts */
  T1MCR = 0x04;		/* stop timer on match */
  T1TCR = 0x01;		/* start timer */

#if (DEBUG_LEVEL == LOW)  
  printf("Press \"CTRL+A\" 3 times in 5 seconds to activate serial console or\r\n \"Enter Key\" to display settings....");
#endif

  while (T1TCR & 0x01)             /* wait until delay time has elapsed */
  {      
    if((iserver.model == EIT_W_2F) || (iserver.model == EIT_D_2F) || (iserver.model == EIT_PCB_2F))
    {
	  if(U1LSR & 0x01)
      {
        option[i] = U1RBR;
        i++;
      }
	}
    else 
	{
	  if(U0LSR & 0x01)
      {
        option[i] = U0RBR;
        i++;
      }
	}
  
	if(option[0] == 0x0d)
	{
	  show_settings_mode();
	  option[0] = 0x00;
	}
	else if(option[0] == 0x01)
	{
	  if((option[1] == 0x01) && (option[2] == 0x01))
	  {
	    //printf("Debug Mode..\r\n");
		option[0] = 0x00;
		T1TCR = 0x02;		/* reset timer */
		T1TCR = 0;		/* Disable Timer */
		serial_console();
	  }
	} 
  }
  T1TCR = 0;		/* Disable Timer */
  
  if(flag.serial_console == ENABLED)
  {
    serial_console();
  }

  flag.rs485type = M32(FLASH_ADDR_RS232_485_GPIO);
}
