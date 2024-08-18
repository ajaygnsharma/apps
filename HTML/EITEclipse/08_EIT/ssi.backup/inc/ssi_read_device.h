#ifndef MICRONET_read_device
#define MICRONET_read_device		1

void read_sno(void) cmx_reentrant;
void read_column(int i) cmx_reentrant;
void get_reading(void) cmx_reentrant;
word16 read_unit(void) cmx_reentrant;
word16 read_dev_name(void) cmx_reentrant;
word16 ssi_chkd_devs(byte **str_) cmx_reentrant;
word16 ssi_timeout_select(byte **str_) cmx_reentrant;
word16 ssi_load_timeout(byte **str) cmx_reentrant;
word16 ssi_load_on(byte **str_) cmx_reentrant;

#endif
