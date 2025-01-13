#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/conta.h"

void criarConta(Conta contas[], int *numContas, sqlite3 *db) {
    Conta novaConta;
    novaConta.id = *numContas + 1;

    printf("Digite o nome do titular da conta: ");
    fgets(novaConta.nome, sizeof(novaConta.nome), stdin);
    novaConta.nome[strcspn(novaConta.nome, "\n")] = 0; // Remove o newline

    printf("Digite a senha da conta: ");
    fgets(novaConta.senha, sizeof(novaConta.senha), stdin);
    novaConta.senha[strcspn(novaConta.senha, "\n")] = 0; // Remove o newline

    novaConta.saldo = 0.0;

    char sql[256];
    char *errMsg = 0;
    snprintf(sql, sizeof(sql), "INSERT INTO contas (id, nome, senha, saldo) VALUES (%d, '%s', '%s', %f);",
             novaConta.id, novaConta.nome, novaConta.senha, novaConta.saldo);

    int rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao criar conta: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    contas[*numContas] = novaConta;
    (*numContas)++;

    printf("Conta criada com sucesso! ID da conta: %d\n", novaConta.id);
}

void listarContas(Conta contas[], int numContas) {
    for (int i = 0; i < numContas; i++) {
        printf("ID: %d, Nome: %s, Saldo: %.2f\n", contas[i].id, contas[i].nome, contas[i].saldo);
    }
}