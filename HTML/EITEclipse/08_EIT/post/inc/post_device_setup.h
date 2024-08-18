#ifndef device_setup
#define device_setup 1

void post_setdeviceparam(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_updatedevparam(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_check(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_read_dev(PSOCKET_INFO socket_ptr)cmx_reentrant;

#endif
