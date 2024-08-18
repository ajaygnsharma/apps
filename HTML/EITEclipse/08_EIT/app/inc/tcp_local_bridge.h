#ifndef tcp_local_bridge
#define tcp_local_bridge		1

SCHAR SerialToEthernet_PktLen(PSOCKET_INFO socket_ptr) cmx_reentrant;
SCHAR SerialToEthernet_Endchar(PSOCKET_INFO socket_ptr) cmx_reentrant;
SCHAR SerialToEthernet_BufferingTime(PSOCKET_INFO socket_ptr) cmx_reentrant;
SCHAR SerialToEthernet(PSOCKET_INFO socket_ptr) cmx_reentrant;
SCHAR EthernetToSerial_EndChar(void) cmx_reentrant;
SCHAR EthernetToSerial_PktLen(void) cmx_reentrant;
SCHAR EthernetToSerial_BufferingTime(void) cmx_reentrant;
SCHAR EthernetToSerial(void) cmx_reentrant;
												  
#endif
