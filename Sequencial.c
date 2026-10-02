#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "casos.h"

/*
 * ============================================================
 * SEQUENCIAL
 * ============================================================
 *
 * Executa a versao sequencial de referencia.
 * Cada caso e executado REPETICOES vezes e o valor apresentado
 * e a mediana dos tempos.
 * ============================================================
 */

static double tempoAtual(void) {
    struct timespec tempo;

    if (clock_gettime(CLOCK_MONOTONIC, &tempo) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    return (double)tempo.tv_sec +
           (double)tempo.tv_nsec / 1000000000.0;
}

/*
 * Detecta objetos com conectividade 8 usando DFS iterativa.
 */
static int contarObjetosSequencial(
    const int *matriz,
    int linhas,
    int colunas
) {
    int total;
    unsigned char *visitado;
    int *pilha;
    int quantidade;
    int linha;
    int coluna;

    total = linhas * colunas;
    visitado = (unsigned char *)calloc(
        (size_t)total,
        sizeof(unsigned char)
    );
    pilha = (int *)malloc((size_t)total * sizeof(int));

    if (visitado == NULL || pilha == NULL) {
        fprintf(stderr,
                "Erro de memoria no processamento sequencial.\n");
        free(visitado);
        free(pilha);
        exit(EXIT_FAILURE);
    }

    quantidade = 0;

    for (linha = 0; linha < linhas; linha++) {
        for (coluna = 0; coluna < colunas; coluna++) {
            int atual;

            atual = linha * colunas + coluna;

            if (matriz[atual] == 1 && !visitado[atual]) {
                int topo;

                quantidade++;
                topo = 0;
                pilha[topo++] = atual;
                visitado[atual] = 1;

                while (topo > 0) {
                    int pos;
                    int l;
                    int c;
                    int dl;
                    int dc;

                    pos = pilha[--topo];
                    l = pos / colunas;
                    c = pos % colunas;

                    for (dl = -1; dl <= 1; dl++) {
                        for (dc = -1; dc <= 1; dc++) {
                            int nl;
                            int nc;
                            int vizinho;

                            if (dl == 0 && dc == 0) {
                                continue;
                            }

                            nl = l + dl;
                            nc = c + dc;

                            if (nl < 0 || nl >= linhas ||
                                nc < 0 || nc >= colunas) {
                                continue;
                            }

                            vizinho = nl * colunas + nc;

                            if (matriz[vizinho] == 1 &&
                                !visitado[vizinho]) {
                                visitado[vizinho] = 1;
                                pilha[topo++] = vizinho;
                            }
                        }
                    }
                }
            }
        }
    }

    free(visitado);
    free(pilha);

    return quantidade;
}

int main(void) {
    Caso casos[NUM_CASOS];
    double temposGrande[REPETICOES];
    int i;

    prepararCasos(casos);

    printf("\n");
    printf("===============================================================\n");
    printf("                 DETECCAO DE OBJETOS - SEQUENCIAL\n");
    printf("===============================================================\n");
    printf("Conectividade: 8 vizinhos\n");
    printf("Repeticoes por caso: %d\n", REPETICOES);
    printf("Valor representativo: mediana dos tempos\n");
    printf("Matriz grande: %dx%d\n", GRANDE_LINHAS, GRANDE_COLUNAS);
    printf("===============================================================\n\n");

    printf(
        "Caso | Dimensoes   | Esperado | Obtido | Tempo mediano (s) | Status\n"
    );
    printf(
        "-----|-------------|----------|--------|--------------------|--------\n"
    );

    for (i = 0; i < NUM_CASOS; i++) {
        double tempos[REPETICOES];
        int obtido;
        int r;
        double tempoMediano;
        int correto;

        obtido = 0;

        for (r = 0; r < REPETICOES; r++) {
            double inicio;
            double fim;

            inicio = tempoAtual();
            obtido = contarObjetosSequencial(
                casos[i].matriz,
                casos[i].linhas,
                casos[i].colunas
            );
            fim = tempoAtual();
            tempos[r] = fim - inicio;
            
            if (i == NUM_CASOS - 1) {
                temposGrande[r] = tempos[r];
            }
        }

        tempoMediano = calcularMediana(tempos, REPETICOES);
        correto = (obtido == casos[i].esperado);

        printf(
            "%4d | %4dx%-6d | %8d | %6d | %18.9f | %s\n",
            i + 1,
            casos[i].linhas,
            casos[i].colunas,
            casos[i].esperado,
            obtido,
            tempoMediano,
            correto ? "OK" : "ERRO"
        );
    }

    printf("\n");
    printf("Tempos individuais - Caso 6 (4000x4000)\n");
    printf("Repeticao | Tempo (s)\n");
    printf("----------|-------------\n");

    for (i = 0; i < REPETICOES; i++) {
        printf("%9d | %11.9f\n", i + 1, temposGrande[i]);
    }
    
    printf("\n");
    printf("===============================================================\n");
    printf("Teste sequencial finalizado.\n");
    printf("Cada caso foi executado %d vezes.\n", REPETICOES);
    printf("===============================================================\n");

    liberarCasos(casos);

    return 0;
}
