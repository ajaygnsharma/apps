#include <stdlib.h>
#include <iostream>
#include <time.h>

#include "Patient.h"

int main(){
  
  int sim_length = 100;
  int n = 0;
  bool maxVirusPopReached = false;

  srand(time(NULL));

  // initialize a patient with no immunity and 5 virus cells
  Patient p(0.0, 5);   
  p.takeDrug();

  // run3 simulation for sim_length number of steps
  while (n < sim_length) {
    maxVirusPopReached = p.simulateStep();
    
    if (maxVirusPopReached) {
      std::cout << "Max virus population reached in " 
                << n << " steps!\n";
      break;
    }
    n++;
  }
    
}
