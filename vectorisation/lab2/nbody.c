/*******************************************************************************
!  Content:
!    nbody Program  
!
! LRZ:           Sun. Jun. 26 2016
!
!******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#ifndef SIZE
#define SIZE 2000
#endif

struct Object
{
  float x, y, z; 
  float vx, vy, vz;
  float ax, ay, az;
  float mass;
};

float x_objects[SIZE];
float y_objects[SIZE];
float z_objects[SIZE];
float vx_objects[SIZE];
float vy_objects[SIZE];
float vz_objects[SIZE];
float ax_objects[SIZE];
float ay_objects[SIZE];
float az_objects[SIZE];
float mass_objects[SIZE];

float meanv;

float totalfps = 0.0f; 
float totaltime = 0.0f;
int frames = 0;
float max_fps = 0.0f; 
float min_fps = 0.0f;

void compute()
{
  const float softeningSquared = 0.001215000f*0.001215000f;
  const float G = 6.67259e-11f;
  const float timestep = 0.1f;
  const float damping = 1.0f; 

  int i = 0;
  int j = 0;

  //--------------------------
  // LRZ WORK FOR YOU LRZ: Parallelize this region
  //--------------------------

  #pragma omp parallel
  {
  
  //--------------------------
  // LRZ WORK FOR YOU LRZ: Parallelize this region
  //--
	#pragma omp for private(j)
	for (i = 0; i < SIZE; i++)// update velocity
	{
 //--------------------------
 // LRZ WORK FOR YOU LRZ: vectorise this region
 //--
	  #pragma simd
	  for (j = 0; j < SIZE; j++)
	  {
	    if (i < j || i > j)
		{
		  float distance[3];
		  float distanceSqr = 0.0f, distanceInv = 0.0f;
		  distance[0] = x_objects[j] - x_objects[i];
		  distance[1] = y_objects[j] - y_objects[i];
		  distance[2] = z_objects[j] - z_objects[i];

		  distanceSqr = sqrt(distance[0]*distance[0] + distance[1]*distance[1] + distance[2]*distance[2]) + softeningSquared;
		  distanceInv = 1.0f / sqrt(distanceSqr);
		  ax_objects[i] += distance[0] * G * mass_objects[j] * distanceInv * distanceInv * distanceInv;
		  ay_objects[i] += distance[1] * G * mass_objects[j] * distanceInv * distanceInv * distanceInv;
		  az_objects[i] += distance[2] * G * mass_objects[j] * distanceInv * distanceInv * distanceInv;
		}
	  }
	}

  	#pragma omp for reduction(+:meanv)
	for (i = 0; i < SIZE; ++i)// update position
	{		
	  vx_objects[i] += ax_objects[i] * timestep * damping;
	  vy_objects[i] += ay_objects[i] * timestep * damping;
	  vz_objects[i] += az_objects[i] * timestep * damping;

	  x_objects[i] += vx_objects[i] * timestep;
	  y_objects[i] += vy_objects[i] * timestep;
	  z_objects[i] += vz_objects[i] * timestep;

	  ax_objects[i] = 0.0f;
	  ay_objects[i] = 0.0f;
	  az_objects[i] = 0.0f;

	  meanv += vx_objects[i] + vy_objects[i] + vz_objects[i];
	}

  }//end of parallel region
  //-------------------------------
  // end of region to parallelize
  //-------------------------------

}

void update()
{


  struct timeval time1, time2;
  char textBuf[512];
  float frameTime = 0;
  float fps = 0;
  long mtime, seconds, useconds;

  gettimeofday(&time1, NULL);

  compute();

  gettimeofday(&time2, NULL);
	
  seconds = time2.tv_sec - time1.tv_sec;
  useconds = time2.tv_usec - time1.tv_usec;

  frameTime = seconds + useconds/(1000.0*1000.0); 
  totaltime += frameTime;
  frames++;

  fps = frames / totaltime;

  if (frames % 50 == 0)
    {
      printf("%d|t: %.02fs=%.01f FPS\n", frames, totaltime, fps);
    }
}


static void init_objects()
{

  int i = 0;

  for (i = 0; i < SIZE; ++i)
    {
      x_objects[i] = -1.0f + 2.0f * rand() / (float)RAND_MAX, -1.0f + 2.0f * rand() / (float)RAND_MAX, -1.0f + 2.0f * rand() / (float)RAND_MAX;
      y_objects[i] = -1.0f + 2.0f * rand() / (float)RAND_MAX, -1.0f + 2.0f * rand() / (float)RAND_MAX, -1.0f + 2.0f * rand() / (float)RAND_MAX;
      z_objects[i] = -1.0f + 2.0f * rand() / (float)RAND_MAX, -1.0f + 2.0f * rand() / (float)RAND_MAX, -1.0f + 2.0f * rand() / (float)RAND_MAX;
      vx_objects[i] = -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f, -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f, -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f;
      vy_objects[i] = -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f, -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f, -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f;
      vz_objects[i] = -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f, -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f, -1.0e-4f + 2.0f * rand() / (float)RAND_MAX * 1.0e-4f;
      ax_objects[i] = 0.0f;
      ay_objects[i] = 0.0f;
      az_objects[i] = 0.0f;
      mass_objects[i] = (float)SIZE + (float)SIZE * rand() / (float)RAND_MAX;
    }
}

static struct Object crc(void)
{
  int i;
  struct Object result;

  result.x  = 0.0;
  result.y  = 0.0;
  result.z  = 0.0;
  result.vx = 0.0;
  result.vy = 0.0;
  result.vz = 0.0;
  result.ax = 0.0;
  result.ay = 0.0;
  result.az = 0.0;
  result.mass = 0.0;
  
  for (i = 0; i < SIZE; ++i)
    {
      result.x  += x_objects[i];
      result.y  += y_objects[i];
      result.z  += z_objects[i];
      result.vx += vx_objects[i];
      result.vy += vy_objects[i];
      result.vz += vz_objects[i];
      result.ax += ax_objects[i];
      result.ay += ay_objects[i];
      result.az += az_objects[i];
      result.mass += mass_objects[i];
    }
  return result;
}

int main(int argc, char* a[])
{
  int size = SIZE;
  int x = 200;
  int i = 0;

  meanv = 0.0;

  if (argc > 1)
    {
      x = atoi(a[1]);
    }

  init_objects();

  //--------------------------
  // LRZ WORK FOR YOU LRZ: Parallelize this region 
  //--------------------------
 
  #pragma omp parallel sections 
  {
    #pragma omp section
    {
 
 //--------------------------
  // LRZ WORK FOR YOU LRZ: Display number of threads 
  //-------------------------      
      printf("The number of threads is: %d \n",omp_get_num_threads());
    }
  }

  printf("starting simulation for %d frames...\n", x);

  for (;i != x; ++i)
    {
      update();
    }

  printf("simulation finished\n");

  struct Object crcr = crc();

  {
    printf("crc=%g %g %g %g %g %g %g\n", crcr.x, crcr.y, crcr.z, crcr.vx, crcr.vy, crcr.vz, meanv);
  }
  return 0;
}
