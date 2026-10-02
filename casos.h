#ifndef CASOS_H
#define CASOS_H

#include <stdio.h>
#include <stdlib.h>

#define NUM_CASOS 6
#define REPETICOES 10

#define THREADS_PADRAO_1 2
#define THREADS_PADRAO_2 4

/* Matriz grande usada pelos dois programas. */
#define GRANDE_LINHAS 4000
#define GRANDE_COLUNAS 4000

/*
 * Caso 6:
 * objetos 40x40 separados por 10 celulas de zero.
 *
 * 4000 / (40 + 10) gera 80 objetos por linha/coluna.
 * Total = 80 x 80 = 6400 objetos.
 */

typedef struct {
    const char *nome;
    int linhas;
    int colunas;
    int esperado;
    int *matriz;
} Caso;

/* Casos 1 a 5 do enunciado oficial. */
static const int CASO1[5][5] = {
    {1,1,0,0,0},
    {1,1,0,0,0},
    {0,0,0,1,0},
    {0,0,0,1,0},
    {1,0,0,0,0}
};

static const int CASO2[6][8] = {
    {0,0,0,0,0,0,1,1},
    {0,1,1,1,1,0,1,0},
    {0,0,1,1,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,1,0,0,1},
    {1,1,0,0,0,0,1,1}
};

static const int CASO3[8][8] = {
    {1,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,1,1,0,1,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,1},
    {0,0,1,0,0,0,1,1}
};

static const int CASO4[9][12] = {
    {0,1,1,0,0,0,0,0,0,0,1,0},
    {0,0,1,1,1,1,0,0,0,1,1,0},
    {0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0,0},
    {0,1,0,0,0,0,1,0,0,1,0,0},
    {0,1,1,0,0,0,0,0,1,1,0,0},
    {0,0,1,1,0,0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0,1,1,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,1}
};

static const int CASO5[12][12] = {
    {1,0,0,0,0,1,1,1,1,0,1,1},
    {0,1,0,0,0,1,0,0,1,0,1,0},
    {0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0},
    {1,1,0,0,0,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,1,1,0,0,1,0,0,0,0},
    {0,0,0,1,1,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0},
    {0,1,0,0,0,0,0,0,0,0,1,0},
    {0,1,1,0,0,0,1,0,0,0,0,1}
};

static void copiarMatriz(
    int *destino,
    const int *origem,
    int linhas,
    int colunas
) {
    int total;
    int i;

    total = linhas * colunas;

    for (i = 0; i < total; i++) {
        destino[i] = origem[i];
    }
}

static int gerarMatrizGrande(
    int *matriz,
    int linhas,
    int colunas
) {
    const int TAM_OBJETO = 40;
    const int ESPACO = 10;
    int quantidade;
    int i;
    int j;
    int linha;
    int coluna;

    quantidade = 0;

    for (i = 0; i < linhas * colunas; i++) {
        matriz[i] = 0;
    }

    for (linha = 0;
         linha + TAM_OBJETO <= linhas;
         linha += TAM_OBJETO + ESPACO) {

        for (coluna = 0;
             coluna + TAM_OBJETO <= colunas;
             coluna += TAM_OBJETO + ESPACO) {

            quantidade++;

            for (i = linha; i < linha + TAM_OBJETO; i++) {
                for (j = coluna; j < coluna + TAM_OBJETO; j++) {
                    matriz[i * colunas + j] = 1;
                }
            }
        }
    }

    return quantidade;
}

static void liberarCasos(Caso casos[NUM_CASOS]) {
    int i;

    for (i = 0; i < NUM_CASOS; i++) {
        free(casos[i].matriz);
        casos[i].matriz = NULL;
    }
}

static void prepararCasos(Caso casos[NUM_CASOS]) {
    int i;

    for (i = 0; i < NUM_CASOS; i++) {
        casos[i].nome = NULL;
        casos[i].linhas = 0;
        casos[i].colunas = 0;
        casos[i].esperado = 0;
        casos[i].matriz = NULL;
    }

    casos[0].nome = "Caso 1";
    casos[0].linhas = 5;
    casos[0].colunas = 5;
    casos[0].esperado = 3;
    casos[0].matriz = (int *)malloc(5 * 5 * sizeof(int));

    casos[1].nome = "Caso 2";
    casos[1].linhas = 6;
    casos[1].colunas = 8;
    casos[1].esperado = 4;
    casos[1].matriz = (int *)malloc(6 * 8 * sizeof(int));

    casos[2].nome = "Caso 3";
    casos[2].linhas = 8;
    casos[2].colunas = 8;
    casos[2].esperado = 5;
    casos[2].matriz = (int *)malloc(8 * 8 * sizeof(int));

    casos[3].nome = "Caso 4";
    casos[3].linhas = 9;
    casos[3].colunas = 12;
    casos[3].esperado = 6;
    casos[3].matriz = (int *)malloc(9 * 12 * sizeof(int));

    casos[4].nome = "Caso 5";
    casos[4].linhas = 12;
    casos[4].colunas = 12;
    casos[4].esperado = 7;
    casos[4].matriz = (int *)malloc(12 * 12 * sizeof(int));

    casos[5].nome = "Caso 6 - Matriz Grande";
    casos[5].linhas = GRANDE_LINHAS;
    casos[5].colunas = GRANDE_COLUNAS;
    casos[5].matriz = (int *)malloc(
        (size_t)GRANDE_LINHAS *
        (size_t)GRANDE_COLUNAS *
        sizeof(int)
    );

    for (i = 0; i < NUM_CASOS; i++) {
        if (casos[i].matriz == NULL) {
            fprintf(stderr,
                    "Erro ao alocar memoria para %s.\n",
                    casos[i].nome != NULL ? casos[i].nome : "caso");
            liberarCasos(casos);
            exit(EXIT_FAILURE);
        }
    }

    copiarMatriz(casos[0].matriz, &CASO1[0][0], 5, 5);
    copiarMatriz(casos[1].matriz, &CASO2[0][0], 6, 8);
    copiarMatriz(casos[2].matriz, &CASO3[0][0], 8, 8);
    copiarMatriz(casos[3].matriz, &CASO4[0][0], 9, 12);
    copiarMatriz(casos[4].matriz, &CASO5[0][0], 12, 12);

    casos[5].esperado = gerarMatrizGrande(
        casos[5].matriz,
        GRANDE_LINHAS,
        GRANDE_COLUNAS
    );
}

static int compararDouble(const void *a, const void *b) {
    double x;
    double y;

    x = *(const double *)a;
    y = *(const double *)b;

    if (x < y) {
        return -1;
    }
    if (x > y) {
        return 1;
    }
    return 0;
}

static double calcularMediana(double valores[], int quantidade) {
    qsort(valores, (size_t)quantidade, sizeof(double), compararDouble);

    if (quantidade % 2 == 1) {
        return valores[quantidade / 2];
    }

    return (valores[quantidade / 2 - 1] +
            valores[quantidade / 2]) / 2.0;
}

#endif
