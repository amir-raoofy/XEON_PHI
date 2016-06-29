#mpicc hellompi0.c -o hellompi-host
#mpiexec -n 2 -host i01r13c13 ./hellompi-host


#mpicc -mmic hellompi0.c -o hellompi-mic
#mpirun -n 4 -perhost 2 -host i01r13c13-mic0,i01r13c13-mic1 ./hellompi-mic

#mpirun -n 2 -host i01r13c13 ./hellompi-host : -n 2 -host i01r13c13-mic0 ./hellompi-mic : -n 2 -host i01r13c13-mic1 ./hellompi-mic

#mpicc hellompi.c -o hellompi-offload
#mpiexec -n 2 -host i01r13c13 ./hellompi-offload

#mpiexec -n 1 -host i01r13c13 IMB-MPI1 PingPong : -n 1 -host i01r13c13-mic0  /bin/IMB-MPI1 PingPong

mpiexec -n 1 -host i01r13c13-mic0 /bin/IMB-MPI1 PingPong : -n 1 -host i01r13c13-mic1  /bin/IMB-MPI1 PingPong
