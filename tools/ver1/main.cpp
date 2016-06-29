#include <iostream>
#include <iomanip>
#include <omp.h>
#include "potential.hpp"

// Calculate the gravitational potential associated with a mass distribution 
// using the superposition of the potentials of point masses. 
// The mass distribution is a finite collection of point masses located at 
// the points x_1, ..., x_n and have masses m_1, ..., m_n
using namespace std;
int main(int argc, char** argv) 
{
  const int n = 4096;		//number of total point masses
  const int m = 64; 		//number of points in the free space (m*m*m)
  const int nTrials = 10; 	//number ot integration steps
  
  real_type *computedPot;	//array of computed potential
  
  computedPot = new real_type[m*m*m];
  
  potential pot(n);
  
  cout << "Start the Test Code for Gravity Potential" << endl;
  cout << setw(9) << left << "Trial" << setw(10) << "Time, s" << setw(8) << "GFLOP/s" << endl;
  
  double av=0.0, dev=0.0;
  const double gflops = 1e-9*12.0*double(m*m*m)*double(n);
  
  for (int t=1; t<=nTrials; t++)
  {
    computedPot[0:m*m*m]=0.0;
    const double t0 = omp_get_wtime();
#pragma omp parallel for collapse(3) schedule(guided)
    for (int i = 0; i < m; i++) 
      for (int j = 0; j < m; j++) 
	for (int k = 0; k < m; k++) 
	{ 
	      pot.CalculateGravitationalPotential( i, j, k, computedPot[i*m*m+j*m+k]);
	}
    const double t1 = omp_get_wtime();
    if (t > 2) 			//do not consider the first 2 trials
    {		
     av  += gflops/(t1-t0);
     dev += gflops*gflops/((t1-t0)*(t1-t0));
    }
   cout << setw(9) << left << t << setw(10) << setprecision(5) << (t1-t0) << gflops/(t1-t0) << endl;
  }
    
  av/=(double)(nTrials-2);
  dev=sqrt(dev/(double)(nTrials-2)-av*av);
  cout << "-----------------------------------------------------" << endl;
  cout << "Average performance: " << av << " +- " <<  dev << endl;
  cout << "-----------------------------------------------------" << endl;
  
    
  delete [] computedPot;
  return 0;
}