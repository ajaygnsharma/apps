#include "Class.h"

Class::Class(std::string name){
  this->name = name;
}

std::string Class::getName(){
  return name;
}
