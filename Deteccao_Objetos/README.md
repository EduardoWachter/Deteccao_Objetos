# Contagem paralela de objetos em uma matriz binaria

Trabalho pratico da disciplina de Sistemas Operacionais - PUCRS - 2026/II.

## Integrantes

- Eduardo Dieter Wachter
- Joao Paulo Fritsch dos Santos
- Juliana Scapini Consoli
- Julia Santos de Melo

## Descricao

O projeto conta componentes conexos de valor `1` em matrizes binarias usando conectividade 8. Sao fornecidas duas implementacoes:

- `Sequencial.c`: versao sequencial de referencia, com DFS iterativa e pilha explicita;
- `paralelo.c`: versao paralela com Pthreads, divisao por faixas de linhas e consolidacao das fronteiras por Union-Find.

Os cinco primeiros casos correspondem as matrizes obrigatorias do enunciado. O sexto caso e uma matriz 4000x4000 usada para avaliacao de desempenho.

## Compilacao e execucao

A forma mais simples e executar:

```bash
chmod +x Executar.sh
./Executar.sh
```

O script usa ANSI C89/C90 e habilita os avisos do compilador:

```bash
cc -std=c89 -Wall -Wextra -pedantic -O2 Sequencial.c -o Sequencial
cc -std=c89 -Wall -Wextra -pedantic -O2 -pthread paralelo.c -o Paralelo
```

Tambem e possivel executar separadamente:

```bash
./Sequencial
./Paralelo 2 4
```

Na versao paralela, os dois argumentos permitem escolher as duas quantidades de threads testadas. Como o menor caso obrigatorio possui 5 linhas, sao aceitos valores entre 2 e 5.

## Arquitetura paralela

A matriz e dividida em faixas horizontais. Cada thread rotula, de forma independente, os componentes da sua faixa considerando os oito vizinhos que permanecem dentro dela. Depois de `pthread_join`, a thread principal converte os rotulos locais para identificadores globais e verifica as fronteiras entre faixas. Conexoes verticais e diagonais entre faixas sao unificadas por Union-Find. A contagem final corresponde ao numero de representantes distintos.

## Resultados

As saidas da execucao padrao sao gravadas em:

- `resultado_sequencial.txt`
- `resultado_paralelo.txt`

Cada configuracao e executada 10 vezes e o tempo apresentado e a mediana.

## Ferramentas externas

O ChatGPT (OpenAI) foi utilizado como ferramenta de apoio para revisao textual, conferencia dos requisitos do enunciado e adequacao do codigo. A versao final foi validada por compilacao com as flags indicadas acima e pela execucao dos casos de teste.
