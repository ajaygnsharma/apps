#include <stdlib.h>
#include <iostream>
#include "Virus.h"

Virus::Virus(float newResistance) {
  reproductionRate = defaultReproductionRate;
  resistance = newResistance;
}

Virus::Virus(float newReproductionRate, float newResistance) {
  reproductionRate = newReproductionRate;
  resistance = newResistance;
}

// If this virus cell reproduces, 
// returns a new offspring with identical genetic info.
// Otherwise, returns NULL.
Virus* Virus::reproduce(float immunity) {
 
  float prob = (float) rand() / RAND_MAX; // generate number between 0 and 1

  // If the patient's immunity is too strong, it cannot reproduce
  if (immunity > prob)
    return NULL;  
 
  // Does the virus reproduce this time?
  if (prob > reproductionRate) 
    return NULL;        // No!

  return new Virus(reproductionRate, resistance);
}

// Returns true if this virus cell survives, given the patient's immunity
bool Virus::survive(float immunity) {
  
  // If the patient's immunity is too strong, then this cell cannot survive
  if (immunity > resistance) 
    return false;

  return true;  
}

const float Virus::defaultReproductionRate;
