#ifndef MICRONET_post_serial
#define MICRONET_post_serial	1

void post_serial(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_bridge(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_packing(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_multihost(PSOCKET_INFO socket_ptr) cmx_reentrant;

#endif
