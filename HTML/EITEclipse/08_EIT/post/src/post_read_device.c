/*------------------------------------------------------------------------ 
					POST_DEVICE_SETUP.c

Description:	Post for read_device.htm page
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

byte timeout_buff[5];

void set_timeout_func(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  memset(timeout_buff,'0',3);
  timeout_buff[3] = '\0';

  mn_http_find_value(BODYptr,(byte *)"T",timeout_buff);
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("read_device.htm"));
  
}
