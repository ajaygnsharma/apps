/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MODEM_H_INC
#define MODEM_H_INC  1

/* In the script array the fields are the following:
   1. The string to send. Set to PTR_NULL if waiting for a string first.
   2. The answer to number 1.
   3. The reply to send in response to number 2.
   4. The timeout in seconds to wait for number 2. If 1, 2 and 3 are all set
      to PTR_NULL then a pause for the specified number of seconds will occur.
   5. The error code to return if the timeout period elapses. This
      should be a negative number.
*/
typedef struct modem_script_s {
   PCONST_BYTE send_str;
   PCONST_BYTE answer_str;
   PCONST_BYTE reply_str;
   word16   timeout;
   int      retval;
} MODEM_SCRIPT_T;

typedef MODEM_SCRIPT_T * PMODEM_SCRIPT;
#define ITEM_SEND    0
#define ITEM_ANSWER  1
#define ITEM_REPLY   2
#define SCRIPT_END   -127

typedef struct modem_info_s {
   PMODEM_SCRIPT  script_line;
   PCONST_BYTE    match_str;
   word16         match_len;
   word16         matchedBytes;
   word16         cur_item;
   int            status;
   TIMER_INFO_T   modem_timer;
} MODEM_INFO_T;

typedef MODEM_INFO_T * PMODEM_INFO;

void mn_modem_init(PMODEM_SCRIPT) cmx_reentrant;
int mn_modem_connect(PMODEM_SCRIPT) cmx_reentrant;
int mn_process_modem_script(PMODEM_SCRIPT) cmx_reentrant;
void mn_modem_disconnect(void) cmx_reentrant;
void mn_modem_send_string(PCONST_BYTE, word16) cmx_reentrant;
#if (!(MN_DIRECT_CONNECT))
int mn_modem_wait_reply(PCONST_BYTE str,word16 len,word16 ticks) cmx_reentrant;
#endif      /* (!(MN_DIRECT_CONNECT)) */

#define ANSWER_MODE  0
#define DIAL_MODE    1

/* MN_ONE_SECOND is defined in mn_env.h */
#define THREE_SECONDS   (3*(MN_ONE_SECOND))

#endif   /* #ifndef MODEM_H_INC */


