# Relatório técnico - Contagem paralela de objetos em uma matriz binária

> **Disciplina:** Sistemas Operacionais - 2026/II  
> **Professor:** Prof. Filipo Novo Mór  
> **Instituição:** Pontifícia Universidade Católica do Rio Grande do Sul - Escola Politécnica    
> **Versão do relatório:** 1.0  
> **Data:** 02/10/2026

## Identificação

| Campo | Informação |
|---|---|
| Integrante 1 | Eduardo Dieter Wachter |
| Matrícula do integrante 1 | 24200503 |
| Integrante 2 | João Paulo Fritsch dos Santos |
| Matrícula do integrante 2 | 23103898 |
| Integrante 3 | Juliana Scapini Consoli |
| Matrícula do integrante 3 | 24280057 |
| Integrante 4 | Júlia Santos de Melo |
| Matrícula do integrante 4 | 24103892 |
| Modalidade | Grupo de 4 integrantes |
| Turma | 330 |
| Estratégia paralela | Pthreads |
| Plataforma testada | macOS |
| Commit avaliado | [PREENCHER HASH DO COMMIT] |

## Resumo

Este trabalho apresenta uma implementação sequencial e uma implementação paralela para a contagem de objetos em uma matriz binária. Um objeto é definido como um componente de células de valor `1` conectado por qualquer um dos oito vizinhos possíveis, incluindo diagonais. A versão sequencial utiliza busca em profundidade iterativa, com vetor de visitados e pilha explícita. A versão paralela utiliza Pthreads e divide a matriz em faixas horizontais de linhas; cada thread identifica componentes locais de forma independente e, após a sincronização por `pthread_join`, os rótulos são convertidos para identificadores globais e consolidados com Union-Find. As cinco matrizes obrigatórias produziram exatamente as contagens esperadas nas versões sequencial, com 2 threads e com 4 threads. Em uma matriz adicional de 4000 × 4000, com 6400 objetos, a execução apresentou mediana de 192,783 ms na versão sequencial, 236,405 ms com 2 threads e 237,393 ms com 4 threads. As acelerações foram de aproximadamente 0,815 e 0,812, respectivamente, indicando que, nessa execução, a sobrecarga da solução paralela superou o ganho obtido pela divisão do processamento.

**Palavras-chave:** sistemas operacionais; paralelismo; Pthreads; conectividade 8; flood fill; componentes conexos; Union-Find.

## 1. Visão geral do problema

O programa trabalha com uma matriz binária na qual `0` representa o fundo e `1` representa parte de um objeto. Um objeto corresponde a um componente de células de valor `1` conectadas horizontalmente, verticalmente ou diagonalmente, seguindo a **conectividade 8**.

O projeto contém duas implementações funcionalmente equivalentes:

1. uma versão sequencial, usada como referência de correção e desempenho;
2. uma versão paralela baseada em Pthreads, com decomposição da matriz em faixas horizontais de linhas.

### 1.1 Objetivos da implementação

- Contar corretamente os objetos com conectividade 8.
- Manter uma versão sequencial como referência de correção e desempenho.
- Distribuir trabalho efetivo entre duas ou mais threads POSIX.
- Reconhecer e unificar objetos que atravessem as divisões da matriz.
- Produzir resultados determinísticos e idênticos nas versões sequencial e paralela.
- Evitar condições de corrida, deadlocks, atualizações perdidas e contagens duplicadas.
- Avaliar a sobrecarga e o desempenho da solução paralela.

### 1.2 Requisitos atendidos

| Requisito | Como foi atendido | Evidência no repositório |
|---|---|---|
| ANSI C C89/C90 | Código ajustado para C89/C90 e compilado com `-std=c89 -Wall -Wextra -pedantic`. | `Sequencial.c`, `paralelo.c`, `casos.h`, `Executar.sh` |
| Conectividade 8 | As buscas verificam os oito vizinhos; a consolidação entre faixas inclui conexões verticais e diagonais. | `Sequencial.c` e `paralelo.c` |
| Versão sequencial | DFS iterativa com vetor de visitados e pilha explícita. | `Sequencial.c` |
| Versão paralela | Pthreads, divisão por faixas de linhas, rotulação local e Union-Find. | `paralelo.c` |
| Duas ou mais unidades concorrentes | A execução padrão testa 2 e 4 threads. | `Executar.sh` e `resultado_paralelo.txt` |
| Quantidade configurável de trabalhadores | As duas quantidades podem ser informadas por argumentos da linha de comando. | `./Paralelo 2 4` |
| Consolidação entre regiões | Rótulos locais recebem offsets globais e equivalências de fronteira são unidas. | `paralelo.c` |
| Tratamento horizontal, vertical e diagonal | A conectividade 8 é preservada durante a busca local, verificando vizinhos horizontais, verticais e diagonais. Na consolidação entre faixas, são verificadas a célula diretamente abaixo e as duas células diagonalmente adjacentes. | `Sequencial.c` e `paralelo.c` |
| Verificação das chamadas POSIX | Retornos de `clock_gettime`, `pthread_create` e `pthread_join` são verificados. | `Sequencial.c` e `paralelo.c` |
| Liberação dos recursos | As estruturas alocadas dinamicamente são liberadas com `free`, e todas as threads criadas são finalizadas e aguardadas com `pthread_join`. | `Sequencial.c`, `paralelo.c` e `casos.h` |
| Compilação reproduzível | O script `Executar.sh` contém os comandos necessários para compilar e executar as versões sequencial e paralela com as mesmas opções de compilação. | `Executar.sh` e `README.md` |

## 2. Organização do repositório

A estrutura atual do projeto mantém as implementações, os casos de teste, os resultados e a documentação diretamente na raiz do repositório.

```text
.
├── README.md
├── RELATORIO_TECNICO.md
├── RELATORIO_TECNICO.docx
├── Sequencial.c
├── paralelo.c
├── casos.h
├── Executar.sh
├── resultado_sequencial.txt
└── resultado_paralelo.txt
```

| Caminho | Finalidade |
|---|---|
| `README.md` | Descrição do projeto, compilação, execução e arquitetura resumida. |
| `RELATORIO_TECNICO.md` | Relatório técnico em Markdown. |
| `RELATORIO_TECNICO.docx` | Versão editável do mesmo relatório técnico. |
| `Sequencial.c` | Implementação sequencial da contagem de objetos. |
| `paralelo.c` | Implementação paralela utilizando Pthreads. |
| `casos.h` | Matrizes obrigatórias, matriz adicional e funções auxiliares compartilhadas. |
| `Executar.sh` | Script para compilar e executar as versões sequencial e paralela. |
| `resultado_sequencial.txt` | Saída obtida nos testes da versão sequencial. |
| `resultado_paralelo.txt` | Saída obtida nos testes da versão paralela. |

## 3. Ambiente de desenvolvimento e execução

### 3.1 Hardware e software

Os testes de desempenho apresentados neste relatório foram executados no ambiente abaixo.

| Item | Especificação |
|---|---|
| Processador | A18 Pro |
| Núcleos físicos | 6 |
| Processadores lógicos | 6 |
| Memória RAM | 8 GB |
| Sistema operacional | macOS |
| Arquitetura | ARM |
| Compilador | gcc |
| Padrão da linguagem | ANSI C C89/C90 (`-std=c89`) |
| APIs POSIX utilizadas | Sequencial: `clock_gettime`; Paralela: Pthreads (`pthread_create` e `pthread_join`) e `clock_gettime` |
| Flags de compilação | Sequencial: `-std=c89 -O2 -Wall -Wextra -pedantic`; Paralela: `-std=c89 -O2 -Wall -Wextra -pedantic -pthread` |

### 3.2 Compilação

A compilação pode ser executada pelo script:

```bash
chmod +x Executar.sh
./Executar.sh
```

Ou diretamente:

```bash
cc -std=c89 -Wall -Wextra -pedantic -O2 Sequencial.c -o Sequencial
cc -std=c89 -Wall -Wextra -pedantic -O2 -pthread paralelo.c -o Paralelo
```

### 3.3 Execução

```bash
./Sequencial
./Paralelo 2 4
```

Também é possível testar outras duas quantidades entre 2 e 5 threads:

```bash
./Paralelo 3 5
```

O limite superior atual decorre do fato de o menor caso obrigatório possuir cinco linhas; assim, cada thread configurada recebe pelo menos uma linha de trabalho.

### 3.4 Formato da entrada e da saída

As matrizes estão definidas em `casos.h`. O programa não solicita uma matriz pela entrada padrão. As versões executam automaticamente os cinco casos obrigatórios e a matriz adicional de desempenho.

A saída informa:

- dimensões da matriz;
- quantidade esperada de objetos;
- quantidade obtida;
- tempo mediano das 10 repetições;
- situação `OK` ou `ERRO`;
- para a matriz 4000 × 4000, os 10 tempos individuais usados no cálculo da mediana.

Na versão paralela, são exibidas duas configurações de threads em cada execução. Para a matriz de desempenho, os tempos individuais das duas configurações também são apresentados lado a lado.

## 4. Arquitetura da solução

### 4.1 Fluxo geral

```text
Preparar casos
    |
    +-- Sequencial
    |     -> percorrer matriz
    |     -> DFS com 8 vizinhos
    |     -> contar componentes
    |
    +-- Paralelo
          -> dividir a matriz em faixas de linhas
          -> criar Pthreads
          -> rotular componentes locais
          -> pthread_join
          -> aplicar offsets aos rótulos
          -> verificar fronteiras entre faixas
          -> consolidar equivalências com Union-Find
          -> contar representantes globais
```

Cada caso é executado 10 vezes e a mediana dos tempos é apresentada.

### 4.2 Estruturas de dados principais

| Estrutura | Tipo/representação | Responsabilidade | Compartilhada? | Proteção utilizada |
|---|---|---|---|---|
| Matriz de entrada | Vetor linear de `int` | Armazenar `0` e `1`. | Sim, somente leitura | Não requer bloqueio |
| Células visitadas | Vetor de `unsigned char` | Marcar células processadas na versão sequencial. | Não | Não se aplica |
| Pilha da DFS | Vetor de `int` | Guardar posições pendentes da busca. | Não | Uma pilha por execução/thread |
| Rótulos | Vetor de `int` | Identificar componentes locais. | Sim | Cada thread escreve somente em sua faixa |
| `DadosThread` | `struct` | Passar matriz, limites e resultado local. | Não entre trabalhadores | Uma entrada por thread |
| Offsets | Vetor de `int` | Converter rótulos locais em IDs globais. | Usado após `pthread_join` | Não se aplica |
| Union-Find | Vetor `pai` | Consolidar rótulos equivalentes. | Usado pela thread principal | Não se aplica |

## 5. Implementação sequencial

### 5.1 Algoritmo

A versão sequencial percorre todas as células da matriz. Quando encontra uma célula de valor `1` ainda não visitada, incrementa o contador de objetos e inicia uma busca em profundidade iterativa. A posição inicial é marcada como visitada e inserida em uma pilha. Enquanto a pilha contém posições, o algoritmo remove uma célula e examina os oito vizinhos possíveis. Todo vizinho válido, igual a `1` e ainda não visitado, é marcado e colocado na pilha.

Quando a pilha esvazia, todo o componente conectado àquela célula inicial foi processado. A busca utiliza pilha explícita em vez de recursão, evitando crescimento excessivo da pilha de chamadas em componentes grandes.

### 5.2 Pseudocódigo

```text
FUNÇÃO contar_objetos_sequencial(matriz):
    criar vetor de visitados
    criar pilha
    quantidade = 0

    PARA cada célula da matriz:
        SE célula == 1 E ainda não visitada:
            quantidade = quantidade + 1
            marcar célula
            empilhar célula

            ENQUANTO pilha não vazia:
                atual = desempilhar()

                PARA cada um dos 8 vizinhos:
                    SE vizinho válido, igual a 1 e não visitado:
                        marcar vizinho
                        empilhar vizinho

    liberar estruturas
    retornar quantidade
```

### 5.3 Complexidade e uso de memória

| Aspecto | Análise | Justificativa |
|---|---|---|
| Complexidade de tempo | `O(L × C)` | Cada célula é examinada e uma célula de objeto é marcada no máximo uma vez. |
| Complexidade de espaço | `O(L × C)` | Vetor de visitados e pilha podem crescer proporcionalmente ao número de células. |
| Risco de recursão excessiva | Não existe | A DFS é iterativa e usa pilha alocada dinamicamente. |

## 6. Implementação paralela

### 6.1 Modelo de concorrência

| Decisão | Escolha do grupo | Justificativa |
|---|---|---|
| Unidade de execução | Thread POSIX | Permite compartilhar a matriz e os rótulos no mesmo processo. |
| Quantidade de trabalhadores | Configurável; padrão 2 e 4 | Permite comparar ao menos duas configurações. |
| Divisão do trabalho | Faixas horizontais de linhas | Mantém regiões de escrita disjuntas e simplifica a consolidação. |
| Escalonamento | Estático | Cada thread recebe previamente um intervalo de linhas. |
| Comunicação | Memória compartilhada | Não é necessário IPC entre processos. |
| Sincronização | `pthread_join` | A consolidação só começa após o término de todas as threads. |

### 6.2 Decomposição da matriz

Para uma matriz com `L` linhas e `p` threads, a thread `t` recebe:

```text
inicio = (L × t) / p
fim    = (L × (t + 1)) / p
```

Como os limites são calculados por divisão inteira, as sobras são distribuídas entre as faixas e a diferença entre os tamanhos das regiões é de no máximo uma linha.

A decomposição adotada é **somente horizontal**. Portanto, com quatro threads a matriz é dividida em quatro faixas de linhas, e não em uma grade 2 × 2. Essa estratégia é permitida pelo enunciado, que aceita faixas de linhas, colunas, blocos ou outra decomposição justificada.

Cada thread realiza a DFS considerando os oito vizinhos, mas só aceita vizinhos cuja linha permaneça dentro da faixa atribuída. Componentes que atravessam a fronteira entre duas faixas são temporariamente separados e unidos na consolidação.

### 6.3 Paralelismo efetivo

| Etapa | Sequencial ou paralela? | Unidade responsável | Motivo |
|---|---|---|---|
| Preparação dos casos | Sequencial | Thread principal | Executada antes dos testes. |
| Particionamento | Sequencial | Thread principal | Define os intervalos das faixas. |
| Identificação local | Paralela | Todas as Pthreads | Cada thread realiza DFS em linhas diferentes. |
| Espera das threads | Sincronização | Thread principal | `pthread_join` atua como barreira. |
| Aplicação dos offsets | Sequencial | Thread principal | Converte IDs locais em globais. |
| Análise das fronteiras | Sequencial | Thread principal | Registra equivalências entre faixas. |
| Contagem final | Sequencial | Thread principal | Conta representantes do Union-Find. |

O paralelismo é efetivo porque diferentes threads executam simultaneamente a busca e a rotulação dos componentes de regiões distintas da matriz.

### 6.4 Sincronização, comunicação e regiões críticas

| Recurso/dado | Risco concorrente | Mecanismo usado | Escopo da proteção | Justificativa |
|---|---|---|---|---|
| Matriz de entrada | Leitura concorrente | Nenhum bloqueio | Toda a matriz | Não há escrita. |
| Vetor de rótulos | Condição de corrida | Particionamento por linhas | Faixa de cada thread | Cada thread escreve em posições distintas. |
| Dados de cada thread | Atualização concorrente | Uma `struct` por thread | Entrada individual | Trabalhadores não compartilham a mesma estrutura. |
| Union-Find | Condição de corrida | Execução após `pthread_join` | Etapa de consolidação | Apenas a thread principal acessa a estrutura. |

Não existem múltiplos mutexes ou semáforos, portanto não há risco de deadlock por ordem de aquisição de bloqueios. A sincronização necessária é obtida aguardando cada trabalhador com `pthread_join`.

## 7. Consolidação dos componentes

A soma direta das contagens locais não é suficiente, porque um único objeto pode aparecer em duas faixas e ser contado uma vez por cada thread. A consolidação identifica esses componentes equivalentes e os transforma em um único componente global.

### 7.1 Identificação local

Cada thread inicia seus rótulos em `0` e incrementa o identificador a cada novo componente encontrado dentro de sua faixa.

Como diferentes threads podem produzir o mesmo rótulo local, após o término de todas elas é criado um vetor de offsets. O offset de uma faixa corresponde à soma das quantidades de componentes encontrados nas faixas anteriores. Somar esse valor aos rótulos locais produz identificadores globais únicos.

### 7.2 Verificação das fronteiras

Como a matriz é particionada somente em faixas de linhas, existe apenas uma fronteira de partição horizontal entre regiões consecutivas. Para cada célula de valor `1` na última linha da faixa superior, são verificadas até três posições na primeira linha da faixa inferior.

| Situação | Pares de células verificados | Como a equivalência é registrada |
|---|---|---|
| Conexão vertical entre faixas | `(linha superior, c)` com `(linha inferior, c)` | União dos rótulos globais no Union-Find |
| Conexão diagonal esquerda | `(linha superior, c)` com `(linha inferior, c-1)` | União dos rótulos globais no Union-Find |
| Conexão diagonal direita | `(linha superior, c)` com `(linha inferior, c+1)` | União dos rótulos globais no Union-Find |
| Fronteira vertical entre blocos | Não se aplica | A estratégia não divide a matriz em colunas |
| Encontro de quatro blocos | Não se aplica | Faixas horizontais não criam encontro de quatro regiões |

### 7.3 Unificação e contagem global

O Union-Find mantém um vetor `pai` para os identificadores globais. Sempre que duas células de valor `1` em lados opostos de uma fronteira são adjacentes pela conectividade 8, os representantes de seus rótulos são unidos.

Após a análise de todas as fronteiras, a função de busca do representante aplica compressão de caminho. Em seguida, o programa marca cada representante distinto e conta quantos permanecem, obtendo a quantidade final de objetos sem duplicar componentes que atravessam regiões.

### 7.4 Exemplo rastreável

No Caso 5, com 2 threads, a matriz 12 × 12 é dividida em duas faixas de seis linhas. A primeira faixa encontra 4 componentes locais e a segunda encontra 5, totalizando inicialmente 9 identificadores. O offset da segunda faixa é 4.

Duas equivalências globais distintas são identificadas na fronteira, reduzindo os 9 componentes locais para 7 componentes globais, que é o resultado esperado.

| Região | Rótulo local/global | Células de fronteira relevantes | Equivalência global |
|---|---|---|---|
| Faixa superior / inferior | `0 / 0` e `1 / 5` | `(6,6)` ↔ `(7,7)` | `0 ↔ 5` |
| Faixa superior / inferior | `3 / 3` e `0 / 4` | `(6,1)` ↔ `(7,1)` | `3 ↔ 4` |

## 8. Correção e testes funcionais

### 8.1 Procedimento de validação

O script `Executar.sh` recompila as duas versões e executa todos os casos. Cada caso é executado 10 vezes.

Na versão paralela, as repetições de uma mesma configuração são verificadas quanto ao resultado. Se uma repetição produzir uma contagem diferente da primeira, o programa encerra indicando resultado não determinístico. A contagem final também é comparada com o valor esperado de cada caso.

### 8.2 Matrizes obrigatórias

| Exemplo | Dimensões | Objetos esperados | Resultado sequencial | Resultado paralelo | Trabalhadores | Situação | Evidência |
|---:|---:|---:|---:|---:|---|---|---|
| 1 | 5 × 5 | 3 | 3 | 3 | 2 e 4 threads | Aprovado | `resultado_sequencial.txt` e `resultado_paralelo.txt` |
| 2 | 6 × 8 | 4 | 4 | 4 | 2 e 4 threads | Aprovado | `resultado_sequencial.txt` e `resultado_paralelo.txt` |
| 3 | 8 × 8 | 5 | 5 | 5 | 2 e 4 threads | Aprovado | `resultado_sequencial.txt` e `resultado_paralelo.txt` |
| 4 | 9 × 12 | 6 | 6 | 6 | 2 e 4 threads | Aprovado | `resultado_sequencial.txt` e `resultado_paralelo.txt` |
| 5 | 12 × 12 | 7 | 7 | 7 | 2 e 4 threads | Aprovado | `resultado_sequencial.txt` e `resultado_paralelo.txt` |

### 8.3 Caso de teste adicional

Foi adicionada uma matriz 4000 × 4000, gerada deterministicamente em `casos.h`. Ela contém objetos quadrados 40 × 40 separados por 10 células de fundo. São formados 80 objetos em cada direção, totalizando 6400 objetos.

| ID | Dimensões | Característica avaliada | Referência | Configurações paralelas | Resultado obtido | Situação |
|---|---:|---|---:|---|---:|---|
| A1 | 4000 × 4000 | Matriz grande para desempenho | 6400 | 2 e 4 threads | 6400 | Aprovado |

### 8.4 Repetibilidade e determinismo

| Teste | Repetições | Configurações | Resultados idênticos? | Observações |
|---|---:|---|---|---|
| Casos 1 a 6 | 10 por configuração | Sequencial, 2 threads e 4 threads | Sim | Nenhuma divergência de contagem foi observada na saída fornecida. |

## 9. Avaliação de desempenho

### 9.1 Metodologia experimental

| Parâmetro | Valor adotado |
|---|---|
| Matriz principal | 4000 × 4000, com 6400 objetos |
| Mesmos dados em todas as versões? | Sim |
| Relógio/API de medição | `clock_gettime(CLOCK_MONOTONIC, ...)` |
| Trecho medido | Função de contagem; preparação dos casos e impressão não fazem parte do tempo medido |
| Aquecimentos descartados | Não há etapa específica de aquecimento |
| Repetições por configuração | 10 |
| Medida representativa | Mediana |
| Critério para dispersão | Desvio-padrão amostral calculado a partir das 10 repetições |
| Carga do sistema durante os testes | Não registrada |
| Flags de otimização | `-O2` |

### 9.2 Métricas

A aceleração é calculada por:

```text
S(p) = Tsequencial / Tparalelo(p)
```

A eficiência paralela é:

```text
E(p) = S(p) / p
```

### 9.3 Resultados consolidados

| Versão | Trabalhadores (`p`) | Tempo representativo (ms) | Dispersão (ms) | Aceleração `S(p)` | Eficiência `E(p)` | Resultado correto? |
|---|---:|---:|---:|---:|---:|---|
| Sequencial | 1 | 192,783 | 25,085360 | 1,000 | 1,000 | Sim |
| Paralela | 2 | 236,405 | 23,171472 | 0,815 | 0,408 | Sim |
| Paralela | 4 | 237,393 | 28,280412 | 0,812 | 0,203 | Sim |

### 9.4 Dados brutos das repetições

A matriz 4000 × 4000 foi executada 10 vezes em cada configuração. A saída atual registra os 10 tempos individuais e utiliza a mediana como valor representativo. Todos os valores das tabelas estão em milissegundos. Para manter a leitura adequada, as repetições foram distribuídas em duas tabelas.

**Repetições 1 a 5**

| Versão | Trabalhadores | Rep. 1 (ms) | Rep. 2 (ms) | Rep. 3 (ms) | Rep. 4 (ms) | Rep. 5 (ms) |
|---|---:|---:|---:|---:|---:|---:|
| Sequencial | 1 | 205,457345 | 194,346702 | 268,299076 | 190,831679 | 191,218852 |
| Paralela | 2 | 279,169026 | 237,534296 | 262,400521 | 233,675399 | 229,775685 |
| Paralela | 4 | 314,242695 | 240,750924 | 231,296596 | 229,755918 | 236,999497 |

**Repetições 6 a 10 e mediana**

| Versão | Trabalhadores | Rep. 6 (ms) | Rep. 7 (ms) | Rep. 8 (ms) | Rep. 9 (ms) | Rep. 10 (ms) | Mediana (ms) |
|---|---:|---:|---:|---:|---:|---:|---:|
| Sequencial | 1 | 187,328976 | 191,086014 | 225,914203 | 207,505380 | 190,733355 | 192,782777 |
| Paralela | 2 | 230,627045 | 235,274937 | 230,674193 | 265,597207 | 292,293360 | 236,404616 |
| Paralela | 4 | 237,785868 | 235,549226 | 230,208142 | 271,840961 | 278,902139 | 237,392683 |

Os mesmos valores permanecem registrados em `resultado_sequencial.txt` e `resultado_paralelo.txt`.

### 9.5 Gráfico de tempo de execução

Não foi gerado gráfico para a versão atual. O enunciado exige a apresentação dos tempos e da aceleração, mas não torna obrigatória a geração de gráfico.

### 9.6 Gráfico de aceleração

Não foi gerado gráfico para a versão atual.

### 9.7 Gráfico de eficiência

Não foi gerado gráfico para a versão atual.

### 9.8 Análise dos resultados

Nas matrizes pequenas, o custo de criar, agendar e finalizar threads é muito maior do que o trabalho necessário para processar poucas dezenas de células. Por isso, a versão paralela apresenta tempos muito superiores aos da versão sequencial nesses casos.

Na matriz 4000 × 4000, a versão sequencial apresentou mediana de **192,783 ms**. A configuração com 2 threads apresentou **236,405 ms**, resultando em aceleração de aproximadamente **0,815**. A configuração com 4 threads apresentou **237,393 ms**, com aceleração de aproximadamente **0,812**. Como os dois valores de `S(p)` são inferiores a 1, nenhuma das configurações paralelas foi mais rápida que a versão sequencial nessa execução.

Em relação à versão sequencial, 2 threads ficaram aproximadamente **22.6%** mais lentas e 4 threads aproximadamente **23.1%** mais lentas. As duas configurações paralelas tiveram tempos muito próximos; com 4 threads, o tempo mediano foi cerca de **0.4%** maior que com 2 threads. Assim, aumentar de 2 para 4 trabalhadores não trouxe ganho mensurável nessa execução. A diferença entre as duas configurações foi pequena em relação à variação observada entre as repetições, não sendo possível identificar um ganho relevante ao aumentar de 2 para 4 threads neste experimento.

Esse comportamento pode ser explicado pelo trabalho adicional da versão paralela. Além da rotulação local, ela precisa criar e finalizar threads, manter um vetor de rótulos maior, percorrer novamente a matriz para aplicar os offsets dos identificadores locais, analisar as fronteiras e executar a consolidação global com Union-Find. Essas etapas introduzem acessos adicionais à memória e parte delas permanece sequencial. A variação observada nas 10 repetições também mostra que o tempo de execução é sensível ao escalonamento e à carga momentânea do sistema.

## 10. Tratamento de erros e qualidade do código

### 10.1 Chamadas e recursos POSIX

| Chamada/recurso | Erro verificado? | Ação em caso de falha | Liberação/finalização |
|---|---|---|---|
| `clock_gettime` | Sim | `perror` e encerramento com falha | Não se aplica |
| `pthread_create` | Sim | Mensagem com `strerror`; threads já criadas são aguardadas | `pthread_join` |
| `pthread_join` | Sim | Mensagem com `strerror` e indicação de falha | Uma chamada para cada thread criada |
| `malloc`/`calloc` | Sim | Mensagem de erro e liberação do que já foi alocado quando aplicável | `free` |

### 10.2 Compilação e análise

| Verificação | Comando/ferramenta | Resultado |
|---|---|---|
| Compilação C89/C90 | `cc -std=c89 -Wall -Wextra -pedantic ...` | Código preparado para esse padrão |
| Avisos do compilador | `-Wall -Wextra -pedantic` | Compilação final validada sem erros e sem avisos. |
| Vazamentos de memória | Não foi utilizado Valgrind/Leaks na evidência fornecida | Não avaliado por ferramenta específica |
| Condições de corrida | Validação funcional e separação das regiões de escrita | Não foi utilizado ThreadSanitizer/Helgrind |

### 10.3 Separação de responsabilidades

`casos.h` concentra os dados de teste, a geração da matriz grande e funções auxiliares compartilhadas. `Sequencial.c` contém o algoritmo sequencial e sua medição. `paralelo.c` contém a criação das threads, a rotulação local, a consolidação e a medição da versão paralela. `Executar.sh` reúne os comandos de compilação e execução.

## 11. Limitações e decisões de projeto

| Limitação ou decisão | Impacto | Alternativa considerada | Motivo da escolha |
|---|---|---|---|
| Particionamento somente por linhas | Não existem blocos 2D nem encontro de quatro regiões | Blocos retangulares | Faixas simplificam a independência de escrita e a consolidação |
| Consolidação sequencial | Limita a escalabilidade | Paralelizar offsets e fronteiras | Reduz complexidade e evita concorrência no Union-Find |
| Casos embutidos em `casos.h` | É necessário recompilar para trocar matrizes | Leitura de arquivo | Garante os mesmos dados nas duas versões |
| Interface limitada a 2–5 threads | Restringe testes com muitos trabalhadores | Executar somente casos grandes ou usar tarefas menores | O menor caso obrigatório possui apenas 5 linhas |
| Criação de threads em cada repetição | Aumenta a sobrecarga medida | Pool de threads reutilizável | Implementação atual prioriza simplicidade |
| Tempos individuais são exibidos apenas para a matriz de desempenho | Os casos pequenos permanecem resumidos pela mediana | Registrar todas as repetições de todos os casos ou gerar CSV | O foco dos dados brutos é a matriz utilizada na análise de desempenho |

## 12. Conclusão

A implementação atingiu o objetivo de contar componentes conexos em matrizes binárias com conectividade 8 nas versões sequencial e paralela. A versão sequencial utiliza DFS iterativa e serve como referência de correção. A versão paralela distribui a identificação local dos componentes entre Pthreads, usando faixas horizontais de linhas, e posteriormente consolida objetos que atravessam as fronteiras por meio de rótulos globais e Union-Find.

Os cinco casos obrigatórios produziram exatamente as contagens esperadas nas três configurações avaliadas, e a matriz adicional 4000 × 4000 produziu 6400 objetos em todas elas. Isso demonstra equivalência funcional entre as implementações testadas.

Os testes de desempenho mostraram que o paralelismo não resultou em aceleração nesta execução. A versão sequencial apresentou 192,783 ms, enquanto 2 e 4 threads apresentaram 236,405 ms e 237,393 ms, respectivamente. As acelerações ficaram próximas de 0,815 e 0,812. Portanto, as duas configurações paralelas permaneceram mais lentas que a referência sequencial. Os tempos das configurações com 2 e 4 threads foram muito próximos, não sendo observada diferença relevante de desempenho entre elas neste experimento. Os resultados evidenciam o custo de criação e sincronização das threads, das estruturas auxiliares, dos acessos adicionais à memória e da consolidação sequencial. Como melhoria futura, seria possível reutilizar threads entre repetições e reduzir ou paralelizar as passagens sequenciais realizadas após a rotulação local.

## 13. Vídeo de apresentação

Conforme a orientação atualizada do professor, a apresentação do trabalho será realizada em vídeo. Os dados abaixo devem ser preenchidos após a publicação na plataforma escolhida.

| Campo | Informação |
|---|---|
| Plataforma | YouTube |
| Link do vídeo | [PREENCHER URL COMPLETA] |
| Duração | [PREENCHER MM:SS - máximo de 10 minutos] |
| Privacidade | Não listado |
| Senha, se aplicável | `Não se aplica` |


Antes da entrega, o link deve ser testado em uma janela anônima ou em uma conta sem acesso ao projeto, garantindo que o professor consiga visualizar o vídeo durante todo o período de avaliação.

### 13.1 Conteúdo do vídeo

- [ ] Problema e estratégia escolhida.
- [ ] Implementação sequencial e referência de correção.
- [ ] Decomposição em faixas, Pthreads e sincronização.
- [ ] Consolidação de objetos que atravessam regiões.
- [ ] Demonstração executável.
- [ ] Testes obrigatórios e adicionais.
- [ ] Resultados de desempenho, incluindo a ausência de speedup.
- [ ] Conclusões.
- [ ] Participação dos integrantes.

## 14. Contribuições dos integrantes

As atividades foram distribuídas entre os integrantes, com colaboração cruzada nas etapas de implementação, testes e documentação. A distribuição registrada para o trabalho é apresentada abaixo.

| Atividade | Eduardo | João Paulo | Juliana | Julia | Evidência/observação |
|---|---|---|---|---|---|
| Projeto da solução sequencial | Apoio | — | Principal | Apoio | Definição e revisão da DFS iterativa e da conectividade 8 em `Sequencial.c`. |
| Projeto da solução paralela | Apoio | Principal | — | Apoio | Divisão por faixas de linhas, criação das Pthreads e rotulação local em `paralelo.c`. |
| Sincronização/comunicação | Principal | Apoio | — | — | Revisão do uso de `pthread_create`, `pthread_join` e das regiões de escrita independentes. |
| Consolidação | — | Apoio | Apoio | Principal | Implementação e revisão dos offsets, fronteiras e Union-Find em `paralelo.c`. |
| Testes e medições | Apoio | Principal | Principal | — | Execução dos casos obrigatórios, matriz 4000 × 4000 e conferência das medianas. |
| Documentação e apresentação | — | Apoio | Principal | Principal | Organização do relatório, README e preparação do conteúdo do vídeo. |

Todos os integrantes declaram compreender integralmente o código, as estruturas de dados, a divisão do trabalho, a sincronização, a comunicação, a consolidação e os resultados apresentados.

## 15. Ferramentas, bibliotecas, referências e códigos externos

| Recurso | Finalidade | Origem/link | Licença, quando aplicável | Partes do projeto afetadas |
|---|---|---|---|---|
| Biblioteca padrão C | Alocação, entrada/saída e utilidades | Implementação padrão da linguagem | Conforme implementação | Todo o projeto |
| POSIX Threads | Criação e sincronização das threads | `pthread.h` | API POSIX | Versão paralela |
| `clock_gettime` / `CLOCK_MONOTONIC` | Medição de tempo | API POSIX | API POSIX | Duas versões |
| ChatGPT (OpenAI) | Apoio na revisão textual, conferência de requisitos e adequação do código | ChatGPT | Não se aplica | Documentação e revisão da implementação |
| Enunciado da disciplina | Requisitos, matrizes obrigatórias e critérios de avaliação | Material fornecido pelo professor | Não se aplica | Todo o trabalho |

## 16. Checklist de entrega

### Código e execução

- [x] O código segue a estrutura exigida para ANSI C C89/C90.
- [x] A versão sequencial conta componentes com conectividade 8.
- [x] A versão paralela distribui cálculo real entre pelo menos duas threads.
- [x] A quantidade de threads é configurável.
- [x] Conexões horizontais, verticais e diagonais são preservadas.
- [x] Componentes que atravessam regiões são consolidados sem duplicidade.
- [x] As principais chamadas POSIX utilizadas têm seus retornos verificados.
- [x] Recursos alocados são liberados e threads são aguardadas.
- [ ] Confirmar a compilação final sem avisos na máquina usada para a entrega.

### Testes e desempenho

- [x] As cinco matrizes obrigatórias foram executadas nas duas versões.
- [x] A versão paralela produziu os mesmos resultados da sequencial.
- [x] Foi criada uma matriz maior para desempenho.
- [x] Foram testadas duas quantidades de threads: 2 e 4.
- [x] Cada configuração foi medida 10 vezes e a mediana foi informada.
- [x] Tempo sequencial, tempo paralelo, aceleração e eficiência foram calculados.
- [x] Foi explicada a razão de a versão paralela ter sido mais lenta.
- [x] Os 10 tempos individuais da matriz de desempenho foram registrados para cada configuração.

### Repositório e apresentação

- [ ] Inserir URL do repositório público.
- [ ] Registrar o hash do commit avaliado.
- [ ] Confirmar `README.md` final.
- [ ] Incluir o relatório final no repositório.
- [ ] Inserir o link do vídeo e confirmar que o acesso funciona.
- [ ] Confirmar que o vídeo respeita a duração máxima definida pelo professor.
- [x] Contribuições dos integrantes registradas no relatório.
- [x] Dados de hardware e software registrados na Seção 3.1.

## Apêndice A - Registro de comandos

```bash
# Compilação
cc -std=c89 -Wall -Wextra -pedantic -O2 Sequencial.c -o Sequencial
cc -std=c89 -Wall -Wextra -pedantic -O2 -pthread paralelo.c -o Paralelo

# Execução
./Sequencial
./Paralelo 2 4

# Execução automatizada
chmod +x Executar.sh
./Executar.sh
```

## Apêndice B - Formato dos dados brutos

A execução atual imprime os 10 tempos individuais da matriz 4000 × 4000 e o script `Executar.sh` salva essas saídas em `resultado_sequencial.txt` e `resultado_paralelo.txt`. Caso o grupo deseje também manter uma versão estruturada em CSV, pode utilizar o formato abaixo:

```csv
matriz,linhas,colunas,versao,trabalhadores,repeticao,tempo_ms,objetos,resultado_correto
matriz_grande,4000,4000,sequencial,1,1,205.457345,6400,true
matriz_grande,4000,4000,paralela,2,1,279.169026,6400,true
matriz_grande,4000,4000,paralela,4,1,314.242695,6400,true
```

Os dados completos das 10 repetições estão apresentados na Seção 9.4.

## Apêndice C - Correspondência com os critérios de avaliação

| Critério | Peso | Seções com evidências |
|---|---:|---|
| Correção sequencial e paralela, incluindo conectividade 8 | 2,0 | 5, 6, 7 e 8 |
| Decomposição do problema e paralelismo efetivo | 1,5 | 6.1, 6.2 e 6.3 |
| Sincronização, comunicação e ausência de condições de corrida | 1,5 | 6.4 e 10 |
| Consolidação de objetos que atravessam regiões | 1,5 | 7 |
| Testes obrigatórios, adicionais e análise de desempenho | 1,0 | 8 e 9 |
| Qualidade do código ANSI C e tratamento de erros | 1,0 | 3 e 10 |
| Organização do repositório e documentação | 0,5 | 2, 3 e 16 |
| Apresentação, demonstração e domínio da implementação | 1,0 | 13 e 14 |
