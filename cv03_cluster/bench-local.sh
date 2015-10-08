#!/bin/bash

processes=32;
size=1000;
maxsize=10000000;

echo "Running sort in $processes processes."
for (( i=$size; $i<=$maxsize; i*=2 )); do
	echo -n "$i elements: ";
	t=`(time ./sort $i $processes > /dev/null) 2>&1`;
	echo `echo "$t" | grep "real" | cut -d '	' -f2`;
done
