/*********************************************************
Copyright (c) CMX Systems, Inc. 2008. All rights reserved
**********************************************************
   Added HTTP file POST support.
   See the conditional define _POST_FILES
   4.9.2008 - M.J.Butcher                                */

#include "micronet.h"
#include "np_includes.h"

#define MJB_FIX_1   1

#if MN_HTTP

static byte http_file_ok_to_send(VF_PTR) cmx_reentrant;
static void http_set_in_use_flag(PSOCKET_INFO, VF_PTR) cmx_reentrant;
static void http_clear_in_use_flag(PSOCKET_INFO) cmx_reentrant;
static void http_close_files(void) cmx_reentrant;

static word16 MakeHTTPHeader(VF_PTR) cmx_reentrant;
static void copyContentType(byte *, byte *) cmx_reentrant;

#if (MN_SERVER_SIDE_INCLUDES)
static SCHAR process_ram_includes(PSOCKET_INFO, word16) cmx_reentrant;
static SCHAR process_rom_includes(PSOCKET_INFO, word16) cmx_reentrant;
#if CMX_FFS_USED
static SCHAR process_effs_includes(PSOCKET_INFO, word16) cmx_reentrant;
#endif      /* CMX_FFS_USED */
static SCHAR process_get_func(PSOCKET_INFO, word16 *) cmx_reentrant;
static SCHAR process_include_char(PSOCKET_INFO, byte, word16 *) cmx_reentrant;
static word16 copy_leftovers(PSOCKET_INFO, word16 *) cmx_reentrant;
#endif      /* (MN_SERVER_SIDE_INCLUDES) */

byte *URIptr;
byte *BODYptr;
byte http_parse;
#ifdef _POST_FILES				                                         // global variables needed for HTTP POST method
    word16 PostBODYlength = 0;
    int posting_data = 0;
    unsigned long PostFileLen = 0;   
	                                        
    static unsigned short usByteCount = 0;                               // static variables needed for HTTP POST method
    static unsigned short usBoundaryLength = 0;
#endif

#if (defined(MCHP_C18))
#pragma udata http2
#endif
static byte URIBuffer[MN_URI_BUFFER_LEN]; /* store the URI from GET requests */
static byte BODYBuffer[MN_BODY_BUFFER_LEN];  /* store the body from POST requests */
static byte HTTPBuffer[MN_HTTP_BUFFER_LEN];  /* for HTTP responses */
#if (MN_SERVER_SIDE_INCLUDES)
static byte *HTTPBufferptr;
#endif      /* (MN_SERVER_SIDE_INCLUDES) */
#if (defined(MCHP_C18))
#pragma udata
#endif

static byte http_method;
static byte skipNextChar;
static byte lastChar;
static word16 PostBodyLen;
static SCHAR http_curr_socket;

typedef struct content_type_t {
   PCONST_BYTE extension;
   PCONST_BYTE mime_type;
} CONTENT_TYPE_T;

/* Add other types below if necessary. */
#if (defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60))
#define NUM_TYPES 7
#else
#define NUM_TYPES 12
#endif
static cmx_const CONTENT_TYPE_T content_types[NUM_TYPES] = {
   {(PCONST_BYTE)"htm",  (PCONST_BYTE)"text/html"},
   {(PCONST_BYTE)"html", (PCONST_BYTE)"text/html"},
   {(PCONST_BYTE)"gif",  (PCONST_BYTE)"image/gif"},
   {(PCONST_BYTE)"jpg",  (PCONST_BYTE)"image/jpeg"},
   {(PCONST_BYTE)"jpeg", (PCONST_BYTE)"image/jpeg"},
   {(PCONST_BYTE)"class",(PCONST_BYTE)"application/java-vm"},
   {(PCONST_BYTE)"jar",  (PCONST_BYTE)"application/java-archive"},
#if (!(defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60)))
   {(PCONST_BYTE)"png",  (PCONST_BYTE)"image/png"},
   {(PCONST_BYTE)"css",  (PCONST_BYTE)"text/css"},
   {(PCONST_BYTE)"txt",  (PCONST_BYTE)"text/plain"},
   {(PCONST_BYTE)"cla",  (PCONST_BYTE)"application/java-vm"},
   {(PCONST_BYTE)"swf",  (PCONST_BYTE)"application/x-shockwave-flash"},
#endif
};

static cmx_const byte head_1[] = "HTTP/1.0 200 OK\r\nContent-type: ";
static cmx_const byte head_2[] = "\r\nContent-Length: ";
static cmx_const byte head_3[] = "\r\nLast-Modified: Sun, 01 Jun 2008 13:09:28 GMT";
static cmx_const byte head_4[] = "\r\n\r\n";

static cmx_const byte content_len_text[] = "CONTENT-LENGTH:";
#define CONTENT_TEXT_LEN   15

#ifdef _POST_FILES                                                       // headers needing to be recognised during HTTP file posting
    static cmx_const byte multipart_form_data[] = "CONTENT-TYPE: MULTIPART/FORM-DATA; BOUNDARY="; 
    static cmx_const byte content_type_binary[] = "CONTENT-TYPE: APPLICATION/OCTET-STREAM"; // support only binary files
		                                                                 // a simple html page to return in upload success
    static const byte HTTPUploadSuccess[] = "<html><head><title>New SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b>Device</font> - SW Update</i></b></td><br></td><td align=left><br><br>SW Upload successful. The device will now reset and start the new program. Please wait 10s...</body></html>";
#endif

/* if you change these you MUST change the STATUS_xxx_LEN defines in http.h */
cmx_const byte HTTPStatus204[] = "HTTP/1.0 204 No Content\r\n\r\n";
cmx_const byte HTTPStatus400[] = "HTTP/1.0 400 Bad Request\r\n\r\nBad Request";
cmx_const byte HTTPStatus404[] = "HTTP/1.0 404 Not Found\r\n\r\nNot Found";
cmx_const byte HTTPStatus501[] = "HTTP/1.0 501 Not Implemented\r\n\r\nNot Implemented";
cmx_const byte HTTPStatus503[] = "HTTP/1.0 503 Service Unavailable\r\n\r\nService Unavailable";

///* these are the web pages the server looks for when the client requests
//   the default main page. Customarily this is index.htm or index.html but
//   they can be changed if desired.
//*/
////cmx_const byte default_page1[] = "index.htm"; //Ajay 
//byte default_page1[] = "index.htm"; 
////cmx_const byte default_page2[] = "index.html"; // Ajay
//byte default_page2[] = "index.html";

#if (MN_SERVER_SIDE_INCLUDES)
#define TEMP_CGI_BUFF_LEN     ((MN_FUNC_NAME_LEN)+18)

typedef struct temp_ssi_t {
   byte *temp_ssi_ptr;
   word16 num_ssi_bytes;
   byte temp_cgi_buff[TEMP_CGI_BUFF_LEN];
   byte in_cgi;
   byte cgi_pos;
} TEMP_SSI_S;

#if (defined(MCHP_C18))
#pragma udata http3
#endif
static TEMP_SSI_S temp_ssi_info[MN_NUM_SOCKETS];
#if (defined(MCHP_C18))
#pragma udata
#endif

cmx_const byte cgi_tag[] = "<!--#exec cgi=\"";
#define CGI_TAG_LEN  15
#define MIN_CGI_LEN  20

#if CMX_FFS_USED
byte temp_http_buff[MN_HTTP_BUFFER_LEN];
#endif      /* CMX_FFS_USED */
#endif      /* (MN_SERVER_SIDE_INCLUDES) */

/* HTTP_WAIT_TICKS is the time we wait for the parsing stage to complete
   before resetting.
*/
#if ((300*(MN_ONE_SECOND)) < 65536)
#define HTTP_WAIT_TICKS    (300*(MN_ONE_SECOND))
#else
#define HTTP_WAIT_TICKS    (0xFFFF)
#endif
static TIMER_INFO_T http_timer;
#define RESET_HTTP_TIMER    mn_reset_timer(&http_timer,(HTTP_WAIT_TICKS))
#define HTTP_TIMER_EXPIRED  mn_timer_expired(&http_timer)


/* The code in this module assumes that only one socket at a time will use
   HTTPBuffer. Since only .htm pages use this buffer this is okay for now.
*/
/* ----------------------------------------------------------------------- */

/* called from mn_server */
int mn_http_server_recv(PSOCKET_INFO *psocket_ptr)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
   int retval;
   SCHAR socket_no;

   socket_ptr = *psocket_ptr;
   socket_no = socket_ptr->socket_no;

   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   if ( (socket_ptr->tcp_state == TCP_CLOSED) || \
         (WAS_SENDING(socket_no) && (socket_ptr->send_ptr == PTR_NULL)) )
      retval = 0;
   else
      retval = NEED_TO_SEND;
   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);

   mn_http_server_send(psocket_ptr, 0);

   return (retval);
}


/* called from mn_server */
void mn_http_server_set_flag(PSOCKET_INFO socket_ptr)
cmx_reentrant {

   if (socket_ptr->send_ptr)
      {
      MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
      SET_SENDING(socket_ptr->socket_no);
      MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
      }
}

/* called from mn_server */
void mn_http_server_send(PSOCKET_INFO *psocket_ptr, int sent)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
   SCHAR socket_no;

   socket_ptr = *psocket_ptr;
   socket_no = socket_ptr->socket_no;

   if ((sent < 0) || (socket_ptr->tcp_state == TCP_CLOSED))
      {
      MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
      CLEAR_SENDING(socket_no);
      http_clear_in_use_flag(socket_ptr);
      page_send_close(socket_no);
      MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
      (void)mn_abort(socket_no);
      *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
      }
   else if (WAS_SENDING(socket_no) && (socket_ptr->send_ptr == PTR_NULL))
      {
#if (MN_TCP_DELAYED_ACKS)
      if (socket_ptr->tcp_unacked_bytes == 0)
#endif
         {
         MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
         CLEAR_SENDING(socket_no);
         http_clear_in_use_flag(socket_ptr);
         MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
         mn_tcp_close(socket_ptr);
         }
      }
}

void check_http_parse(PSOCKET_INFO *psocket_ptr)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   if (http_curr_socket != SOCKET_NONE)
      {
      /* check if there was an error or a timeout in the middle of a parse */
      if ((http_curr_socket < 0) || (http_curr_socket >= MN_NUM_SOCKETS))
         {
         /* this should never happen ... */
         MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
         http_close_files();
         mn_http_init();
         }
      else if (((http_parse != HTTP_START) && HTTP_TIMER_EXPIRED) || \
            (sock_info[http_curr_socket].tcp_state == TCP_CLOSED))
         {
         http_parse = HTTP_START;
         CLEAR_SENDING(http_curr_socket);
         (void)mn_abort(http_curr_socket);
         /* If the parsing socket is also the current mn_server socket then
            reset the socket_ptr.
         */
         if (*psocket_ptr == &sock_info[http_curr_socket])
            *psocket_ptr = (PSOCKET_INFO)PTR_NULL;
         http_curr_socket = SOCKET_NONE;
         }
      }
   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
}

/* initialize http specific variables */
void mn_http_init(void)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
#if (MN_SERVER_SIDE_INCLUDES)
   MEMSET(temp_ssi_info, 0, sizeof(temp_ssi_info));
#endif      /* (MN_SERVER_SIDE_INCLUDES) */
   http_curr_socket = SOCKET_NONE;
   http_parse = HTTP_START;
   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
}

/* initialization at start of receiving data. Called from mn_app_init_recv(). */
void mn_http_init_recv(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   if ((http_curr_socket != SOCKET_NONE) && \
         (sock_info[http_curr_socket].tcp_state == TCP_CLOSED))
      {
      http_parse = HTTP_START;
      CLEAR_SENDING(http_curr_socket);
      http_curr_socket = SOCKET_NONE;
      }

   /* only initialize at beginning of a request */
   if ((http_parse == HTTP_START) && (http_curr_socket == SOCKET_NONE))
      {
      http_curr_socket = socket_ptr->socket_no;
#if (MN_SERVER_SIDE_INCLUDES)
      temp_ssi_info[http_curr_socket].temp_ssi_ptr = BYTE_PTR_NULL;
      temp_ssi_info[http_curr_socket].num_ssi_bytes = 0;
      temp_ssi_info[http_curr_socket].temp_cgi_buff[0] = 0;
      temp_ssi_info[http_curr_socket].in_cgi = FALSE;
      temp_ssi_info[http_curr_socket].cgi_pos = 0;
#endif      /* (MN_SERVER_SIDE_INCLUDES) */
      http_method = 0;
      skipNextChar = 0;
      PostBodyLen = 0;
#ifdef _POST_FILES	                                                     // reset additional variables at the start of each frame
      if (posting_data == 0) {
          PostFileLen = 0;
      }
      usByteCount = 0;
#endif
      lastChar = 0;
      URIBuffer[0] = '\0';
      URIptr = URIBuffer;
      BODYBuffer[0] = '\0';
      BODYptr = BODYBuffer;
      RESET_HTTP_TIMER;
      }
   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
}

/* process a received byte.  called from mn_app_recv_byte(). */
void mn_http_recv_byte(byte c, PSOCKET_INFO socket_ptr)
cmx_reentrant {
#ifdef _POST_FILES
   #define HEADER_BUFF_LEN (sizeof(multipart_form_data))                 // we need a larger buffer to identify largest file post information
#else
   #define HEADER_BUFF_LEN 23
#endif
   static byte headerBuff[HEADER_BUFF_LEN];
   static byte headerBuffCount = 0;

#ifdef _POST_FILES
   if (posting_data != 0) {                                              // all reception is presently diverted to binary post
       *BODYptr++ = c;                                                   // just collect the data to the body buffer
       return;
   }
   usByteCount++;                                                        // count bytes in present frame
#endif

   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   if (socket_ptr->socket_no == http_curr_socket)
      {
      switch (http_parse)
         {
         case HTTP_START:
            if (c == ' ')
               {
               http_parse++;
               skipNextChar = 0;
               }
            else if (!skipNextChar)
               {
               if (!http_method)
                  http_method = c;
               else if ((http_method == 'P') && (c != 'O'))
                  http_method = 0;        /* POST only for now */
               if (c != 'P')
                  skipNextChar = 1;
               }
            break;
         case HTTP_URI:
            if (c == ' ')
               {
               *URIptr = '\0';
               http_parse++;
               }
            else if (c == '\r')     /* simple request was made */
               {
               *URIptr = '\0';
               http_parse = HTTP_LASTCHAR;
               }
            else
               {
               if (URIptr < &URIBuffer[MN_URI_BUFFER_LEN-1])
                  *URIptr++ = c;    /* copy URI to buffer to process later */
               else
                  {
                  *URIptr = '\0';
                  http_parse = HTTP_END;  /* would have buffer overflow so stop */
                  }
               }
            break;
         case HTTP_VER:
            if (c == '\n')          /* ignore version for now */
               {
               http_parse++;
               lastChar = '\n';
               if (http_method == 'P')
                  {
                  headerBuff[0] = '\0';
                  headerBuffCount=0;
                  }
               }
            break;
         case HTTP_HEADER:
            /* if we get a line with just a CRLF the header is done */
            /* lastChar S/B initialized to '\n' before we get here */
            if ((c == '\r') && (lastChar == '\n'))
               {
               if ((http_method != 'P') || !PostBodyLen) {
#ifdef _POST_FILES
                  if (PostFileLen != 0) {                                // we are presently posting a file so remain in this state
  #ifdef MJB_FIX_1
                    usByteCount = 2;                                    //include last CR/LF
  #endif
                    break;
                  }
#endif
                  http_parse = HTTP_LASTCHAR;
			   }
               else
                  {
                  http_parse++;
                  skipNextChar = 1;    /* next char S/B line feed */
                  }
               }
            else if (skipNextChar)
               {
                 lastChar = c;
                 if (c == '\n') {
                    skipNextChar = 0;
#ifdef _POST_FILES
                    if ((PostFileLen == 1) && (usBoundaryLength == 0)) { // we are counting the boundary length
                      usBoundaryLength = usByteCount;                    // save the boundary length for subtraction from the complete post data length
                    }              
#endif
				  }
               }
            else
               {
               lastChar = c;
               if ((http_method == 'P') && !PostBodyLen)
                  {
                  /* need to get Content-Length value */
                  if ((c == '\r') || (headerBuffCount >= HEADER_BUFF_LEN))
                     {
                     if (c == '\r')
                        headerBuff[headerBuffCount] = '\0';
                     else
                        headerBuff[HEADER_BUFF_LEN-1] = '\0';

                     /* see if we have a Content-Length entry */
                     if (mn_strnicmp_cb(headerBuff,content_len_text,CONTENT_TEXT_LEN) == 0)
                        {
#ifdef _POST_FILES
                          if (PostFileLen != 0) {
                            PostFileLen = mn_atous_long(&headerBuff[CONTENT_TEXT_LEN]); // the length - possibly larger than 64k due to file posts
                            usByteCount = 0;                             // reset byte count in order to count the additional post header content
                          }
                          else {
                            PostBodyLen = mn_atous(&headerBuff[CONTENT_TEXT_LEN]);
                          }
#else
                          PostBodyLen = mn_atous(&headerBuff[CONTENT_TEXT_LEN]);
#endif
                          if (PostBodyLen > (MN_BODY_BUFFER_LEN-1))
                            PostBodyLen = (MN_BODY_BUFFER_LEN-1);
                        }
#ifdef _POST_FILES
                     else if (mn_strnicmp_cb(headerBuff,multipart_form_data,(sizeof(multipart_form_data)-1)) == 0)
                     {
                         PostFileLen = 1;                                // mark that we are posting a file
                         usBoundaryLength = 0;
                         usByteCount = 0;                                // count the boundary length
                     }
                     else if ((PostFileLen != 0) && (mn_strnicmp_cb(headerBuff,content_type_binary,(sizeof(content_type_binary)-1))) == 0) {
                         PostBodyLen = 1;
                     }
#endif
                     headerBuff[0] = '\0';
                     headerBuffCount=0;
                     skipNextChar = 1;
                     }
                  else
                     {
                     /* convert c to uppercase. some compilers don't do this
                        properly so we do it ourselves instead of using the
                        toupper macro.
                     */
/*                     c = (c >= 'a' && c <= 'z') ? c & 0x5F : c; */
                     if ((c >= 'a') && (c <= 'z'))
                        c &= 0x5F;
                     headerBuff[headerBuffCount++] = c;
                     }
                  }
               }
            break;
         case HTTP_BODY:
            if (skipNextChar)
               skipNextChar = 0;
            else if (c == '\r')
               http_parse++;
            else
               {
#ifdef _POST_FILES
                if (PostFileLen != 0) {                                  // this is the start of file data
                    http_parse = HTTP_DATA_POST;                         // move to the next state
                    PostFileLen -= usByteCount;                          // subtract all data received after the content length field
                    PostFileLen -= (usBoundaryLength + 4);               // subtract the boundary length for the first and single encapsulated data part
                    BODYptr = BODYBuffer;								 // reset the body buffer pointer
                    *BODYptr++ = c;                                      // collect first byte
                    break;
                }
#endif
               if (PostBodyLen)
                  {
                  *BODYptr++ = c;      /* copy body to buffer to process later */
                   --PostBodyLen;
                  }
               if (!PostBodyLen)
                  {
                  *BODYptr = '\0';
                  http_parse = HTTP_END;
                  }
               }
            break;
#ifdef _POST_FILES
         case HTTP_DATA_POST:
             *BODYptr++ = c;                                             // collect following bytes during a data post
             break;
#endif
         case HTTP_LASTCHAR:
            /* Eat the remaining char (LF) */
            http_parse++;
            break;
         case HTTP_END:
            break;
         }
      }
   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
}

/* process a received packet.  called from mn_app_process_packet(). */
SCHAR mn_http_process_packet(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   /* var   http_method has request method
            URIptr points to position after last char in URIBuffer
            BODYptr points to position after last char in BODYBuffer
   */
   word16 URIlen, BODYlen;
   VF_PTR vf_ptr;
   POST_FP post_func;
   byte *temp_ptr;

#ifdef _POST_FILES
   if (posting_data != 0) {                                              // performing a data post upload
       extern int fnSaveUpload(void);
       BODYlen = (word16)(BODYptr - &BODYBuffer[0]);
       PostBODYlength = (word16)(BODYptr - &BODYBuffer[0]);
       BODYptr = &BODYBuffer[0];
       if (fnSaveUpload() != 0) {                                        // generic upload handler
           mn_http_set_message(socket_ptr,HTTPUploadSuccess,(sizeof(HTTPUploadSuccess)-1));
           return 0;
       }
       else {
           return 1;
       }
   }
#endif
   vf_ptr = (VF_PTR)PTR_NULL;

   if (http_curr_socket == SOCKET_NONE)
      return 0;
   else if (http_curr_socket != socket_ptr->socket_no)
      return (NEED_IGNORE_PACKET);

#ifdef _POST_FILES
   if ((http_parse == HTTP_END) || (http_parse == HTTP_DATA_POST))       // include HTTP_END or HTTP_DATA_POST states
#else
   if (http_parse == HTTP_END)
#endif
      {
      MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
      http_curr_socket = SOCKET_NONE;
      http_parse = HTTP_START;
      URIlen = (word16)(URIptr - &URIBuffer[0]);
      if (URIlen == 0)
         {
         MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
         mn_http_set_message(socket_ptr,HTTPStatus400,STATUS_400_LEN);
         return 0;
         }
      URIptr = &URIBuffer[0];
#if MN_INCLUDE_HEAD
      if ((http_method == 'G') || (http_method == 'H'))
#else
      if (http_method == 'G')
#endif      /* MN_INCLUDE_HEAD */
         {
         if ((URIlen == 1) && (*URIptr == '/'))
            {
            /* look for index.htm or index.html */
            (void)mn_strcpy_cb(URIptr,default_page1);
            vf_ptr = mn_vf_get_entry(URIptr);
            if (vf_ptr == PTR_NULL)
               {
               (void)mn_strcpy_cb(URIptr,default_page2);
               vf_ptr = mn_vf_get_entry(URIptr);
               }
            }
         else
            {
            /* check for Get-method posts e.g.
               GET /some_func?name=value
               if we find a ? move everything after it to BODYBuffer,
               and treat it as a post.

               Note: the first character in the filename in a GET is
                     expected to be a '/'.
            */
            temp_ptr = URIptr;
            while (*temp_ptr)
               {
               if (*temp_ptr == '?')
                  {
                  *temp_ptr++ = '\0';
                  /* Ignore the '?' if there is nothing after it. */
                  if (*temp_ptr != '\0')
                     {
                     (void)strcpy((char *)BODYBuffer,(char *)temp_ptr);
                     BODYptr = &BODYBuffer[0] + strlen((char *)BODYBuffer);
                     http_method = 'P';
                     }
                  break;
                  }
               temp_ptr++;
               }

            if (http_method != 'P')
               {
               URIptr++;
               vf_ptr = mn_vf_get_entry(URIptr);
               }
            }

         if (http_method != 'P')
            {
            if (!http_file_ok_to_send(vf_ptr))
               {
               MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
               mn_http_set_message(socket_ptr,HTTPStatus404,STATUS_404_LEN);
               return 0;
               }
            }

         if (http_method == 'G')
            {
            MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
            if (!mn_http_set_file(socket_ptr, vf_ptr))
               {
               mn_http_set_message(socket_ptr,HTTPStatus503,STATUS_503_LEN);
               return (0);
               }
            }

#if (MN_INCLUDE_HEAD)
         else if (http_method == 'H')
            {
            http_set_in_use_flag(socket_ptr,vf_ptr);
            socket_ptr->send_len = MakeHTTPHeader(vf_ptr);
            socket_ptr->send_ptr = HTTPBuffer;
            http_clear_in_use_flag(socket_ptr);
            MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
            }
#endif      /* (MN_INCLUDE_HEAD) */

         if (http_method != 'P')
            return 1;
         }

      if (http_method == 'P')
         {
         BODYlen = (word16)(BODYptr - &BODYBuffer[0]);
#ifdef _POST_FILES
         PostBODYlength = BODYlen;                                       // save data length for function use
#endif
         if (BODYlen == 0)
            {
            MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
            mn_http_set_message(socket_ptr,HTTPStatus400,STATUS_400_LEN);
            return 0;
            }
         BODYptr = &BODYBuffer[0];
         URIptr++;                           /* skip past leading / */
         MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
         post_func = mn_pf_get_entry(URIptr);
         if (post_func == PTR_NULL)
            {
            mn_http_set_message(socket_ptr,HTTPStatus400,STATUS_400_LEN);
            return 0;
            }
         /* called function must parse BODYptr and call mn_http_set_file. */
         (*post_func)(socket_ptr);
         return 1;
         }

         MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
         /* bad method */
         mn_http_set_message(socket_ptr,HTTPStatus501,STATUS_501_LEN);
      }

   return 0;
}

/* prepare to send HTTP message */
void mn_http_set_message(PSOCKET_INFO socket_ptr, PCONST_BYTE msg, word16 len)
cmx_reentrant {
   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   socket_ptr->send_ptr = &HTTPBuffer[0];
   socket_ptr->send_len = len;
   page_send_info[socket_ptr->socket_no].page_ptr = msg;
   socket_ptr->user_data = (void *)PTR_NULL;
   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
}

/* sets up a file pointed to by vf_ptr to be sent by the http server.
   returns 1 if able to send the file, otherwise returns 0.
*/
byte mn_http_set_file(PSOCKET_INFO socket_ptr, VF_PTR vf_ptr)
cmx_reentrant {
   byte retval;
   const char * open_mode = "r";
   word16 header_len;
   PAGE_SEND_PTR ps_ptr;

   retval = 0;

   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   if (http_file_ok_to_send(vf_ptr) && (mn_vf_open_file(vf_ptr,open_mode) == 1))
      {
      http_set_in_use_flag(socket_ptr,vf_ptr);

#if (RTOS_USED == RTOS_NONE)
#if MN_SERVER_SIDE_INCLUDES
      if (!(vf_ptr->page_type & VF_PTYPE_HTML))
#endif
         {
         /* create HTTP header. */
         header_len = MakeHTTPHeader(vf_ptr);
         }
#if MN_SERVER_SIDE_INCLUDES
      else
         header_len = 0;
#endif
#else
      header_len = 0;
#endif

      socket_ptr->send_ptr = &HTTPBuffer[0];
      socket_ptr->send_len = vf_ptr->page_size + header_len;
      ps_ptr = &page_send_info[socket_ptr->socket_no];
      ps_ptr->header_bytes = header_len;
      if (vf_ptr->page_type & VF_PTYPE_RAM)
         ps_ptr->ram_page_ptr = vf_ptr->ram_page_ptr;
      else
         {
#if CMX_FFS_USED
         if (vf_ptr->page_type & VF_PTYPE_EFFS)
            ps_ptr->vf_ptr = vf_ptr;
         else
#endif      /* CMX_FFS_USED */
            ps_ptr->page_ptr = vf_ptr->page_ptr;
         }
      retval = 1;
      }

   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
   return (retval);
}

/* find the value of the match in the search_ptr and copies it into buff.
   Returns 1 if a value found otherwise returns 0.  Looks for match=value
*/
int mn_http_find_value(byte *search_ptr, byte *match, byte *buff)
cmx_reentrant {
   int retval;
   byte* match_ptr;

   match_ptr = match;
   retval = 0;

   while (1)
      {
      /* If end of data, no match, so leave */
      if (*search_ptr == 0)
         break;
      /* else if chars match, continue */
      else if( *search_ptr == *match_ptr)
         {
         search_ptr++;
         match_ptr++;
         }
      /* else if "=" and end of match string, match found; put value in buff */
      else if ((*search_ptr == '=')  &&  (*match_ptr == 0))
         {
         ++search_ptr;
         mn_http_decodeString(search_ptr,buff);
         retval = 1;
         break;
         }
      /* else chars don't match, so move to next name-value pair
         or end of data
      */
      else
         {
         for (; *search_ptr != 0 && *search_ptr != '&'; search_ptr++) ;
         /* if not end of data bump past "&" */
         if (*search_ptr != 0)
            {
            search_ptr++;
            match_ptr = match;
            }
         }
      }  /* end while forever */

   return (retval);
}

/* copy str into buff while decoding escaped chars */
void mn_http_decodeString(byte *str, byte *buff)
cmx_reentrant {
   byte * tmp_ptr;
   byte num;
   byte num2;

   tmp_ptr = str;

   while ((*tmp_ptr != '\r') && (*tmp_ptr != '&') && *tmp_ptr)
      {
      if (*tmp_ptr == '+')
         {
         *buff++ = ' ';
         tmp_ptr++;
         }
      else if (*tmp_ptr == '%')
         {
         /* %XX is hex representation of an escaped char.
            e.g. we are going to replace %2C with ','
         */
         num = (byte)(*(tmp_ptr+1) - '0');
         num2 = (byte)(*(tmp_ptr+2) - '0');
         if (num > 0x09)
            num -= 7;
         if (num2 > 0x09)
            num2 -= 7;
         num *= 16;
         num = num + num2;
         *buff++ = num;
         tmp_ptr += 3;
         }
      else
         *buff++ = *tmp_ptr++;
      }
   *buff = '\0';
}

/* makes http header for response to http head request */
static word16 MakeHTTPHeader(VF_PTR vf_ptr)
cmx_reentrant {
   word16 len;
   byte * ptr;

   ptr = &HTTPBuffer[0];

   /* In future handle server-side-includes to get correct length for .htm */

   (void)mn_strcpy_cb(ptr,head_1);
   copyContentType(ptr,vf_ptr->filename);
   (void)mn_strcat_cb(ptr,head_2);
#if MN_USE_LONG_FSIZE
   (void)mn_ultoa(ptr + strlen((char *)ptr),vf_ptr->page_size);
#else
   (void)mn_ustoa(ptr + strlen((char *)ptr),vf_ptr->page_size);
#endif      /* MN_USE_LONG_FSIZE */
   (void)mn_strcat_cb(ptr,head_3);
   (void)mn_strcat_cb(ptr,head_4);

   len = (word16)(strlen((char *)ptr));

   return (len);
}

static void copyContentType(byte *buff, byte *filename)
cmx_reentrant {
   byte i;

   while (*filename && (*filename != '.'))     /* look for . */
      ++filename;

   if (*filename == '.')
      {
      filename++;
      for (i = 0; i < NUM_TYPES; i++)
         {
         if (mn_stricmp_cb(filename,content_types[i].extension) == 0)
            break;
         }

      if (i >= NUM_TYPES)
         i = 0;            /* default type */

      (void)mn_strcat_cb(buff,content_types[i].mime_type);
      }
}

/* checks if the entry pointed to by vf_ptr is flagged as being IN_USE by
   FTP. returns TRUE if not in use, FALSE if in use or deleted.
*/
static byte http_file_ok_to_send(VF_PTR vf_ptr)
cmx_reentrant {
#if (MN_FTP_SERVER || MN_FTP_CLIENT)
   return (byte)( vf_ptr != PTR_NULL && vf_ptr->filename[0] != '\0' && \
      !(vf_ptr->in_use_flag & VF_PAGE_IN_USE_FTP) );
#else
   return (byte)( vf_ptr != PTR_NULL && vf_ptr->filename[0] != '\0');
#endif      /* (MN_FTP_SERVER || MN_FTP_CLIENT) */
}

/* set a flag so the vf entry doesn't get deleted by FTP while we are
   sending it.
*/
static void http_set_in_use_flag(PSOCKET_INFO socket_ptr, VF_PTR vf_ptr)
cmx_reentrant {
   if ((vf_ptr != PTR_NULL) && (socket_ptr != PTR_NULL))
      {
      vf_ptr->in_use_flag |= VF_PAGE_IN_USE_HTTP;
      socket_ptr->user_data = (void *)vf_ptr;
      }
}

/* clear the in_use_flag set by http_set_in_use_flag() */
static void http_clear_in_use_flag(PSOCKET_INFO socket_ptr)
cmx_reentrant {
   SCHAR socket_no;
   VF_PTR vf_ptr;
   int i;

   if (socket_ptr != PTR_NULL)
      {
      socket_no = socket_ptr->socket_no;
      vf_ptr = (VF_PTR)socket_ptr->user_data;

      if (vf_ptr != PTR_NULL)
         {
         /* don't clear flag if any other http sockets are using this
            vf entry.
         */
         for (i = 0; i < MN_NUM_SOCKETS; i++)
            {
            if (i == socket_no)     /* don't count current socket */
               continue;

            if (sock_info[i].user_data == (void *)vf_ptr)
               return;
            }

         vf_ptr->in_use_flag &= (byte)(~VF_PAGE_IN_USE_HTTP);
         socket_ptr->user_data = (void *)PTR_NULL;
         }
      }
}

/* Closes all files opened by HTTP. */
static void http_close_files(void)
cmx_reentrant {
   byte i;

   for (i = 0; i < MN_NUM_SOCKETS; i++)
      mn_http_close_file(i);
}

void mn_http_close_file(SCHAR socket_no)
cmx_reentrant {
   VF_PTR vf_ptr;

   if (sock_info[socket_no].user_data != (void *)PTR_NULL)
      {
      CLEAR_SENDING(socket_no);
      page_send_close(socket_no);
      vf_ptr = (VF_PTR)(sock_info[socket_no].user_data);
      vf_ptr->in_use_flag &= (byte)(~VF_PAGE_IN_USE_HTTP);
      sock_info[socket_no].user_data = (void *)PTR_NULL;
      }
}

#if MN_SERVER_SIDE_INCLUDES

word16 mn_http_process_includes(PSOCKET_INFO socket_ptr, word16 bytes_to_send)
cmx_reentrant {
   SCHAR result;
   word16 retval;
   word16 ssi_bytes;
   VF_PTR vf_ptr;

   retval = 0;

   MN_MUTEX_WAIT(MUTEX_HTTP_SERVER,INFINITE_WAIT);
   HTTPBufferptr = &HTTPBuffer[0];
   vf_ptr = (VF_PTR)socket_ptr->user_data;

   if (vf_ptr->page_type & VF_PTYPE_RAM)
      result = process_ram_includes(socket_ptr,bytes_to_send);
#if CMX_FFS_USED
   else if (vf_ptr->page_type & VF_PTYPE_EFFS)
      result = process_effs_includes(socket_ptr,bytes_to_send);
#endif      /* CMX_FFS_USED */
   else
      result = process_rom_includes(socket_ptr,bytes_to_send);

   if (result > 0)
      {
      /* make sure we come back for the left over chars */
      ssi_bytes = temp_ssi_info[socket_ptr->socket_no].num_ssi_bytes;
      if (ssi_bytes)
         {
         if (ssi_bytes > MN_HTTP_BUFFER_LEN)
            socket_ptr->send_len += MN_HTTP_BUFFER_LEN;
         else
            socket_ptr->send_len += ssi_bytes;
         }
      socket_ptr->send_ptr = HTTPBuffer;
      retval = ((word16)(HTTPBufferptr - &HTTPBuffer[0]));
      }

   MN_MUTEX_RELEASE(MUTEX_HTTP_SERVER);
   return (retval);
}

static SCHAR process_ram_includes(PSOCKET_INFO socket_ptr, word16 bytes_to_send)
cmx_reentrant {
   byte c;
   word16 bytes_left;
   byte *MsgSendPointer;
   PAGE_SEND_PTR ps_ptr;

   ps_ptr = &page_send_info[socket_ptr->socket_no];
   MsgSendPointer = ps_ptr->ram_page_ptr;

   bytes_left = copy_leftovers(socket_ptr,&bytes_to_send);

   while (bytes_left && bytes_to_send)
      {
      c = *MsgSendPointer;
      MsgSendPointer++;
      bytes_to_send--;
      if (process_include_char(socket_ptr,c,&bytes_left) < 0)
         return (GET_FUNC_ERROR);
      }

   ps_ptr->bytes_read = (word16)(MsgSendPointer - ps_ptr->ram_page_ptr);
   return (1);
}

static SCHAR process_rom_includes(PSOCKET_INFO socket_ptr, word16 bytes_to_send)
cmx_reentrant {
   byte c;
   word16 bytes_left;
   PCONST_BYTE MsgSendPointer;
   PAGE_SEND_PTR ps_ptr;


   ps_ptr = &page_send_info[socket_ptr->socket_no];
   MsgSendPointer = ps_ptr->page_ptr;

   bytes_left = copy_leftovers(socket_ptr,&bytes_to_send);

   while (bytes_left && bytes_to_send)
      {
      c = *MsgSendPointer;
      MsgSendPointer++;
      bytes_to_send--;
      if (process_include_char(socket_ptr,c,&bytes_left) < 0)
         return (GET_FUNC_ERROR);
      }

   ps_ptr->bytes_read = (word16)(MsgSendPointer - ps_ptr->page_ptr);
   return (1);
}

#if CMX_FFS_USED
static SCHAR process_effs_includes(PSOCKET_INFO socket_ptr, word16 bytes_to_send)
cmx_reentrant {
   byte c;
   SCHAR retval;
   word16 bytes_left, num_bytes;
   byte * MsgSendPointer;
   long filepos;
   F_FILE * file_ptr;
   PAGE_SEND_PTR ps_ptr;

   ps_ptr = &page_send_info[socket_ptr->socket_no];
   file_ptr = ps_ptr->vf_ptr->effs_file_ptr;

   bytes_left = copy_leftovers(socket_ptr,&bytes_to_send);

   retval = vf_set_filepos(ps_ptr->vf_ptr);
   if (retval < 0)
      return(VFILE_SEEK_FAILED);
   else if (retval == 0)
      {
      /* save current file position */
      filepos = f_tell(file_ptr);
      }
   else
      filepos = ps_ptr->vf_ptr->filepos;

   /* read from effs into temporary buffer */
   MsgSendPointer = &temp_http_buff[0];
   if ((word16)F_READ_LEN(MsgSendPointer,bytes_to_send,1,file_ptr) != bytes_to_send)
      return(VFILE_READ_FAILED);

   num_bytes = bytes_to_send;

   while (bytes_left && bytes_to_send)
      {
      c = *MsgSendPointer;
      MsgSendPointer++;
      bytes_to_send--;
      if (process_include_char(socket_ptr,c,&bytes_left) < 0)
         return (GET_FUNC_ERROR);
      }

   ps_ptr->bytes_read = MsgSendPointer - &temp_http_buff[0];

   /* adjust file pointer, if necessary, to reflect the actual number of
      characters we processed.
   */
   if (ps_ptr->bytes_read < num_bytes)
      {
      if (f_seek(file_ptr, (filepos+ps_ptr->bytes_read), F_SEEK_SET) != 0)
         return(VFILE_SEEK_FAILED);
      }
   return (1);
}
#endif      /* CMX_FFS_USED */

static SCHAR process_include_char(PSOCKET_INFO socket_ptr,byte c,word16 *pbytes_left)
cmx_reentrant {
   byte i;
   TEMP_SSI_S *tsi_ptr;

   tsi_ptr = &temp_ssi_info[socket_ptr->socket_no];

   /* we are looking for <!--#exec cgi="somefunc"-->
      If we find it we will replace that string with the value from somefunc.
   */
   if (!tsi_ptr->in_cgi)
      {
      if (c == '<')        /* start of cgi tag */
         {
         tsi_ptr->in_cgi = TRUE;
         tsi_ptr->cgi_pos = 0;
         }
      else
         {
         /* not in a cgi tag, so just copy the char */
         *HTTPBufferptr = c;
         HTTPBufferptr++;
         *pbytes_left -= 1;
         }
      }

   if (tsi_ptr->in_cgi)
      {
      if ((c == '>') && (tsi_ptr->cgi_pos >= MIN_CGI_LEN))     /* end of cgi tag */
         {
         tsi_ptr->temp_cgi_buff[tsi_ptr->cgi_pos-3] = '\0';    /* mark end of func name */
         tsi_ptr->in_cgi = FALSE;
         if (process_get_func(socket_ptr,pbytes_left) < 0)
            return (GET_FUNC_ERROR);
         }
      else if ( ((cgi_tag[tsi_ptr->cgi_pos] == c) || \
            (tsi_ptr->cgi_pos >= CGI_TAG_LEN)) && \
            (tsi_ptr->cgi_pos < TEMP_CGI_BUFF_LEN) )
         {
         /* the current tag still matches the cgi tag so copy the
            char to a temp buffer.
         */
         tsi_ptr->temp_cgi_buff[tsi_ptr->cgi_pos] = c;
         tsi_ptr->cgi_pos++;
         }
      else
         {
         /* the current tag does not match the cgi tag so put the chars
            placed so far in the temp buffer into the HTTPBuffer.
         */
         if (*pbytes_left > tsi_ptr->cgi_pos)
            {
            *pbytes_left = *pbytes_left - (tsi_ptr->cgi_pos+1);
            i = 0;
            while (tsi_ptr->cgi_pos)
               {
               *HTTPBufferptr = tsi_ptr->temp_cgi_buff[i];
               HTTPBufferptr++;
               i++;
               tsi_ptr->cgi_pos -= 1;
               }
            *HTTPBufferptr = c;
            HTTPBufferptr++;
            }
         else
            {
            /* not enough room to fit the entire tag into the HTTPBuffer
               so fit what we can and put the rest in via the copy_leftovers
               function next time.
            */
            tsi_ptr->temp_cgi_buff[tsi_ptr->cgi_pos] = c;
            i = 0;
            while (*pbytes_left)
               {
               tsi_ptr->cgi_pos -= 1;
               *HTTPBufferptr = tsi_ptr->temp_cgi_buff[i];
               HTTPBufferptr++;
               i++;
               *pbytes_left -= 1;
               }
            tsi_ptr->num_ssi_bytes = tsi_ptr->cgi_pos + 1;
            tsi_ptr->temp_ssi_ptr = &(tsi_ptr->temp_cgi_buff[i]);
            }
         tsi_ptr->in_cgi = FALSE;
         }
      }

   return (1);
}

static SCHAR process_get_func(PSOCKET_INFO socket_ptr, word16 *pbytes_left)
cmx_reentrant {
   GET_FP get_func;
   TEMP_SSI_S *tsi_ptr;

   tsi_ptr = &temp_ssi_info[socket_ptr->socket_no];
   tsi_ptr->num_ssi_bytes = 0;

   get_func = mn_gf_get_entry(&(tsi_ptr->temp_cgi_buff[CGI_TAG_LEN]));
   if (get_func == PTR_NULL)
      {
/*      mn_http_set_message(socket_ptr,HTTPStatus400,STATUS_400_LEN); */
      socket_ptr->send_ptr = &HTTPBuffer[0];
      socket_ptr->send_len = STATUS_400_LEN;
      page_send_info[socket_ptr->socket_no].page_ptr = HTTPStatus400;
      socket_ptr->user_data = (void *)PTR_NULL;

      (void)mn_memcpy_cb(socket_ptr->send_ptr, HTTPStatus400, STATUS_400_LEN);
      return (GET_FUNC_ERROR);
      }

   /* get pointer to buffer filled in by user function */
   tsi_ptr->num_ssi_bytes = (*get_func)(&(tsi_ptr->temp_ssi_ptr));
   if (tsi_ptr->num_ssi_bytes && (tsi_ptr->temp_ssi_ptr != PTR_NULL))
      {
      /* copy from user buffer to HTTPBuffer up to end of HTTPBuffer */
      if (tsi_ptr->num_ssi_bytes <= *pbytes_left)
         {
         /* we can fit the entire ssi in the rest of the HTTPBuffer */
         *pbytes_left -= tsi_ptr->num_ssi_bytes;
         while (tsi_ptr->num_ssi_bytes)
            {
            *HTTPBufferptr = *(tsi_ptr->temp_ssi_ptr);
            HTTPBufferptr++;
            tsi_ptr->temp_ssi_ptr++;
            tsi_ptr->num_ssi_bytes--;
            }
         }
      else
         {
         /* the entire ssi won't fit so copy what we can and save the rest */
         while (*pbytes_left)
            {
            *HTTPBufferptr = *(tsi_ptr->temp_ssi_ptr);
            HTTPBufferptr++;
            tsi_ptr->temp_ssi_ptr++;
            tsi_ptr->num_ssi_bytes--;
            *pbytes_left -= 1;
            }
         }
      }

   return (1);
}

static word16 copy_leftovers(PSOCKET_INFO socket_ptr, word16 *pbytes_to_send)
cmx_reentrant {
   word16 bytes_left,len;
   TEMP_SSI_S *tsi_ptr;

   tsi_ptr = &temp_ssi_info[socket_ptr->socket_no];
   bytes_left = MN_HTTP_BUFFER_LEN;

   if (tsi_ptr->num_ssi_bytes)
      {
      /* take care of the chars left over from the last time */
      if (tsi_ptr->num_ssi_bytes > MN_HTTP_BUFFER_LEN)
         len = MN_HTTP_BUFFER_LEN;
      else
         len = tsi_ptr->num_ssi_bytes;
      socket_ptr->send_len -= len;
      *pbytes_to_send -= len;
      bytes_left -= len;
      while (len)
         {
         tsi_ptr->num_ssi_bytes -= 1;
         *HTTPBufferptr = *(tsi_ptr->temp_ssi_ptr);
         HTTPBufferptr++;
         tsi_ptr->temp_ssi_ptr++;
         --len;
         }
      }

   return (bytes_left);
}
#endif      /* #if MN_SERVER_SIDE_INCLUDES */

#if (RTOS_USED != RTOS_NONE)
static void http_send_data(PSOCKET_INFO *psocket_ptr) cmx_reentrant;

void mn_http_server_task(void)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
#if (!MN_TCP_DELAYED_ACKS)
   PSOCKET_INFO check_socket_ptr;
#endif
   SCHAR socket_no;
   SCHAR status;
   int i;
   int recvd;
   int retval;

   wait_for_mn_init();

#if CMX_FFS_USED
   F_ENTERFS;
#endif

#if (!MN_TCP_DELAYED_ACKS)
   check_socket_ptr = &sock_info[MN_NUM_SOCKETS];
#endif

   while (1)
      {
      /* Make sure we always have a socket listening on port 80. */
      socket_ptr = mn_find_socket(HTTP_PORT, 0, null_addr, PROTO_TCP, IF_ANY);
      if (socket_ptr == PTR_NULL)
         {
         mn_open(null_addr, HTTP_PORT, 0, NO_OPEN, PROTO_TCP, \
            (HTTP_TYPE | INACTIVITY_TIMER),  PTR_NULL, 0, IF_ANY);
         }

      socket_ptr = (PSOCKET_INFO)PTR_NULL;
      for (i=0; i<MN_NUM_SOCKETS; i++)
         {
         if (SOCKET_ACTIVE(i) && (sock_info[i].src_port == HTTP_PORT))
            {
            socket_no = sock_info[i].socket_no;
            if (MN_SIGNAL_GET(signal_socket[socket_no]) == SIGNAL_SUCCESS)
               {
               socket_ptr = MK_SOCKET_PTR(socket_no);

               MN_TASK_LOCK;
               /* recvd is return value from mn_tcp_recv() */
               recvd = socket_ptr->last_return_value;
               MN_TASK_UNLOCK;

               retval = 0;

               if ((recvd >= 0) || (recvd == NEED_TO_LISTEN) || \
                     (recvd == TCP_NO_CONNECT))
                  retval = mn_http_server_recv(&socket_ptr);

               if ((recvd >= 0) || (retval == NEED_TO_SEND))
                  http_send_data(&socket_ptr);
               }
            }
         }

      if (socket_ptr == PTR_NULL)   /* No packets were found above. */
         {
         check_http_parse(&socket_ptr);

#if (!MN_TCP_DELAYED_ACKS)
         /* Check if the socket pointed to by check_socket_ptr has bytes
            we need to resend. If it does, try a resend. Increment
            check_socket_ptr so that another socket will be checked the next
            time around.
         */
         check_socket_ptr++;
         if (check_socket_ptr >= &sock_info[MN_NUM_SOCKETS])
            check_socket_ptr = &sock_info[0];
         if (check_socket_ptr->src_port == HTTP_PORT)
            {
            if (TCP_TIMER_EXPIRED(check_socket_ptr) && \
                  (check_socket_ptr->send_len || \
                  (check_socket_ptr->tcp_state >= TCP_FIN_WAIT_1)))
               {
               socket_ptr = check_socket_ptr;
               http_send_data(&socket_ptr);
               continue;
               }
            }
#endif

         /* Call the callback to allow application to do something. */
         status = mn_app_server_idle(&socket_ptr);
         if (status == NEED_TO_SEND)
            http_send_data(&socket_ptr);
         else
            MN_TASK_WAIT(1);
         }
      }
}

static void http_send_data(PSOCKET_INFO *psocket_ptr)
cmx_reentrant {
   PSOCKET_INFO socket_ptr;
   int sent;

   if (psocket_ptr != PTR_NULL)
      {
      socket_ptr = *psocket_ptr;

      if (socket_ptr != PTR_NULL)
         {
send_data:
         mn_http_server_set_flag(socket_ptr);
         sent = (int)mn_tcp_send(socket_ptr);
#if (MN_TCP_DELAYED_ACKS)
         if ((sent != UNABLE_TO_SEND) && (sent != UNABLE_TO_TCP_SEND))
#else
         if (sent != UNABLE_TO_SEND)
#endif
            mn_http_server_send(psocket_ptr,sent);

#if (MN_TCP_DELAYED_ACKS)
         if ((sent > 0) && (socket_ptr->packet_count < 2) && \
               (socket_ptr->send_len > socket_ptr->tcp_unacked_bytes))
            {
            /* If there is something left to send, and we have only sent
               one packet so far, then send another packet.
            */
            goto send_data;
            }
#endif
         }
      }
}

#endif      /* (RTOS_USED != RTOS_NONE) */
#endif   /* MN_HTTP */
