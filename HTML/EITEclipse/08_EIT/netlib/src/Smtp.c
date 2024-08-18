/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
*********************************************************/

#include "micronet.h"

#if (MN_SMTP)
/* Note: for the first version there are a few limitations:
   There are no cc: or bcc: fields.
   There are no signatures.

   The origination date is hardcoded (see HEADER_DATE[] below) if
   SNTP is not included. If SNTP code is included then SNTP will
   be used to get the date and time.

   Only one recipient allowed for each call to mn_smtp_send_mail.
   Only one attachment allowed for each call to mn_smtp_send_mail.
   The attachment must be in us-ascii format.

   Messages and attachments are expected to conform with the RFC2822
   requirement of no more than 998 characters before a <CR><LF> pair.
   This is not checked by the functions below.

   Uses functions in support.c.
*/

/* variables */

static byte smtp_state;
byte SMTPBuffer[MN_SMTP_BUFFER_LEN];     /* holds smtp commands */
static int smtp_send_size;

/* local functions */
static int smtp_send_cmd(SCHAR, PCONST_BYTE) cmx_reentrant;
static int smtp_wait_reply(SCHAR) cmx_reentrant;
static void smtp_load_buff(SCHAR, byte *) cmx_reentrant;
static void smtp_load_buff_cb(SCHAR, PCONST_BYTE) cmx_reentrant;
static int smtp_empty_buff(SCHAR socket_no) cmx_reentrant;
static int smtp_send_body(SCHAR, byte *) cmx_reentrant;
static void set_send(PSOCKET_INFO, byte *) cmx_reentrant;
#if (MN_SMTP_AUTH)
static int smtp_auth_login(SCHAR, char *, char *) cmx_reentrant;
static void base64_group(char [], byte, byte *) cmx_reentrant;
static int mn_base64_encode(char *, word16, byte *, word16) cmx_reentrant;
#endif

/* constant strings */
#if (MN_SMTP_AUTH)
static cmx_const byte CMD_HELO_1[]     = "EHLO [";
#else
static cmx_const byte CMD_HELO_1[]     = "HELO [";
#endif
static cmx_const byte CMD_HELO_2[]     = "]\r\n";
static cmx_const byte CMD_QUIT[]       = "QUIT\r\n";
static cmx_const byte CMD_MAIL_1[]     = "MAIL FROM:<";
static cmx_const byte CMD_MAIL_2[]     = ">\r\n";
static cmx_const byte CMD_RCPT_1[]     = "RCPT TO:<";
static cmx_const byte CMD_DATA_START[] = "DATA\r\n";
static cmx_const byte CMD_DATA_END[]   = "\r\n.\r\n";
#if (MN_SMTP_AUTH)
static cmx_const byte CMD_RSET[]       = "RSET\r\n";
static cmx_const byte CMD_AUTH[]       = "AUTH LOGIN\r\n";
static cmx_const byte CMD_CR[]         = "\r\n";
/* BASE64 encoding array */
static cmx_const byte base64[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#endif

#if (MN_SNTP)
static cmx_const byte HEADER_DATE[]    = "Date: ";
#else
static cmx_const byte HEADER_DATE[]    = "Date: Sun, 01 Jun 2008 00:00:00 -0500";
#endif
static cmx_const byte HEADER_TO[]      = "\r\nTo: ";
static cmx_const byte HEADER_FROM[]    = "\r\nFrom: ";
static cmx_const byte HEADER_SUBJ[]    = "\r\nSubject: ";
static cmx_const byte HEADER_MIME[]    = "\r\nMime-Version: 1.0\r\n";
static cmx_const byte HEADER_TYPE[]    = "Content-Type: text/plain; charset=us-ascii\r\n";
static cmx_const byte HEADER_END[]     = "\r\n";

static cmx_const byte HEADER_DISP[]    = "Content-Disposition: attachment; filename=\"";
static cmx_const byte HEADER_MIX[]     = "Content-Type: multipart/mixed; boundary=\"";
static cmx_const byte HEADER_MIX_END[] = "\"\r\n\r\n";
static cmx_const byte BOUNDARY[]       = "_s1m9e5m8m_";
static cmx_const byte BOUNDARY_END[]   = "--";

/********************************************************************/

/* initialize smtp state machine, called from mn_init in socket.c */
void mn_smtp_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_SMTP,INFINITE_WAIT);
   smtp_state = SMTP_CLOSED;
   MEMSET(SMTPBuffer, 0, MN_SMTP_BUFFER_LEN);
   MN_MUTEX_RELEASE(MUTEX_SMTP);
}

/* open a smtp socket, establish connection and say HELO. returns socket
   number of smtp socket opened if successful, negative number on error.

   Input: source port number to be used by the smtp socket.
*/
SCHAR mn_smtp_start_session(word16 port_no, SCHAR interface_no, char *user, char *pwd)
cmx_reentrant {
   SCHAR socket_no;
   int retval;
   int reply_code;
   byte *temp_ptr;
   PINTERFACE_INFO interface_ptr;

#if (!(MN_SMTP_AUTH))
   user=user;
   pwd=pwd;
#endif

   if ((interface_no < 0) || (interface_no >= MN_NUM_INTERFACES))
      return (INTERFACE_NOT_FOUND);

   interface_ptr = MK_INTERFACE_PTR(interface_no);

   retval = NOT_SUPPORTED;

   if (port_no >= DEFAULT_PORT)  /* port_no must be 1025 or more */
      {
      MN_MUTEX_WAIT(MUTEX_SMTP,INFINITE_WAIT);
      if (smtp_state == SMTP_CLOSED)
         {
         smtp_state = SMTP_OPEN;

         /* open a smtp socket with a TCP active open */
         socket_no = mn_open(ip_smtp_addr,port_no,SMTP_PORT,ACTIVE_OPEN,PROTO_TCP,\
                                   SMTP_TYPE,0,0,interface_no);

         if (socket_no >= 0)  /* successful open */
            {
            /* wait for opening message from SMTP server */
            reply_code = smtp_wait_reply(socket_no);

            switch (reply_code)
               {
               case SMTP_START_OK:
                  smtp_state = SMTP_HELO;

                  /* send a command like: HELO [192.6.94.2]<CRLF> */
                  (void)mn_strcpy_cb(SMTPBuffer, CMD_HELO_1);
                  (void)mn_getMyIPAddr_func(&temp_ptr,interface_ptr);
                  /* SMTPBuffer+6 because we added 6 chars above */
                  (void)strcpy((char *)(SMTPBuffer+6),(char *)temp_ptr);
                  (void)mn_strcat_cb(SMTPBuffer, CMD_HELO_2);

                  retval = smtp_send_cmd(socket_no, PTR_NULL);
                  if (retval > 0)
                     reply_code = smtp_wait_reply(socket_no);
                  else
                     reply_code = 0;

#if (MN_SMTP_AUTH)
                  if ((reply_code == SMTP_OK) && (user != PTR_NULL) && (pwd != PTR_NULL))
                     {
                     smtp_state = SMTP_LOGIN;
                     if (smtp_auth_login(socket_no, user, pwd) < 0)
                        {
                        /* If AUTH LOGIN fails reset and try without
                           authentication. If the reset command fails
                           then drop through to SMTP_FAILURE.
                        */
                        (void)mn_strcpy_cb(SMTPBuffer, CMD_RSET);
                        retval = smtp_send_cmd(socket_no, PTR_NULL);
                        if (retval > 0)
                           reply_code = smtp_wait_reply(socket_no);
                        else
                           reply_code = 0;
                        }
                     }
#endif

                  /* if reply_code not ok let drop thru to SMTP_FAILURE */
                  if (reply_code == SMTP_OK)
                     {
                     smtp_state = SMTP_MAIL;
                     retval = (int)socket_no;
                     break;
                     }
               case SMTP_FAILURE:
                  MN_MUTEX_RELEASE(MUTEX_SMTP);
                  mn_smtp_end_session(socket_no);
                  retval = SMTP_OPEN_FAILED;
                  break;
               default:    /* no reply or invalid reply */
                  smtp_state = SMTP_CLOSED;
                  if (reply_code == 0)
                     retval = SMTP_OPEN_FAILED;
                  else
                     retval = reply_code;
                  (void)mn_close(socket_no);
                  break;
               }
            }
         else                       /* couldn't open socket, mn_open failed */
            {
            smtp_state = SMTP_CLOSED;
            retval = (int)socket_no;      /* save error status */
            }
         }
      else                          /* smtp_state was not SMTP_CLOSED */
         retval = SMTP_ALREADY_OPEN;

      MN_MUTEX_RELEASE(MUTEX_SMTP);
      }

   return ((SCHAR)retval);
}

/* sends a QUIT command and closes the socket.

   Input: socket number obtained from mn_smtp_start_session
*/
void mn_smtp_end_session(SCHAR socket_no)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_SMTP,INFINITE_WAIT);
   if (smtp_state != SMTP_CLOSED)
      {
      smtp_state = SMTP_QUIT;
      if ((smtp_send_cmd(socket_no, CMD_QUIT) <= 0) || (smtp_wait_reply(socket_no) < 0))
         mn_abort(socket_no);    /* some sort of error occurred */
      else
         {
         /* smtp server should initiate the close after sending a 221 reply. */
         (void)mn_recv(socket_no,0,0);
         (void)mn_close(socket_no);
         }
      smtp_state = SMTP_CLOSED;
      }
   MN_MUTEX_RELEASE(MUTEX_SMTP);
}

#if (MN_SNTP)
#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60))
static word32 seconds_1900;
static char email_date[EMAIL_DATE_LEN];
#endif
#endif

/* sends mail and any attachment to one recipient. returns positive number
   if successful or negative number on error. this function assumes the
   pmail->to and pmail-from variables point to valid email addresses and
   that all strings are null terminated.

   Input: socket number obtained from mn_smtp_start_session
         pointer to a SMTP_INFO_T structure filled in with the proper data.
*/
int mn_smtp_send_mail(SCHAR socket_no, PSMTP_INFO pmail)
cmx_reentrant {
   int retval;
#if (MN_SNTP)
#if (!(defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60)))
   word32 seconds_1900;
   char email_date[EMAIL_DATE_LEN];
#endif
#endif

   retval = 0;
   MN_MUTEX_WAIT(MUTEX_SMTP,INFINITE_WAIT);

   if (smtp_state != SMTP_MAIL)
      retval = SMTP_NOT_OPEN;
   else if (pmail->to && pmail->from && (pmail->message || pmail->attachment))
      {
      smtp_send_size = 0;

#if (MN_SNTP)
      email_date[0] = '\0';

      /* Look up date/time from time server IP address. */
      if (mn_sntp(pmail->ip_sntp_addr, \
            sock_info[socket_no].interface_no, &seconds_1900) > 0)
         {
         /* Convert seconds since 1900 into date/time.
            e.g. Sat, 01 Jan 2000 23:59:59 -0500
         */
         mn_get_email_date(email_date, seconds_1900, \
            pmail->utc_offset, pmail->daylight_saving);
         }
      else
         {
         MN_MUTEX_RELEASE(MUTEX_SMTP);
         return (SNTP_ERROR);
         }
#endif

      /* loop until we are done or we get an error */
      while (1)
         {
         /* if we get an error in this state we need to continue to the
            next state.
         */
         if (smtp_state == SMTP_DATA_2)
            {
            SMTPBuffer[0] = '\0';
            smtp_load_buff_cb(socket_no, HEADER_DATE);
#if (MN_SNTP)
            smtp_load_buff(socket_no, (byte *)email_date);
#endif
            smtp_load_buff_cb(socket_no, HEADER_TO);
            smtp_load_buff(socket_no, pmail->to);
            smtp_load_buff_cb(socket_no, HEADER_FROM);
            smtp_load_buff(socket_no, pmail->from);
            smtp_load_buff_cb(socket_no, HEADER_SUBJ);
            smtp_load_buff(socket_no, pmail->subject);
            smtp_load_buff_cb(socket_no, HEADER_MIME);
            if (pmail->attachment != PTR_NULL)
               {
               /* additional lines for attachment */
               smtp_load_buff_cb(socket_no, HEADER_MIX);
               smtp_load_buff_cb(socket_no, BOUNDARY);
               smtp_load_buff_cb(socket_no, HEADER_MIX_END);
               /* additional header lines for message needed for multi-part */
               smtp_load_buff_cb(socket_no, BOUNDARY_END);
               smtp_load_buff_cb(socket_no, BOUNDARY);
               smtp_load_buff_cb(socket_no, HEADER_END);
               }

            smtp_load_buff_cb(socket_no, HEADER_TYPE);
            smtp_load_buff_cb(socket_no, HEADER_END);

            /* send message header if no errors so far */
            retval = smtp_empty_buff(socket_no);

            /* send message body if there is one */
            if (retval > 0)
               if (pmail->message != PTR_NULL)
                  retval = smtp_send_body(socket_no, pmail->message);

            if (retval <= 0)
               {
               smtp_state = SMTP_DATA_3;
               continue;
               }

            if (pmail->attachment != PTR_NULL)
               {
               /* send attachment header lines first */
               smtp_load_buff_cb(socket_no, HEADER_END);
               smtp_load_buff_cb(socket_no, BOUNDARY_END);
               smtp_load_buff_cb(socket_no, BOUNDARY);
               smtp_load_buff_cb(socket_no, HEADER_END);
               smtp_load_buff_cb(socket_no, HEADER_TYPE);
               smtp_load_buff_cb(socket_no, HEADER_DISP);
               smtp_load_buff(socket_no, pmail->filename);
               smtp_load_buff_cb(socket_no, HEADER_MIX_END);
               retval = smtp_empty_buff(socket_no);
               if (retval > 0)
                  {
                  /* send attachment */
                  retval = smtp_send_body(socket_no,pmail->attachment);
                  if (retval >= 0)
                     {
                     /* send closing boundary */
                     smtp_load_buff_cb(socket_no, HEADER_END);
                     smtp_load_buff_cb(socket_no, BOUNDARY_END);
                     smtp_load_buff_cb(socket_no, BOUNDARY);
                     smtp_load_buff_cb(socket_no, BOUNDARY_END);
                     smtp_load_buff_cb(socket_no, HEADER_END);
                     (void)smtp_empty_buff(socket_no);
                     }
                  }
               }

            /* finished with email data */
            smtp_state = SMTP_DATA_3;
            }
         else     /* handle other smtp states */
            {
            /* prepare appropriate command */
            switch (smtp_state)
               {
               case SMTP_MAIL:
                  mn_strcpy_cb(SMTPBuffer, CMD_MAIL_1);
                  (void)strcat((char *)SMTPBuffer, (char *)pmail->from);
                  (void)mn_strcat_cb(SMTPBuffer, CMD_MAIL_2);
                  break;
               case SMTP_RCPT:
                  mn_strcpy_cb(SMTPBuffer, CMD_RCPT_1);
                  (void)strcat((char *)SMTPBuffer, (char *)pmail->to);
                  (void)mn_strcat_cb(SMTPBuffer, CMD_MAIL_2);
                  break;
               case SMTP_DATA_1:
                  mn_strcpy_cb(SMTPBuffer, CMD_DATA_START);
                  break;
               case SMTP_DATA_3:
                  mn_strcpy_cb(SMTPBuffer, CMD_DATA_END);
                  break;
               default:             /* should never get here... */
                  /* return without resetting smtp_state */
                  MN_MUTEX_RELEASE(MUTEX_SMTP);
                  return (SMTP_ERROR);
                  /* break; */
               }

            /* send the command and wait for the reply */
            retval = smtp_send_cmd(socket_no, PTR_NULL);
            if (retval > 0)
               retval = smtp_wait_reply(socket_no);

            /* process the reply */
            if ((retval == SMTP_OK) || ((retval == SMTP_START_SENDING_DATA) && (smtp_state == SMTP_DATA_1)))
               smtp_state++;
            else
               retval = SMTP_ERROR;
            }

         /* check if we are done or we got an error */
         if ((smtp_state == SMTP_QUIT) || (retval == SMTP_ERROR))
            break;
         }                       /* end of while (1) */

      smtp_state = SMTP_MAIL;    /* reset state */
      }
   else
      retval = SMTP_BAD_PARAM_ERR;

   MN_MUTEX_RELEASE(MUTEX_SMTP);
   return (retval);
}

/********************************************************************/

/* sends a command out the passed socket number. if cmd is null it is
   assumed that SMTPBuffer was setup properly before calling this function.
   returns the result of mn_tcp_send().
*/
static int smtp_send_cmd(SCHAR socket_no, PCONST_BYTE cmd)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;

   socket_ptr = MK_SOCKET_PTR(socket_no);
   if (socket_ptr->tcp_unacked_bytes)
      {
      (void)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
      if (socket_ptr->tcp_unacked_bytes)
         return (0);
      }

   if (cmd != PTR_NULL)
      mn_strcpy_cb(SMTPBuffer, cmd);
   set_send(socket_ptr, SMTPBuffer);

   return ((int)mn_tcp_send(socket_ptr));
}

/* wait for a reply to a command we sent and convert the command into an
   integer. returns the integer if successful, or negative number on error.
   This function assumes that the SMTPBuffer will not be used for sending at
   the same time we are waiting for a reply.
*/
static int smtp_wait_reply(SCHAR socket_no)
cmx_reentrant {
   long retval;
   word16 reply_code;
   int i;

   reply_code = 0;
   SMTPBuffer[0] = '\0';
   SMTPBuffer[1] = '\0';
   SMTPBuffer[2] = '\0';

   do
      {
      retval = mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
      if ((retval == SOCKET_TIMED_OUT) || (retval == NEED_TO_EXIT) || \
            (retval == TCP_NO_CONNECT))
         break;

      /* The first three characters are the reply code.
         We expect all replies to end in a <CR><LF>. For small smtp
         buffers it may take several packets before we get a <CR><LF>.
      */
      if ((retval > 0) || (retval == NEED_TO_LISTEN))
         {
         if (!reply_code)
            {
            reply_code = mn_get_reply_code(SMTPBuffer);
            mn_do_multi_line(socket_no, reply_code, SMTPBuffer, \
               MN_SMTP_BUFFER_LEN, MN_SOCKET_WAIT_TICKS);
            }

         /* check if packet ends in a <CR><LF>. If not, we need to get
            another packet. If we don't have a reply_code yet we also
            need to get another packet.
         */
         retval = 0;
         if (reply_code)
            {
            for (i=SMTP_RECV_LEN;i<(MN_SMTP_BUFFER_LEN-1);i++)
               {
               if ((SMTPBuffer[i] == '\r') && (SMTPBuffer[i+1] == '\n'))
                  retval = (long)reply_code;
               }
            }
         }
      }
   while (retval <= 0);

   return (reply_code);
}

/* loads the send buffer and sends the contents to the server if neccessary.
   called by mn_smtp_send_mail. needed in case MicroNet is setup so that we
   can't send the whole message header at once.

   Input: socket number obtained from mn_smtp_start_session
         msg is a null-terminated string to add to buffer
*/
static void smtp_load_buff(SCHAR socket_no, byte *msg)
cmx_reentrant {
   word16 len;

   if (smtp_send_size >= 0)
      {
      len = strlen((char *)msg);
      /* if the new msg can't be added to the buffer, send the contents
         of the buffer and reset the buffer with the new msg.
      */
      if ( (smtp_send_size + len) > MN_SMTP_BUFFER_LEN )
         {
         if ( (mn_send(socket_no, SMTPBuffer, smtp_send_size)) > 0 )
            {
#if (MN_TCP_DELAYED_ACKS)
            (void)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
#endif
            /* good send */
            (void)strcpy((char *)SMTPBuffer, (char *)msg);
            smtp_send_size = len;
            }
         else
            {
            /* flag error sending */
            smtp_send_size = -1;
            }
         }
      else
         {
         /* just add the new stuff to the buffer */
         (void)strcat((char *)SMTPBuffer, (char *)msg);
         smtp_send_size = smtp_send_size + len;
         }
      }
}

/* same as above except takes a PCONST_BYTE msg */
static void smtp_load_buff_cb(SCHAR socket_no, PCONST_BYTE msg)
cmx_reentrant {
   word16 len;

   if (smtp_send_size >= 0)
      {
      len = mn_strlen_cb(msg);
      /* if the new msg can't be added to the buffer, send the contents
         of the buffer and reset the buffer with the new msg.
      */
      if ( (smtp_send_size + len) > MN_SMTP_BUFFER_LEN )
         {
         if ( (mn_send(socket_no, SMTPBuffer, smtp_send_size)) > 0 )
            {
#if (MN_TCP_DELAYED_ACKS)
            (void)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
#endif
            /* good send */
            (void)mn_strcpy_cb(SMTPBuffer, msg);
            smtp_send_size = len;
            }
         else
            {
            /* flag error sending */
            smtp_send_size = -1;
            }
         }
      else
         {
         /* just add the new stuff to the buffer */
         (void)mn_strcat_cb(SMTPBuffer, msg);
         smtp_send_size = smtp_send_size + len;
         }
      }
}

/* sends the contents of SMTPBuffer to the smtp server and resets the
   buffer. returns positive number if successful, negative number on error.
   Input: socket number obtained from mn_smtp_start_session
*/
static int smtp_empty_buff(SCHAR socket_no)
cmx_reentrant {
   int retval;

   retval = 0;

   /* send message header if no errors so far */
   if (smtp_send_size > 0)
      {
      retval = (int)mn_send(socket_no, SMTPBuffer, smtp_send_size);
#if (MN_TCP_DELAYED_ACKS)
      (void)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
#endif
      smtp_send_size = 0;
      SMTPBuffer[0] = '\0';
      }
   else if (smtp_send_size < 0)
      retval = SMTP_ERROR;

   return (retval);
}

/* sends the passed string to the server. the string will be either the
   message or attachment. returns positive number if successful,
   negative number on error. called by mn_smtp_send_mail.

   Input: socket number obtained from mn_smtp_start_session
         msg is a null-terminated string to send.
*/
static int smtp_send_body(SCHAR socket_no, byte *msg)
cmx_reentrant {
   int retval;
   PSOCKET_INFO socket_ptr;

   retval = 0;
   socket_ptr = MK_SOCKET_PTR(socket_no);
   if (socket_ptr->tcp_unacked_bytes)
      {
      (void)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
      if (socket_ptr->tcp_unacked_bytes)
         return (0);
      }
   set_send(socket_ptr, msg);

   /* loop until we are done sending or there is an error */
   while (socket_ptr->send_len && (retval >= 0))
      {
      retval = (int)mn_send(socket_no, 0, 0);
#if (MN_TCP_DELAYED_ACKS)
      if (socket_ptr->packet_count >= 2)
         retval = (int)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
#endif
      }

#if (MN_TCP_DELAYED_ACKS)
   if (socket_ptr->tcp_unacked_bytes)
      (void)mn_recv(socket_no,SMTPBuffer,MN_SMTP_BUFFER_LEN);
#endif

   return (retval);
}

static void set_send(PSOCKET_INFO socket_ptr, byte *msg)
cmx_reentrant {
   socket_ptr->send_ptr = msg;
   socket_ptr->send_len = strlen((char *)msg);
}

#if (MN_SMTP_AUTH)
/* Authenticate using the AUTH LOGIN mechanism.
   Returns negative number on error.
*/
static int smtp_auth_login(SCHAR socket_no, char *user, char *pwd)
cmx_reentrant {
   int retval;
   int reply_code;
   byte i;

   retval = 0;
   reply_code = 0;

   /* Sequence:

      Send AUTH LOGIN command.
      Wait for 334 reply (assume it is asking for username).
      Send username.
      Wait for 334 reply (assume it is asking for password).
      Send password.
      Wait for 235 reply.
   */

   for (i=0; i<3; i++)
      {
      MEMSET(SMTPBuffer, 0, MN_SMTP_BUFFER_LEN);

      if (i == 0)
         {
         retval = 1;
         (void)mn_strcpy_cb(SMTPBuffer, CMD_AUTH);
         }
      else if (i == 1)
         retval = mn_base64_encode(user, (word16)strlen(user), SMTPBuffer, (MN_SMTP_BUFFER_LEN-2));
      else if (i == 2)
         retval = mn_base64_encode(pwd, (word16)strlen(pwd), SMTPBuffer, (MN_SMTP_BUFFER_LEN-2));

      if (retval < 0)
         return (retval);

      if (i > 0)
         (void)mn_strcat_cb(SMTPBuffer, CMD_CR);

      retval = smtp_send_cmd(socket_no, PTR_NULL);
      if (retval > 0)
         reply_code = smtp_wait_reply(socket_no);
      else
         reply_code = 0;

      if (i < 2 && reply_code != 334)
         break;
      }


   if (reply_code == 235)
      return (1);
   else
      return (-1);
}

/* Encode a group of three characters. */
static void base64_group(char in_buff[], byte len, byte * out_ptr)
cmx_reentrant {
   *out_ptr = base64[(in_buff[0] >> 2)];
   *(out_ptr+1) = base64[((in_buff[0] & 0x3) << 4) | ((in_buff[1] & 0xF0) >> 4)];

   /* Len is the number of non-padding bytes. Must be 1 - 3. */
   if (len > 1)
      *(out_ptr+2) = base64[((in_buff[1] & 0x0F) << 2) | ((in_buff[2] & 0xC0) >> 6)];
   else
      *(out_ptr+2) = '=';

   if (len == 3)
      *(out_ptr+3) = base64[(in_buff[2] & 0x3F)];
   else
      *(out_ptr+3) = '=';
}

/* Encode the contents of in_buff to out_buff. */
static int mn_base64_encode(char * in_buff, word16 in_len, byte * out_buff, word16 out_len)
cmx_reentrant {
   word16 i, min_size;
   byte line_len, num_chars;
   char buff[3];

   /* Check parameters. */
   if (!in_len || (in_buff == PTR_NULL) || (out_buff == PTR_NULL))
      return (-1);

   /* Make sure there is enough room in out_buff. */
   /* Every 3 input chars equals 4 outputs chars. */
   min_size = ( (in_len + 2) / 3 ) * 4;
   /* Add 2 for CRLF for every BASE64_MAX_LINE_LEN output chars. */
   min_size = min_size + ( (min_size/BASE64_MAX_LINE_LEN) * 2 );
   if ( min_size > out_len )
      return (-1);

   line_len = 0;
   i = 0;

   while (i < in_len)
      {
      buff[0] = *in_buff++;
      i++;
      num_chars = 1;
      if (i == in_len)
         {
         buff[1] = 0;
         buff[2] = 0;
         }
      else
         {
         buff[1] = *in_buff++;
         i++;
         num_chars++;
         if (i == in_len)
            buff[2] = 0;
         else
            {
            buff[2] = *in_buff++;
            i++;
            num_chars++;
            }
         }

      base64_group(buff, num_chars, out_buff);
      out_buff = out_buff + 4;
      line_len = line_len + 4;
      if (line_len == BASE64_MAX_LINE_LEN)
         {
         *out_buff++ = '\r';
         *out_buff++ = '\n';
         line_len = 0;
         }
   }

   return (1);
}
#endif
#endif      /* (MN_SMTP) */

