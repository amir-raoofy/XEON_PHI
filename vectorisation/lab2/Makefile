CC=icc 
CFLAGS= -openmp -O3   

export SINK_LD_LIBRARY_PATH=/apps/all/icc/2016.1.150-GCC-4.9.3-2.25/compilers_and_libraries_2016.1.150/linux/compiler/lib/mic

all: host mic

host:  h_nbody

mic:   m_nbody



h_nbody: 
	$(CC) $(CFLAGS) -c nbody.c 
	$(CC) $(CFLAGS) -o nbody.exe nbody.o

m_nbody: 
	$(CC) $(CFLAGS) -mmic -c nbody.c -o nbody_mic.o 
	$(CC) $(CFLAGS) -mmic -o nbody.mic nbody_mic.o


run:
	@echo  "================ HOST ================="
	./nbody.exe
	@echo 
	@echo  "================ MIC =================="
	/opt/intel/mic/bin/micnativeloadex ./nbody.mic -e "OMP_NUM_THREADS=120"


clean:


clean:
	rm -f *.optrpt nbody_*.exe nbody_*.mic *.o *~

.PHONY: nbody
