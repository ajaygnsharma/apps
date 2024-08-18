#ifndef newport_port2002
#define newport_port2002	1

#define CMD_BUFF_SIZE  200

extern byte cmd_buff[CMD_BUFF_SIZE];
extern byte *cmd_in_ptr;
extern byte *cmd_out_ptr;
extern word16 cmd_count;

void init_cmd_buff(void) cmx_reentrant;
byte check_cmd(byte *in_ptr) cmx_reentrant;
void port2002_handler(PSOCKET_INFO *psocket_ptr) cmx_reentrant;
void print_read(void) cmx_reentrant;
char mn_port2002_process_packet(PSOCKET_INFO socket_ptr) cmx_reentrant;

#endif
