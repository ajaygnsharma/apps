#include "micronet.h"
#include "np_includes.h"

byte id_num_buff[2];

//byte frt_tp_f(byte *str,byte i)
//cmx_reentrant
//{
//  switch(i)
//  {
//    case 0:
//	  strcpy((char *)str,(char *)" raw ");
//	  break;
//	case 1:
//	  strcpy((char *)str,(char *)"decimal");
//	  break;
//	default:
//	  break;
//  }
//  return strlen((char *)" raw ");
//}

//
///*------------------------------------------------------------------------
// Function:    MAX_read_func()
// Description: SSI: Show the Maximumm no of devices
// Input:       string
// Output:      string
//------------------------------------------------------------------------*/
//word16 MAX_read_func(byte **str)
//cmx_reentrant 
//{
//  int i,j;
//  j=0;	
//  
//  //eeprom_read(temp_buff,eeprom_chk,eeprom_CHK);
//  temp_buff[0]=device.check;
//  for(i=0;i<MAX_UNITS;i++)
//  {
//    if(temp_buff[0]&0x01)
//	{
//	  if(++j>MAX_READ)
//	  i=MAX_UNITS;
//	}
//	temp_buff[0]=temp_buff[0]>>1;
//  }
//  temp_buff[0]=j+'0';
//  *str=temp_buff;
//  
//  return (1);
//}

/*------------------------------------------------------------------------
 Function:    show_unit_num_func()
 Description: SSI: Show the Unit no on mod_dev.htm
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 show_unit_num_func(byte **str)
cmx_reentrant 
{
  if(mn_http_find_value(BODYptr,(byte *)"N",id_num_buff))
  {
    *str=id_num_buff;
	return 1;
  }
  else
  {
    return 0;
  }
}

///*------------------------------------------------------------------------
// Function:    dev_fmt_func()
// Description: Get the Format of the device
// Input:       string
// Output:      string
//------------------------------------------------------------------------*/
//word16 dev_fmt_func(byte *str,byte opt)
//cmx_reentrant 
//{
//  int i,j,m,temp;
//  byte temp_b[3];
//  
//  j=0;
//  m=0;
//  temp_buff[0]=device.check;
//  //eeprom_read(temp_buff,eeprom_chk,eeprom_CHK);
//  for(i=0;i<MAX_UNITS;i++)
//  {
//    if(opt)
//	{
//	  if(temp_buff[0]&0x01)
//	  {
//	    *(str++)='"';
//		j++;
//		//eeprom_reads(temp_b,eeprom_ufrt(i),eeprom_FRT);
//        strcpy((char *)temp_b,device.format);
//		temp=temp_b[0]-'0';
//		frt_tp_f(str,temp);
//		str +=strlen((char *)" raw ");
//		j+=strlen((char *)" raw ");
//		*(str++)='"';
//		j++;
//		*(str++)=',';
//		j++;
//		if(++m>MAX_READ)
//		i=MAX_UNITS;
//	  }
//	  temp_buff[0]=temp_buff[0]>>1;
//	}
//	else
//	{
//	  *(str++)='"';
//	  j++;
//	  //eeprom_reads(temp_b,eeprom_ufrt(i),eeprom_FRT);
//			temp=temp_b[0]-'0';
//			frt_tp_f(str,temp);
//			str +=strlen((char *)" raw ");
//			j+=strlen((char *)" raw ");
//			
//			*(str++)='"';
//			j++;
//			*(str++)=',';
//			j++;
//		}
//	}
//	*(str-1)=' ';
//	return (j-1);
//}


/*------------------------------------------------------------------------
 Function:    show_cmd_func()
 Description: Show the Command of the devices
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 show_cmd_func(byte **str)
cmx_reentrant 
{
  byte i;
  
  i=id_num_buff[0]-'0'-1;
  strcpy((char *)temp_buff,device.command[i]);
  *str=temp_buff;
  
  return ((word16)strlen((char *)temp_buff));
}

/*------------------------------------------------------------------------
 Function:    show_name_func()
 Description: Show the Name of the device
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 show_name_func(byte **str)
cmx_reentrant 
{
  byte i;
  i=id_num_buff[0]-'0'-1;
  strcpy((char *)temp_buff,device.name[i]);
  *str=temp_buff;
  return ((word16)strlen((char *)temp_buff));
}



///*------------------------------------------------------------------------
// Function:    dev_scmd_func()
// Description: Send its Command to each device 
// Input:       Command and the Device No
// Output:      string
//------------------------------------------------------------------------*/
//word16 dev_scmd_func(byte *str,byte opt)
//cmx_reentrant 
//{
//  int i,j,m,temp, k;
//  
//  j=0;
//  m=0;
//  //eeprom_read(temp_buff,eeprom_chk,eeprom_CHK);
//  temp_buff[0]=device.check;
//
//  for(i=0;i<MAX_UNITS;i++)
//  {
//    if(opt)
//	{
//	  if(temp_buff[0]&0x01)
//	  {
//	    *(str++)='"';
//		j++;
//		//sent_unit_cmd(i);
//		//init_rcom_buffs();            Need to be here?
//		k=0;
//		if(Serial_in(str,comm_tout))// wait for response and new message will be stored in msg_buff
//		{
//		  temp=(word16)strlen((char *)str);
//		  while(k++<temp-1)//ver 4.0 functions char. can not be displayed
//		  {
//		    if(*(str+k)<' ')
//			*(str+k)=' ';
//		  }			
//		  str +=temp;
//		  j +=temp;
//		}
//		*(str++)='"';
//		j++;
//		*(str++)=',';
//		j++;
//		if(++m>MAX_READ)
//		i=MAX_UNITS;
//	  }
//	  temp_buff[0]=temp_buff[0]>>1;
//	}
//	else
//	{
//	  *(str++)='"';
//	  j++;
//	  //sent_unit_cmd(i);
//	  //init_rcom_buffs();   -- Need to be here
//	  if(Serial_in(str,comm_tout))// wait for response and new message will be stored in msg_buff
//	  {		
//	    str +=strlen((char *)str);
//		j +=strlen((char *)str);
//	  }
//      *(str++)='"';
//	  j++;
//	  *(str++)=',';
//	  j++;
//	}
//  }
//  *(str-1)=' ';
//  
//  return (j);
//}


/*------------------------------------------------------------------------
 Function:    show_units_func()
 Description: Show the unit of the Device
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 show_units_func(byte **str)
cmx_reentrant 
{
  byte i;
  
  i=id_num_buff[0]-'0'-1;
  strcpy((char *)temp_buff,device.unit[i]);
  *str=temp_buff;
  
  return ((word16)strlen((char *)temp_buff));
}

//word16 timeout_select_func(byte **str_)
//cmx_reentrant 
//{
//  word16 i,j;
//  byte *str;
//  byte read_tmout_0[]="<input type=text name=T size=3 value=";
//  byte read_tmout_1[]="<input type=submit value=\"Auto update\">";
//  byte read_tmout_2[]="setTimeout(\"AutoReload()\",";
//
//  *str_=cgi_buff;
//  str=cgi_buff;
//
//  if((np_atoi((char *)timeout_buff))>0)
//  {
//    strcpy((char *)str,(char *)read_tmout_2);
//	i=((word16)strlen((char *)read_tmout_2));		
//  }
//  else
//  {
//    memset(timeout_buff,'0',3);
//	timeout_buff[3] = '\0';
//	strcpy((char *)str,(char *)read_tmout_0);
//	i=((word16)strlen((char *)read_tmout_0));
//	str+=i;
//	j=reload_timeout_func(str);
//	str+=j;
//	i+=j;
//	*str++='>';
//	i++;
//	strcpy((char *)str,(char *)read_tmout_1);
//	i+=((word16)strlen((char *)read_tmout_1));
//  }
//  return i;
//}
//

//word16 reload_timeout_func(byte *str)
//cmx_reentrant 
//{
//  if((np_atoi((char *)timeout_buff))<5 && (np_atoi((char *)timeout_buff))>0)
//  {
//    timeout_buff[0]='5';
//	timeout_buff[1]='\0';
//  }
//  strcpy((char *)str,(char *)timeout_buff);
//  
//  return ((word16)strlen((char *)timeout_buff));
//}

///*------------------------------------------------------------------------
// Function:    load_timeout_func()
// Description: Send command to the Device
// Input:       Device number
// Output:      None
//------------------------------------------------------------------------*/
//word16 load_timeout_func(byte **str)
//cmx_reentrant 
//{
//  if((np_atoi((char *)timeout_buff))<5 && (np_atoi((char *)timeout_buff))>0)
//  {
//    timeout_buff[0]='5';
//	timeout_buff[1]='\0';
//  }
//  
//  *str=timeout_buff;
//  return ((word16)strlen((char *)timeout_buff));
//}

//word16 load_on_func(byte **str_)
//cmx_reentrant
//{
//  word16 i,j;
//  byte *str;
//  byte read_tmout_3[]="setTimeout(\"AutoReload()\",";
//  byte read_tmout_4[]="000);";
//    
//    *str_=cgi_buff;
//	str=cgi_buff;
//	i=0;
//
//	if((np_atoi((char *)timeout_buff))>0)
//	{
//		strcpy((char *)str,(char *)read_tmout_3);
//		i=((word16)strlen((char *)read_tmout_3));
//		str+=i;
//		j=reload_timeout_func(str);
//		i+=j;
//		str+=j;
//		strcpy((char *)str,(char *)read_tmout_4);
//		i+=((word16)strlen((char *)read_tmout_4));
//	}
//	else
//	{
//		i=0;
//	}
//	memset(timeout_buff,'0',3);
//	timeout_buff[3] = '\0';
//	return i;
//}


///*------------------------------------------------------------------------
// Function:    dev_read_func()
// Description: SSI: To Set up a table that reads continously the reading 
//              of each device.
// Input:       string
// Output:      string
//------------------------------------------------------------------------*/
//word16 dev_read_func(byte **str_)
//cmx_reentrant 
//{
//  int i,j,temp;
//  byte *str;
//  byte para_header[]="var dev_n";
//  byte para_header0[]="= new Array( ";
//
//  *str_=cgi_buff;
//  str=cgi_buff;
//  j=0;
//
//  for (i=0;i<5;i++)
//  {
//    strcpy((char *)str,(char *)para_header);
//	temp=(word16)strlen((char *)para_header);
//	str +=temp;
//	j +=temp;
//	*(str++)=i+'a';
//	j++;
//	strcpy((char *)str,(char *)para_header0);
//	temp=(word16)strlen((char *)para_header0);
//	str +=temp;
//	j +=temp;
//	temp = dev_rcall_func(str,i);
//	str += temp;
//	j += temp;
//	*(str++)= ')';
//	j++;
//	*(str++)= ';';
//	j++;
//	*(str++)= 0x0a;
//	j++;
//  }
//  return (j);
//}
//
//

///*------------------------------------------------------------------------
// Function:    dev_rcall_func()
// Description: SSI: To Set up a table that reads continously the reading 
//              of each device.
// Input:       string
// Output:      string
//------------------------------------------------------------------------*/
//word16 dev_rcall_func(byte *str,int i)
//cmx_reentrant 
//{
//  int temp;
//  switch(i)
//  {
//    case 0:
//	  temp = dev_fmt_func(str,1); /* Get the Format Type: Raw/Decimal   */
//	  break;
//	case 1:
//	  temp = show_item_func(str,1);  /* Show No: to left of each device */
//	  break;
//	case 2:
//	  temp = dev_name_func(str,1);          /* Show Name of each device */
//	  break;
//	case 3:
//	  temp = dev_scmd_func(str,1);       /* Send Command of each device */
//	  break;
//	case 4:
//	  temp = dev_units_func(str,1);/* Show Response Unit of each Device */
//	  break;
//	default: temp=0;
//	  break;
//  }
//  return (temp);
//}


/*------------------------------------------------------------------------
Function:     show_idnum_func()
Description:  Show the ID of the device on the mod_dev.htm page
Input:        String
Output:       String
------------------------------------------------------------------------*/
word16 show_idnum_func(byte **str)
cmx_reentrant 
{
  *str=id_num_buff;
  return ((word16)strlen((char *)id_num_buff));
}

/*------------------------------------------------------------------------
 Function:    dev_all_func()
 Description: SSI: Get all the Devices displayed on the screen
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_all_func(byte **str_)
cmx_reentrant 
{
  int i,j,temp;
  byte para_header[]="var dev_n";
  byte para_header0[]="= new Array( ";
  byte *str;


  *str_=cgi_buff;
  str=cgi_buff;
  j=0;

  for (i=0;i<5;i++)
  {
    strcpy((char *)str,(char *)para_header);
	temp=(word16)strlen((char *)para_header);
	str +=temp;
	j +=temp;
	*(str++)=i+'a';
	j++;
	strcpy((char *)str,(char *)para_header0);
	temp=(word16)strlen((char *)para_header0);
	str +=temp;
	j +=temp;
	temp = dev_call_func(str,i);      /* Call All the Columns Functions */
	str += temp;
	j += temp;
	*(str++)= ')';
	j++;
	*(str++)= ';';
	j++;
	*(str++)= 0x0a;
	j++;
  }
  
  return (j);
}

/*------------------------------------------------------------------------
 Function:    dev_call_func()
 Description: SSI: Get all the Devices displayed on the screen
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_call_func(byte *str,int i)
cmx_reentrant 
{
  int temp;

  switch(i)
  {
    case 0:
	  temp = show_item_func(str,0);       /* dev_na: Show the Device No */
	  break;
	
    case 1:
	  temp = dev_chk_func(str);         /* dev_nb: Show the Check Marks */
	  break;
	
    case 2:
	  temp = dev_name_func(str,0);     /* Show the name of the devices  */
	  break;
	
    case 3:
	  temp = dev_cmd_func(str);          /* Show the command of device  */
	  break;
	
    case 4:
	  temp = dev_units_func(str,0);   /* Show the Unit no of the device */
	  break;
    
    default: temp=0;               
	  break;
  }

  return (temp);
}

/*------------------------------------------------------------------------
 Function:    show_item_func()
 Description: Show the Device No: 
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 show_item_func(byte *str,byte opt)
cmx_reentrant 
{
  int i,j,m;
  j=0;
  m=0;
  
  //eeprom_read(temp_buff,eeprom_chk,eeprom_CHK);
  temp_buff[0]=device.check;              /* Device check to temp Buff  */

  for(i=1; i<MAX_UNITS+1; i++)
  {
    if(opt)
	{
	  if(temp_buff[0]&0x01)
	  {
	    *str++='"';
		*str++=i+'0';
		*str++='"';
		*str++=',';
		j+=4;
		if(++m>MAX_READ)
		i=MAX_UNITS;
	  }
      temp_buff[0]=temp_buff[0]>>1;/* Right Shift to get the BIT we need*/
	}
	else
	{
	  *str++='"';
	  *str++=i+'0';
	  *str++='"';
	  *str++=',';
	  j+=4;
	}
  }
  *(str-1)=' ';
  return (j);
}

/*------------------------------------------------------------------------
 Function:    dev_chk_func()
 Description: SSI: Get all the Devices displayed on the screen
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_chk_func(byte *str)
cmx_reentrant 
{
  int i,j,k;
  byte checked[] = "checked";

  temp_buff[0]=device.check; /* Get the byte that represents check mark */

  j=0;
  for(i=0;i<MAX_UNITS;i++)
  {
    *(str++)='"';
	j++;
	if(temp_buff[0]&0x01)
	{
	  strcpy((char *)str,(char *)checked);
	  k=(word16)strlen((char *)checked);
	  j+=k;
	  str+=k;
	}
	temp_buff[0]=temp_buff[0]>>1;/*Shift 1 bit at a time to get the mask*/
	*(str++)='"';
	j++;
	*(str++)=',';
	j++;
  }
  *(str-1)=' ';
  return(j);
}





/*------------------------------------------------------------------------
 Function:    dev_name_func()
 Description: Show the Name of the devices
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_name_func(byte *str,byte opt)
cmx_reentrant 
{
  int i,j,m,temp;
  j=0;
  m=0;

  //eeprom_read(temp_buff,eeprom_chk,eeprom_CHK);
  temp_buff[0]=device.check;

  for(i=0;i<MAX_UNITS;i++)
  {
    if(opt)
	{
	  if(temp_buff[0]&0x01)
	  {
	    *(str++)='"';
		j++;
		//eeprom_reads(str,eeprom_uname(i),eeprom_NAME);
		strcpy((char *)str,device.name[i]);
        temp=strlen((char *)str);
        str +=temp;
		j+=temp;
		*(str++)='"';
		j++;
		*(str++)=',';
		j++;
		if(++m>MAX_READ)
		i=MAX_UNITS;
	  }
	  temp_buff[0]=temp_buff[0]>>1;
	}
	else
	{
	  *(str++)='"';
	  j++;
	  //temp=eeprom_reads(str,eeprom_uname(i),eeprom_NAME);
      strcpy((char *)str,device.name[i]);
      temp=strlen((char *)str);
	  str +=temp;
	  j+=temp;
	  *(str++)='"';
	  j++;
	  *(str++)=',';
	  j++;
	}
  }
  *(str-1)=' ';
  return (j);
}



/*------------------------------------------------------------------------
 Function:    show_cmd_func()
 Description: Show the Command in the npdevices.htm
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_cmd_func(byte *str)
cmx_reentrant 
{
  int i,temp,j;
  j=0;
  
  for(i=0;i<MAX_UNITS;i++)
  {
    *(str++)='"';
	j++;
    //temp=eeprom_reads(str,eeprom_ucmd(i),eeprom_CMD);
    strcpy((char *)str,device.command[i]);
    temp=strlen((char *)str);
    str +=temp;
	j+=temp;
	*(str++)='"';
	j++;
	*(str++)=',';
	j++;
  }
  *(str-1)=' ';
  return (j);
}



/*------------------------------------------------------------------------
 Function:    dev_units_func()
 Description: Show the units of the Devices (dev_nj)
 Input:       string
 Output:      string
------------------------------------------------------------------------*/
word16 dev_units_func(byte *str,byte opt)
cmx_reentrant 
{
  int i,j,m,temp;
  j=0;
  m=0;

  temp_buff[0]=device.check;

  for(i=0;i<MAX_UNITS;i++)
  {
    if(opt)
	{
	  if(temp_buff[0]&0x01)
	  {
	    *(str++)='"';
		j++;
		//temp=eeprom_reads(str,eeprom_uunit(i),eeprom_UNIT);
        strcpy((char *)str,device.unit[i]);
        temp=strlen((char *)str);
        j+=temp;
		str +=temp;
		*(str++)='"';
		j++;
		*(str++)=',';
		j++;
		if(++m>MAX_READ)
		i=MAX_UNITS;
	  }
	  temp_buff[0]=temp_buff[0]>>1;
	}
	else
	{
	  *(str++)='"';
	  j++;
	  //temp=eeprom_reads(str,eeprom_uunit(i),eeprom_UNIT);
      strcpy((char *)str,device.unit[i]);
      temp=strlen((char *)str);
	  j+=temp;
	  str +=temp;
	  *(str++)='"';
	  j++;
	  *(str++)=',';
	  j++;
	}
  }
  *(str-1)=' ';
  return (j);
}
