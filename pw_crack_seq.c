#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>

// Define o tamanho da senha
#define TAMANHO_SENHA 6

// Conjunto de caracteres que serão usados na senha (A-Z, a-z)
const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// Função que calcula o SHA-256 e preenche o buffer com a string hexadecimal (64 + '\0')
void calculaHash(const char *senha, char *hashStr) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256((const unsigned char *)senha, strlen(senha), hash);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(hashStr + (i * 2), "%02x", hash[i]);
	}
	hashStr[64] = '\0'; // finalizador da string
}

// Função que converte um número (índice) em uma senha baseada no charset
void indiceParaSenha(long long indice, char *senha, int base) {
	for (int i = TAMANHO_SENHA - 1; i >= 0; i--) {
		senha[i] = charset[indice % base];
		indice /= base;
	}
	senha[TAMANHO_SENHA] = '\0';
}

int main() {
	int base = strlen(charset);
	char senhaAlvo[] = "AFPMGo";  // senha fixa definida pelo programador
	char hashAlvo[65];
	char tentativa[TAMANHO_SENHA + 1];
	char hashTentativa[65];

	calculaHash(senhaAlvo, hashAlvo);  // Calcula o hash da senha real
	printf("Hash alvo: %s\n", hashAlvo);
	fflush(stdout);

	clock_t inicio = clock();

	//long long maxCombinacoes = 10000000;  // limite de combinações para teste (ajuste se quiser)
	//for (long long i = 0; i < maxCombinacoes; i++) {
	//bool senhaEncontrada = false;
	long long i = 0;
	while (1) {
		i++;
		indiceParaSenha(i, tentativa, base);
		calculaHash(tentativa, hashTentativa);

		if (strcmp(hashTentativa, hashAlvo) == 0) {
			printf("Senha encontrada: %s\n", tentativa);
			//senhaEncontrada = true;
			break;
		}

		// Debug opcional
		if (i % 100000 == 0) {
			printf("Tentativa %lld: %s\n", i, tentativa);
			fflush(stdout);
		}
	}

	clock_t fim = clock();
	double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
	printf("Tempo total de execução: %.2f segundos\n", tempo);
	return 0;
}
