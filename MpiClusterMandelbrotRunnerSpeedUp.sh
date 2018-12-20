#!/bin/sh

#SBATCH -p short

#SBATCH -n32

#SBATCH -C beta

mpiCC mpi-mandelbrot.cpp

mpirun -np 2 a.out 0 0 5000 400 200 10 10
mpirun -np 4 a.out 0 0 5000 400 200 10 10
mpirun -np 8 a.out 0 0 5000 400 200 10 10
mpirun -np 16 a.out 0 0 5000 400 200 10 10
mpirun -np 32 a.out 0 0 5000 400 200 10 10
