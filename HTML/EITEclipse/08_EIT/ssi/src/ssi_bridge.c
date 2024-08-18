/*------------------------------------------------------------------------ 
					SSI_SERIAL.C

Description:	Return SSIs for serial.htm page
				1. 
                2. 
				3. 
				4. 
				5. 
                6. Half Duplex Enabled/Disabled

Author:			Ajay Sharma

Rev:			0

History:					  
------------------------------------------------------------------------*/
#include "micronet.h"
#include "np_includes.h"

#define N_OPTS  6                      /* No of Options for No of Conns */

/*------------------------------------------------------------------------
  Function:    no_of_conns_func()
  Description: Output the No of Connections Selection Menu
  Input:       Pointer to the String Selection Menu String
  Output:      Length of the Menu String
------------------------------------------------------------------------*/
word16 no_of_conns_func(byte **str_)
cmx_reentrant 
{
  int i;
  char tmp[(N_OPTS * LEN_OPTIONS) + LEN_SELECTED + 50 ];
  char str[N_OPTS+2][LEN_OPTIONS];
  char s[N_OPTS][LEN_SELECTED];
  char opt[N_OPTS][10]={"0","1","2","3","4","5"};
  
  memset(s,NULL,(N_OPTS * LEN_SELECTED));       
  	
  switch(terminal_server.no_connections)/* Parity = "selected",else " " */
  {
    case 0:  strcpy(s[0],SELECTED);  break;
    case 1:  strcpy(s[1],SELECTED);  break;
    case 2:  strcpy(s[2],SELECTED);  break;
    case 3:  strcpy(s[3],SELECTED);  break;
    case 4:  strcpy(s[4],SELECTED);  break;
    case 5:  strcpy(s[5],SELECTED);  break;
    default: strcpy(s[0],SELECTED);  break;
  }

  sprintf(str[0],"<select name=\"N\">\r\n");/*str[0]=<select name=\"S\">*/
  strcpy(tmp,str[0]);
  
  for(i=1;i<(N_OPTS+1);i++)
  {
    sprintf(str[i],"<option value=\"%d\" %s> %s </option>\r\n",\
      i, s[i-1],opt[i-1]);   
      /*    str[i] = <option value=i selected="selected"> One </option> */
    strcat(tmp,str[i]);
  }

  sprintf(str[N_OPTS+1],"</select>\r\n");            /* str[4]=</select */
  strcat(tmp,str[N_OPTS+1]);
  
  //printf("%s\n\r",tmp);
  *str_ = (byte *)tmp;
  
  return (strlen((char *)*str_));
}


/*------------------------------------------------------------------------
  Function:    lp_func()
  Description: Output the Terminal Server's Local Port
  Input:       Pointer to the String that will contain the Local port
  Output:      Length of the Port Number
------------------------------------------------------------------------*/
word16 lp_func(byte **str_)
cmx_reentrant 
{
  int n;

  n=sprintf((char *)temp_buff,"%d",terminal_server.local_port);
  *str_=temp_buff;

  return ((word16)n);
}

