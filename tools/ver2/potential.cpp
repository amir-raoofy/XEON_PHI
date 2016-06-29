#include "potential.hpp"
#include <iostream>
potential :: potential(const int nMasses)
{
  _nMasses = nMasses;
  particles = new Particle[nMasses];
  
  std::mt19937 gen(42);      			//set seed = 42
  std::uniform_real_distribution<real_type> unif_d(0.0,1.0);
  
  for(int i=0; i<_nMasses; ++i)
  {
   particles[i].position[0] = unif_d(gen);
   particles[i].position[1] = unif_d(gen);
   particles[i].position[2] = unif_d(gen);
   particles[i].mass 	    = 10000.*unif_d(gen);
  }
  
}
void potential :: CalculateGravitationalPotential(int Xi, int Xj, int Xk, real_type &pot)
{
  int n;
  real_type local_p = 0.0;
  pot = 0.0;
  #pragma simd reduction(-: local_p),
  for (n=0; n < _nMasses; n++)  
  {  
    const real_type dx = particles[n].position[0] - (real_type) Xi;	//1flop
    const real_type dy = particles[n].position[1] - (real_type) Xj;	//1flop
    const real_type dz = particles[n].position[2] - (real_type) Xk;	//1flop
    const real_type dist = dx*dx+dy*dy+dz*dz;				//5flops
	  
    local_p -= G * particles[n].mass / sqrt(dist);			//2flops+div+sqrt = 4flops
  }
  pot = local_p;
}

potential :: ~potential()
{
  delete [] particles;
}
