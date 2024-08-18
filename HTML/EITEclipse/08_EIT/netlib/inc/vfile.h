/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef VFILE_H_INC
#define VFILE_H_INC  1

#define VF_PTYPE_DYNAMIC   0x01  /* Page Type Dynamic allocated memory */
#define VF_PTYPE_STATIC    0x02  /* Page Type Static allocated memory */
#define VF_PTYPE_FLASH     0x04  /* Page Type Type FLASH memory */
#define VF_PTYPE_RAM       0x08  /* Page Type Type RAM memory */
#define VF_PTYPE_EFFS      0x10  /* Page Type Type EFFS file system */
#define VF_PTYPE_HTML      0x80  /* web page is an HTML page */

#define VF_PAGE_IN_USE_FTP    0x01
#define VF_PAGE_IN_USE_HTTP   0x02

#if (defined(MCHP_C18))
typedef void (*POST_FP)(auto PSOCKET_INFO) cmx_reentrant; 
typedef word16 (*GET_FP)(auto byte **) cmx_reentrant;
#else
typedef void (*POST_FP)(PSOCKET_INFO) cmx_reentrant; 
typedef word16 (*GET_FP)(byte **) cmx_reentrant;
#endif

typedef struct vf {
   byte filename[MN_VF_NAME_LEN];
   SEND_LEN_T page_size;
   PCONST_BYTE page_ptr;
   byte * ram_page_ptr;
   byte page_type;         /* Page type bit flags for page data control */
   byte in_use_flag;
#if (CMX_FFS_USED)
   F_FILE * effs_file_ptr;
   long filepos;
#endif      /* (CMX_FFS_USED) */
} VF;

typedef VF * VF_PTR;

typedef struct page_send_s {
   PCONST_BYTE page_ptr;
   byte * ram_page_ptr;
   word16 bytes_read;
   word16 header_bytes;
#if (CMX_FFS_USED)
   VF_PTR vf_ptr;
#endif      /* (CMX_FFS_USED) */
} PAGE_SEND_T;

typedef PAGE_SEND_T * PAGE_SEND_PTR;

extern VF vf_dir[MN_NUM_VF_PAGES];
extern PAGE_SEND_T page_send_info[MN_NUM_SOCKETS];

VF_PTR mn_vf_get_entry(byte *) cmx_reentrant;
VF_PTR mn_vf_set_entry(byte *, SEND_LEN_T, PCONST_BYTE, byte) cmx_reentrant;
VF_PTR mn_vf_set_ram_entry(byte *, SEND_LEN_T, byte *, byte) cmx_reentrant;
SCHAR mn_vf_del_entry(byte *) cmx_reentrant;
SCHAR mn_vf_open_file(VF_PTR vf_ptr, const char * mode) cmx_reentrant;
byte mn_vf_close_file(VF_PTR vf_ptr) cmx_reentrant;
void page_send_close(SCHAR socket_no) cmx_reentrant;
#if (CMX_FFS_USED)
SCHAR vf_set_filepos(VF_PTR vf_ptr) cmx_reentrant;
#endif      /* (CMX_FFS_USED) */

/* --------------------------------------------------------------- */
#if MN_HTTP

typedef struct post_funcs {
   byte func_name[MN_FUNC_NAME_LEN];
   POST_FP func_ptr;
} POST_FUNCS;

typedef POST_FUNCS * PF_PTR;

#if (MN_NUM_POST_FUNCS)
extern POST_FUNCS pf[MN_NUM_POST_FUNCS];  
#endif      /* (MN_NUM_POST_FUNCS) */

POST_FP mn_pf_get_entry(byte *) cmx_reentrant;
#if (defined(MCHP_C18))
PF_PTR mn_pf_set_entry(byte *, auto POST_FP) cmx_reentrant;
#else
PF_PTR mn_pf_set_entry(byte *, POST_FP) cmx_reentrant;
#endif
byte mn_pf_del_entry(byte *) cmx_reentrant;

/* --------------------------------------------------------------- */
#if MN_SERVER_SIDE_INCLUDES

typedef struct get_funcs {
   byte func_name[MN_FUNC_NAME_LEN];
   GET_FP func_ptr;
} GET_FUNCS;

typedef GET_FUNCS * GF_PTR;

extern GET_FUNCS gf[MN_NUM_GET_FUNCS];

GET_FP mn_gf_get_entry(byte *) cmx_reentrant;
#if (defined(MCHP_C18))
GF_PTR mn_gf_set_entry(byte *, auto GET_FP) cmx_reentrant;
#else
GF_PTR mn_gf_set_entry(byte *, GET_FP) cmx_reentrant;
#endif
byte mn_gf_del_entry(byte *) cmx_reentrant;

#endif      /* #if MN_SERVER_SIDE_INCLUDES */
#endif      /* MN_HTTP */
#endif      /* #ifndef VFILE_H_INC */
