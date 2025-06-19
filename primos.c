#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

// Função para verificar se um número é primo
int eh_primo(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    int limite = (int)sqrt(n);
    for (int i = 3; i <= limite; i += 2) {
        if (n % i == 0) return 0;
    }

    return 1;
}

int main(int argc, char** argv) {
    int A = 2;        // Início do intervalo
    int B = 100000;   // Fim do intervalo

    int rank, size;

    // Inicializa o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &size); // número total de processos

    // Divide o intervalo entre os processos MPI
    int total = B - A + 1;
    int chunk = total / size;
    int start = A + rank * chunk;
    int end = (rank == size - 1) ? B : start + chunk - 1;

    // Contador local de primos para cada processo
    int local_count = 0;

    // Paraleliza com OpenMP dentro do processo
    #pragma omp parallel for reduction(+:local_count)
    for (int i = start; i <= end; i++) {
        if (eh_primo(i)) {
            local_count++;
        }
    }

    // Reduz os contadores locais em um total global (somando tudo no rank 0)
    int total_count = 0;
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Somente o processo 0 imprime o resultado final
    if (rank == 0) {
        printf("Número total de primos entre %d e %d: %d\n", A, B, total_count);
    }

    // Finaliza o MPI
    MPI_Finalize();
    return 0;
}
