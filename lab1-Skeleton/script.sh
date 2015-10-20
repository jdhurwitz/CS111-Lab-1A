#!/bin/bash

rm -rf testdir
ARRAY=( Ram Jonny Tim Jeremy Ricky Max)

NUM_ITEMS=${#ARRAY[@]}

for((i=0; i<$NUM_ITEMS;i++)); do
    echo ${ARRAY[${i}]}
done

mkdir testdir
cd testdir
echo pizza > p.txt
cd ..

