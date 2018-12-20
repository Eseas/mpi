#!/bin/sh
mpirun -np 9 a.out 0 0 100 200 100 10 10
mpirun -np 9 a.out 0 0 1000 200 100 10 10
mpirun -np 9 a.out 0 0 2000 200 100 10 10
mpirun -np 9 a.out 0 0 5000 200 100 10 10
mpirun -np 9 a.out 0 0 10000 200 100 10 10
mpirun -np 9 a.out 0 0 20000 200 100 10 10
