#ifndef U2E_HTTP_INC_H
#define U2E_HTTP_INC_H

/* Note: #include micronet.h before #including this file. */

#define MY_PORT      2000

#define BUFF_SIZE    2048

/* buffer variables for incoming UART chars. */
extern byte uart_in_buff[BUFF_SIZE];
extern volatile word16 uib_in;
extern volatile word16 uib_count;
extern word16 uib_out;
extern byte eth_out_buff[MN_ETH_TCP_WINDOW];

/* buffer variables used by incoming ethernet chars and outgoing UART chars */
typedef struct buff_pool_s {
   byte data_buff[MN_ETH_TCP_WINDOW];
   byte uart_out_buff[BUFF_SIZE];
   word16 len;
   volatile word16 db_out;
   volatile word16 db_in;
   volatile byte xmit_busy;
   unsigned long count;
} BUFF_POOL_T;

extern BUFF_POOL_T buff_pool;

void start_xmit(void) cmx_reentrant;

#endif
