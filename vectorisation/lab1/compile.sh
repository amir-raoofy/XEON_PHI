#mpicc -no-vec -qopenmp -mmic nBody.c -o nBody.exe

#mpicc -qopt-report -qopt-report-phase:vec -qopenmp -mmic nBody.c -o nBody.exe

mpicc -qopenmp -mmic nBody.c -o nBody.exe
