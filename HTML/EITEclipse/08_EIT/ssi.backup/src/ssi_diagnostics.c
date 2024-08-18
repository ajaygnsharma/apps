/*------------------------------------------------------------------------ 
					SSI_DIAGNOSTICS.C

Description:	Return SSIs for diagnostics.htm page
				1. Serial RX bytes
                2. 
				3. 
				4. 
				5. 
                6. 

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/
#include "micronet.h"
#include "np_includes.h"

/*------------------------------------------------------------------------
  Function:    ssi_rx_bytes()
  Description: Output the no of bytes recieved on the UART
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_fullserial(byte **str_)
cmx_reentrant
{
  int n;

#if(MODEL_TYPE == EIT_RS232_FULL)
  n = sprintf((char *)temp_buff,"1");
#elif((MODEL_TYPE == EIT_RS232_GPIO) || (MODEL_TYPE == EIT_RS232_TTL))
  n = sprintf((char *)temp_buff,"2");
#elif(MODEL_TYPE == EIT_RS485)
  n = sprintf((char *)temp_buff,"3");
#endif  	
	*str_ = temp_buff;

	return ((word16)n);
}

//#if(MODEL_TYPE == EIT_RS232_FULL)
/*------------------------------------------------------------------------
 Function:    ssi_cts()
 Description: SSI: Display whether the SNMP is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_cts(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  /* U1MSR: Bit 5 is CTS status and is 0: when high and 1 when low */
  switch(U1MSR & 0x10) 
  {
    case 0: strcpy(str,"green.jpg"); break;
    case 1: strcpy(str,"grey.jpg"); break;
    default: strcpy(str,"grey.jpg"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_rts()
 Description: SSI: Display whether the SNMP is enabled or not 
              Enabled/Disabled   
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_rts(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  /* U1MCR: Bit 2 is RTS status and is 0: when high and 1 when low */
  switch(U1MCR & 0x02) 
  {
    case 0: strcpy(str,"green.jpg"); break;
    case 1: strcpy(str,"grey.jpg"); break;
    default: strcpy(str,"grey.jpg"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_dsr()
 Description: SSI: Display whether the DSR Status
              high/low 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_dsr(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  /* U1MSR: Bit 6 is DSR status and is 0: when high and 1 when low */
  switch(U1MSR & 0x20) 
  {
    case 0: strcpy(str,"green.jpg"); break;
    case 1: strcpy(str,"grey.jpg"); break;
    default: strcpy(str,"grey.jpg"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}

/*------------------------------------------------------------------------
 Function:    ssi_dcd()
 Description: SSI: Display whether the DCD Status
              high/low 
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_dcd(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  /* U1MSR: Bit 8 is DCD status and is 0: when high and 1 when low */
  switch(U1MSR & 0x80) 
  {
    case 0: strcpy(str,"green.jpg"); break;
    case 1: strcpy(str,"grey.jpg"); break;
    default: strcpy(str,"grey.jpg"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}


/*------------------------------------------------------------------------
 Function:    ssi_dtr()
 Description: SSI: Display whether the DTR is high or low
              
 Input:       String containing Status
 Output:      Length of Response
------------------------------------------------------------------------*/
word16 ssi_dtr(byte **str_)
cmx_reentrant 
{
  char str[10];
  
  /* U1MCR: Bit 1 is DTR status and is 0: when high and 1 when low */
  switch(U1MCR & 0x01) 
  {
    case 0: strcpy(str,"green.jpg"); break;
    case 1: strcpy(str,"grey.jpg"); break;
    default: strcpy(str,"grey.jpg"); break;
  }

  *str_=(byte *)str;           
  return (strlen((char *)str));
}
//#endif

/*------------------------------------------------------------------------
  Function:    ssi_rx_bytes()
  Description: Output the no of bytes recieved on the UART
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_rx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",serial.rx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_tx_bytes()
  Description: Output the no of bytes transmitted on the UART
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the Time
------------------------------------------------------------------------*/
word16 ssi_tx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",serial.tx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_tcp_rx_bytes()
  Description: Output the no of TCP packets recieved
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the value
------------------------------------------------------------------------*/
word16 ssi_tcp_rx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",network.tcp_rx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_tcp_tx_bytes()
  Description: Output the no of TCP packets sent
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the value
------------------------------------------------------------------------*/
word16 ssi_tcp_tx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",network.tcp_tx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_udp_rx_bytes()
  Description: Output the no of UDP packets recieved
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the value
------------------------------------------------------------------------*/
word16 ssi_udp_rx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",network.udp_rx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_udp_tx_bytes()
  Description: Output the no of UDP packets sent
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the value
------------------------------------------------------------------------*/
word16 ssi_udp_tx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",network.udp_tx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_icmp_rx_bytes()
  Description: Output the no of ICMP packets recieved
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the value
------------------------------------------------------------------------*/
word16 ssi_icmp_rx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",network.icmp_rx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}

/*------------------------------------------------------------------------
  Function:    ssi_icmp_tx_bytes()
  Description: Output the no of ICMP packets sent
  Input:       Pointer to the String that will contain the Value
  Output:      Length of the value
------------------------------------------------------------------------*/
word16 ssi_icmp_tx_bytes(byte **str_)
cmx_reentrant
{
	int n;
	
	n = sprintf((char *)temp_buff,"%d",network.icmp_tx_bytes);
	*str_ = temp_buff;

	return ((word16)n);
}
