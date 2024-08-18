#ifndef MICRONET_ssi_multihost
#define MICRONET_ssi_multihost		1

#define LEN_SELECTED 25                /* Length of selected="selected" */
#define LEN_OPTIONS  80                /* Length of <option value=...>  */
#define SELECTED "selected=\"selected\"" 
#define CT_OPTS 3                      /* Connection type multihost     */

word16 ssi_connection_type(byte **str_)cmx_reentrant;
word16 ssi_retry_counter(byte **str_) cmx_reentrant;
word16 ssi_retry_timeout(byte **str_) cmx_reentrant;
word16 ssi_mipaddr(byte **str_) cmx_reentrant;
word16 ssi_multi_port(byte **str_) cmx_reentrant;

#endif
