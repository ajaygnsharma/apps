#ifndef MICRONET_My_super
#define MICRONET_My_super	1

void network_func(PSOCKET_INFO socket_ptr) cmx_reentrant; 
word16 spswd_func(byte **str) cmx_reentrant; 
word16 pswd0_func(byte **str) cmx_reentrant;
word16 hname_func(byte **str) cmx_reentrant;
void hname_supplement(byte *buff,byte number) cmx_reentrant;
word16 gw_ip_func(byte **str_) cmx_reentrant;
word16 web_func(byte **str_) cmx_reentrant;
void upgrade_func(PSOCKET_INFO socket_ptr) cmx_reentrant;
void remove_space(byte *str) cmx_reentrant;
void post_network(PSOCKET_INFO socket_ptr) cmx_reentrant;

#endif
