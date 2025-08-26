#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10000000 
int fator = 1; 

int main(int argc, char* argv[]) {
    long int n;
    FILE *arquivo;
    float *vetor1, *vetor2;
    double produto;
    long int i;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <dimensao> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    n = atol(argv[1]);
    arquivo = fopen(argv[2], "wb");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo para escrita: %s\n", argv[2]);
        return 2;
    }

    vetor1 = malloc(sizeof(float) * n);
    vetor2 = malloc(sizeof(float) * n);
    if (!vetor1 || !vetor2) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 3;
    }

    srand(time(NULL));
    for (i = 0; i < n; i++) {
        vetor1[i] = (rand() % MAX) / 3.0 * fator;
        vetor2[i] = (rand() % MAX) / 3.0 * fator;
        fator *= -1;
    }

    produto = 0;
    for (i = 0; i < n; i++) {
        produto += vetor1[i] * vetor2[i];
    }

    fwrite(&n, sizeof(long int), 1, arquivo);
    fwrite(vetor1, sizeof(float), n, arquivo);
    fwrite(vetor2, sizeof(float), n, arquivo);
    fwrite(&produto, sizeof(double), 1, arquivo);

    fclose(arquivo);
    free(vetor1);
    free(vetor2);

    printf("\nArquivo gerado com sucesso!\n");
    return 0;
}
