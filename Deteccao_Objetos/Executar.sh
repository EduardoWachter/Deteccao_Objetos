#!/bin/bash

set -o pipefail

echo "============================================================"
echo "       TESTE DE DETECCAO DE OBJETOS"
echo "       SEQUENCIAL x PARALELO"
echo "============================================================"
echo ""

echo "[1/4] Compilando Sequencial.c..."
cc -std=c89 -Wall -Wextra -pedantic -O2 Sequencial.c -o Sequencial

if [ $? -ne 0 ]; then
    echo ""
    echo "ERRO: nao foi possivel compilar Sequencial.c"
    exit 1
fi

echo "OK!"
echo ""

echo "[2/4] Compilando paralelo.c..."
cc -std=c89 -Wall -Wextra -pedantic -O2 -pthread paralelo.c -o Paralelo

if [ $? -ne 0 ]; then
    echo ""
    echo "ERRO: nao foi possivel compilar paralelo.c"
    exit 1
fi

echo "OK!"
echo ""

echo "============================================================"
echo "              EXECUTANDO SEQUENCIAL"
echo "============================================================"
echo ""

./Sequencial | tee resultado_sequencial.txt

if [ $? -ne 0 ]; then
    echo ""
    echo "ERRO durante a execucao do sequencial."
    exit 1
fi

echo ""
echo ""
echo "============================================================"
echo "              EXECUTANDO PARALELO"
echo "============================================================"
echo ""

./Paralelo 2 4 | tee resultado_paralelo.txt

if [ $? -ne 0 ]; then
    echo ""
    echo "ERRO durante a execucao do paralelo."
    exit 1
fi

echo ""
echo ""
echo "============================================================"
echo "                 TESTES FINALIZADOS"
echo "============================================================"
echo ""
echo "Resultados salvos em:"
echo ""
echo "  resultado_sequencial.txt"
echo "  resultado_paralelo.txt"
echo ""
