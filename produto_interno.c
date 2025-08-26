/*
Programa principal que recebe o arquivo binario gerado por gerador.c
Calcula o produto interno dos vetores de forma concorrente com o número de threads indicado pelo input
Calcula também o erro relativo e o tempo de execução do calculo do produto interno 
*/


#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <math.h>       
#include "timer.h"      

// Vetores globais 
float *vetor1;
float *vetor2;

// Estrutura de argumentos para pthread_create
typedef struct {
    long int n;
    int id;
    int nthreads;
} t_args;

// Função passada por pthread_create. Cada thread calcula o produto interno parcial da sua fatia.
void *produto_thread(void *arg) {
    t_args *args;
    long int n, fatia, ini, fim, i;
    int id, nthreads;
    double *produto_interno;

    args = (t_args *) arg;
    n = args->n;
    id = args->id;
    nthreads = args->nthreads;

    // Define a fatia de cada thread
    fatia = n / nthreads;
    ini = id * fatia;
    fim = (id == nthreads - 1) ? n : ini + fatia;

    // Aloca memória
    produto_interno = malloc(sizeof(double));
    *produto_interno = 0;

    // Calcula o produto interno da fatia do vetor
    for (i = ini; i < fim; i++) {
        *produto_interno += vetor1[i] * vetor2[i];
    }

    free(arg); // Libera os argumentos alocados
    pthread_exit((void *) produto_interno); // Retorna o resultado parcial
}

int main(int argc, char *argv[]) {
    int nthreads, i;
    long int n;
    FILE *arquivo;
    double produto_seq, resultado_concorrente = 0;
    double inicio, fim, erro_relativo;
    pthread_t *tid;
    void *retorno;

    // Verifica os argumentos
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <nthreads>\n", argv[0]);
        return 1;
    }

    // Recebe o número de threads
    nthreads = atoi(argv[2]);
    if (nthreads < 1) nthreads = 1;

    // Abre o arquivo binário
    arquivo = fopen(argv[1], "rb");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 2;
    }

    // Obtem do arquivo o tamanho dos vetores
    fread(&n, sizeof(long int), 1, arquivo);

    // Aloca memória 
    vetor1 = malloc(sizeof(float) * n);
    vetor2 = malloc(sizeof(float) * n);
    if (!vetor1 || !vetor2) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 3;
    }

    // Lê o restante do arquivo binário(vetores e valor do produto interno sequencial)
    fread(vetor1, sizeof(float), n, arquivo);
    fread(vetor2, sizeof(float), n, arquivo);
    fread(&produto_seq, sizeof(double), 1, arquivo);
    fclose(arquivo);

    // Garante que o número de threads não supera o de elementos
    if (nthreads > n) nthreads = n;

    // Vetor de identificadores exigido por pthread_create
    tid = malloc(sizeof(pthread_t) * nthreads);

    GET_TIME(inicio); // Início da cronometragem 

    // Cria as threads 
    for (i = 0; i < nthreads; i++) {
        t_args *args = malloc(sizeof(t_args));
        args->n = n;
        args->id = i;
        args->nthreads = nthreads;

        if (pthread_create(&tid[i], NULL, produto_thread, (void *) args)) {
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return 4;
        }
    }

    // Espera o fim das threads e junta os produtos parciais
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(tid[i], &retorno)) {
            fprintf(stderr, "Erro ao aguardar thread %d\n", i);
            return 5;
        }
        resultado_concorrente += *((double *) retorno);
        free(retorno); // Libera o espaço do resultado parcial
    }

    GET_TIME(fim); // Fim da cronometragem

    // Calcula o erro relativo
    erro_relativo = fabs((produto_seq - resultado_concorrente) / produto_seq);

    // Imprime os resultados
    printf("Produto interno calculado sequencialmente = %.10f\n", produto_seq);
    printf("Produto interno calculado de forma concorrente = %.10f\n", resultado_concorrente);
    printf("Erro relativo = %.10e\n", erro_relativo);
    printf("Tempo de execução do produto interno concorrente = %e segundos\n", fim - inicio);

    // Libera os recursos
    free(vetor1);
    free(vetor2);
    free(tid);
    
    //encerra o programa
    return 0;
}
