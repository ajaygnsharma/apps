/*
 * assn_01.c
 *
 *  Created on: Mar 22, 2012
 *      Author: user1
 */
#define squared1(x) x*x
#define squared2(x) (x*x)
#define squared3(x) (x)*(x)
#define squared4(x) ((x)*(x))

#define MSG1 "All your base are belong to us!"
#define MESSAGE "Happy new year!"

int prog_1_1_c(double *ans){
	*ans=10.0+(2.0/((3.0-2.0)*2.0));
	return 1;
}

int prog_1_1_c_err(double *ans){
	*ans=10.0+2.0/3.0-2.0*2.0;
	return 1;
}

int prog_1_2_1(double *ans){
	*ans= 18.0/squared1(2+1);
	return 1;
}

int prog_1_2_2(double *ans){
	*ans= 18.0/squared2(2+1);
	return 1;
}

int prog_1_2_3(double *ans){
	*ans= 18.0/squared3(2+1);
	return 1;
}

int prog_1_2_4(double *ans){
	*ans= 18.0/squared4(2+1);
	return 1;
}


int prog_1_4(void){
	const char msg[]=MSG1;
	puts(msg);
	return 0;
}

int prog_1_5(void *arg1){
	return (int)arg1-1;
}

int prog_1_6(){
	puts(MESSAGE);
	return 0;
}
