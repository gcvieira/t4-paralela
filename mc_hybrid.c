#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int total_pontos, pontos_por_proc;
    int rank, size;
    int local_dentro = 0, total_dentro = 0;

    double inicio_total, fim_total;

    // Inicializa o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Processo 0 lê o número total de pontos
    if (rank == 0) {
        //printf("Informe o número total de pontos a gerar: ");
        scanf("%d", &total_pontos);
    }

    // Broadcast do número total de pontos para todos os processos
    MPI_Bcast(&total_pontos, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada processo calcula sua parte
    pontos_por_proc = total_pontos / size;

    // Início da contagem de tempo (rank 0)
    if (rank == 0)
        inicio_total = MPI_Wtime();

    // OpenMP: paraleliza dentro de cada processo
    #pragma omp parallel
    {
        unsigned int seed = time(NULL) ^ (rank * 1000) ^ omp_get_thread_num();
        int local = 0;

        #pragma omp for
        for (int i = 0; i < pontos_por_proc; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0)
                local++;
        }

        #pragma omp atomic
        local_dentro += local;
    }

    // Reduz todos os resultados locais em total_dentro no processo 0
    MPI_Reduce(&local_dentro, &total_dentro, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Rank 0 calcula Pi e imprime resultado
    if (rank == 0) {
        double pi = 4.0 * total_dentro / total_pontos;
        fim_total = MPI_Wtime();
        double tempo_total = fim_total - inicio_total;

        printf("\n--- RESULTADO HÍBRIDO (MPI + OpenMP) ---\n");
        printf("Total de pontos:          %d\n", total_pontos);
        printf("Total dentro do círculo:  %d\n", total_dentro);
        printf("Estimativa de Pi:         %.10f\n", pi);
        printf("Tempo total:              %.4f segundos\n", tempo_total);
    }

    // Finaliza MPI
    MPI_Finalize();
    return 0;
}
