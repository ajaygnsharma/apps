#ifndef MICRONET_ssi_serial
#define MICRONET_ssi_serial		1

#define LEN_SELECTED 25                /* Length of selected="selected" */
#define LEN_OPTIONS  80                /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 

word16 baud_func(byte **str_) cmx_reentrant;
word16 databits_func(byte **str_) cmx_reentrant;
word16 parity_func(byte **str_) cmx_reentrant;
word16 stopbit_func(byte **str_) cmx_reentrant;
word16 flowcontrol_func(byte **str_) cmx_reentrant;

word16 ssi_interface(byte **str_) cmx_reentrant;
word16 ssi_rs485txrx(byte **str_) cmx_reentrant;
word16 ssi_rs485timer(byte **str_) cmx_reentrant;
word16 ssi_rs485type(byte **str_) cmx_reentrant;

word16 ssi_connect_on(byte **str_) cmx_reentrant;

#endif
