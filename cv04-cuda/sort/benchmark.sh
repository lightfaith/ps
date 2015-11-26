#!/bin/bash

size=16
maxsize=129072

echo "Running sort on CPU and GPU..."
for (( i=$size; $i<=$maxsize; i*=2 )); do
echo -n "$i elements - CPU: ";
t=`(time ./cpusort $i 1 > /dev/null) 2>&1`;
echo -n `echo "$t" | grep "real" | cut -d '	' -f2`;
echo -n " GPU: "
t2=`(time ./sort $i > /dev/null) 2>&1`;
echo `echo "$t2" | grep "real" | cut -d '	' -f2`;

done
