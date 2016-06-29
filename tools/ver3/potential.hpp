
#ifndef _LJPOTENTIAL_HPP
#define _LJPOTENTIAL_HPP

#include <random>
#include "Particle.hpp"

class potential 
{
private:
  const real_type G = 6.67259e-11;
  Particle *particles;
  int _nMasses;
public:
  potential(const int nMasses);
  ~potential();
  
  void CalculateGravitationalPotential(int Xi, int Xj, int Xk, real_type &pot);
};  

#endif
