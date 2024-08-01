#include "Student.h"

Student::Student(int id, std::string name, std::string address, 
                 int course, int year) : MITPerson(id, name, address) {
  this->course = course;
  this->year = year;  
}

void Student::addClassTaken(Class* newClass){
  classesTaken.push_back(newClass);
}

void Student::displayProfile(){

  std::cout << "-----------------------------\n";
  std::cout << "Name: " << name << " ID: " << id 
            << " Address: " << address << "\n";
  std::cout << "Course: " << course << "\n";

  std::vector<Class*>::iterator it;

  std::cout << "Classes taken:\n";
  for (it = classesTaken.begin(); it != classesTaken.end(); it++){
    Class* c = *it;
    std::cout << c->getName() << "\n";
  }
  std::cout << "-----------------------------\n";

}
