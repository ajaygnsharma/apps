#include <iostream>
#include <string>

#include "Student.h"

int main(){
  
  MITPerson* john= new MITPerson(901289, "John Doe", "500 Massachusetts Ave.");
  Student* james = new Student(971232, "James Lee", "32 Vassar St.", 6, 2);
  Class* c1 = new Class("6.088");
  Class* c2 = new Class("6.01");
  Class* c3 = new Class("6.824"); 
  
  MITPerson* steve = new Student(911923, "Steve", "99 Cambridge St.", 18, 3);
  Class* c4 = new Class("18.100");
  Class* c5 = new Class("18.504"); 

  john->displayProfile();
  james->addClassTaken(c1);
  james->displayProfile();
  james->addClassTaken(c2);
  james->addClassTaken(c3);
  james->displayProfile();  

  Student* steve2 = dynamic_cast<Student*>(steve);

  steve2->addClassTaken(c4);
  steve2->addClassTaken(c5);  
  steve->displayProfile();

  delete john;
  delete james;
  delete steve;
  delete c1;
  delete c2;
  delete c3;
  delete c4;
  delete c5;
}
