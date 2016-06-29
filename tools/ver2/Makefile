CXX = icpc
OPTFLAGS = -std=c++11 -O3  -qopt-report=5 -qopt-report-phase=vec

DEBUGFLAGS = -g
OMPFLAGS = -qopenmp
INCLUDES = 

CXXFLAGS = $(OPTFLAGS) $(DEBUGFLAGS) $(OMPFLAGS) -xAVX
MICFLAGS = $(OPTFLAGS) $(DEBUGFLAGS) $(OMPFLAGS) -mmic

SOURCES = potential.cpp main.cpp

.SUFFIXES: .o .cpp .omic

##########################################
OBJSC = $(SOURCES:.cpp=.o)
OBJSM = $(SOURCES:.cpp=.omic)
##########################################

EXEC=gravity-cpu.x
EXEM=gravity-mic.x

all: instructions

%.o: %.cpp
	$(info )
	$(info Compiling the object file for CPU: )
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ 
	
%.omic: %.cpp
	$(info )
	$(info Compiling the object file for MIC: )
	$(CXX) $(MICFLAGS) $(INCLUDES) -c $< -o $@ 

cpu: $(OBJSC)
	$(info )
	$(info Linking the CPU executable:)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(EXEC) $(OBJSC)
	
mic: $(OBJSM)
	$(info )
	$(info Linking the CPU executable:)
	$(CXX)  $(MICFLAGS) $(INCLUDES) -o $(EXEM) $(OBJSM)

instructions: 
	$(info )
	$(info To Compile the Application: )
	$(info "make cpu" to compile the application on the host CPU)
	$(info "make mic" to compile the application on the target MIC)
	$(info )

run-cpu: 
	$(info )
	$(info Run the default test case on CPU: )
	OMP_NUM_THREADS=32 ./gravity-cpu.x 
	
run-mic: 
	$(info )
	$(info Run the default test case on MIC: )
	scp gravity-mic.x $(HOST)-mic0:~/
	ssh $(HOST)-mic0 OMP_NUM_THREADS=240 ./gravity-mic.x 

clean: 
	rm -f $(OBJSC) $(OBJSM) gravity-cpu.x gravity-mic.x *.optrpt


