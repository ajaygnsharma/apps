#ifndef MICRONET_gpio
#define MICRONET_gpio	1

void post_gpio(PSOCKET_INFO socket_ptr) cmx_reentrant ;
void io_dir(byte port_no, byte io_no, byte dir);
void io_set(byte port_no, byte io_no, byte value);
byte test_bit(byte port_no, byte io_no);

#endif
