/*------------------------------------------------------------------------ 
					post_system.c

Description:	Return SSIs for network.htm page
				1. DHCP Flag Enabled/Disabled
                2. Autonegotiation Enabled/Disabled
				3. 10 Mbps Enabled / Disabled
				4. 100 Mbps Enabled / Disabled
				5. Full Duplex Enabled/Disabled
                6. Half Duplex Enabled/Disabled

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/

#include "micronet.h"
#include "np_includes.h"

#define BUFFLEN 2048

/*------------------------------------------------------------------------
 Function:    post_defaults()
 Description: POST FUNCTION: Restore Defaults
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_rdef(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  flash_write_default();                              /* Write Defaults */
  
  mn_http_set_file(socket_ptr, mn_vf_get_entry("defaults.htm"));
}


/*------------------------------------------------------------------------
 Function:    post_reboot()
 Description: POST FUNCTION: Reboots the Device
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void post_reboot(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
  flag.reboot = TRUE;
  mn_http_set_file(socket_ptr, mn_vf_get_entry("rebooting.htm"));
}


/*------------------------------------------------------------------------
 Function:    upgrade_func()
 Description: POST FUNCTION: Upgrade New Firmware
 Input:       Socket Pointer 
 Output:      None
------------------------------------------------------------------------*/
void upgrade_func(PSOCKET_INFO socket_ptr)
cmx_reentrant
{
//  byte buff[BUFFLEN];
//  int length;
//
//  length = 0;
//
//  //length = mn_tftp_get_file(socket_ptr->ip_dest_addr,"portlcd.axf",buff,\
//  //  BUFFLEN,eth_interface_no);
//
//  if(length < 0)
//  {
//    printf("Error Getting Data\n\r");
//  }
//  else
//  {
//    printf("TFTP Get Successful\r\n");
//  }
//
//  flag.upgrade = TRUE;                              /* Set Upgrade Flag */
//  write_all();                                        /* Write to Flash */
 mn_http_set_file(socket_ptr, mn_vf_get_entry("system.htm")); 
}

