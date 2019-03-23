#!/bin/bash
for i in {1..4}
do
  for j in {1..1000}
  do
     mpirun -np $i bin/lab1
  done
done
