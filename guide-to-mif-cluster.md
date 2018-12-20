The guide for use MIF computing cluster
Batch mode computing on MIF computing cluster

Manual (LT): https://mif.vu.lt/cluster/

The requirements:

1) SSH ir SCP clients (e.g., PuTTY ir WinSCP)

2) Linux commands (ls, cd mkdir, nano)

3) The program (sources) should be ready for linux compiling and execution.



1) Prepare source files and batch file and copy to mif.vu.lt (using SCP client or directly accessing files using linux terminal in the class.

TTest.java - source file

TTest.sh - execution script



2) Connect to uosis.mif.vu.lt or use linux terminal (in the class). Check if required files exist (assuming the curent directory is your home dir):

rvck@linux4:~$ ls TTest*

TTest.java  TTest.sh



3) Connect to the cluster and copy files from MIF home dir to the cluster home dir

rvck@linux4:~$ ssh cluster

<.......>

rvck@lxibm102:~$ scp rvck@uosis.mif.vu.lt:~/TTest.java TTest.java

rvck@lxibm102:~$ scp rvck@uosis.mif.vu.lt:~/TTest.sh TTest.sh



4) You may use "nano" editor for quick changes of provided sources or view file content with "cat":

rvck@lxibm102:~$ nano TTest.java

rvck@lxibm102:~$ cat TTest.sh

#!/bin/sh

#SBATCH -p short # queue name

#SBATCH -N1 # number of computers (1 computer for OpenMP)

#SBATCH -c8 # number of cores for 1 computer

#SBATCH -C alpha # name of the cluster

java TTest # Executable



5) Compile sources:

rvck@lxibm102:~$ javac TTest.java



6) Load script for execution:

rvck@lxibm102:~$ sbatch TTest.sh

Submitted batch job 510723



7) Check if script is waiting / running

squeue

or

squeue -j 510723

8) After completion you will find in the cluster home directory file slurm_510723.out, that contains output of the program

rvck@lxibm102:~$ cat slurm-510723.out

<...>

#nThreads #workload #timeS #speedup

1 1024 1.282 1.0

2 1024 0.641 2.0

4 1024 0.324 3.95679012345679

8 1024 0.173 7.410404624277457

16 1024 0.187 6.855614973262032

32 1024 0.186 6.89247311827957

#completed

9) Then you can repeat from the 4-th step or disconnect from cluster using "exit"

