#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>

#define TAMANHO_SENHA 6
#define NUM_TAREFAS 6 // Número de escravos (mestre é o processo 0)
#define CHARSET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define CHARSET_SIZE 52

// Função que calcula o SHA-256 e preenche o buffer com a string hexadecimal (64 + '\0')
void calculaHash(const char *senha, char *hashStr) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256((const unsigned char *)senha, strlen(senha), hash);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(hashStr + (i * 2), "%02x", hash[i]);
	}
	hashStr[64] = '\0'; // finalizador da string
}

// Gera uma senha correspondente a um índice numérico
void indiceParaSenha(long long indice, char* senha) {
	for (int i = TAMANHO_SENHA - 1; i >= 0; i--) {
		senha[i] = CHARSET[indice % CHARSET_SIZE];
		indice /= CHARSET_SIZE;
	}
	senha[TAMANHO_SENHA] = '\0';
}

int main(int argc, char** argv) {
	int my_rank, proc_n;
	MPI_Status status;
	int tarefa[NUM_TAREFAS]; // só para seguir o pseudocódigo
	int msg;
	char hashAlvo[256];

	// Senha correta hardcoded — você pode mudar isso
	const char* senhaCorreta = "AFPMGo";

	// Inicialização MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(&my_rank, &proc_n);

	// Precisamos de 5 processos: 1 mestre + 4 escravos
	/*
	   if (proc_n != NUM_TAREFAS + 1) {
	   if (my_rank == 0)
	   printf("Erro: Este programa precisa de exatamente %d processos (1 mestre + %d escravos).\n", NUM_TAREFAS + 1, NUM_TAREFAS);
	   MPI_Finalize();
	   return 1;
	   }
	 */

	// Total de combinações possíveis com TAMANHO_SENHA e CHARSET_SIZE
	long long totalComb = 1;
	for (int i = 0; i < TAMANHO_SENHA; i++)
		totalComb *= CHARSET_SIZE;

	double start_time = 0.0;

	if (my_rank == 0) {
		// === MESTRE ===
		// Calcula o hash da senha correta
		calculaHash(senhaCorreta, hashAlvo);

		// Marca o início da contagem de tempo aqui — após o hash estar pronto
		start_time = MPI_Wtime();

		// Envia o hash alvo para todos os escravos
		MPI_Bcast(hashAlvo, sizeof(hashAlvo), MPI_CHAR, 0, MPI_COMM_WORLD);

		// Envia tarefas (apenas número, pois o range está implícito pelo rank)
		for (int i = 0; i < NUM_TAREFAS; i++) {
			msg = i;
			MPI_Send(&msg, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
		}

		// Espera a resposta de qualquer processo com a senha correta
		char senhaEncontrada[TAMANHO_SENHA + 1];
		MPI_Recv(senhaEncontrada, TAMANHO_SENHA + 1, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		double end_time = MPI_Wtime();

		printf(">>> Senha encontrada pelo processo %d: %s\n", status.MPI_SOURCE, senhaEncontrada);
		fflush(stdout);
		printf(">>> Tempo total de execução: %.8f segundos\n", end_time - start_time);
		fflush(stdout);

		// Finaliza todos os processos
		MPI_Abort(MPI_COMM_WORLD, 0);
	} else {
		// === ESCRAVO ===

		// Recebe o hash alvo
		MPI_Bcast(hashAlvo, sizeof(hashAlvo), MPI_CHAR, 0, MPI_COMM_WORLD);

		// Recebe "tarefa" (apenas para manter o pseudocódigo original)
		MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		// Define intervalo de senhas a testar com base no rank
		long long intervalo = totalComb / NUM_TAREFAS;
		long long inicio = (my_rank - 1) * intervalo;
		long long fim = (my_rank == NUM_TAREFAS) ? totalComb : inicio + intervalo;

		char tentativa[TAMANHO_SENHA + 1];
		char hashLocal[65];  // buffer para armazenar hash da tentativa

		for (long long i = inicio; i < fim; i++) {
			indiceParaSenha(i, tentativa);
			printf("P %d testando: %s\n", my_rank, tentativa);
			fflush(stdout);

			calculaHash(tentativa, hashLocal);
			if (strcmp(hashLocal, hashAlvo) == 0) {
				// Encontrou a senha correta, envia ao mestre
				MPI_Send(tentativa, TAMANHO_SENHA + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
				break;
			}
		}
	}

	MPI_Finalize();
	return 0;
}
