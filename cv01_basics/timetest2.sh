#!/bin/bash
# Shell script destined to simplify time measuring for different numbers of threads.

lscpu | grep -i -E "^CPU\(s\):|core|socket" --color=no

size=32768 # number of ints to sort
echo "Running ./sort for $size random numbers";

for (( i=32; $i<=128; i=i+1 )); do
	# run ./sort for SIZE numbers and 32-128 processes, measure real time
	echo -n "$i processes: ";
	t=`(time ./sort $size $i > /dev/null) 2>&1 | grep real | cut -d'	' -f2`;
	sleep 0.05
	echo "$t"
done
