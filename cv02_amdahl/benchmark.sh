#!/bin/bash
# Shell script destined to simplify time measuring for different numbers of threads.

#lscpu | grep -i -E "^CPU\(s\):|core|socket" --color=no

#size=32768 # number of ints to sort
size=100
children=20
#echo "Running ./sort for $size random numbers";

for (( i=1; $i<=$children; i+=2 )); do # for that many parts/children
	for (( j=0; $j<2; j+=1 )); do # sequentially and in parallel
	./sort $size $i $j
	#echo -n "$i processes: ";
	#t=`(time ./sort $size $i > /dev/null) 2>&1 | grep real | cut -d'	' -f2`;
	#echo "$t"
	done;
done
