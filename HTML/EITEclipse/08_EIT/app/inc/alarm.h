#ifndef MICRONET_alarm
#define MICRONET_alarm		1

void power_on_alarm(void) cmx_reentrant;     /* Return Model No    */
void ip_changed_alarm(void)cmx_reentrant;
void isvr_accessed_alarm(void) cmx_reentrant;
void isvr_condition_alarm(void) cmx_reentrant;
void isvr_disconnect_alarm(void) cmx_reentrant;
void isvr_char_alarm(void) cmx_reentrant;

#endif
