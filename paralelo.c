#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "casos.h"

/*
 * ============================================================
 * PARALELO
 * ============================================================
 *
 * Implementacao com Pthreads e divisao por faixas de linhas.
 * Cada thread rotula componentes locais com conectividade 8.
 * A thread principal consolida componentes que cruzam fronteiras
 * usando Union-Find.
 *
 * Por padrao sao testadas 2 e 4 threads. Esses valores podem ser
 * configurados pela linha de comando, por exemplo:
 *     ./Paralelo 2 3
 *
 * Como o menor caso possui 5 linhas, cada configuracao aceita
 * entre 2 e 5 threads para manter trabalho real em cada faixa.
 * ============================================================
 */

typedef struct {
    const int *matriz;
    int *rotulos;
    int colunas;
    int linhaInicio;
    int linhaFim;
    int quantidadeLocal;
} DadosThread;

static double tempoAtual(void) {
    struct timespec tempo;

    if (clock_gettime(CLOCK_MONOTONIC, &tempo) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    return (double)tempo.tv_sec +
           (double)tempo.tv_nsec / 1000000000.0;
}

static int encontrar(int *pai, int x) {
    int raiz;

    raiz = x;

    while (pai[raiz] != raiz) {
        raiz = pai[raiz];
    }

    while (pai[x] != x) {
        int proximo;

        proximo = pai[x];
        pai[x] = raiz;
        x = proximo;
    }

    return raiz;
}

static void unir(int *pai, int a, int b) {
    int raizA;
    int raizB;

    raizA = encontrar(pai, a);
    raizB = encontrar(pai, b);

    if (raizA != raizB) {
        pai[raizB] = raizA;
    }
}

static void *processarBloco(void *arg) {
    DadosThread *dados;
    const int *matriz;
    int *rotulos;
    int colunas;
    int inicio;
    int fim;
    int quantidadeCelulas;
    int *pilha;
    int quantidade;
    int linha;
    int coluna;

    dados = (DadosThread *)arg;
    matriz = dados->matriz;
    rotulos = dados->rotulos;
    colunas = dados->colunas;
    inicio = dados->linhaInicio;
    fim = dados->linhaFim;
    quantidadeCelulas = (fim - inicio) * colunas;

    pilha = (int *)malloc(
        (size_t)(quantidadeCelulas > 0 ? quantidadeCelulas : 1) *
        sizeof(int)
    );

    if (pilha == NULL) {
        fprintf(stderr, "Erro de memoria na thread.\n");
        dados->quantidadeLocal = -1;
        return NULL;
    }

    quantidade = 0;

    for (linha = inicio; linha < fim; linha++) {
        for (coluna = 0; coluna < colunas; coluna++) {
            rotulos[linha * colunas + coluna] = -1;
        }
    }

    for (linha = inicio; linha < fim; linha++) {
        for (coluna = 0; coluna < colunas; coluna++) {
            int atual;

            atual = linha * colunas + coluna;

            if (matriz[atual] == 1 && rotulos[atual] == -1) {
                int rotuloAtual;
                int topo;

                rotuloAtual = quantidade++;
                topo = 0;
                pilha[topo++] = atual;
                rotulos[atual] = rotuloAtual;

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

                            if (nl < inicio || nl >= fim ||
                                nc < 0 || nc >= colunas) {
                                continue;
                            }

                            vizinho = nl * colunas + nc;

                            if (matriz[vizinho] == 1 &&
                                rotulos[vizinho] == -1) {
                                rotulos[vizinho] = rotuloAtual;
                                pilha[topo++] = vizinho;
                            }
                        }
                    }
                }
            }
        }
    }

    dados->quantidadeLocal = quantidade;
    free(pilha);

    return NULL;
}

static int contarObjetosParalelo(
    const int *matriz,
    int linhas,
    int colunas,
    int numeroThreads
) {
    int totalCelulas;
    int *rotulos;
    pthread_t *threads;
    DadosThread *dados;
    int *offset;
    int *pai;
    unsigned char *raizExiste;
    int t;
    int criadas;
    int retorno;
    int totalObjetosLocais;
    int linha;
    int coluna;
    int quantidadeFinal;
    int falha;

    totalCelulas = linhas * colunas;
    rotulos = NULL;
    threads = NULL;
    dados = NULL;
    offset = NULL;
    pai = NULL;
    raizExiste = NULL;
    criadas = 0;
    totalObjetosLocais = 0;
    quantidadeFinal = -1;
    falha = 0;

    rotulos = (int *)malloc((size_t)totalCelulas * sizeof(int));
    threads = (pthread_t *)malloc(
        (size_t)numeroThreads * sizeof(pthread_t)
    );
    dados = (DadosThread *)malloc(
        (size_t)numeroThreads * sizeof(DadosThread)
    );

    if (rotulos == NULL || threads == NULL || dados == NULL) {
        fprintf(stderr,
                "Erro de memoria no processamento paralelo.\n");
        falha = 1;
    }

    if (!falha) {
        for (t = 0; t < numeroThreads; t++) {
            dados[t].matriz = matriz;
            dados[t].rotulos = rotulos;
            dados[t].colunas = colunas;
            dados[t].linhaInicio = (linhas * t) / numeroThreads;
            dados[t].linhaFim = (linhas * (t + 1)) / numeroThreads;
            dados[t].quantidadeLocal = 0;
        }

        for (t = 0; t < numeroThreads; t++) {
            retorno = pthread_create(
                &threads[t],
                NULL,
                processarBloco,
                &dados[t]
            );

            if (retorno != 0) {
                fprintf(stderr,
                        "Erro ao criar thread: %s.\n",
                        strerror(retorno));
                falha = 1;
                break;
            }
            criadas++;
        }
    }

    for (t = 0; t < criadas; t++) {
        retorno = pthread_join(threads[t], NULL);
        if (retorno != 0) {
            fprintf(stderr,
                    "Erro ao aguardar thread: %s.\n",
                    strerror(retorno));
            falha = 1;
        }
    }

    if (!falha) {
        for (t = 0; t < numeroThreads; t++) {
            if (dados[t].quantidadeLocal < 0) {
                fprintf(stderr,
                        "Uma thread falhou ao alocar memoria.\n");
                falha = 1;
                break;
            }
        }
    }

    if (!falha) {
        offset = (int *)malloc(
            (size_t)numeroThreads * sizeof(int)
        );
        if (offset == NULL) {
            fprintf(stderr, "Erro de memoria para offsets.\n");
            falha = 1;
        }
    }

    if (!falha) {
        for (t = 0; t < numeroThreads; t++) {
            offset[t] = totalObjetosLocais;
            totalObjetosLocais += dados[t].quantidadeLocal;
        }

        for (t = 0; t < numeroThreads; t++) {
            for (linha = dados[t].linhaInicio;
                 linha < dados[t].linhaFim;
                 linha++) {
                for (coluna = 0; coluna < colunas; coluna++) {
                    int pos;

                    pos = linha * colunas + coluna;
                    if (rotulos[pos] >= 0) {
                        rotulos[pos] += offset[t];
                    }
                }
            }
        }

        pai = (int *)malloc(
            (size_t)(totalObjetosLocais > 0
                     ? totalObjetosLocais
                     : 1) * sizeof(int)
        );
        raizExiste = (unsigned char *)calloc(
            (size_t)(totalObjetosLocais > 0
                     ? totalObjetosLocais
                     : 1),
            sizeof(unsigned char)
        );

        if (pai == NULL || raizExiste == NULL) {
            fprintf(stderr,
                    "Erro de memoria no Union-Find.\n");
            falha = 1;
        }
    }

    if (!falha) {
        int i;

        for (i = 0; i < totalObjetosLocais; i++) {
            pai[i] = i;
        }

        /*
         * A divisao ocorre por faixas de linhas. Em cada fronteira,
         * uma celula da ultima linha da faixa superior pode se ligar
         * a tres celulas da primeira linha da faixa inferior:
         * diagonal esquerda, vertical e diagonal direita.
         */
        for (t = 0; t < numeroThreads - 1; t++) {
            int linhaSuperior;
            int linhaInferior;

            linhaSuperior = dados[t].linhaFim - 1;
            linhaInferior = dados[t + 1].linhaInicio;

            for (coluna = 0; coluna < colunas; coluna++) {
                int acima;
                int delta;

                acima = linhaSuperior * colunas + coluna;

                if (matriz[acima] != 1) {
                    continue;
                }

                for (delta = -1; delta <= 1; delta++) {
                    int colunaInferior;
                    int abaixo;

                    colunaInferior = coluna + delta;
                    if (colunaInferior < 0 ||
                        colunaInferior >= colunas) {
                        continue;
                    }

                    abaixo = linhaInferior * colunas +
                             colunaInferior;

                    if (matriz[abaixo] == 1) {
                        unir(
                            pai,
                            rotulos[acima],
                            rotulos[abaixo]
                        );
                    }
                }
            }
        }

        for (i = 0; i < totalObjetosLocais; i++) {
            int raiz;

            raiz = encontrar(pai, i);
            raizExiste[raiz] = 1;
        }

        quantidadeFinal = 0;
        for (i = 0; i < totalObjetosLocais; i++) {
            if (raizExiste[i]) {
                quantidadeFinal++;
            }
        }
    }

    free(raizExiste);
    free(pai);
    free(offset);
    free(dados);
    free(threads);
    free(rotulos);

    return falha ? -1 : quantidadeFinal;
}

static double executarConfiguracao(
    const Caso *caso,
    int numeroThreads,
    int *resultado
) {
    double tempos[REPETICOES];
    int r;
    int referencia;

    referencia = -1;

    for (r = 0; r < REPETICOES; r++) {
        double inicio;
        double fim;
        int resultadoAtual;

        inicio = tempoAtual();
        resultadoAtual = contarObjetosParalelo(
            caso->matriz,
            caso->linhas,
            caso->colunas,
            numeroThreads
        );
        fim = tempoAtual();

        if (resultadoAtual < 0) {
            fprintf(stderr,
                    "Falha no processamento paralelo.\n");
            exit(EXIT_FAILURE);
        }

        if (r == 0) {
            referencia = resultadoAtual;
        } else if (resultadoAtual != referencia) {
            fprintf(stderr,
                    "Resultado nao deterministico detectado.\n");
            exit(EXIT_FAILURE);
        }

        tempos[r] = fim - inicio;
    }

    *resultado = referencia;
    return calcularMediana(tempos, REPETICOES);
}

static int lerNumeroThreads(const char *texto, int *valor) {
    char *fim;
    long numero;

    fim = NULL;
    numero = strtol(texto, &fim, 10);

    if (texto[0] == '\0' || fim == NULL || *fim != '\0') {
        return 0;
    }

    if (numero < 2 || numero > 5) {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

int main(int argc, char *argv[]) {
    Caso casos[NUM_CASOS];
    int threads1;
    int threads2;
    int i;

    threads1 = THREADS_PADRAO_1;
    threads2 = THREADS_PADRAO_2;

    if (argc != 1 && argc != 3) {
        fprintf(stderr,
                "Uso: %s [THREADS_1 THREADS_2]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        if (!lerNumeroThreads(argv[1], &threads1) ||
            !lerNumeroThreads(argv[2], &threads2)) {
            fprintf(stderr,
                    "As quantidades de threads devem estar entre 2 e 5.\n");
            return EXIT_FAILURE;
        }
    }

    prepararCasos(casos);

    printf("\n");
    printf("==========================================================================\n");
    printf("                 DETECCAO DE OBJETOS - PARALELO\n");
    printf("==========================================================================\n");
    printf("Implementacao: Pthreads\n");
    printf("Conectividade: 8 vizinhos\n");
    printf("Threads testadas: %d e %d\n", threads1, threads2);
    printf("Repeticoes por configuracao: %d\n", REPETICOES);
    printf("Valor representativo: mediana dos tempos\n");
    printf("Matriz grande: %dx%d\n", GRANDE_LINHAS, GRANDE_COLUNAS);
    printf("==========================================================================\n\n");

    printf(
        "Caso | Dimensoes   | Esperado | "
        "T1 Obtido | T1 Tempo(s) | "
        "T2 Obtido | T2 Tempo(s) | Status\n"
    );
    printf(
        "-----|-------------|----------|"
        "-----------|-------------|"
        "-----------|-------------|--------\n"
    );

    for (i = 0; i < NUM_CASOS; i++) {
        int resultado1;
        int resultado2;
        double tempo1;
        double tempo2;
        int correto;

        resultado1 = 0;
        resultado2 = 0;

        tempo1 = executarConfiguracao(
            &casos[i],
            threads1,
            &resultado1
        );
        tempo2 = executarConfiguracao(
            &casos[i],
            threads2,
            &resultado2
        );

        correto = (resultado1 == casos[i].esperado) &&
                  (resultado2 == casos[i].esperado);

        printf(
            "%4d | %4dx%-6d | %8d | "
            "%9d | %11.9f | "
            "%9d | %11.9f | %s\n",
            i + 1,
            casos[i].linhas,
            casos[i].colunas,
            casos[i].esperado,
            resultado1,
            tempo1,
            resultado2,
            tempo2,
            correto ? "OK" : "ERRO"
        );
    }

    printf("\n");
    printf("==========================================================================\n");
    printf("Teste paralelo finalizado.\n");
    printf("Cada caso foi executado %d vezes com %d e %d threads.\n",
           REPETICOES,
           threads1,
           threads2);
    printf("==========================================================================\n");

    liberarCasos(casos);

    return 0;
}
