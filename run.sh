#!/bin/bash
for i in {1..2}
do
  mpirun -np $i bin/lab1
done
