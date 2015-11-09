#!/bin/bash

rm -rf testdir
#ARRAY=( Ram Jonny Tim Jeremy Ricky Max)

#NUM_ITEMS=${#ARRAY[@]}

for((i=0; i<3;i++)); do
    echo ARRAY[0]
done

mkdir testdir
cd testdir
echo pizza > p.txt
cd ..

