#ifndef MICRONET_ssi_packing
#define MICRONET_ssi_packing		1

#define LEN_SELECTED 25                /* Length of selected="selected" */
#define LEN_OPTIONS  80                /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 
#define CO_OPTS 3                      /* Connection Control options    */
#define DC_OPTS 4                      /* Disconnect by                 */

word16 end_char_func(byte **str_) cmx_reentrant;
word16 fwd_endchar_func(byte **str_) cmx_reentrant;
word16 buff_time_func(byte **str) cmx_reentrant;
word16 ssi_pkt_len(byte **str_) cmx_reentrant;
word16 itimeout_func(byte **str_) cmx_reentrant;
word16 ssi_disconnect_by(byte **str_)cmx_reentrant;

#endif
