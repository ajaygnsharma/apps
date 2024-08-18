#ifndef NEWPORT_modbus
#define NEWPORT_modbus	1

typedef struct tcp_modbus_header 
{
  byte trans_id[2];
  byte protocol_id[2];
  byte length[2];
  byte u_id;
  byte f_code;
} TCP_MODBUS_HEADER;

#define POLYNOMIAL 0xA001;

//void modbus_rt_handle(byte *str, word16) cmx_reentrant;
//void modbus_trans(void) cmx_reentrant;
word16 calc_crc(byte *start_of_packet, byte *end_of_packet) cmx_reentrant;
byte modbus_return_package(void) cmx_reentrant;
void modbus_exception(byte error) cmx_reentrant;
void modbus_send(byte *str,word16 count) cmx_reentrant;
void modbus_recv(void) cmx_reentrant;

#endif
