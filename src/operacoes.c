#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/operacoes.h"
#include "../include/config.h"

void depositar(Conta contas[], int numContas, sqlite3 *db, Config *config) {
    int id;
    double valor;
    char senha[50];
    char sql[256];
    char *errMsg = 0;

    printf("Digite o ID da conta: ");
    scanf("%d", &id);
    getchar(); // Limpar o buffer do teclado
    if (id <= 0 || id > numContas) {
        printf("Conta inválida.\n");
        return;
    }

    printf("Digite a senha da conta: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0; // Remove o newline

    // Verificar a senha
    if (strcmp(contas[id - 1].senha, senha) != 0) {
        printf("Senha incorreta.\n");
        return;
    }

    printf("Digite o valor a depositar: ");
    scanf("%lf", &valor);
    if (valor <= 0) {
        printf("Valor inválido.\n");
        return;
    }

    contas[id - 1].saldo += valor;
    snprintf(sql, sizeof(sql), "UPDATE contas SET saldo = %f WHERE id = %d;", contas[id - 1].saldo, id);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao atualizar saldo: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    // Registrar transação
    snprintf(sql, sizeof(sql), "INSERT INTO transacoes (conta_id, tipo, valor, data) VALUES (%d, 'Depósito', %f, datetime('now'))", id, valor);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao registrar transação: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    printf("Depósito realizado com sucesso! Novo saldo: %.2f\n", contas[id - 1].saldo);
}

void sacar(Conta contas[], int numContas, sqlite3 *db, Config *config) {
    int id;
    double valor;
    char senha[50];
    char sql[256];
    char *errMsg = 0;

    printf("Digite o ID da conta: ");
    scanf("%d", &id);
    getchar(); // Limpar o buffer do teclado
    if (id <= 0 || id > numContas) {
        printf("Conta inválida.\n");
        return;
    }

    printf("Digite a senha da conta: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0; // Remove o newline

    // Verificar a senha
    if (strcmp(contas[id - 1].senha, senha) != 0) {
        printf("Senha incorreta.\n");
        return;
    }

    printf("Digite o valor a sacar: ");
    scanf("%lf", &valor);
    if (valor <= 0 || valor > contas[id - 1].saldo || valor > config->limite_saque) {
        printf("Valor inválido ou acima do limite de saque.\n");
        return;
    }

    contas[id - 1].saldo -= valor;
    snprintf(sql, sizeof(sql), "UPDATE contas SET saldo = %f WHERE id = %d;", contas[id - 1].saldo, id);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao atualizar saldo: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    // Registrar transação
    snprintf(sql, sizeof(sql), "INSERT INTO transacoes (conta_id, tipo, valor, data) VALUES (%d, 'Saque', %f, datetime('now'))", id, valor);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao registrar transação: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    printf("Saque realizado com sucesso! Novo saldo: %.2f\n", contas[id - 1].saldo);
}

void transferir(Conta contas[], int numContas, sqlite3 *db) {
    int idOrigem, idDestino;
    double valor;
    char senha[50];
    char sql[256];
    char *errMsg = 0;

    printf("Digite o ID da conta de origem: ");
    scanf("%d", &idOrigem);
    getchar(); // Limpar o buffer do teclado
    if (idOrigem <= 0 || idOrigem > numContas) {
        printf("Conta de origem inválida.\n");
        return;
    }

    printf("Digite a senha da conta de origem: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0; // Remove o newline

    // Verificar a senha
    if (strcmp(contas[idOrigem - 1].senha, senha) != 0) {
        printf("Senha incorreta.\n");
        return;
    }

    printf("Digite o ID da conta de destino: ");
    scanf("%d", &idDestino);
    if (idDestino <= 0 || idDestino > numContas) {
        printf("Conta de destino inválida.\n");
        return;
    }

    printf("Digite o valor a transferir: ");
    scanf("%lf", &valor);
    if (valor <= 0 || valor > contas[idOrigem - 1].saldo) {
        printf("Valor inválido ou saldo insuficiente.\n");
        return;
    }

    contas[idOrigem - 1].saldo -= valor;
    contas[idDestino - 1].saldo += valor;

    snprintf(sql, sizeof(sql), "UPDATE contas SET saldo = %f WHERE id = %d;", contas[idOrigem - 1].saldo, idOrigem);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao atualizar saldo da conta de origem: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    snprintf(sql, sizeof(sql), "UPDATE contas SET saldo = %f WHERE id = %d;", contas[idDestino - 1].saldo, idDestino);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao atualizar saldo da conta de destino: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    // Registrar transações
    snprintf(sql, sizeof(sql), "INSERT INTO transacoes (conta_id, tipo, valor, data) VALUES (%d, 'Transferência Enviada', %f, datetime('now'))", idOrigem, valor);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao registrar transação de origem: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    snprintf(sql, sizeof(sql), "INSERT INTO transacoes (conta_id, tipo, valor, data) VALUES (%d, 'Transferência Recebida', %f, datetime('now'))", idDestino, valor);
    if (sqlite3_exec(db, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro ao registrar transação de destino: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    printf("Transferência realizada com sucesso! Novo saldo da conta de origem: %.2f, Novo saldo da conta de destino: %.2f\n", contas[idOrigem - 1].saldo, contas[idDestino - 1].saldo);
}

void exibirSaldo(Conta contas[], int numContas) {
    int id;
    char senha[50];

    printf("Digite o ID da conta: ");
    scanf("%d", &id);
    getchar(); // Limpar o buffer do teclado
    if (id <= 0 || id > numContas) {
        printf("Conta inválida.\n");
        return;
    }

    printf("Digite a senha da conta: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0; // Remove o newline

    // Verificar a senha
    if (strcmp(contas[id - 1].senha, senha) != 0) {
        printf("Senha incorreta.\n");
        return;
    }

    printf("Saldo da conta %d: %.2f\n", id, contas[id - 1].saldo);
}

void exibirExtrato(Conta contas[], int numContas, sqlite3 *db) {
    int id;
    char senha[50];
    char sql[256];
    sqlite3_stmt *stmt;

    printf("Digite o ID da conta: ");
    scanf("%d", &id);
    getchar(); // Limpar o buffer do teclado
    if (id <= 0 || id > numContas) {
        printf("Conta inválida.\n");
        return;
    }

    printf("Digite a senha da conta: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0; // Remove o newline

    // Verificar a senha
    if (strcmp(contas[id - 1].senha, senha) != 0) {
        printf("Senha incorreta.\n");
        return;
    }

    // Exibir extrato real
    snprintf(sql, sizeof(sql), "SELECT tipo, valor, data FROM transacoes WHERE conta_id = %d;", id);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Erro ao preparar consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("Extrato da conta %d:\n", id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *tipo = (const char *)sqlite3_column_text(stmt, 0);
        double valor = sqlite3_column_double(stmt, 1);
        const char *data = (const char *)sqlite3_column_text(stmt, 2);
        printf("%s: R$ %.2f em %s\n", tipo, valor, data);
    }

    sqlite3_finalize(stmt);
    printf("Saldo atual: %.2f\n", contas[id - 1].saldo);
}