# Trabalho 4 - programacao hibrida

## Problema
Transformar o trabalho 2, que utiliza apenas MPI, em um programa hibrido que utiliza MPI e OpenMP para melhor utilizar os recursos computacionais.

## Running

Sequencial:

```
gcc mc_sequential.c -o seq
./seq
```

OpenMP:

```
gcc mc_parallel.c -o parallel
srun --exclusive -N 1 ./parallel
```

MPI:

```
ladcomp -env mpicc mc_mpi.c -o mpi
srun --exclusive -N 2 -n 2 ./mpi
```

Hybrid (OpenMP + MPI):

```
ladcomp --env mpiompcc mc_hybrid.c -o hybrid
srun --exclusive -N 2 -n 2 ./hybrid
```
