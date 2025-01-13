#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include "cJSON.h"
#include "../include/config.h"

void criarDiretorioSeNecessario(const char *caminho) {
    struct stat st = {0};
    if (stat(caminho, &st) == -1) {
#ifdef _WIN32
        if (_mkdir(caminho) != 0) {
#else
        if (mkdir(caminho, 0700) != 0) {
#endif
            perror("Erro ao criar diretório");
            exit(1);
        }
    }
}

void criarArquivoConfiguracao(const char *arquivo) {
    criarDiretorioSeNecessario("../data");

    FILE *fp = fopen(arquivo, "w");
    if (fp == NULL) {
        perror("Erro ao criar arquivo de configuração");
        exit(1);
    }

    const char *conteudoPadrao = "{\n"
                                 "    \"limite_saque\": 1000.0,\n"
                                 "    \"taxa\": 0.02,\n"
                                 "    \"arquivo_dados\": \"banco.db\"\n"
                                 "}\n";

    fwrite(conteudoPadrao, sizeof(char), strlen(conteudoPadrao), fp);
    fclose(fp);
    printf("Arquivo de configuração criado: %s\n", arquivo);
}

int carregarConfiguracao(const char *arquivo, Config *config) {
    printf("Tentando abrir o arquivo de configuração: %s\n", arquivo);

    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) {
        perror("Erro ao abrir arquivo de configuração");
        printf("Criando arquivo de configuração padrão...\n");
        criarArquivoConfiguracao(arquivo);
        fp = fopen(arquivo, "r");
        if (fp == NULL) {
            perror("Erro ao abrir arquivo de configuração após criação");
            return 0;
        }
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    if (data == NULL) {
        perror("Erro ao alocar memória");
        fclose(fp);
        return 0;
    }
    fread(data, 1, length, fp);
    fclose(fp);
    data[length] = '\0';

    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        fprintf(stderr, "Erro ao analisar arquivo de configuração\n");
        free(data);
        return 0;
    }

    config->limite_saque = cJSON_GetObjectItem(json, "limite_saque")->valuedouble;
    config->taxa = cJSON_GetObjectItem(json, "taxa")->valuedouble;
    strcpy(config->arquivo_dados, cJSON_GetObjectItem(json, "arquivo_dados")->valuestring);

    cJSON_Delete(json);
    free(data);
    return 1;
}