#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    double limite_saque;
    double taxa;
    char arquivo_dados[256];
} Config;

int carregarConfiguracao(const char *arquivo, Config *config);

#endif // CONFIG_H