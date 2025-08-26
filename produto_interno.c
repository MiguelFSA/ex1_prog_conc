#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

float *vetor1;
float *vetor2;

typedef struct {
    long int n;
    int id;
    int nthreads;
} t_args;

void *produto_thread(void *arg) {
    t_args *args;
    long int n, fatia, ini, fim, i;
    int id, nthreads;
    double *produto_interno;

    args = (t_args *) arg;
    n = args->n;
    id = args->id;
    nthreads = args->nthreads;

    fatia = n / nthreads;
    ini = id * fatia;
    fim = (id == nthreads - 1) ? n : ini + fatia;

    produto_interno = malloc(sizeof(double));
    *produto_interno = 0;

    for (i = ini; i < fim; i++) {
        *produto_interno += vetor1[i] * vetor2[i];
    }

    free(arg);
    pthread_exit((void *) produto_interno);
}

int main(int argc, char *argv[]) {
    int nthreads, i;
    long int n;
    FILE *arquivo;
    double produto_seq, resultado_concorrente = 0;
    double inicio, fim, erro_relativo;
    pthread_t *tid;
    void *retorno;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <nthreads>\n", argv[0]);
        return 1;
    }

    nthreads = atoi(argv[2]);
    if (nthreads < 1) nthreads = 1;

    arquivo = fopen(argv[1], "rb");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 2;
    }

    fread(&n, sizeof(long int), 1, arquivo);

    vetor1 = malloc(sizeof(float) * n);
    vetor2 = malloc(sizeof(float) * n);
    if (!vetor1 || !vetor2) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 3;
    }

    fread(vetor1, sizeof(float), n, arquivo);
    fread(vetor2, sizeof(float), n, arquivo);
    fread(&produto_seq, sizeof(double), 1, arquivo);
    fclose(arquivo);

    if (nthreads > n) nthreads = n;

    tid = malloc(sizeof(pthread_t) * nthreads);

    GET_TIME(inicio);

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

    for (i = 0; i < nthreads; i++) {
        if (pthread_join(tid[i], &retorno)) {
            fprintf(stderr, "Erro ao aguardar thread %d\n", i);
            return 5;
        }
        resultado_concorrente += *((double *) retorno);
        free(retorno);
    }

    GET_TIME(fim);

    erro_relativo = fabs((produto_seq - resultado_concorrente) / produto_seq);

    printf("Produto interno calculado sequencialmente = %.10f\n", produto_seq);
    printf("Produto interno calculado de forma concorrente = %.10f\n", resultado_concorrente);
    printf("Erro relativo = %.10e\n", erro_relativo);
    printf("Tempo de execução do produto interno concorrente = %e segundos\n", fim - inicio);

    free(vetor1);
    free(vetor2);
    free(tid);

    return 0;
}
