/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

/* TFTP callback functions for flash.
   Modify as needed for your own applications.
*/

#include "micronet.h"

#if (MN_TFTP && MN_TFTP_USE_FLASH)

#include "tftpflsh.h"

/************************************************************************/

/* This function is called before the first TFTP packet is received. Perform
   any operations necessary to set up the flash for writing here.

   Return positive number on success, return negative number on error.
*/
SCHAR mn_tftp_flash_start(byte *filename)
cmx_reentrant {
   SCHAR retval;

   retval = 1;

   return (retval);
}


/* This function is called after each TFTP data packet is received. Perform
   any operations necessary to write to the flash. 512 bytes, or less on the
   final packet, will be received at a time. If the sector size of the flash
   is not equal to 512 then you may need to copy some or all of the data to
   a temporary buffer.
   
   On error do any cleanup here or call mn_tftp_flash_end yourself.

   Return positive number on success, return negative number on error.
*/
SCHAR mn_tftp_flash_write(byte * buffer, word16 bufsize)
cmx_reentrant {
   SCHAR retval;

   retval = 1;

   return (retval);
}


/* This function is called after the last TFTP packet is received. Perform
   any operations necessary to clean up the flash. This function is not
   called if mn_tftp_flash_start or mn_tftp_flash_write return an error.
*/
void mn_tftp_flash_end(void)
cmx_reentrant {
}

#endif      /* (MN_TFTP && MN_TFTP_USE_FLASH) */
