/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if MN_VIRTUAL_FILE

#if (defined(MCHP_C18))
#pragma udata mn_vfile2
#endif
VF vf_dir[MN_NUM_VF_PAGES];
#if (defined(MCHP_C18))
#pragma udata
#endif

#if MN_HTTP
#if (defined(MCHP_C18))
#pragma udata mn_vfile3
#endif
#if (MN_NUM_POST_FUNCS)
POST_FUNCS pf[MN_NUM_POST_FUNCS];
#endif      /* (MN_NUM_POST_FUNCS) */

#if MN_SERVER_SIDE_INCLUDES
GET_FUNCS gf[MN_NUM_GET_FUNCS];
#endif      /* #if MN_SERVER_SIDE_INCLUDES */

#if (defined(MCHP_C18))
#pragma udata
#endif
#endif      /* MN_HTTP */

#define NUM_HTML_EXTS   2
static cmx_const byte HTMLExtensions[NUM_HTML_EXTS][5] = {"htm","html"};

PAGE_SEND_T page_send_info[MN_NUM_SOCKETS];

static VF_PTR set_vf_entry(byte *name, SEND_LEN_T size, PCONST_BYTE page, \
   byte * ram_page, byte type) cmx_reentrant;

/* ---------------------------------------------------------------------- */

/* get the directory entry corresponding to name or NULL if not found */
VF_PTR mn_vf_get_entry(byte *name)
cmx_reentrant {
   byte i;  /* assume no more than 255 pages */
   VF_PTR retval;

   retval = (VF_PTR)PTR_NULL;

   MN_MUTEX_WAIT(MUTEX_VFS_DIR,INFINITE_WAIT);

   if (strlen((char *)name) <= MN_VF_NAME_LEN)  /* check if name length is OK */
      {
      for (i=0;i<MN_NUM_VF_PAGES;i++)
         {
         if (mn_stricmp(vf_dir[i].filename,name) == 0)
            {
            retval= &vf_dir[i];
            break;
            }
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_DIR);
   return (retval);
}

/* put directory entry in first empty spot in structure and return pointer
   to that spot.  Returns NULL if directory is full.
*/
VF_PTR mn_vf_set_entry(byte *name, SEND_LEN_T size, PCONST_BYTE page, byte type)
cmx_reentrant {
   return(set_vf_entry(name, size, page, PTR_NULL, type));
}


VF_PTR mn_vf_set_ram_entry(byte *name, SEND_LEN_T size, byte * page, byte type)
cmx_reentrant {
   return(set_vf_entry(name, size, PTR_NULL, page, (byte)(type | VF_PTYPE_RAM)));
}

static VF_PTR set_vf_entry(byte *name, SEND_LEN_T size, PCONST_BYTE page, \
   byte * ram_page, byte type)
cmx_reentrant {
   byte i;  /* assume no more than 255 pages */
   byte *name_ptr;
   byte j;
   VF_PTR retval;

   retval = (VF_PTR)PTR_NULL;

   MN_MUTEX_WAIT(MUTEX_VFS_DIR,INFINITE_WAIT);

   for (i=0;i<MN_NUM_VF_PAGES;i++)
      {
      if (vf_dir[i].filename[0] == '\0')
         {
         vf_dir[i].in_use_flag = 0xff;
         STRCPY(vf_dir[i].filename,name);
         vf_dir[i].page_size = size;
         if (type & VF_PTYPE_RAM)
            vf_dir[i].ram_page_ptr = ram_page;
         else
            vf_dir[i].page_ptr = page;

         /* set HTML flag if the page is an HTML page */
         name_ptr = vf_dir[i].filename;
         while (*name_ptr && (*name_ptr != '.'))     /* look for . */
            ++name_ptr;
         if (*name_ptr == '.')
            {
            ++name_ptr;
            for (j = 0; j < NUM_HTML_EXTS; j++)
               {
               if (mn_stricmp_cb(name_ptr,HTMLExtensions[j]) == 0)
                  {
                  type |= VF_PTYPE_HTML;
                  break;
                  }
               }
            }

         vf_dir[i].page_type = type;
         vf_dir[i].in_use_flag = 0;
         retval = &vf_dir[i];
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_DIR);
   return (retval);
}

/* remove the directory entry corresponding to name. returns 1 if entry
   found, 0 if not found, and negative number if being used or error.
*/
SCHAR mn_vf_del_entry(byte *name)
cmx_reentrant {
   byte i;  /* assume no more than 255 pages */
   SCHAR retval;
#if (CMX_FFS_USED)
#if (MN_USE_EFFS_THIN)
   byte del_status;
#else
   int del_status;
#endif
#endif      /* (CMX_FFS_USED) */

   retval = 0;

   MN_MUTEX_WAIT(MUTEX_VFS_DIR,INFINITE_WAIT);

   for (i=0;i<MN_NUM_VF_PAGES;i++)
      {
      if (mn_stricmp(vf_dir[i].filename,name) == 0)
         {
         if (vf_dir[i].in_use_flag)
            {
            retval = VFILE_ENTRY_IN_USE;
            break;
            }
         vf_dir[i].in_use_flag = 0xff;
#if (CMX_FFS_USED)
         if (vf_dir[i].page_type & VF_PTYPE_EFFS)
            {
#if ((RTOS_USED != RTOS_NONE) && (MN_USE_EFFS || MN_USE_EFFSM || MN_USE_EFFS_FAT))
            F_ENTERFS;
            if (f_chdrive(MN_FFS_DRIVENUM))
               {
               vf_dir[i].in_use_flag = 0;
               retval = VFILE_DELETE_FAILED;
               break;
               }
            else
#endif
               {
               /* delete file from EFFS */
               del_status = f_delete((const char *)vf_dir[i].filename);
               if ((del_status != F_NO_ERROR) && (del_status != F_ERR_NOTFOUND))
                  {
                  vf_dir[i].in_use_flag = 0;
                  retval = VFILE_DELETE_FAILED;
                  break;
                  }
               else
                  vf_dir[i].effs_file_ptr = 0;
               }
            }
#endif      /* (CMX_FFS_USED) */
         vf_dir[i].filename[0] = '\0';
         vf_dir[i].page_size = 0;
#if ((MN_FTP_SERVER || MN_FTP_CLIENT) && DYNAMIC_MEM_AVAILABLE && !CMX_FFS_USED)
         if ( (vf_dir[i].page_type & VF_PTYPE_DYNAMIC) && \
               vf_dir[i].ram_page_ptr != PTR_NULL)
            {
            MN_TASK_LOCK;
            free(vf_dir[i].ram_page_ptr);     /* Release dynamic allocated memory */
            MN_TASK_UNLOCK;
            }
#endif
         vf_dir[i].ram_page_ptr = 0;
         vf_dir[i].page_ptr = 0;
         vf_dir[i].page_type = 0;
         vf_dir[i].in_use_flag = 0;
         retval = 1;
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_DIR);
   return (retval);
}

/* Open a file calling the EFFS-Thin f_open function if required. Returns 1
   if successful, negative number on error. It is assumed that the necessary
   initialization has been performed before calling this function.
*/
SCHAR mn_vf_open_file(VF_PTR vf_ptr, const char * mode)
cmx_reentrant {
   SCHAR retval;

   if ((vf_ptr == PTR_NULL) || (vf_ptr->filename == PTR_NULL))
      retval = VFILE_OPEN_FAILED;
   else
      {
      retval = 1;
#if (CMX_FFS_USED)

      if (vf_ptr->page_type & VF_PTYPE_EFFS)
         {
         MN_MUTEX_WAIT(MUTEX_VFS_DIR,INFINITE_WAIT);
#if ((RTOS_USED != RTOS_NONE) && (MN_USE_EFFS || MN_USE_EFFSM || MN_USE_EFFS_FAT))
         F_ENTERFS;
         if (f_chdrive(MN_FFS_DRIVENUM))
            {
            vf_ptr->effs_file_ptr = 0;
            retval = VFILE_OPEN_FAILED;
            }
         else
#endif
            {
            vf_ptr->effs_file_ptr = f_open((const char *)vf_ptr->filename, \
               mode);
            if (vf_ptr->effs_file_ptr == 0)
               retval = VFILE_OPEN_FAILED;
            vf_ptr->filepos = 0;
            }
         MN_MUTEX_RELEASE(MUTEX_VFS_DIR);
         }
#else
      mode = mode;
#endif      /* (CMX_FFS_USED) */
      }

   return (retval);
}

/* Closes a file calling the EFFS f_close function if required. Returns
   F_NO_ERROR if successful, any other number on error. It is assumed that the
   necessary initialization has been performed before calling this function.
*/
byte mn_vf_close_file(VF_PTR vf_ptr)
cmx_reentrant {
   byte retval;

   retval = 0;

#if (CMX_FFS_USED)
   if ((vf_ptr != PTR_NULL) && (vf_ptr->page_type & VF_PTYPE_EFFS) && \
         (vf_ptr->effs_file_ptr != 0))
      {
      MN_MUTEX_WAIT(MUTEX_VFS_DIR,INFINITE_WAIT);
      retval = (byte)f_close(vf_ptr->effs_file_ptr);
      vf_ptr->effs_file_ptr = 0;
      vf_ptr->filepos = 0;
      MN_MUTEX_RELEASE(MUTEX_VFS_DIR);
      }
#else
   vf_ptr = vf_ptr;
#endif      /* (CMX_FFS_USED) */

   return (retval);
}

void page_send_close(SCHAR socket_no)
cmx_reentrant {
   PAGE_SEND_PTR ps_ptr;

   ps_ptr = &page_send_info[socket_no];
   ps_ptr->ram_page_ptr = PTR_NULL;
   ps_ptr->page_ptr = PTR_NULL;
   ps_ptr->bytes_read = 0;
   ps_ptr->header_bytes = 0;
#if (CMX_FFS_USED)
   (void)mn_vf_close_file(ps_ptr->vf_ptr);
   ps_ptr->vf_ptr = PTR_NULL;
#endif      /* (CMX_FFS_USED) */
}

#if (CMX_FFS_USED)
/* Resets the file position if needed. Returns 1 if file position ok at the
   completion of the function and no resetting done, returns 0 if file
   position ok after reset, returns a negative number otherwise.
*/
SCHAR vf_set_filepos(VF_PTR vf_ptr)
cmx_reentrant {
   long cur_filepos;
   SCHAR retval;

   retval = VFILE_SEEK_FAILED;

   if (vf_ptr->effs_file_ptr)
      {
      /* get current file position */
      cur_filepos = f_tell(vf_ptr->effs_file_ptr);

      /* if cur_filepos > vf_ptr->filepos then we are doing a resend and need
         to reset the file position.
         if cur_filepos > vf_ptr->filepos everything is fine.
         if cur_filepos < vf_ptr->filepos then something is messed up.
      */
      if (cur_filepos == vf_ptr->filepos)
         retval = 1;
      else if (cur_filepos > vf_ptr->filepos)
         {
         if (f_seek(vf_ptr->effs_file_ptr,vf_ptr->filepos,F_SEEK_SET) == F_NO_ERROR)
            retval = 0;
         }
      }

   return (retval);
}
#endif      /* (CMX_FFS_USED) */

#if MN_HTTP
/* get the function corresponding to name or NULL if not found */
POST_FP mn_pf_get_entry(byte *name)
cmx_reentrant {
#if (MN_NUM_POST_FUNCS)
   byte i;  /* assume no more than 255 functions */
   POST_FP retval;

   retval = (POST_FP)PTR_NULL;

   MN_MUTEX_WAIT(MUTEX_VFS_PF,INFINITE_WAIT);

   for (i=0;i<MN_NUM_POST_FUNCS;i++)
      {
      if (mn_stricmp(pf[i].func_name,name) == 0)
         {
         retval = pf[i].func_ptr;
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_PF);
   return (retval);
#else
   name = name;
   return (PTR_NULL);
#endif      /* (MN_NUM_POST_FUNCS) */
}

/* put function in first empty spot in structure and return pointer
   to that spot.  Returns NULL if directory is full.
*/
#if (defined(MCHP_C18))
PF_PTR mn_pf_set_entry(byte *name, auto POST_FP func)
#else
PF_PTR mn_pf_set_entry(byte *name, POST_FP func)
#endif
cmx_reentrant {
#if (MN_NUM_POST_FUNCS)
   byte i;  /* assume no more than 255 functions */
   PF_PTR retval;

   retval = (PF_PTR)PTR_NULL;

   MN_MUTEX_WAIT(MUTEX_VFS_PF,INFINITE_WAIT);

   for (i=0;i<MN_NUM_POST_FUNCS;i++)
      {
      if (pf[i].func_name[0] == '\0')
         {
         STRCPY(pf[i].func_name,name);
         pf[i].func_ptr = func;
         retval = &pf[i];
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_PF);
   return (retval);
#else
   name = name;
   func = func;
   return (PTR_NULL);
#endif      /* (MN_NUM_POST_FUNCS) */
}

/* remove the function corresponding to name. returns 1 if function found
   or 0 otherwise.
*/
byte mn_pf_del_entry(byte *name)
cmx_reentrant {
#if (MN_NUM_POST_FUNCS)
   byte i;  /* assume no more than 255 functions */
   byte retval;

   retval = 0;

   MN_MUTEX_WAIT(MUTEX_VFS_PF,INFINITE_WAIT);

   for (i=0;i<MN_NUM_POST_FUNCS;i++)
      {
      if (mn_stricmp(pf[i].func_name,name) == 0)
         {
         pf[i].func_name[0] = '\0';
         pf[i].func_ptr = PTR_NULL;
         retval = 1;
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_PF);
   return (retval);
#else
   name = name;
   return (0);
#endif      /* (MN_NUM_POST_FUNCS) */
}

#if MN_SERVER_SIDE_INCLUDES
/* get the function corresponding to name or NULL if not found */
GET_FP mn_gf_get_entry(byte *name)
cmx_reentrant {
   byte i;  /* assume no more than 255 functions */
   GET_FP retval;

   retval = (GET_FP)PTR_NULL;

   MN_MUTEX_WAIT(MUTEX_VFS_GF,INFINITE_WAIT);

   for (i=0;i<MN_NUM_GET_FUNCS;i++)
      {
      if (mn_stricmp(gf[i].func_name,name) == 0)
         {
         retval = gf[i].func_ptr;
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_GF);
   return (retval);
}

/* put function in first empty spot in structure and return pointer
   to that spot.  Returns NULL if directory is full.
*/
#if (defined(MCHP_C18))
GF_PTR mn_gf_set_entry(byte *name, auto GET_FP func)
#else
GF_PTR mn_gf_set_entry(byte *name, GET_FP func)
#endif
cmx_reentrant {
   byte i;  /* assume no more than 255 functions */
   GF_PTR retval;

   retval = (GF_PTR)PTR_NULL;

   MN_MUTEX_WAIT(MUTEX_VFS_GF,INFINITE_WAIT);

   for (i=0;i<MN_NUM_GET_FUNCS;i++)
      {
      if (gf[i].func_name[0] == '\0')
         {
         STRCPY(gf[i].func_name,name);
         gf[i].func_ptr = func;
         retval = &gf[i];
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_GF);
   return (retval);
}

/* remove the function corresponding to name. returns 1 if function found
   or 0 otherwise.
*/
byte mn_gf_del_entry(byte *name)
cmx_reentrant {
   byte i;  /* assume no more than 255 functions */
   byte retval;

   retval = 0;

   MN_MUTEX_WAIT(MUTEX_VFS_GF,INFINITE_WAIT);

   for (i=0;i<MN_NUM_GET_FUNCS;i++)
      {
      if (mn_stricmp(gf[i].func_name,name) == 0)
         {
         gf[i].func_name[0] = '\0';
         gf[i].func_ptr = (GET_FP)PTR_NULL;
         retval = 1;
         break;
         }
      }

   MN_MUTEX_RELEASE(MUTEX_VFS_GF);
   return (retval);
}
#endif      /* #if MN_SERVER_SIDE_INCLUDES */
#endif      /* MN_HTTP */
#endif      /* #if MN_VIRTUAL_FILE */
