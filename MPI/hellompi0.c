/* C Example */
#include <stdio.h>
#include <mpi.h>


int main (int argc, char* argv[])
     
{
  char hostname[100];
  int rank, size;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

  // LRZ WORK FOR YOU

  gethostname(hostname,100);
  printf("Hello world, I am rank: %d of %d: host: %s\n",rank, size, hostname);
  MPI_Finalize();
  return 0;

}
