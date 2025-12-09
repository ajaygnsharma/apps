#include <iostream>
#include <string>

#include "MITPerson.h"

void print(MITPerson p){
		p.displayProfile();
}

int main(){
  MITPerson p1(901289, "James Lee", "32 Vassar St.");
  MITPerson p3(978123, "Alice Smith", "121 Ames St.");
  MITPerson p2=p1;
  p3=p1;
  print(p2);
  print(p3);

  return 0;
}
