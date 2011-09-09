#!/bin/bash
mkdir -p out
for level in $(seq 1 3); do
    for iter in $(seq 1 200); do
        echo "$level iteration $iter"
        echo "$level iteration $iter" > out/sim
        bin/sim configFiles/sim$level 2> out/err >> out/sim
        
        if [[ -s out/err ]]; then
            echo "Error!"
            exit
        fi
        errs=`grep "INVALID" out/sim | wc -l | cut -d ' ' -f 1`
        if [[ $errs -ne 0 ]]; then
            echo "Error!"
            exit
        fi
    done
done
