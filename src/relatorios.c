#include <stdio.h>
#include "..\include\relatorios.h"

void exibirRelatorioSimples(Conta contas[], int numContas) {
    printf("\n--- Relatório Simples ---\n");
    printf("Total de contas: %d\n", numContas);

    double saldoTotal = 0;
    for (int i = 0; i < numContas; i++) {
        saldoTotal += contas[i].saldo;
    }

    printf("Saldo total no sistema: %.2f\n", saldoTotal);
}

void exibirRelatorioDetalhado(Conta contas[], int numContas) {
    printf("\n--- Relatório Detalhado ---\n");
    for (int i = 0; i < numContas; i++) {
        printf("ID: %d\n", contas[i].id);
        printf("Titular: %s\n", contas[i].nome);
        printf("Saldo: %.2f\n", contas[i].saldo);
        printf("-----------------------------\n");
    }
}
