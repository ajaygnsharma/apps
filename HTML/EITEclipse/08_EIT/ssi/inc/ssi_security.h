#ifndef SSI_SECURITY
#define SSI_SECURITY	1

word16 ssi_spca(byte **str_) cmx_reentrant ;
word16 ssi_wsa(byte **str_) cmx_reentrant ;
word16 ssi_ica(byte **str_) cmx_reentrant ;
word16 ssi_login(byte **str_) cmx_reentrant ;
word16 ssi_login_msg(byte **str_) cmx_reentrant;

word16 ssi_admin(byte **str_) cmx_reentrant ;
word16 ssi_admin_msg(byte **str_) cmx_reentrant;

word16 ssi_serial_msg(byte **str_) cmx_reentrant;
word16 ssi_serial(byte **str_) cmx_reentrant ;

word16 ssi_ip1(byte **str_) cmx_reentrant;
word16 ssi_ip2(byte **str_) cmx_reentrant;
word16 ssi_ip3(byte **str_) cmx_reentrant;
word16 ssi_ip4(byte **str_) cmx_reentrant;
word16 ssi_ip5(byte **str_) cmx_reentrant;
word16 ssi_ip6(byte **str_) cmx_reentrant;
word16 ssi_ip7(byte **str_) cmx_reentrant;
word16 ssi_ip8(byte **str_) cmx_reentrant;

word16 ssi_ipx(byte **str_) cmx_reentrant;


#endif
