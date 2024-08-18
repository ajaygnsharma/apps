#ifndef NEWPORT_ipport
#define NEWPORT_ipport	1

extern byte set_ip_buff[4];
extern byte find_ip;
extern byte wait_ip;
extern TIMER_INFO_T reset_wait_timer;
//void start_reset(void);

//void set_ip(PSOCKET_INFO *psocket_ptr) reentrant;
//void ip_backup(void) reentrant;
void port_check(word16 port) cmx_reentrant;
//void reset_server(void);
//void check_reset(void);
//void start_reset(void);

#endif
