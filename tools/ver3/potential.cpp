#include "potential.hpp"
#include <iostream>
potential :: potential(const int nMasses)
{
  _nMasses = nMasses;
  particles = new Particle;
  
  particles->pos_x = new real_type[_nMasses];
  particles->pos_y = new real_type[_nMasses];
  particles->pos_z = new real_type[_nMasses];
  particles->mass  = new real_type[_nMasses];
  
  std::mt19937 gen(42);      			//set seed = 42
  std::uniform_real_distribution<real_type> unif_d(0.0,1.0);
  
  for(int i=0; i<_nMasses; ++i)
  {
   particles->pos_x[i] = unif_d(gen);
   particles->pos_y[i] = unif_d(gen);
   particles->pos_z[i] = unif_d(gen);
   particles->mass[i]  = 10000.*unif_d(gen);
  }
  
}
void potential :: CalculateGravitationalPotential(int Xi, int Xj, int Xk, real_type &pot)
{
  int n;
  real_type local_p = 0.0;
  pot = 0.0;
  #pragma simd reduction(-: local_p) 
  for (n=0; n < _nMasses; n++)  
  {  
    const real_type dx = particles->pos_x[n] - (real_type) Xi;	//1flop
    const real_type dy = particles->pos_y[n] - (real_type) Xj;	//1flop
    const real_type dz = particles->pos_z[n] - (real_type) Xk;	//1flop
    const real_type dist = dx*dx+dy*dy+dz*dz;				//5flops
	  
    pot -= G * particles->mass[n] / sqrt(dist);				//2flops+div+sqrt = 4flops
  }

}

potential :: ~potential()
{
  delete [] particles->pos_x;
  delete [] particles->pos_y;
  delete [] particles->pos_z;
  delete [] particles->mass;
  delete  particles;
}
