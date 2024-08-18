#ifndef MICRONET_ssi_diagnostics
#define MICRONET_ssi_diagnostics		1

word16 ssi_fullserial(byte **str_) cmx_reentrant;
word16 ssi_cts(byte **str_) cmx_reentrant;
word16 ssi_rts(byte **str_) cmx_reentrant;
word16 ssi_dsr(byte **str_) cmx_reentrant;
word16 ssi_dcd(byte **str_) cmx_reentrant;
word16 ssi_dtr(byte **str_) cmx_reentrant;
word16 ssi_rx_bytes(byte **str_) cmx_reentrant;
word16 ssi_tx_bytes(byte **str_) cmx_reentrant;
word16 ssi_tcp_rx_bytes(byte **str_) cmx_reentrant;
word16 ssi_tcp_tx_bytes(byte **str_) cmx_reentrant;
word16 ssi_udp_rx_bytes(byte **str_) cmx_reentrant;
word16 ssi_udp_tx_bytes(byte **str_) cmx_reentrant;
word16 ssi_icmp_rx_bytes(byte **str_) cmx_reentrant;
word16 ssi_icmp_tx_bytes(byte **str_) cmx_reentrant;

#endif
