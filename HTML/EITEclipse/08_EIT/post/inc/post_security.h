#ifndef POST_SECURITY
#define POST_SECURITY   1

void post_security(PSOCKET_INFO socket_ptr) cmx_reentrant; 
void post_change_login(PSOCKET_INFO socket_ptr) cmx_reentrant; 
void post_change_admin(PSOCKET_INFO socket_ptr) cmx_reentrant; 
void post_change_serial(PSOCKET_INFO socket_ptr) cmx_reentrant; 

#endif
