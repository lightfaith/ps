#!/bin/bash
# Shell script destined to simplify time measuring for different numbers of threads.

lscpu | grep -i -E "^CPU\(s\):|core|socket" --color=no

size=32768 # number of ints to sort
echo "Running ./sort for $size random numbers";

for (( i=1; $i<=$size; i=i*2 )); do
	# run ./sort for SIZE numbers and 2^n processes, measure real time
	echo -n "$i processes: ";
	t=`(time ./sort $size $i > /dev/null) 2>&1 | grep real | cut -d'	' -f2`;
	echo "$t"
done
