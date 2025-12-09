#include <stdlib.h>
#include <iostream>
#include "Patient.h"

Patient::Patient(float initImmunity, int initNumVirusCells) {
  // must check values correct
  float resistance;

  immunity = initImmunity;
  for (int i = 0; i < initNumVirusCells; i++) {
    // randomly generate virus resistance
    resistance = (float) rand()/RAND_MAX; 
    virusPop[i] = new Virus(resistance);
  }
  numVirusCells = initNumVirusCells;
}

Patient::~Patient(){
  for (int i = 0; i < numVirusCells; i++){
    delete virusPop[i];
  }
}

// Increases the immunity of the patient by taking drugs
void Patient::takeDrug(){
  // incorrect; can violate rep invariant
  immunity = immunity + 0.1;
}

// Simulates the growth of virus population inside patient for one step
// returns true iff population reached max
bool Patient::simulateStep() {
  
  Virus* newVirusPop[MAX_VIRUS_POP];
  Virus* dyingCells[numVirusCells];

  int numNewCells = 0;
  int numDyingCells = 0;

  bool maxReached = false;

  Virus* curVirus;
  Virus* offspring;

  for (int i = 0; i < numVirusCells; i++){
    curVirus = virusPop[i];

    // if this virus cell survives, then 
    // put it into the next list of virus cells
    if (curVirus->survive(immunity)) {
      newVirusPop[numNewCells] = virusPop[i];
      numNewCells++;
      if (numNewCells >= MAX_VIRUS_POP){
        maxReached = true;
        break;
      }
    } else {
      // this virus cell is dying; put it into the list of dying cells
      dyingCells[numDyingCells] = virusPop[i];
      numDyingCells++;
    }
    
    offspring = curVirus->reproduce(immunity);
    if (offspring != NULL) {
      newVirusPop[numNewCells] = offspring;
      numNewCells++;      
      if (numNewCells >= MAX_VIRUS_POP){
        maxReached = true;
        break;
      }
    }

  }
  
  // delete all dying cells
  for (int i = 0; i < numDyingCells; i++) {
    delete dyingCells[i];
  }

  // can't do this
  //virusPop = newVirusPop;  
  // instead, must copy array
  for (int i = 0; i < numNewCells; i++) {
    virusPop[i] = newVirusPop[i];
  }
  numVirusCells = numNewCells;

  std::cout << "No. virus cells: " << numVirusCells << "\n";

  return maxReached;

}
