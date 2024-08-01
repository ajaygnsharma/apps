#include <my_limits.h>
#include <stdio.h>

int main(void){
	printf("type,size,min,max\n");
	printf("char,%d,%d,%d\n",char_size(),char_min(),char_max());
	printf("uchar,%u,%u,%u\n",uchar_size(),uchar_min(),uchar_max());
	printf("short,%d,%d,%d\n",short_size(),short_min(),short_max());
	printf("int,%d,%d,%d\n",int_size(),int_min(),int_max());
	printf("uint,%u,%u,%u\n",uint_size(),uint_min(),uint_max());
	printf("ulong,%lu,%lu,%lu\n",ulong_size(),ulong_min(),ulong_max());
	printf("float,%f,%f,%f\n",float_size(),float_min(),float_max());
	printf("tst of 3 bits:%d\n",tst_3bit(0xCAFE));
	printf("after reverse byte order:%4X\n",r_byte_order(0xCAFE));
	printf("after reverse byte order:%4X\n",r_4bit(0xCAFE));
	return 0;
}

