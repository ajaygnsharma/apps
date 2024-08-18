#ifndef MICRONET_My_readdev
#define MICRONET_My_readdev		1

//extern byte code para_header[];
//extern byte code para_header0[];

//word16 set_id_num_func(byte *str) cmx_reentrant;
word16 show_unit_num_func(byte **str) cmx_reentrant;
word16 show_idnum_func(byte **str) cmx_reentrant;
//word16 show_uid_func(byte **str) cmx_reentrant;
//word16 dev_uid_func(byte *str) cmx_reentrant;
//word16 show_fmt_func(byte **str) cmx_reentrant;
//word16 unit_fmt_func(byte **str_) reentrant;
word16 dev_fmt_func(byte *str,byte opt) cmx_reentrant; 
//word16 have_cmd_func(byte *str) cmx_reentrant;
word16 show_cmd_func(byte **str) cmx_reentrant;
word16 dev_cmd_func(byte *str) cmx_reentrant; 
word16 show_name_func(byte **str) cmx_reentrant;
word16 dev_name_func(byte *str,byte opt) cmx_reentrant;
//word16 send_cmd_func(byte **str) cmx_reentrant;
word16 dev_scmd_func(byte *str,byte opt) cmx_reentrant;
word16 show_units_func(byte **str) cmx_reentrant;
//word16 show_unit_func(byte **str) cmx_reentrant;
word16 dev_units_func(byte *str,byte opt) cmx_reentrant;
word16 reload_timeout_func(byte *str) cmx_reentrant; 
word16 load_timeout_func(byte **str) cmx_reentrant;
word16 show_item_func(byte *str,byte opt) cmx_reentrant; 
//word16 MAX_num_func(byte **str) cmx_reentrant; 
word16 MAX_read_func(byte **str) cmx_reentrant; 
//
//word16 wspace_adj(byte *str,word16 n) reentrant;
word16 timeout_select_func(byte **str_) cmx_reentrant;
//
//word16 sp1_cmd_func(byte **str) reentrant;
//word16 dev_sp1_func(byte *str) reentrant;
//word16 sp2_cmd_func(byte **str) reentrant;
//word16 dev_sp2_func(byte *str) reentrant;
//word16 sp3_cmd_func(byte **str) reentrant;
//word16 dev_sp3_func(byte *str) reentrant;
//word16 sp4_cmd_func(byte **str) reentrant;
//word16 dev_sp4_func(byte *str) reentrant;
word16 dev_chk_func(byte *str) cmx_reentrant;
word16 dev_all_func(byte **str_) cmx_reentrant;
word16 dev_call_func(byte *str,int i) cmx_reentrant;
word16 dev_read_func(byte **str_) cmx_reentrant;
word16 dev_rcall_func(byte *str,int i) cmx_reentrant;
//word16 header_func(byte **str) cmx_reentrant;
//word16 npend_func(byte **str) reentrant;
word16 load_on_func(byte **str_) cmx_reentrant;
void check_func(PSOCKET_INFO socket_ptr) cmx_reentrant;
//void set_timeout_func(PSOCKET_INFO socket_ptr) cmx_reentrant; 
//
//int string_to_number(byte *str) cmx_reentrant;
//
//void sent_unit_cmd(byte i) cmx_reentrant;

#endif
