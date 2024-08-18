#ifndef MICRONET_ssi_bridge
#define MICRONET_ssi_bridge		1

#define LEN_SELECTED 25                /* Length of selected="selected" */
#define LEN_OPTIONS  80                /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 

word16 lp_func(byte **str_) cmx_reentrant ;
word16 no_of_conns_func(byte **str_) cmx_reentrant;

#endif
