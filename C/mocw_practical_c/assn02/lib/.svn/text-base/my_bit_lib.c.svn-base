
int tst_3bit(int val){
  int i=0,count=0;

  for(i=0;i<4;i++){
    if(val&(1<<i)){
      count++;
    }
  }

  if(count>=3){
	  return 1;
  }else{
	  return 0;
  }
}

int r_byte_order(int val){
  char l_byte=0,h_byte=0;
  int r_int=0;

  l_byte=(val&0xff);
  h_byte=((val&0xff00)>>8);

  r_int=(((l_byte<<8)|(h_byte&0xff))&0x0000ffff);

  return r_int;
}

int r_4bit(int val){
  int r_4bit=0;
  int l_4bit=0;

  l_4bit=((val&0xf)<<12);
  r_4bit=(((val>>4)|(l_4bit))&0x0000ffff);

  return r_4bit;
}
