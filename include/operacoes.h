#ifndef OPERACOES_H
#define OPERACOES_H

#include "conta.h"
#include <sqlite3.h>
#include "config.h"

void depositar(Conta contas[], int numContas, sqlite3 *db, Config *config);
void sacar(Conta contas[], int numContas, sqlite3 *db, Config *config);
void transferir(Conta contas[], int numContas, sqlite3 *db);
void exibirSaldo(Conta contas[], int numContas);
void exibirExtrato(Conta contas[], int numContas, sqlite3 *db);

#endif // OPERACOES_H