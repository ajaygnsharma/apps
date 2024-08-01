#ifndef MIT_PERSON_H
#define MIT_PERSON_H

#include <string>

class MITPerson {

 protected:

  int id;
  std::string name;
  std::string address;
  
 public:
  
  MITPerson(int id, std::string name, std::string address);
  virtual ~MITPerson();
  
  virtual void displayProfile();
  void changeAddress(std::string newAddress);

};

#endif
