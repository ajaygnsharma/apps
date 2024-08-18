/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if (MN_TFTP)

#if (MN_TFTP_USE_FLASH)
/* include header file for tftp callback functions */
#include "tftpflsh.h"
#endif      /* #if (MN_TFTP_USE_FLASH) */

/* To reduce code space, we make some variables global to this code only.
   Without making these locally global, they'd have to be passed to functions
   consuming a lot of code space.
*/

static word16     tftp_block_n;
static word16     tftp_client_tid;
static PSOCKET_INFO tftp_srv_p;
static SCHAR       tftp_srv_n;
static byte tftp_pkt[TFTP_PACKET_SIZE];

/* Prototypes for non-exported functions */
static int tftp_xchg_packet(byte *, word16) cmx_reentrant;
static int tftp_send_error(byte *, word16) cmx_reentrant;
static int makeRequestString(byte *,byte *) cmx_reentrant;

static cmx_const byte octet_text[] = "octet";

/************************************************************************
* Function  : tftp_get_file
* Input     : Address of tftp server
*             File name pointer of file to retrieve from server
*             Pointer to where file should be stored
*             Size of file storage memory
* Output    : Zero or positive: Good: Number of bytes downloaded
*             Negative: Failure:
*
* Description  : TFTP expects a file name to be supplied.  This file is then
*                requested from the ftp server.  The file is downloaded into
*             the region of memory specified in parameter list.  The file
*             will not be allowed to grow past the amount of bytes specified
*             in the parameter list.
*
*             If MN_TFTP_USE_FLASH is set to 1 the data received is passed to a
*             tftp callback function which writes the data to flash. It is
*             up to the user to modify the tftp callback functions to work
*             with their flash. The buffer and bufsize parameters are not
*             used.
*************************************************************************/

long mn_tftp_get_file(byte *dest_ip_addr, byte *fn, byte *buffer, \
   long bufsize, SCHAR interface_no)
cmx_reentrant {
   long retVal;
   int packetSize;
   word16 tftp_cur_blk;
   word16 data_len;
#if (!MN_TFTP_USE_FLASH)
   word16 packetIndex;
#endif      /* (!MN_TFTP_USE_FLASH) */

   /* Steps for processing tftp as a client:

      Assumption: Assume mn_init() has been previously called
      Assumption: Assume mn_timer() has been previously called
      Assumption: Assume srand() has been previously called

      1. Dream up TID
      2. Open UDP socket
      3. Client sends RRQ for filename fn
      4. Receive first data packet from server
         a. If timeout, inc retry counter.  If less than 3, go to step 3
         b. If timeout counter is 3, abort.  Return error.
         c. If first block client TID is not ours, reject.
         d. If first block block_num not 1, abort. out of sync.
      5. Keep store of server's TID from first block.
      6. Keep store of data from first block, send ACK to server.
      7. Read n data packet from server
         a. If timeout, inc retry counter.  If less than 3, go to step 7
         b. If timeout counter is 3, abort.  Return error.
         c. If n block client TID is not ours, reject.
         d. If n block block_num = our expected block_num, send ACK, go to step
         e. If n block block_num < our expected block_num, reject.
            i Inc low block counter by 1.  If less than 3, go to step 7.
            ii If low block counter 3, abort.  Return error.
         f. If n block block_num > our expected block_num, abort.  out of sync.
      8. Store data from n block, send ACK to server.
      9. If n block data size < 512, this was the last block, exit.
      10. If total block count > max data size, send error packet, return error.

      If successful, file will be stored at location specified by function
      parameter. Code outside of this function can then execute downloaded
      file.


      Not all compilers have rand and srand functions, so rand is replaced by
      mn_get_timer_tick.
   */

   /* Create random TID for ourself. A TID is a communications port for which
      all data will be sent to us.  The port is randomized in order to reduce
      the probability it matches the servers,
      69 (tftp initial server TID), 255 (bootp), or 0 (reserved).
   */

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

#if (MN_TFTP_USE_FLASH)
   buffer  = buffer;
   bufsize = bufsize;
   if (fn && *fn && dest_ip_addr && *dest_ip_addr)
#else
   if (fn && *fn && dest_ip_addr && *dest_ip_addr && buffer && (bufsize > 0))
#endif      /* #if (MN_TFTP_USE_FLASH) */
   {
      MN_MUTEX_WAIT(MUTEX_TFTP,INFINITE_WAIT);
      tftp_block_n = 1;
      retVal = 0;
      MEMSET(tftp_pkt, 0, sizeof(tftp_pkt));

      /* Make it a huge TID.  Better if over 1024 */
      tftp_client_tid = (MN_GET_TICK & 0x00ff) + 1025;

      /* Open UDP socket */
      tftp_srv_n = mn_open(dest_ip_addr, tftp_client_tid,
         TFTP_SERVER_INIT_TID, PASSIVE_OPEN, PROTO_UDP, STD_TYPE|TFTP_TYPE,
         tftp_pkt, TFTP_PACKET_SIZE, interface_no);
      if (tftp_srv_n >= 0)
      {
#if (MN_TFTP_USE_FLASH)
         /* Callback to set up flash if necessary. */
         if (mn_tftp_flash_start(fn) < 0)
            return (TFTP_FAILED);
#endif      /* #if (MN_TFTP_USE_FLASH) */

         /* Socket open.  Get pointer to socket. */
         tftp_srv_p = MK_SOCKET_PTR(tftp_srv_n);

         /* Prepare packet to send RRQ for file name */
         tftp_pkt[OFS_OPCODE0] = HIGHBYTE(TFTP_RRQ);
         tftp_pkt[OFS_OPCODE1] = LOWBYTE(TFTP_RRQ);

         /* Setup request string. */
         packetSize = makeRequestString(&tftp_pkt[OFS_DATA0],fn);

         /* Send request packet to kick things off */
         packetSize = tftp_xchg_packet(tftp_pkt, (packetSize+2));
         if (packetSize > TFTP_HEADER_SIZE)
         {
            /* We go into this loop initially with block #1. */

            while (TRUE)
            {
               tftp_cur_blk = MK_WORD16(tftp_pkt[OFS_BLOCK0],tftp_pkt[OFS_BLOCK1]);

               if (tftp_block_n == tftp_cur_blk)
               {
                  /* Correct block, save data. */

                  data_len = (word16)(packetSize - TFTP_HEADER_SIZE);
#if (MN_TFTP_USE_FLASH)
                  /* Callback to write to flash. */
                  if (mn_tftp_flash_write(&tftp_pkt[OFS_DATA2],data_len) < 0)
                  {
                     (void)tftp_send_error(tftp_pkt, TFTP_ERR_OUT_OF_SPACE);
                     (void)mn_abort(tftp_srv_n);
                     return (TFTP_FILE_TOO_BIG);
                  }
                  else
                     retVal += data_len;
#else
                  for (packetIndex=0; packetIndex < data_len; packetIndex++)
                  {
                     *buffer++ = tftp_pkt[OFS_DATA2+packetIndex];
                     retVal++;
                     if (retVal > bufsize)
                     {
                        /* Error, we've exceeded maximum download bytes.
                           Abort.
                        */

                        (void)tftp_send_error(tftp_pkt, TFTP_ERR_OUT_OF_SPACE);
                        (void)mn_abort(tftp_srv_n);
                        return (TFTP_FILE_TOO_BIG);
                     }
                  }
#endif      /* #if (MN_TFTP_USE_FLASH) */
               }

               tftp_pkt[OFS_OPCODE0] = HIGHBYTE(TFTP_ACK);
               tftp_pkt[OFS_OPCODE1] = LOWBYTE(TFTP_ACK);

               if (packetSize == TFTP_PACKET_SIZE)
               {
                  /* Send ack, get new packet. */

                  tftp_block_n++;
                  packetSize = tftp_xchg_packet(tftp_pkt, TFTP_HEADER_SIZE);
                  if (packetSize < 0)
                  {
                     retVal = packetSize; /* Keep error code to return */
                     break;
                  }
               } else   {

                  /* Getting here means our last packet was smaller
                     than the max size, which indicates the last packet
                     was received. Send ack and get ready to exit.
                  */

                  tftp_srv_p->send_ptr = tftp_pkt;
                  tftp_srv_p->send_len = TFTP_HEADER_SIZE;

                  packetSize = (int)mn_udp_send(tftp_srv_p);
                  if (packetSize < 0)
                     retVal = packetSize; /* Keep error code to return */

                  break;
               }
            }
         } else
            retVal = packetSize; /* error from first call to tftp_xchg_packet */

#if (MN_TFTP_USE_FLASH)
         /* Callback to wrap up flashing if necessary. */
         mn_tftp_flash_end();
#endif      /* #if (MN_TFTP_USE_FLASH) */

         (void)mn_abort(tftp_srv_n);
      } else
         retVal = tftp_srv_n; /* Error from mn_open */

      MN_MUTEX_RELEASE(MUTEX_TFTP);
   } else
      retVal = TFTP_NO_FILE_SPECIFIED; /* Error from parameter check */

   return retVal;
}

/************************************************************************
* Function  : tftp_xchg_packet
* Input     : I/O buffer
* Output    : Zero - Good
*             Zero or positive: Good: Number of bytes downloaded
*             Negative: Failure:
*
* Description  : Transmits input tftp packet to server and waits for reply.
************************************************************************/

static int tftp_xchg_packet(byte * pkt, word16 outp_size)
cmx_reentrant {
   int retry;
   int retVal;
   word16 opcode;
   word16 blk_or_err;
   word16 errCode;

   /*    Method of tranceiving:

      1. Transmit input packet
      2. Read data packet from server
         a. If timeout, inc retry counter.  If less than 3, go to step 2
         b. If timeout counter is 3, abort.  Return error.
         c. If client TID is not ours, reject.
         d. If block_num = our expected block_num, send ACK, exit.
         e. If block_num < our expected block_num, reject.
            i Inc low block counter by 1.  If less than 3, go to step 2.
            ii If low block counter 3, abort.  Return error.
         f. If block_num > our expected block_num, abort.  out of sync.
   */

   /* We may just went to send out a LAST ack, but not receive anything back.
      In this case, set pkt to null.
   */

   retry = 0;
   tftp_srv_p->send_ptr = pkt;
   tftp_srv_p->send_len = outp_size;

   retVal = (int)mn_udp_send(tftp_srv_p);
   if (retVal >= 0)
      {
      /* Begin read */
      do
         {
         retVal = (int)mn_recv(tftp_srv_n, pkt, TFTP_PACKET_SIZE);

         errCode = 0; /* Go in optimistically  */

         /* The first two bytes are the opcode. */
         opcode = LSHIFT8(*pkt);
         opcode += (word16)(*(pkt+1));

         /* The next two bytes are either the block number or error code */
         blk_or_err = LSHIFT8(*(pkt+2));
         blk_or_err += (word16)(*(pkt+3));

         if ((retVal < 0) || (blk_or_err != tftp_block_n))
            {
            errCode = TFTP_ERR_ND;
            retry++;             /* Retries allowable */
            }
         else if (tftp_srv_p->src_port != tftp_client_tid)
            {
            errCode = TFTP_ERR_UNKNOWN_TID;
            break;               /* Fatal error, no retries */
            }
         else if (opcode == TFTP_ERROR)
            {
            switch (blk_or_err)
               {
               case TFTP_ERR_FILENOTFOUND:
                  retVal = TFTP_FILE_NOT_FOUND;
                  break;
               default:
                  retVal = TFTP_FAILED;
                  break;
               }
            break;               /* Fatal error, no retries */
            }
         else if (opcode == TFTP_OPT_ACK)
            {
            /* We didn't request any options, so all option acks are bad.
               If we add options in future releases the code to check the
               option acks will go here.
            */
            errCode = TFTP_UNREQUESTED_OPTION;
            break;               /* Fatal error, no retries */
            }
         else if (opcode != TFTP_DATA)
            {
            errCode = TFTP_ERR_ILL_TFTP_OP;
            break;               /* Fatal error, no retries */
            }
         else
            break;               /* Break out of loop, everything is worked fine. */

         }
      while (retry < MN_TFTP_RESEND_TRYS);

      if (errCode)
         {
         (void)tftp_send_error(pkt, errCode);
         retVal = TFTP_FAILED;
         }
      }

   return retVal;
}

/************************************************************************
* Function     : tftp_send_error
* Input     : Tftp packet buffer
*             Tftp Error Code
* Output       : Zero - Good
*                Zero or positive: Good: Number of bytes downloaded
*             Negative: Failure:
*
* Description  : Transmits error packet to server.
************************************************************************/

static int tftp_send_error(byte * pkt, word16 tftp_err)
cmx_reentrant {

   *pkt     = HIGHBYTE(TFTP_ERROR);
   *(pkt+1) = LOWBYTE(TFTP_ERROR);

   *(pkt+2) = HIGHBYTE(tftp_err);
   *(pkt+3) = LOWBYTE(tftp_err);

   tftp_srv_p->send_ptr = pkt;
   tftp_srv_p->send_len = TFTP_HEADER_SIZE;
   return ((int)mn_udp_send(tftp_srv_p));
}

static int makeRequestString(byte *dest, byte *fn)
cmx_reentrant {
   int len;
   int i;

   len = strlen( (char *)fn );
   for (i=0;i<len;i++)
      *(dest+i) = *(fn+i);

   *(dest+len) = '\0';
   dest = dest + len + 1;

   (void)mn_strcpy_cb(dest, octet_text);
   len += 7;

/* Currently we are requesting no options. If we request options in
   future releases the code to do that would go here.
*/

   return (len);
}
#endif      /* MN_TFTP */


