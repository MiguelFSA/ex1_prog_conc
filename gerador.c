/*
Programa auxiliar que cria dois vetores com valores aleatórios e realiza o poduto interno entre eles de forma sequencial
Armazena tudo em um arquivo binário
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10000000
// Fator para gerar números positivos e negativos 
int fator = 1; 

int main(int argc, char* argv[]) {
    long int n; // Tamanho dos vetores 
    FILE *arquivo; 
    float *vetor1, *vetor2;
    double produto;
    long int i; // Variável auxiliar

    // Verifica os argumentos
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <dimensao> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Recebe o tamanho dos vetores como long int
    n = atol(argv[1]);

    // Abre o arquivo binário para escrita
    arquivo = fopen(argv[2], "wb");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo para escrita: %s\n", argv[2]);
        return 2;
    }

    // Aloca memória para os vetores
    vetor1 = malloc(sizeof(float) * n);
    vetor2 = malloc(sizeof(float) * n);
    if (!vetor1 || !vetor2) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 3;
    }

    // Gerador de números aleatórios
    srand(time(NULL));

    // Atribui valores nos vetores
    for (i = 0; i < n; i++) {
        vetor1[i] = (rand() % MAX) / 3.0 * fator;
        vetor2[i] = (rand() % MAX) / 3.0 * fator;
        fator *= -1; // Alterna o sinal
    }

    // Calcula o produto interno 
    produto = 0;
    for (i = 0; i < n; i++) {
        produto += vetor1[i] * vetor2[i];
    }

    // Escreve os dados no arquivo binário
    fwrite(&n, sizeof(long int), 1, arquivo);
    fwrite(vetor1, sizeof(float), n, arquivo);
    fwrite(vetor2, sizeof(float), n, arquivo);
    fwrite(&produto, sizeof(double), 1, arquivo);

    // Libera os recurso
    fclose(arquivo);
    free(vetor1);
    free(vetor2);

    // Encerra o programa
    printf("\nArquivo gerado com sucesso!\n");
    return 0;
}
