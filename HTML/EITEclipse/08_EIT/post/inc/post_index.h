#ifndef POST_INDEX
#define POST_INDEX	1

int np_atoi(const char* ch);
//int np_atoi(char *str_to_convert, int limit);
byte check_spassword_func(byte *str) cmx_reentrant;
byte check_password(byte *flash_pswd, byte *tmp_pswd) cmx_reentrant;
void main_func(PSOCKET_INFO socket_ptr) cmx_reentrant;
void post_load(PSOCKET_INFO socket_ptr) cmx_reentrant;

//void remove_space(byte *str) cmx_reentrant;

#endif
