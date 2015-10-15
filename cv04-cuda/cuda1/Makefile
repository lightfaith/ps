CC:=/home/edu/cuda2/bin/nvcc
OPT:=-arch=sm_21

main1: cuda1.o main1.o
	${CC} ${OPT} $^ -o $@
	export LD_LIBRARY_PATH=/home/edu/cuda2/lib

cuda1.o: cuda1.cu
	${CC} ${OPT} -c $^  -o $@

main1.o: main1.cpp
	${CC} ${OPT} -c $^  -o $@

clean:
	rm *.o main1
