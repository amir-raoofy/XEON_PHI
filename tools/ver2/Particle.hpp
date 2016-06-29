#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

typedef double real_type;

struct Particle
{
  public:
    Particle() { init();}
    void init() 
    {
      position[0] = 0.;
      position[1] = 0.;
      position[2] = 0.;
      mass = 0.0;
    }
    real_type position[3];
    real_type mass;
    
};

#endif