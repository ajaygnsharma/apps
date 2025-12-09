#ifndef VIRUS_H
#define VIRUS_H

class Virus {

  static const float defaultReproductionRate = 0.20;

  float reproductionRate;  // rate of reproduction, in %
  float resistance;        // resistance against drugs, in %
  
 public:

  Virus(float newResistance);
  Virus(float newReproductionRate, float newResistance);
  Virus* reproduce(float immunity);
  bool survive(float immunity);

};

#endif // VIRUS_H
