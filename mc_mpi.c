#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int total_pontos, pontos_por_proc;
    int rank, size;
    int local_dentro = 0, total_dentro = 0;
    double inicio_total, fim_total;

    // Inicializa MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Processo 0 lê o número de pontos da linha de comando
    if (rank == 0) {
        if (argc != 2) {
            fprintf(stderr, "Uso: %s <total_de_pontos>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        total_pontos = atoi(argv[1]);
    }

    // Broadcast do total de pontos para todos os processos
    MPI_Bcast(&total_pontos, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide igualmente entre os processos
    pontos_por_proc = total_pontos / size;

    // Semente de aleatoriedade individual por processo
    unsigned int seed = time(NULL) ^ (rank * 1000);

    // Início da contagem de tempo (apenas no processo 0)
    if (rank == 0)
        inicio_total = MPI_Wtime();

    // Geração dos pontos (sequencial dentro de cada processo)
    for (int i = 0; i < pontos_por_proc; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y <= 1.0)
            local_dentro++;
    }

    // Reduz os resultados locais no processo 0
    MPI_Reduce(&local_dentro, &total_dentro, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Processo 0 calcula e imprime o resultado
    if (rank == 0) {
        double pi = 4.0 * total_dentro / total_pontos;
        fim_total = MPI_Wtime();
        double tempo_total = fim_total - inicio_total;

        printf("\n--- RESULTADO MPI PURO ---\n");
        printf("Processos MPI:             %d\n", size);
        printf("Total de pontos:           %d\n", total_pontos);
        printf("Total dentro do círculo:   %d\n", total_dentro);
        printf("Estimativa de Pi:          %.10f\n", pi);
        printf("Tempo total:               %.4f segundos\n", tempo_total);
    }

    // Finaliza MPI
    MPI_Finalize();
    return 0;
}
