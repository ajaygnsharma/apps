#include "MITPerson.h"

#include <iostream>

MITPerson::MITPerson(int id, std::string name, std::string address){
  this->id = id;
  this->name = name;
  this->address = address;
}

MITPerson::MITPerson(const MITPerson& other){
  id = other.id;
  name = other.name;
  address = other.address;
}

MITPerson& MITPerson::operator=(const MITPerson& other){
  name = other.name;
  id = other.id;
  address = other.address;
  return *this;
  // returns a newly assigned MITPerson
}

MITPerson::~MITPerson() {}

void MITPerson::displayProfile() {
  std::cout << "-----------------------------\n";
  std::cout << "Name: " << name << " ID: " << id 
            << " Address: " << address << "\n";
  std::cout << "-----------------------------\n";
}

void MITPerson::changeAddress(std::string newAddress) {
  this->address = newAddress;
}
