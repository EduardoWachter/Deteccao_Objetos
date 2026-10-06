#!/usr/bin/env python3
import re
import statistics
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


ARQ_SEQ = Path("resultado_sequencial.txt")
ARQ_PAR = Path("resultado_paralelo.txt")


def ler_arquivo(caminho):
    if not caminho.exists():
        raise SystemExit("ERRO: arquivo nao encontrado: %s" % caminho)
    return caminho.read_text(encoding="utf-8")


def extrair_tempos_sequencial(texto):
    marcador = "Tempos individuais - Caso 6"
    pos = texto.find(marcador)

    if pos == -1:
        raise SystemExit(
            "ERRO: nao encontrei a secao de tempos individuais em %s." % ARQ_SEQ
        )

    trecho = texto[pos:]
    tempos = []

    for linha in trecho.splitlines():
        m = re.match(r"^\s*\d+\s*\|\s*([0-9]+\.[0-9]+)\s*$", linha)
        if m:
            tempos.append(float(m.group(1)))

    if len(tempos) != 10:
        raise SystemExit(
            "ERRO: esperava 10 tempos sequenciais, mas encontrei %d." % len(tempos)
        )

    return tempos


def extrair_tempos_paralelo(texto):
    cabecalho = re.search(
        r"Threads testadas:\s*(\d+)\s*e\s*(\d+)",
        texto
    )

    if not cabecalho:
        raise SystemExit(
            "ERRO: nao consegui identificar as quantidades de threads."
        )

    threads_1 = int(cabecalho.group(1))
    threads_2 = int(cabecalho.group(2))

    marcador = "Tempos individuais - Caso 6"
    pos = texto.find(marcador)

    if pos == -1:
        raise SystemExit(
            "ERRO: nao encontrei a secao de tempos individuais em %s." % ARQ_PAR
        )

    trecho = texto[pos:]
    tempos_1 = []
    tempos_2 = []

    for linha in trecho.splitlines():
        m = re.match(
            r"^\s*\d+\s*\|\s*([0-9]+\.[0-9]+)\s*\|\s*([0-9]+\.[0-9]+)\s*$",
            linha
        )
        if m:
            tempos_1.append(float(m.group(1)))
            tempos_2.append(float(m.group(2)))

    if len(tempos_1) != 10 or len(tempos_2) != 10:
        raise SystemExit(
            "ERRO: esperava 10 tempos para cada configuracao paralela."
        )

    return threads_1, tempos_1, threads_2, tempos_2


def salvar_grafico_tempo(labels, medianas_ms, desvios_ms):
    fig, ax = plt.subplots(figsize=(8, 5))

    barras = ax.bar(
        labels,
        medianas_ms,
        yerr=desvios_ms,
        capsize=6
    )

    ax.set_title("Tempo de execucao - matriz 4000 x 4000")
    ax.set_ylabel("Tempo mediano (ms)")
    ax.set_xlabel("Configuracao")
    ax.grid(axis="y", alpha=0.25)

    for barra, valor in zip(barras, medianas_ms):
        ax.text(
            barra.get_x() + barra.get_width() / 2,
            barra.get_height(),
            "%.3f" % valor,
            ha="center",
            va="bottom"
        )

    fig.tight_layout()
    fig.savefig("grafico-tempo.png", dpi=200)
    plt.close(fig)


def salvar_grafico_aceleracao(labels, trabalhadores, speedups):
    fig, ax = plt.subplots(figsize=(8, 5))

    barras = ax.bar(labels, speedups)
    ax.plot(labels, trabalhadores, marker="o", linestyle="--", label="Speedup ideal S(p) = p")

    ax.set_title("Aceleracao por quantidade de trabalhadores")
    ax.set_ylabel("Aceleracao S(p)")
    ax.set_xlabel("Configuracao")
    ax.grid(axis="y", alpha=0.25)
    ax.legend()

    for barra, valor in zip(barras, speedups):
        ax.text(
            barra.get_x() + barra.get_width() / 2,
            barra.get_height(),
            "%.3f" % valor,
            ha="center",
            va="bottom"
        )

    fig.tight_layout()
    fig.savefig("grafico-aceleracao.png", dpi=200)
    plt.close(fig)


def salvar_grafico_eficiencia(labels, eficiencias):
    fig, ax = plt.subplots(figsize=(8, 5))

    barras = ax.bar(labels, eficiencias)
    ax.axhline(1.0, linestyle="--", label="Eficiencia ideal = 1")

    ax.set_title("Eficiencia paralela")
    ax.set_ylabel("Eficiencia E(p)")
    ax.set_xlabel("Configuracao")
    ax.set_ylim(0, max(1.1, max(eficiencias) * 1.15))
    ax.grid(axis="y", alpha=0.25)
    ax.legend()

    for barra, valor in zip(barras, eficiencias):
        ax.text(
            barra.get_x() + barra.get_width() / 2,
            barra.get_height(),
            "%.3f" % valor,
            ha="center",
            va="bottom"
        )

    fig.tight_layout()
    fig.savefig("grafico-eficiencia.png", dpi=200)
    plt.close(fig)


def main():
    texto_seq = ler_arquivo(ARQ_SEQ)
    texto_par = ler_arquivo(ARQ_PAR)

    tempos_seq = extrair_tempos_sequencial(texto_seq)
    t1, tempos_t1, t2, tempos_t2 = extrair_tempos_paralelo(texto_par)

    mediana_seq = statistics.median(tempos_seq)
    mediana_t1 = statistics.median(tempos_t1)
    mediana_t2 = statistics.median(tempos_t2)

    desvio_seq = statistics.stdev(tempos_seq)
    desvio_t1 = statistics.stdev(tempos_t1)
    desvio_t2 = statistics.stdev(tempos_t2)

    speedup_t1 = mediana_seq / mediana_t1
    speedup_t2 = mediana_seq / mediana_t2

    eficiencia_t1 = speedup_t1 / t1
    eficiencia_t2 = speedup_t2 / t2

    labels = [
        "Sequencial",
        "%d threads" % t1,
        "%d threads" % t2
    ]

    trabalhadores = [1, t1, t2]

    medianas_ms = [
        mediana_seq * 1000.0,
        mediana_t1 * 1000.0,
        mediana_t2 * 1000.0
    ]

    desvios_ms = [
        desvio_seq * 1000.0,
        desvio_t1 * 1000.0,
        desvio_t2 * 1000.0
    ]

    speedups = [
        1.0,
        speedup_t1,
        speedup_t2
    ]

    eficiencias = [
        1.0,
        eficiencia_t1,
        eficiencia_t2
    ]

    salvar_grafico_tempo(labels, medianas_ms, desvios_ms)
    salvar_grafico_aceleracao(labels, trabalhadores, speedups)
    salvar_grafico_eficiencia(labels, eficiencias)

    print("============================================================")
    print("                 GRAFICOS GERADOS")
    print("============================================================")
    print("")
    print("Sequencial:")
    print("  Mediana: %.3f ms" % medianas_ms[0])
    print("  Desvio-padrao: %.3f ms" % desvios_ms[0])
    print("")
    print("%d threads:" % t1)
    print("  Mediana: %.3f ms" % medianas_ms[1])
    print("  Desvio-padrao: %.3f ms" % desvios_ms[1])
    print("  Speedup: %.3f" % speedups[1])
    print("  Eficiencia: %.3f" % eficiencias[1])
    print("")
    print("%d threads:" % t2)
    print("  Mediana: %.3f ms" % medianas_ms[2])
    print("  Desvio-padrao: %.3f ms" % desvios_ms[2])
    print("  Speedup: %.3f" % speedups[2])
    print("  Eficiencia: %.3f" % eficiencias[2])
    print("")
    print("Arquivos criados:")
    print("  grafico-tempo.png")
    print("  grafico-aceleracao.png")
    print("  grafico-eficiencia.png")
    print("============================================================")


if __name__ == "__main__":
    main()
