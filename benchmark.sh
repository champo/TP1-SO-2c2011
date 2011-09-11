#!/bin/bash

for level in {1..4}; do    
    #if [[ $level -ne 3 ]] ; then 
    echo "Timings for sim$level "
    echo "-----------"
    echo "Time for one iteration"
    time (bin/sim configFiles/sim$level > text)
     
    echo "Time for 100 iterations"
    time ./iterations.sh $level 
    #fi
done
rm text
