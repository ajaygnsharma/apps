/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#if !defined(_SMTP_H)
#define _SMTP_H   1

typedef struct smtp_info_s {
   byte *from;
   byte *to;
   byte *subject;
   byte *message;
   byte *attachment;
   byte *filename;      /* for attachment */
#if (MN_SNTP)
   /* The following holds the data for a time lookup. */
   byte ip_sntp_addr[IP_ADDR_LEN];
   int utc_offset;
   byte daylight_saving;
#endif
} SMTP_INFO_T;
typedef SMTP_INFO_T * PSMTP_INFO;

void mn_smtp_init(void) cmx_reentrant;
SCHAR mn_smtp_start_session(word16, SCHAR, char *, char *) cmx_reentrant;
void mn_smtp_end_session(SCHAR) cmx_reentrant;
int mn_smtp_send_mail(SCHAR, PSMTP_INFO) cmx_reentrant;

#define SMTP_RECV_LEN            4
#define SMTP_REPLY_LEN           3

/* reply codes from SMTP server */
#define SMTP_START_OK            220
#define SMTP_QUIT_OK             221
#define SMTP_AUTH_OK             235
#define SMTP_OK                  250
#define SMTP_USER_NOT_LOCAL_OK   251
#define SMTP_AUTH_CONTINUE       334
#define SMTP_START_SENDING_DATA  354
#define SMTP_SHUTTING_DOWN       421
#define SMTP_MAILBOX_UNAVAILABLE 450
#define SMTP_LOCAL_ERR           451
#define SMTP_NO_STORAGE          452
#define SMTP_BAD_CMD_SEQUENCE    503
#define SMTP_BAD_PARAM           504
#define SMTP_CMD_REJECTED        550
#define SMTP_USER_NOT_LOCAL_ERR  551
#define SMTP_EXCEEDED_STORAGE    552
#define SMTP_BAD_MAILBOX_NAME    553
#define SMTP_FAILURE             554

/* SMTP states */
#define SMTP_CLOSED              0
#define SMTP_OPEN                1
#define SMTP_HELO                2
#define SMTP_LOGIN               3
#define SMTP_MAIL                4
#define SMTP_RCPT                5
#define SMTP_DATA_1              6
#define SMTP_DATA_2              7
#define SMTP_DATA_3              8
#define SMTP_QUIT                9

#define BASE64_MAX_LINE_LEN   64

#endif /* _SMTP_H */
