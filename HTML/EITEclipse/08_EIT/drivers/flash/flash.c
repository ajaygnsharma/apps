#include "flash.h"
#include "micronet.h"
#include "np_includes.h"

U32 command[5];
U32 result[3];

IAP iap_entry;

/**************************************************************************************************
 Function:    _fl_read()
 Description: Get the Data into the string from the location mentioned till the size
 Input:       String, Location and the Size
 Output:      Pointer the String				
**************************************************************************************************/
U8 _fl_read(U8 *str,U32 location,U32 str_size)
{
  U32 temp;

  /* Read all the data starting from the location and till the mentioned size */
  while(str_size)
  {
    temp = M32(location);	/* Get the Data stored in the memory location pointed by location */
	*str = temp;  /* String contains the data now */
	++str;		  /* Increment the String Pointer */
	location++;	  /* Increment the Location pointer to the next location */
	str_size--;		  /* Decrease the size */
  }
  *str='\0';	  /* Terminate the string with a NULL */
  
  return 0;
}


/**************************************************************************************************
Function:    _fl_write()
Description: Write current values to the flash 
Input:       Pointer to the buffer that will be written to the flash
Output:      None
**************************************************************************************************/
void _fl_write(U8 *buf)
{
  __fl_EraseLastSector();      /* Erase Last Sector  */
  __fl_ProgramLastSector(buf); /* Program Last Sector */
}


/**************************************************************************************************
 Function:       Erase the Last Sector of the Flash Memory
 Description:    Erase Sector in Flash Memory
 Parameter:      none
 Return Value:   0 - OK,  1 - Failed
**************************************************************************************************/
int __fl_EraseLastSector (void) 
{
  iap_entry = (IAP) IAP_LOCATION;

  /* Command 0=Actual Command, Command[1]= param 0, Command[2] = param 1 */
  command[0] = 50;                  /* prepare the sector for write operation */
  command[1] = 27;					/* Starting Sector */
  command[2] = 27;					/* Ending Sector */

  DISABLE_INTERRUPTS;
  iap_entry(command, result);       /* call the IAP routine. Execute the actual Command */
  ENABLE_INTERRUPTS;
            
  command[0] = 52;					/* IAP Command : Erase Sector */
  command[3] = (CCLK/1000);         /* Param 3 = system clock frequency (CCLK) in kHz */
  
  DISABLE_INTERRUPTS;
  iap_entry(command, result); 		/* Call the IAP Routine, Execute the actual Command */
  ENABLE_INTERRUPTS;

  return ((int)result);            /* Finished: return results */
}


/***************************************************************************************************
 Function:    __fl_ProgramLastSector()
 Description: Program Last Sector of Flash Memory with the buffer mentioned
 Parameters:  Buffer Pointer
 Return Value:0 - OK,  1 - Failed
***************************************************************************************************/
int __fl_ProgramLastSector (U8 *buf) 
{
  iap_entry = (IAP) IAP_LOCATION;  /* Create a pointer to the IAP Location */
  
  command[0] = 50;                /* prepare the sector for write operation */
  command[1] = 27;                /* Starting Sector */
  command[2] = 27;                /* Ending Sector 27: Last sector in this case */
                      
  DISABLE_INTERRUPTS;
  iap_entry(command, result);     /* call the IAP routine. Execute the actual Command */
  ENABLE_INTERRUPTS;

  command[0] = 51;				  /* Copy RAM to Flash Command */
  command[1] = (U32)0x0007D000;   /* flash destination start address (256 byte boundard) */
  command[2] = (U32)buf;          /* Source Ram Addr, Always the ROW backup buffer(word aligned)*/
  command[3] = 4096;              /* Size to write allowed: 256 | 512 | 1024 | 4096 */
  command[4] = (CCLK/1000);       /* system clock frequency (CCLK) in kHz */
  
  DISABLE_INTERRUPTS;
  iap_entry(command, result);     /* call the IAP routine. Execute the actual Command */
  ENABLE_INTERRUPTS;

  return ((int)result);           /* Finished: return results */
}
