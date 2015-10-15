#!/bin/bash

size=6
maxsize=60000000

echo "Running sort in $processes processes."
for (( i=$size; $i<=$maxsize; i*=10 )); do
echo -n "$i elements - CPU: ";
t=`(time ./mult $i > /dev/null) 2>&1`;
echo -n `echo "$t" | grep "real" | cut -d '	' -f2`;
echo -n " GPU: "
t2=`(time ./mult $i "gpu" > /dev/null) 2>&1`;
echo `echo "$t2" | grep "real" | cut -d '	' -f2`;

done
