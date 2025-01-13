#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/database.h"

int abrirBancoDados(sqlite3 **db, const char *dbPath) {
    int rc = sqlite3_open(dbPath, db);
    if (rc) {
        fprintf(stderr, "Não foi possível abrir o banco de dados: %s\n", sqlite3_errmsg(*db));
        return 0;
    }

    char *errMsg = 0;
    const char *sql = "CREATE TABLE IF NOT EXISTS contas ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "nome TEXT NOT NULL, "
                      "senha TEXT NOT NULL, "
                      "saldo REAL NOT NULL);"
                      "CREATE TABLE IF NOT EXISTS transacoes ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "conta_id INTEGER NOT NULL, "
                      "tipo TEXT NOT NULL, "
                      "valor REAL NOT NULL, "
                      "data TEXT NOT NULL, "
                      "FOREIGN KEY(conta_id) REFERENCES contas(id));";
    rc = sqlite3_exec(*db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao criar tabela: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }

    return 1;
}

int fecharBancoDados(sqlite3 *db) {
    int rc = sqlite3_close(db);
    if (rc) {
        fprintf(stderr, "Não foi possível fechar o banco de dados: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    return 1;
}

int carregarDadosBD(Conta contas[], int *numContas, sqlite3 *db) {
    const char *sql = "SELECT id, nome, senha, saldo FROM contas";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    *numContas = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        contas[*numContas].id = sqlite3_column_int(stmt, 0);
        strcpy(contas[*numContas].nome, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(contas[*numContas].senha, (const char *)sqlite3_column_text(stmt, 2));
        contas[*numContas].saldo = sqlite3_column_double(stmt, 3);
        (*numContas)++;
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Erro ao executar a consulta: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

int salvarDadosBD(Conta contas[], int numContas, sqlite3 *db) {
    char *errMsg = 0;
    int rc;
    const char *sql = "INSERT INTO contas (id, nome, senha, saldo) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    // Iniciar transação
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao iniciar transação: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }

    // Preparar statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Limpar a tabela antes de inserir novos dados
    rc = sqlite3_exec(db, "DELETE FROM contas;", NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao limpar tabela: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_finalize(stmt);
        return 0;
    }

    // Inserir os dados das contas no banco de dados
    for (int i = 0; i < numContas; i++) {
        sqlite3_bind_int(stmt, 1, contas[i].id);
        sqlite3_bind_text(stmt, 2, contas[i].nome, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, contas[i].senha, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, contas[i].saldo);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Erro ao inserir dados: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return 0;
        }

        sqlite3_reset(stmt);
    }

    // Finalizar statement
    sqlite3_finalize(stmt);

    // Commit transação
    rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao finalizar transação: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }

    return 1;
}