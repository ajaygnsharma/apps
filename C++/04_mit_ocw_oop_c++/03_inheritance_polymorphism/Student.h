#ifndef STUDENT_H
#define STUDENT_H

#include <vector>
#include <string>
#include <iostream>

#include "MITPerson.h"
#include "Class.h"

class Student : public MITPerson {
  
  int course;   
  int year;     // 1 = freshman, 2 = sophomore, etc.
  std::vector<Class*> classesTaken;
  
 public:
  
  Student(int id, std::string name, std::string address, int course, int year);

  void displayProfile();
  void addClassTaken(Class* newClass);    
  void changeCourse(int newCourse);
  
};

#endif
