/*
 * my_char_lib.c
 *
 *  Created on: Mar 25, 2012
 *      Author: user1
 */
/* chk_lower_case(): Check if a character is lower case or not.
 * return 1: if lower case or else 0; */
int chk_lower_case(char c){
	if((c>='a')&&(c<='z')){
		return 1;
	}else{
		return 0;
	}
}

/* chk_upper_case(): Check if a character is upper case or not.
 * return 1: if upper case or else 0; */
int chk_upper_case(char c){
	if((c>='A')&&(c<='Z')){
		return 1;
	}else{
		return 0;
	}
}

/* chk_digit(): Check if a character is digit or not.
 * return 1: if digit or else 0; */
int chk_digit(char c){
	if((c>='0')&&(c<='9')){
		return 1;
	}else{
		return 0;
	}
}

/* chk_white_space(): Check if a character is a white space or not.
 * return 1: if white space or else 0; */
int chk_white_space(char c){
	if((c==' ')||(c==0x09)||(c==0x0a)){
		return 1;
	}else{
		return 0;
	}
}
