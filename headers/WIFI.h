#ifndef WIFI_H
#define WIFI_H

typedef struct {
    char NOME[50];
    char SENHA[50];
} Rede;

Rede parseCredenciaisRede(const char* json);
void conectarRede(const char* nomeRede, const char* senhaRede);
void reconectarWiFi(const char* nome, const char* senha);
void verificarReconexao();
bool wifiEstaConectado();
void inicializarWIFI();

extern bool redeConectada;

#endif