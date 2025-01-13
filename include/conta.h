#ifndef CONTA_H
#define CONTA_H

#include <sqlite3.h> // Inclua o cabeçalho sqlite3.h

typedef struct {
    int id;
    char nome[100];
    char senha[50]; // Adiciona o campo de senha
    double saldo;
} Conta;

void criarConta(Conta contas[], int *numContas, sqlite3 *db);
void listarContas(Conta contas[], int numContas);

#endif // CONTA_H