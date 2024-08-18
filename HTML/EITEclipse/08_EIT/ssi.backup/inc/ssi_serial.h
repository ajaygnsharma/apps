#ifndef MICRONET_ssi_serial
#define MICRONET_ssi_serial		1

word16 lp_func(byte **str_) cmx_reentrant ;
word16 ssi_connect_on(byte **str_) cmx_reentrant;
word16 ssi_remote_connect_on(byte **str_) cmx_reentrant;
word16 rmt_ip_func(byte **str_) cmx_reentrant;
word16 rmt_port_func(byte **str_) cmx_reentrant;
word16 ssi_retry_timer(byte **str_) cmx_reentrant;
word16 end_char_func(byte **str_) cmx_reentrant;
word16 buff_time_func(byte **str) cmx_reentrant;
word16 itimeout_func(byte **str_) cmx_reentrant;
word16 rmt_en_func(byte **str_) cmx_reentrant;
word16 fwd_endchar_func(byte **str_) cmx_reentrant;
word16 baud_func(byte **str_) cmx_reentrant;
word16 databits_func(byte **str_) cmx_reentrant;
word16 parity_func(byte **str_) cmx_reentrant;
word16 stopbit_func(byte **str_) cmx_reentrant;
word16 flowcontrol_func(byte **str_) cmx_reentrant;
word16 no_of_conns_func(byte **str_) cmx_reentrant;
word16 ssi_pkt_len(byte **str_) cmx_reentrant;
word16 ssi_interface(byte **str_) cmx_reentrant;
word16 ssi_rs485txrx(byte **str_) cmx_reentrant;
word16 ssi_rs485timer(byte **str_) cmx_reentrant;
word16 ssi_disconnect_by(byte **str_)cmx_reentrant;
word16 ssi_connection_type(byte **str_)cmx_reentrant;
word16 ssi_retry_counter(byte **str_) cmx_reentrant;
word16 ssi_retry_timeout(byte **str_) cmx_reentrant;
word16 ssi_mipaddr(byte **str_) cmx_reentrant;
word16 ssi_multi_port(byte **str_) cmx_reentrant;
word16 ssi_rs485type(byte **str_) cmx_reentrant;

#endif
