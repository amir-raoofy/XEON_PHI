/* C Example */
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

int main (int argc, char* argv[])
     
{
  char hostname[100];
  int rank, size;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

  // LRZ WORK FOR YOU


  gethostname(hostname,100);
  //printf("Hello world, I am rank: %d of %d: host: %s\n",rank, size, hostname);
  #pragma offload target(mic:rank%2)
  {

  	char michostname[50];
  	gethostname(michostname,50);
  	printf("MIC: I am %s and I have %ld logical cores and I was called by process %d of %d: host: %s\n",
	michostname, sysconf(_SC_NPROCESSORS_ONLN), rank, size, hostname);

  }
  MPI_Finalize();
  return 0;

}
