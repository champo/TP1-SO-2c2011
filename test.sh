#!/bin/bash

uname=`uname`
if [[ "$uname" == "Darwin" ]]; then
    count() {
        res=$1
        echo $res
    }
else
    count() {
        echo `echo $1 | cut -d ' ' -f 1`
    }
fi

mkdir -p out
for level in $(seq 1 4); do
    for iter in $(seq 1 250); do
        echo "$level iteration $iter"
        echo "$level iteration $iter" > out/sim
        bin/sim configFiles/sim$level - 2> out/err >> out/sim
        
        if [[ -s out/err ]]; then
            echo "There's something in out/err. Bailing..."
            exit
        fi
        errs=`grep "{ERR}" out/sim`
        if [[ -n $errs ]]; then
            echo "Found {ERR} in out/sim"
            exit
        fi

        biatch=`grep "biatch" out/sim`
        if [[ -z $biatch ]]; then
            echo "Didnt find a biatch signaling end..."
            exit
        fi

        base=`grep "base belong" out/sim`
        if [[ -z $biatch ]]; then
            echo "Didnt find a base belong signaling end..."
            exit
        fi

        airlines=$(count `ls configFiles/sim$level/airline* | wc -l`)
        bye=$(count `grep "Bye folks" out/sim | wc -l`)
        if [[ $bye -ne $airlines ]]; then
            echo "Not enough bye folks for every airline"
            exit
        fi
    done
done
