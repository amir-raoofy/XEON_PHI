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
    
    int align = 32;		//align=32B Xeon and 64B Xeon Phi
    void allocate_align(int n)
    {
      pos_x = static_cast<real_type*> (_mm_malloc (n * (sizeof(real_type)), align) );
      pos_y = static_cast<real_type*> (_mm_malloc (n * (sizeof(real_type)), align) );
      pos_z = static_cast<real_type*> (_mm_malloc (n * (sizeof(real_type)), align) );
      mass  = static_cast<real_type*> (_mm_malloc (n * (sizeof(real_type)), align) );
    }
    void deallocate()
    {
      _mm_free(pos_x);
      _mm_free(pos_y);
      _mm_free(pos_z);
      _mm_free(mass);      
    }
};

#endif
