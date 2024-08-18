/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

/* Send a byte to the xmit buffer. Adds to FCS if doFCS is true. */
void mn_send_escaped_byte(byte c, byte doFCS)
cmx_reentrant {
#if (MN_ETHERNET)
   doFCS=doFCS;                           /* keep compiler happy */

   if (interface_info[send_interface_no].interface_type == IF_ETHER)
      mn_send_byte(c);
#endif      /* (MN_ETHERNET) */

#if MN_PPP
   if (interface_info[send_interface_no].interface_type == IF_PPP)
      {
      if (doFCS)
         mn_ppp_fcs_accum(c, SEND_FCS);

      if ( ( (c < 0x20) && (ppp_accm & (1L<<c) ) ) || \
            c == PPP_FRAME || c == PPP_ESC )
         {  
         mn_send_byte(PPP_ESC);
         mn_send_byte((byte)(c ^ PPP_COMPLEMENT));
         }
      else
         mn_send_byte(c);
      }
#elif MN_SLIP
   if (interface_info[send_interface_no].interface_type == IF_SLIP)
      {
      doFCS=doFCS;                           /* keep compiler happy */

      switch (c)
         {
         case SLIP_END:
            mn_send_byte(SLIP_ESC);
            mn_send_byte(SLIP_ESC_END);
            break;
         case SLIP_ESC:
            mn_send_byte(SLIP_ESC);
            mn_send_byte(SLIP_ESC_ESC);
            break;
         default:
            mn_send_byte(c);
            break;
         }
      }
#endif
}

/* get a byte from the recv buffer, converting escaped chars if needed */
byte mn_recv_escaped_byte(byte doFCS)
cmx_reentrant {
   byte thisChar;
   int ret;

#if (!MN_PPP)
   doFCS=doFCS;                           /* keep compiler happy */
#endif

   ret = mn_recv_byte();
   if (ret == RECV_TIMED_OUT)
      return 0;
   else
      thisChar = (byte)ret;

#if MN_PPP
   if (interface_info[recv_interface_no].interface_type == IF_PPP)
      {
      if (thisChar == PPP_ESC)
         {
         ret = mn_recv_byte();
         if (ret == RECV_TIMED_OUT)
            return 0;
         else
            thisChar = (byte)(((byte)ret) ^ PPP_COMPLEMENT);
         }
      if (doFCS)
         mn_ppp_fcs_accum(thisChar, RECV_FCS);
      }
#elif MN_SLIP
   if (interface_info[recv_interface_no].interface_type == IF_SLIP)
      {
      doFCS = doFCS;

      if (thisChar == SLIP_ESC)
         {
         ret = mn_recv_byte();
         if (ret == RECV_TIMED_OUT)
            return 0;
         else
            {
            if (ret == SLIP_ESC_END)
               thisChar = SLIP_END;
            else
               thisChar = SLIP_ESC;
            }
         }
      }
#endif
   return(thisChar);
}

/* put a byte in the xmit buffer */
void mn_send_byte(byte c)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(send_interface_no);

#if (MN_PPP || MN_SLIP)
   /* for ethernet we calculate in advance whether the packet will fit
      into the transmit buffer.
   */
   if (interface_ptr->send_in_ptr <= interface_ptr->send_buff_end_ptr)
#endif      /* (!MN_ETHERNET) */
      {
      *(interface_ptr->send_in_ptr) = c;
      interface_ptr->send_in_ptr++;
      }
}

/* get a byte from the recv buffer, blocking temporarily if neccessary.
   for ethernet assume we will always have a full packet. Does not advance
   pointer or decrement count.
*/
int mn_peek_recv(void)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;
#if (MN_PPP || MN_SLIP)
   byte have_char;
   TIMER_INFO_T wait_timer;
#endif      /* (MN_PPP || MN_SLIP) */

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

#if (MN_ETHERNET && !USE_RECV_BUFF)
   if (interface_ptr->interface_type == IF_ETHER)
      return(eth_mn_recv_byte());
#endif      /* (MN_ETHERNET && !USE_RECV_BUFF) */

#if (MN_PPP || MN_SLIP)

/* Wait at least one millisecond. */
#if (MN_ONE_SECOND <= 1000)
#define MN_UART_WAIT_TICKS 1
#else
#define MN_UART_WAIT_TICKS ((MN_ONE_SECOND)/1000)
#endif

#if (MN_ETHERNET && USE_RECV_BUFF)
   if (interface_ptr->interface_type == IF_ETHER)
      have_char = TRUE;
   else
#endif      /* (MN_ETHERNET) */
      {
      have_char = FALSE;

      if (mn_recv_byte_present(recv_interface_no))      /* we got a char */
         have_char = TRUE;
      else
         {
         mn_reset_timer(&wait_timer, MN_UART_WAIT_TICKS);
         /* Check if we have a character, if not, wait a little. */
         while (!mn_timer_expired(&wait_timer))
            {
            if (mn_recv_byte_present(recv_interface_no)) /* we got a char */
               {
               have_char = TRUE;
               break;
               }
#if (RTOS_USED != RTOS_NONE)
            MN_TASK_WAIT(1);
#endif      /* (RTOS_USED != RTOS_NONE) */
            }
         }
      }

   if (have_char)
#endif      /* (MN_PPP || MN_SLIP) */
      return ((int)(*(interface_ptr->recv_out_ptr)));
#if (MN_PPP || MN_SLIP)
   else
      return (RECV_TIMED_OUT);
#endif      /* (MN_PPP || MN_SLIP) */
}

/* get a byte from the recv buffer, blocking temporarily if neccessary.
   for ethernet assume we will always have a full packet.
*/
int mn_recv_byte(void)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;
   int c;

   interface_ptr = MK_INTERFACE_PTR(recv_interface_no);

   c = mn_peek_recv();

   if (c != RECV_TIMED_OUT)
      {
      DISABLE_INTERRUPTS;
      interface_ptr->recv_count--;
      ENABLE_INTERRUPTS;
      interface_ptr->recv_out_ptr++;
      if (interface_ptr->recv_out_ptr > interface_ptr->recv_buff_end_ptr)
         interface_ptr->recv_out_ptr = interface_ptr->recv_buff_ptr;
      }

   return (c);
}

byte mn_recv_byte_present(SCHAR interface_no)
cmx_reentrant {
   byte retval;
   PINTERFACE_INFO interface_ptr;

   retval = FALSE;

   if ((interface_no >= 0) && (interface_no < MN_NUM_INTERFACES))
      {
      interface_ptr = MK_INTERFACE_PTR(interface_no);

#if (MN_ETHERNET && !USE_RECV_BUFF)
      if (interface_ptr->interface_type == IF_ETHER)
         return(eth_mn_recv_byte_present(interface_no));
#endif      /* (MN_ETHERNET && !USE_RECV_BUFF) */

      DISABLE_INTERRUPTS;
      /* Anything in the receive buffer? */
      if (interface_ptr->recv_count > 0)
         retval = TRUE;
      ENABLE_INTERRUPTS;
      }

   return (retval);
}

#if (MN_ETHERNET)
/* the next two functions are for use by ethernet drivers */

/* gets a byte to transmit from the xmit buffer. check to make sure that
   send_out_ptr != send_in_ptr before calling this function.
*/
byte mn_get_xmit_byte(void)
cmx_reentrant {
   byte c1;
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   c1 = *(interface_ptr->send_out_ptr);
   interface_ptr->send_out_ptr++;

   return c1;
}

#if (USE_RECV_BUFF)
/* puts a byte into the recv buffer. make sure there is enough room first. */
void mn_put_recv_byte(byte c2)
cmx_reentrant {
   PINTERFACE_INFO interface_ptr;

   interface_ptr = MK_INTERFACE_PTR(eth_interface_no);

   *(interface_ptr->recv_in_ptr) = c2;
   interface_ptr->recv_in_ptr++;
   if (interface_ptr->recv_in_ptr > interface_ptr->recv_buff_end_ptr)
      interface_ptr->recv_in_ptr = interface_ptr->recv_buff_ptr;
   interface_ptr->recv_count++;
}
#endif      /* (USE_RECV_BUFF) */
#endif      /* (MN_ETHERNET) */

