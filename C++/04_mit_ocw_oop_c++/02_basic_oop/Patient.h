#ifndef PATIENT_H
#define PATIENT_H

#include "Virus.h"

#define MAX_VIRUS_POP 1000

class Patient {
  
  Virus* virusPop[MAX_VIRUS_POP];       
  int numVirusCells;
  float immunity;       // degree of immunity, in %

 public:
  
  Patient(float initImmunity, int initNumViruses);    
  ~Patient();
  void takeDrug();
  bool simulateStep();

};

#endif //PATIENT_H
