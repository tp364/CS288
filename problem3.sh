#!/bin/bash
#Problem3.sh
#This sorts a list of command line parameters in ascending order 

arr=("$@")
n=${#arr[@]}

# Insertion sort
for (( i=1; i<n; i++ )); do
    key=${arr[i]}
    j=$((i-1))
    while (( j>=0 && arr[j]>key )); do
        arr[j+1]=${arr[j]}
        j=$((j-1))
    done
    arr[j+1]=$key
done

echo "${arr[@]}"
