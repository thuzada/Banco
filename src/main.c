#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/conta.h"
#include "../include/operacoes.h"
#include "../include/database.h"
#include "../include/relatorios.h"
#include "../include/config.h"
#define MAX_CONTAS 100

int verificarSenhaFuncionario() {
    char senha[50];
    printf("Digite a senha do funcionário: ");
    fgets(senha, sizeof(senha), stdin);
    senha[strcspn(senha, "\n")] = 0; // Remove o newline

    if (strcmp(senha, "0000") == 0) {
        return 1;
    } else {
        printf("Senha incorreta.\n");
        return 0;
    }
}

void menuFuncionario(Conta contas[], int numContas, sqlite3 *db) {
    int opcao;
    do {
        printf("\n--- Menu do Funcionário ---\n");
        printf("1. Listar Contas\n");
        printf("2. Relatório Simples\n");
        printf("3. Relatório Detalhado\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // Limpar o buffer do teclado

        switch (opcao) {
            case 1:
                listarContas(contas, numContas);
                break;
            case 2:
                exibirRelatorioSimples(contas, numContas);
                break;
            case 3:
                exibirRelatorioDetalhado(contas, numContas);
                break;
            case 0:
                printf("Voltando ao menu principal...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (opcao != 0);
}

int main() {
    Conta contas[MAX_CONTAS];
    int numContas = 0;
    int opcao;
    sqlite3 *db;
    Config config;

    // Carregar configuração
    if (!carregarConfiguracao("../data/config.json", &config)) {
        return 1;
    }

    // Abrir o banco de dados
    if (!abrirBancoDados(&db, config.arquivo_dados)) {
        return 1;
    }

    // Carregar os dados das contas do banco de dados
    if (!carregarDadosBD(contas, &numContas, db)) {
        fprintf(stderr, "Erro ao carregar dados das contas.\n");
        fecharBancoDados(db);
        return 1;
    }

    do {
        printf("\n--- Menu Principal ---\n");
        printf("1. Criar Conta\n");
        printf("2. Menu do Funcionário\n");
        printf("3. Depósito\n");
        printf("4. Saque\n");
        printf("5. Transferência\n");
        printf("6. Exibir Saldo\n");
        printf("7. Exibir Extrato\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // Limpar o buffer do teclado

        switch (opcao) {
            case 1:
                criarConta(contas, &numContas, db);
                break;
            case 2:
                if (verificarSenhaFuncionario()) {
                    menuFuncionario(contas, numContas, db);
                }
                break;
            case 3:
                depositar(contas, numContas, db, &config);
                break;
            case 4:
                sacar(contas, numContas, db, &config);
                break;
            case 5:
                transferir(contas, numContas, db);
                break;
            case 6:
                exibirSaldo(contas, numContas);
                break;
            case 7:
                exibirExtrato(contas, numContas, db);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (opcao != 0);

    // Fechar o banco de dados
    fecharBancoDados(db);

    return 0;
}