#ifndef SSI_MANAGEMENT
#define SSI_MANAGEMENT		1

word16 ssi_smtp(byte **str_) cmx_reentrant ;
word16 ssi_smtpip(byte **str_) cmx_reentrant ;
word16 ssi_from(byte **str_) cmx_reentrant ;
word16 ssi_sub(byte **str_) cmx_reentrant ;
word16 ssi_snmp_en(byte **str_) cmx_reentrant ;
word16 ssi_trap_en(byte **str_) cmx_reentrant ;
word16 ssi_trap_ip(byte **str_) cmx_reentrant ;
word16 ssi_contact(byte **str_) cmx_reentrant ;
word16 ssi_location(byte **str_) cmx_reentrant ;
word16 ssi_community(byte **str_) cmx_reentrant;

#endif
