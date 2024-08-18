#ifndef MICRONET_ssi_gpio
#define MICRONET_ssi_gpio		1

word16 ssi_fio(byte **str_) cmx_reentrant;
word16 ssi_cio(byte **str_) cmx_reentrant;
word16 ssi_s_no(byte **str_) cmx_reentrant;
word16 ssi_i_chk(byte **str_) cmx_reentrant;
word16 ssi_fio_status(byte **str_) cmx_reentrant;
word16 ssi_o_chk(byte **str_) cmx_reentrant;
word16 ssi_h_chk(byte **str_) cmx_reentrant;
word16 ssi_l_chk(byte **str_) cmx_reentrant;
word16 ssi_image_source(byte **str_) cmx_reentrant;
word16 ssi_cio_status(byte **str_) cmx_reentrant;
word16 ssi_ci_i_chk(byte **str_) cmx_reentrant;
word16 ssi_ci_o_chk(byte **str_) cmx_reentrant;
word16 ssi_ct_i_chk(byte **str_) cmx_reentrant;
word16 ssi_ct_o_chk(byte **str_) cmx_reentrant;

#endif
