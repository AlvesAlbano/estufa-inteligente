#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "cJSON.h"
#include "flash.h"

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

bool redeConectada = false;

void conectarRede(const char* nomeRede, const char* senhaRede) {

    redeConectada = false;
    printf("nome rede: %s\n",nomeRede);
    printf("senha rede: %s\n",senhaRede);

    while(!redeConectada){
        printf("Conectando Wi-Fi: %s\n", nomeRede);
    
        int status = cyw43_arch_wifi_connect_timeout_ms(
            nomeRede,
            senhaRede,
            CYW43_AUTH_WPA2_MIXED_PSK,
            30000
        );
    
        if (status == 0) {
            redeConectada = true;
            printf("Wi-Fi conectado!\n");
        } else {
            printf("Falha ao conectar Wi-Fi: %d\n", status);
            sleep_ms(1000);
        }
    }
}

void reconectarWiFi(const char* nome, const char* senha) {
    printf("Desconectando da rede atual...\n");

    cyw43_arch_disable_sta_mode();

    sleep_ms(1000);

    printf("Conectando na nova rede: %s\n", nome);

    cyw43_arch_enable_sta_mode();

    int status = cyw43_arch_wifi_connect_timeout_ms(
        nome,
        senha,
        CYW43_AUTH_WPA2_AES_PSK,
        10000 // timeout
    );

    if (status == 0) {
        printf("Conectado com sucesso!\n");
    } else {
        printf("Erro ao conectar: %d\n", status);
    }
}

Rede parseCredenciaisRede(const char* json){
    // {"nomeRede":"qefwer","senhaRede":"qwrqw"}
    Rede novaRede = {0};
    
    cJSON *root = cJSON_Parse(json);
    if (!root) {
        printf("Erro ao parsear json\n");
        cJSON_Delete(root);

        return novaRede;
    }

    cJSON *s = cJSON_GetObjectItem(root, "nomeRede");
    cJSON *p = cJSON_GetObjectItem(root, "senhaRede");

    if (cJSON_IsString(s)) {
        strncpy(novaRede.NOME, s->valuestring, sizeof(novaRede.NOME) - 1);
    }

    if (cJSON_IsString(p)) {
        strncpy(novaRede.SENHA, p->valuestring, sizeof(novaRede.SENHA) - 1);
    }

    cJSON_Delete(root);
    return novaRede;
}

bool wifiEstaConectado() {
    int status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

    return status == CYW43_LINK_UP;
}

void verificarReconexao() {
    if (!wifiEstaConectado()) {

        printf("Wi-Fi caiu! Reconectando...\n");

        while (!wifiEstaConectado()) {

            cyw43_arch_wifi_connect_timeout_ms(nomeRedeAtual,senhaRedeAtual,CYW43_AUTH_WPA2_AES_PSK,60000);

            sleep_ms(3000);
        }

        printf("Reconectado!\n");
    }
}

void conectarRedeSalva() {
    conectarRede(nomeRedeAtual,senhaRedeAtual);
}

void inicializarWIFI(){
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
}