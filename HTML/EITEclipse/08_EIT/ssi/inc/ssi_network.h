#ifndef MICRONET_ssi_network
#define MICRONET_ssi_network		1

word16 dhcpflag_func(byte **str_) cmx_reentrant;     /*SSI:DHCP Flag    */
word16 ssi_protocol(byte **str_) cmx_reentrant;      /*SSI: Protocol    */
word16 autonegotiate_func(byte **str_) cmx_reentrant;/*SSI:Autonegotiate*/
word16 s10_func(byte **str_) cmx_reentrant;          /*SSI: Speed 10    */
word16 s100_func(byte **str_) cmx_reentrant;         /*SSI: Speed 100   */
word16 hduplex_func(byte **str_) cmx_reentrant;      /*SSI: Half Duplex */
word16 fduplex_func(byte **str_) cmx_reentrant;      /*SSI: Full Duplex */
word16 WebPort_func(byte **str_) cmx_reentrant;  //wendy
#endif
