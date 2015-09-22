#!/bin/bash
size=32768
processes=1
echo "Running ./sort of $size random numbers";
for (( i=1; $i<=$size; i=i*2 )); do
	echo -n "$i processes: ";
	t=`(time ./sort $size $i > /dev/null) 2>&1 | grep real | cut -d'	' -f2`;
	echo "$t"
done
