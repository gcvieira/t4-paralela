#! /bin/sh

ladcomp -env mpicc pw_crack_mpi.c -o c_mpi -lcrypto
srun -N 4 -n 8 --exclusive c_mpi
