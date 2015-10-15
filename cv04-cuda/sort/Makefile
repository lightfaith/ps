CC:=/home/edu/cuda2/bin/nvcc
OPT:=-arch=sm_21

all: sort cpusort

sort: cuda2.o sort.o
	${CC} ${OPT} $^ -o $@
	export LD_LIBRARY_PATH=/home/edu/cuda2/lib

cuda2.o: cuda2.cu
	${CC} ${OPT} -c $^  -o $@

sort.o: sort.cpp
	${CC} ${OPT} -c $^  -o $@

clean:
	rm *.o sort

cpusort: cpusort.cpp
	g++ -o cpusort cpusort.cpp
