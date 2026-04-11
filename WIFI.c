#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define NOME_REDE "Somanex Casa 11"
#define SENHA_REDE "23280915"

void conectarRede();
bool redeConectada = false;

void conectarRede() {
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();

    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(
            NOME_REDE,
            SENHA_REDE,
            CYW43_AUTH_WPA2_AES_PSK,
            10000)) {

        printf("Falha ao conectar.\n");
        return;
    }

    redeConectada = true;
    printf("Conectado com sucesso na rede %s!\n",NOME_REDE);
}