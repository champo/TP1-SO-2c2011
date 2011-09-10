#!/bin/bash
mkdir -p out
for level in $(seq 1 3); do
    for iter in $(seq 1 200); do
        echo "$level iteration $iter"
        echo "$level iteration $iter" > out/sim
        bin/sim configFiles/sim$level 2> out/err >> out/sim
        
        if [[ -s out/err ]]; then
            echo "There's something in out/err. Bailing..."
            exit
        fi
        errs=`grep "{ERR}" out/sim | wc -l | cut -d ' ' -f 1`
        if [[ $errs -ne 0 ]]; then
            echo "Found $errs INVALID in out/sim"
            exit
        fi
        biatch=`grep "biatch" out/sim`
        if [[ -z $biatch ]]; then
            echo "Didnt find a biatch signaling end..."
            exit
        fi

        airlines=`ls configFiles/sim$level/airline* | wc -l | cut -d ' ' -f 1`
        bye=`grep "Bye folks" out/sim | wc -l | cut -d ' ' -f 1`
        if [[ $bye -ne $airlines ]]; then
            echo "Not enough bye folks for every airline"
            exit
        fi
    done
done
