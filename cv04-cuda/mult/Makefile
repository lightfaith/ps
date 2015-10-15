CC:=/home/edu/cuda2/bin/nvcc
OPT:=-arch=sm_21

mult: cuda2.o mult.o
	${CC} ${OPT} $^ -o $@
	export LD_LIBRARY_PATH=/home/edu/cuda2/lib

cuda2.o: cuda2.cu
	${CC} ${OPT} -c $^  -o $@

mult.o: mult.cpp
	${CC} ${OPT} -c $^  -o $@

clean:
	rm *.o mult
