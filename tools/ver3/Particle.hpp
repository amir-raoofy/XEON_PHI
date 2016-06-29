#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

typedef double real_type;

struct Particle
{
  public:
    Particle() { init();}
    void init() 
    {
      pos_x = NULL;
      pos_y = NULL;
      pos_z = NULL;
      mass = NULL;
    }
    real_type *pos_x;
    real_type *pos_y;
    real_type *pos_z;
    real_type *mass;
    
};

#endif
