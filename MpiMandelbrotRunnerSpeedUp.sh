#!/bin/sh
mpirun -np 2 a.out 0 0 10000 400 200 10 10
mpirun -np 4 a.out 0 0 10000 400 200 10 10
mpirun -np 6 a.out 0 0 10000 400 200 10 10
mpirun -np 8 a.out 0 0 10000 400 200 10 10
mpirun -np 10 a.out 0 0 10000 400 200 10 10
mpirun -np 16 a.out 0 0 10000 400 200 10 10
mpirun -np 20 a.out 0 0 10000 400 200 10 10
mpirun -np 31 a.out 0 0 10000 400 200 10 10
