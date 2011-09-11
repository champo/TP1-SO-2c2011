#!/bin/bash

for iter in {1..100}; do
    bin/sim configFiles/sim$1 > text
done
