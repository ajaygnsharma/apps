/*------------------------------------------------------------------------ 
					SSI_DEVICE_QUERY.C

Description:	Return SSIs for device_query.htm page
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
  Function:    ssi_device_query()
  Description: Output the Buffering Time of Ethernet-Serial Bridge
  Input:       Pointer to the String that will contain the Time
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_response(byte **str)
cmx_reentrant 
{
	int n;

	n = sprintf((char *)temp_buff,"%s",cmd.response_buff);
	*str=temp_buff;

	return ((word16)n);
}
