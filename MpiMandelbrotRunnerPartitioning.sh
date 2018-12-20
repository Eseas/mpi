#!/bin/sh
mpirun -np 9 a.out 0 0 1000 1000 1000 50 50
mpirun -np 9 a.out 0 0 1000 1000 1000 100 100
mpirun -np 9 a.out 0 0 1000 1000 1000 250 250
mpirun -np 9 a.out 0 0 1000 1000 1000 500 500
mpirun -np 9 a.out 0 0 1000 1000 1000 1000 1000

