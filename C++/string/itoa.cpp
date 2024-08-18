/*
 * itoa.cpp
 *
 *  Created on: May 17, 2019
 *      Author: user1
 */

#include <iostream>
#include <string>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;

string rev(string str)
{
   string r;
   int i;
   for (i=str.length()-1; i >= 0; i--)
      r += str[i];
   return r;
}

int reverse(int x) {
  string str = to_string(x);
  int r = 0;
  string str2, str3;
  bool negative = false;

  if(str[0] == '-'){
    str2 = str.substr(1,str.length());
    negative = true;
  }else{
    str2 = str;
  }

  if(negative)
    str3.append("-");

  str3 += rev(str2);

  try{
    r = std::stoi(str3);
  }catch (...) {
    r = 0;
  }
  return r;
}

int main ()
{
  printf("%d\n",reverse(123));
  printf("%d\n",reverse(1534236469));

  return 0;
}


