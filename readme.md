# Trabalho 4 - programacao hibrida

## Problema
Transformar o trabalho 2, que utiliza apenas MPI, em um programa hibrido que utiliza MPI e OpenMP para melhor utilizar os recursos computacionais.

## Running

Sequencial:

```
gcc pw_crack_seq.c -o crack_sequencial -lcrypto
./crack_sequencial
```

MPI:

```
ladcomp -env mpicc pw_crack_mpi.c -o crack_mpi -lcrypto
srun -N 4 -n 8 --exclusive ./crack_mpi
```
