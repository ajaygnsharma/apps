#ifndef MICRONET_comm
#define MICRONET_comm	1

/*----------------------Function Prototypes ----------------------------*/
int initUART(void) cmx_reentrant;
void uart0_handler(void) cmx_reentrant;
void uart1_handler(void) cmx_reentrant;
void show_settings_mode(void) cmx_reentrant;
void print_all_settings(void) cmx_reentrant;
void show_prompt(void) cmx_reentrant;
void serial_console(void) cmx_reentrant;
void show_help(void) cmx_reentrant;
void show_set_help(void) cmx_reentrant;

#endif
