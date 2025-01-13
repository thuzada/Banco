#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "conta.h" // Inclua o cabeçalho conta.h

int abrirBancoDados(sqlite3 **db, const char *dbPath);
int fecharBancoDados(sqlite3 *db);
int carregarDadosBD(Conta contas[], int *numContas, sqlite3 *db);
int salvarDadosBD(Conta contas[], int numContas, sqlite3 *db);

#endif // DATABASE_H