/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if MN_MODEM

#if (MN_DIRECT_CONNECT)
/* In the script array the fields are the following:
   1. The string to send. Set to PTR_NULL if waiting for a string first.
   2. The answer to number 1.
   3. The reply to send in response to number 2.
   4. The timeout in seconds to wait for number 2, or the number of seconds
      to pause if 1, 2, and 3 are all PTR_NULL.
   5. The error code to return if the timeout period elapses. This
      should be a negative number.
*/

MODEM_SCRIPT_T dial_script[] = {
{ (PCONST_BYTE)"CLIENTCLIENT", (PCONST_BYTE)"CLIENTSERVER", (PCONST_BYTE)PTR_NULL, 3, -1},
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, 1, SCRIPT_END }
};

MODEM_SCRIPT_T answer_script[] = {
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)"CLIENT", (PCONST_BYTE)"CLIENTSERVER", 60, -1 },
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, 0, SCRIPT_END }
};

#else    /* standard dialup connection */

MODEM_SCRIPT_T dial_script[] = {
{ (PCONST_BYTE)"ATM1E0\r", (PCONST_BYTE)"OK", (PCONST_BYTE)PTR_NULL, 3, -1 },
{ (PCONST_BYTE)"ATDT5083928111\r", (PCONST_BYTE)"CONNECT", (PCONST_BYTE)PTR_NULL, 60, -2 },
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, 1, SCRIPT_END }
};

MODEM_SCRIPT_T answer_script[] = {
{ (PCONST_BYTE)"AT&F0S0=1\r", (PCONST_BYTE)"OK", (PCONST_BYTE)PTR_NULL, 3, -1 },
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)"RING", (PCONST_BYTE)PTR_NULL, 30, -2 },
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)"CONNECT", (PCONST_BYTE)PTR_NULL, 60, -3 },
{ (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, (PCONST_BYTE)PTR_NULL, 0, SCRIPT_END }
};

cmx_const byte ModemOffLine[] = "+++";
#define OFFLINE_LEN  3

cmx_const byte ModemHangup[] = "ATH0\r";
#define HANGUP_LEN   5

cmx_const byte ModemOK[] = "OK";
#define MODEM_OK_LEN       2

#endif      /* (MN_DIRECT_CONNECT) */

MODEM_INFO_T modem_info;

static int mn_modem_get_reply(PMODEM_SCRIPT) cmx_reentrant;
static byte modem_check_reply(void) cmx_reentrant;

/* ----------------------------------------------------------------------- */

/* Initializes the modem state machine with the passed script. */
void mn_modem_init(PMODEM_SCRIPT modem_script_ptr)
cmx_reentrant {
   modem_info.script_line = modem_script_ptr;
   modem_info.match_str = modem_script_ptr->answer_str;
   modem_info.match_len = mn_strlen_cb(modem_script_ptr->answer_str);
   modem_info.matchedBytes = 0;
   modem_info.cur_item = 0;
   modem_info.status = 0;
}

/* Connects to a modem using the passed script. Returns 1 if successful */
/* or a negative number on error. */
int mn_modem_connect(PMODEM_SCRIPT modem_script_ptr)
cmx_reentrant {
   int retval;

   if (!(interface_info[uart_interface_no].interface_status & IF_OPEN))
      return (INTERFACE_NOT_FOUND);

   mn_modem_init(modem_script_ptr);
   retval = 1;

   while (retval >= 0)
      {
      do
         {
         retval = mn_process_modem_script(modem_script_ptr);
         }
      while (retval == MN_EWOULDBLOCK);

      if (retval >= 0)
         modem_script_ptr++;
      }

   if (retval == SCRIPT_END)
      retval = 1;

   return (retval);
}

/* Processes a line in a modem script as long as it does not need
   to block. Returns 1 if successful, MN_EWOULDBLOCK if it would
   need to block or a negative number on error.
*/
int mn_process_modem_script(PMODEM_SCRIPT modem_script_ptr)
cmx_reentrant {
   int retval;

   retval = 1;

   if ( (modem_script_ptr->send_str == PTR_NULL) && \
         (modem_script_ptr->answer_str == PTR_NULL) && \
         (modem_script_ptr->reply_str == PTR_NULL) )
      {
      if (modem_script_ptr->timeout)
         MN_TASK_WAIT(modem_script_ptr->timeout * (MN_ONE_SECOND));
      return (modem_script_ptr->retval);
      }

   if (modem_info.cur_item == ITEM_SEND)
      {
      if (modem_script_ptr->send_str != PTR_NULL)
         {
         mn_modem_send_string(modem_script_ptr->send_str, \
            mn_strlen_cb(modem_script_ptr->send_str));
         }
      modem_info.cur_item++;
      modem_info.match_str = modem_script_ptr->answer_str;
      modem_info.match_len = mn_strlen_cb(modem_script_ptr->answer_str);
      modem_info.matchedBytes = 0;
      mn_reset_timer(&(modem_info.modem_timer), \
         (modem_script_ptr->timeout * (MN_ONE_SECOND)));
      }

   if (modem_info.cur_item == ITEM_ANSWER)
      {
      if (modem_script_ptr->answer_str != PTR_NULL)
         {
         retval = mn_modem_get_reply(modem_script_ptr);
         if (retval == 1)
            modem_info.cur_item++;
         }
      }

   if (modem_info.cur_item == ITEM_REPLY)
      {
      if (modem_script_ptr->reply_str != PTR_NULL)
         {
         mn_modem_send_string(modem_script_ptr->reply_str, \
            mn_strlen_cb(modem_script_ptr->reply_str));
         }
      modem_info.cur_item = ITEM_SEND;
      }

   modem_info.status = retval;
   return (retval);
}

/* hangs up modem */
void mn_modem_disconnect(void)
cmx_reentrant {
#if (!(MN_DIRECT_CONNECT))
   if (interface_info[uart_interface_no].interface_status & IF_OPEN)
      {
      mn_modem_send_string(ModemOffLine,OFFLINE_LEN);
      (void)mn_modem_wait_reply(ModemOK,MODEM_OK_LEN,THREE_SECONDS);
      mn_modem_send_string(ModemHangup,HANGUP_LEN);
      }
#endif      /* (!(MN_DIRECT_CONNECT)) */

   modem_info.status = 0;
}

/* sends a string of len length to the modem */
void mn_modem_send_string(PCONST_BYTE str, word16 len)
cmx_reentrant {
#if ( !(MN_DIRECT_CONNECT) )
   TIMER_TICK_T curr_tick;
#endif

   if (!mn_transmit_ready(uart_interface_no))
      return;

   while (len)
      {
      len--;
      mn_send_byte(*str++);   /* Transmit it over the physical layer. */
#if ( !(MN_DIRECT_CONNECT) )
      /* some processors need a delay between chars */
      curr_tick = MN_GET_TICK;
      while (MN_GET_TICK == curr_tick);
#endif
      }
   XMIT_INT_ON;
}

#if (!(MN_DIRECT_CONNECT))
int mn_modem_wait_reply(PCONST_BYTE str,word16 len,word16 ticks)
cmx_reentrant {
   byte forever;
   word16 matchedBytes;
   TIMER_INFO_T wait_timer;
   int retval;

   forever = (byte)(ticks == 0);
   matchedBytes = 0;
   retval = -1;
   recv_interface_no = uart_interface_no;

   if (!forever)
      mn_reset_timer(&wait_timer,ticks);

   do
      {
      if (mn_recv_byte_present(uart_interface_no))
         {
         if (*str == (byte)mn_recv_byte())
            {
            matchedBytes++;
            /* did we get what we are looking for ? */
            if (matchedBytes == len)
               {
               retval = 1;
               break;
               }
            else
               str++;
            }
         else
            {
            /* didn't get a match so reset string to beginning */
            str = str - matchedBytes;
            matchedBytes = 0;
            }
         }
      /* possibly put some sort of delay here for non-rtos case */
      }
   while ( forever || !mn_timer_expired(&wait_timer) );

   return (retval);
}
#endif      /* (!(MN_DIRECT_CONNECT)) */

static int mn_modem_get_reply(PMODEM_SCRIPT modem_script_ptr)
cmx_reentrant {
   int retval;

   recv_interface_no = uart_interface_no;

   retval = MN_EWOULDBLOCK;

   while (mn_recv_byte_present(uart_interface_no))
      {
      if (modem_check_reply())
         {
         retval = 1;
         break;
         }
      }

   if ((retval != 1) && mn_timer_expired(&(modem_info.modem_timer)))
      retval = modem_script_ptr->retval;

   return (retval);
}

static byte modem_check_reply(void)
cmx_reentrant {
   byte retval;

   retval = 0;

   if (*modem_info.match_str == (byte)mn_recv_byte())
      {
      modem_info.matchedBytes++;
      /* did we get what we are looking for ? */
      if (modem_info.matchedBytes == modem_info.match_len)
         retval = 1;
      else
         modem_info.match_str++;
      }
   else
      {
      /* didn't get a match so reset string to beginning */
      modem_info.match_str = modem_info.match_str - modem_info.matchedBytes;
      modem_info.matchedBytes = 0;
      }

   return (retval);
}

#endif   /* MN_MODEM */
