#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"

#include "WIFI.h"
#include "MQTT.h"
#include "Oled.h"
#include "joystick.h"
#include "botao.h"
#include "temperatura-interna.h"
#include "tomada-decisoes.h"
#include "flash.h"

#define BTN_A 5
#define BTN_B 6
#define CENTRO 1932
#define DEADZONE 300
#define STEP_THRESHOLD 200

bool mudou_sensor = false;
bool modo_manual = true;

const char* SENSORES_DISPONIVEIS[] = {
    "MEDIR_UMIDADE",
    "MEDIR_TEMPERATURA",
    "MEDIR_LUMINOSIDADE",
    "MEDIR_PH"
};

const char* TOPICOS_MQTT_SENSOR[] = {
    "projeto/sensor/umidade",
    "projeto/sensor/temperatura",
    "projeto/sensor/luminosidade",
    "projeto/sensor/ph",
};

const char* TOPICOS_MQTT_CONFIGURACAO[] = {
    "projeto/config/modo",
    "projeto/config/rede",
    "projeto/config/limites"
};

const uint LIMITE_PONTEIRO = sizeof(SENSORES_DISPONIVEIS) / sizeof(SENSORES_DISPONIVEIS[0]);
uint valor_anterior_joystick = 0;

uint ponteiro = 0;

void inicializarComponentes();
void alterarSensor();
void enviarValorManual(int valor);
void enviarValorAutonomo();
void simularValores();
void mudarModoOperacao(const char* payload);
void modoOperacao();
void modoManual();
void modoAutonomo();
void esperarCredenciaisRede();
void carregaArquivosFlash();

char* intToString(int valor);

Rede getCredenciaisRede(const char* payload);

char nomeRede[50];
char senhaRede[50]; 

int valor_simulado = 0;
int main(){

    stdio_init_all();
    srand(time(NULL));
    inicializarComponentes();
    // limparFlash();

    carregaArquivosFlash();
    sleep_ms(5000);

    printOled("CONECTANDO AO BROKER");
    BrokerConectar(&brokerModel);
    sleep_ms(5000);

    printOled("AGUARDANDO ENTRADA");
    appendOled("MEXA O JOYSTICK",0,16);

    const int tamanhoLista = sizeof(TOPICOS_MQTT_CONFIGURACAO) / sizeof(TOPICOS_MQTT_CONFIGURACAO[0]);
    MQTTInscreverMultiplos(TOPICOS_MQTT_CONFIGURACAO,tamanhoLista);
    
    while (1) {
        tight_loop_contents();
        
        const char* payload = MQTTReceber();
        
        modoOperacao();

        if (payload != NULL) {
        
            if (strcmp(ultimoTopico, "projeto/config/rede") == 0) {
                printOled("MUDANDO DE REDE");
                getCredenciaisRede(payload);

                Rede novaRede = getCredenciaisRede(payload);

                salvarCredenciais(novaRede.NOME,novaRede.SENHA);

                sleep_ms(2000);
                printOled("AGUARDANDO ENTRADA");
                appendOled("MEXA O JOYSTICK",0,16);

            }
        
            if (strcmp(ultimoTopico, "projeto/config/modo") == 0) {
                printf("mudando modo de operação\n");
                mudarModoOperacao(payload);
            }

            if (strcmp(ultimoTopico, "projeto/config/limites") == 0) {
                printf("payload: %s\n",payload);

                ValoresLimites valoresLimites = getValoresLimites(payload);
                parseValoresLimites(valoresLimites);

                printf("\n===== VALORES LIMITES =====\n");

                printf("Temperatura:\n");
                printf("  Min: %d\n", limiteMinTemp);
                printf("  Max: %d\n", limiteMaxTemp);
                            
                printf("\nUmidade:\n");
                printf("  Min: %d\n", limiteMinUmidade);
                printf("  Max: %d\n", limiteMaxUmidade);
                            
                printf("\nLuminosidade:\n");
                printf("  Min: %d\n", limiteMinLuminosidade);
                printf("  Max: %d\n", limiteMaxLuminosidade);
                            
                printf("\npH:\n");
                printf("  Min: %d\n", limiteMinPh);
                printf("  Max: %d\n", limiteMaxPh);

                salvarValoresLimites();
            }
        }
    }
}

void inicializarComponentes(){
    inicializarOled();
    inicializarJoystick();
    inicializarBtn(BTN_A);
    inicializarBtn(BTN_B);
    inicializarTemperatura();
    inicializarLed();
    inicializarWIFI();
    lfs_init();
}

void alterarSensor(){
    if (!gpio_get(BTN_A)){
        mudou_sensor = true;
        sleep_ms(50);
        
        if (!gpio_get(BTN_A)){
            ponteiro = (ponteiro + 1) % LIMITE_PONTEIRO;
        }
        while(!gpio_get(BTN_A)){}
    }
}

void enviarValorManual(int valor){
    char* payload = intToString(valor);

    if (mqttConectado){
        
        switch (ponteiro) {
            case 0:
                printf("%d\n",ponteiro);
                printf("%s\n",payload);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT_SENSOR[ponteiro],0,32);
                sleep_ms(2000);

                estabilizarUmid(valor);
                printOled("AGUARDANDO ENTRADA");
                appendOled("MEXA O JOYSTICK",0,16);

                sleep_ms(2000);
                break;
            case 1:
                printf("%d\n",ponteiro);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT_SENSOR[ponteiro],0,32);
                sleep_ms(2000);

                estabilizarTemp(valor);
                
                printOled("AGUARDANDO ENTRADA");
                appendOled("MEXA O JOYSTICK",0,16);

                sleep_ms(2000);
            break;
            case 2:
                printf("%d\n",ponteiro);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT_SENSOR[ponteiro],0,32);
                sleep_ms(2000);

                estabilizarLum(valor);
                
                printOled("AGUARDANDO ENTRADA");
                appendOled("MEXA O JOYSTICK",0,16);

                sleep_ms(2000);
            break;
            case 3:
                printf("%d\n",ponteiro);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT_SENSOR[ponteiro],0,32);
                sleep_ms(2000);

                estabilizarPh(valor);
                
                printOled("AGUARDANDO ENTRADA");
                appendOled("MEXA O JOYSTICK",0,16);

                sleep_ms(2000);
            break;
            
            default:
                break;
        }

    } else {
        printf("n tá conectado\n");
    }

    free(payload);
}

char* intToString(int valor){
    // "{\"valor\": %d}"
    const int TAMANHO_MEMORIA = 16;
    char *string = malloc(TAMANHO_MEMORIA);
    snprintf(string,TAMANHO_MEMORIA,"%d",valor);
    
    return string;
}

void simularValores(){
    if (joystick_y > CENTRO + DEADZONE || joystick_x > CENTRO + DEADZONE) {
        valor_simulado++;
        mudou_sensor = true;
        sleep_ms(100);
    } else if (joystick_y < CENTRO - DEADZONE || joystick_x < CENTRO - DEADZONE) {
        valor_simulado--;
        mudou_sensor = true;
        sleep_ms(100);
    }
}

void mudarModoOperacao(const char* payload){
    if (payload == NULL) return;
    if (strlen(payload) == 0) return;

    if(strcmp(payload,"manual") == 0){
        modo_manual = true;
        printOled("MODO MANUAL");
        sleep_ms(1000);
        printOled("AGUARDANDO ENTRADA");
        appendOled("MEXA O JOYSTICK",0,16);
        printf("modo manual ativado\n");

    } else if(strcmp(payload,"autonomo") == 0){
        modo_manual = false;
        printOled("MODO AUTONOMO");
        printf("modo autonomo ativado\n");
        sleep_ms(500);
    }
}

void modoManual(){
    
    alterarSensor();
    lerJoystick();
    simularValores();
    apagarTodos();

    if (mudou_sensor){
        char msg[16]; 
        snprintf(msg,sizeof(msg),"%d",valor_simulado);
        printOled(SENSORES_DISPONIVEIS[ponteiro]);
        appendOled("VALOR A SER ENVIADO",0,16);
        appendOled(msg,0,32);
        appendOled("APERTE B PARA ENVIAR",0,54);
        
        mudou_sensor = false;
    }
    
    if (!gpio_get(BTN_B)){
        sleep_ms(50);
        if (!gpio_get(BTN_B)){
            enviarValorManual(valor_simulado);
        }
        while(!gpio_get(BTN_B)){}
    }

}

void modoAutonomo(){

    int umidade = rand() % 100+1;
    float temperatura = pegarTemperatura();
    int luminosidade = rand() % 80000+1;
    int ph = rand() % 14+1;

    printf("Umidade: %d%% | Temperatura: %.2f °C | Luminosidade: %d lux | pH: %d\n",umidade, temperatura, luminosidade, ph);

    enviarValorAutonomo();
    // ) Sementes e mudas
        // 5.000 a 15.000 lux
        // PPFD: ~100 a 300 µmol/m²/s
        // Luz mais suave para evitar estresse e queimaduras.
        // 2) Crescimento vegetativo (folhas, caules)
        // 15.000 a 50.000 lux
        // PPFD: ~300 a 600 µmol/m²/s
        // Ideal para hortaliças e plantas em desenvolvimento ativo.
        // 3) Floração e frutificação
        // 50.000 a 80.000 lux (ou mais em algumas espécies)
}

void enviarValorAutonomo(){
    // "MEDIR_UMIDADE",
    // "MEDIR_TEMPERATURA",
    // "MEDIR_LUMINOSIDADE",
    // "MEDIR_PH"

    // char* payload = intToString(valor);
    
    char* payload;

    char linha[30];

    int umidade = rand() % 100 + 1;
    float temperatura = pegarTemperatura();
    int luminosidade = rand() % 80000 + 1;
    int ph = rand() % 14 + 1;

    sprintf(linha, "UMIDADE: %d%%", umidade);
    printOled(linha);

    sprintf(linha, "TEMP: %.1f C", temperatura);
    appendOled(linha, 0, 15);

    sprintf(linha, "LUM: %d", luminosidade);
    appendOled(linha, 0, 30);

    sprintf(linha, "PH: %d", ph);
    appendOled(linha, 0, 45);

    if (mqttConectado){
        
        switch (ponteiro) {
            case 0:
                payload = intToString(umidade);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                sleep_ms(2000);
                estabilizarUmid(umidade);

                sleep_ms(2000);
                break;
            case 1:
                payload = intToString(temperatura);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                sleep_ms(2000);

                estabilizarTemp(temperatura);

                sleep_ms(2000);
            break;
            case 2:
                payload = intToString(luminosidade);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                sleep_ms(2000);
                estabilizarLum(luminosidade);

                sleep_ms(2000);
            break;
            case 3:
                payload = intToString(ph);
                MQTTPublicar(payload,TOPICOS_MQTT_SENSOR[ponteiro]);
                sleep_ms(2000);
                estabilizarPh(ph);

                sleep_ms(2000);
            break;
            
            default:
                break;
        }

    } else {
        printf("n tá conectado\n");
    }

    ponteiro = (ponteiro + 1) % LIMITE_PONTEIRO;
    free(payload);
}

Rede getCredenciaisRede(const char* payload){
    Rede novaRede = parseCredenciaisRede(payload);
    printf("a rede ai\n");
    printf("%s\n",novaRede.NOME);
    printf("%s\n",novaRede.SENHA);

    return novaRede;
}

void modoOperacao(){
    if (modo_manual){
        modoManual();
    } else {
        modoAutonomo();
    }
}

void esperarCredenciaisRede(){

    printOled("DEFINA AS CREDENCIAIS");
    appendOled("DA REDE NA DASHBOARD:",0,16);
    
    while(true){
        const char* payload = MQTTReceber();
        if (payload != NULL) {
            
            if (strcmp(ultimoTopico, "projeto/config/rede") == 0) {
                Rede novaRede = getCredenciaisRede(payload);
                
                salvarCredenciais(novaRede.NOME,novaRede.SENHA);

                printOled("CONECTANDO A INTERNET");
                conectarRede(novaRede.NOME,novaRede.SENHA);
                return;
            }
        }
    }
}

void carregaArquivosFlash(){
    if (carregarFlashRede(nomeRede,senhaRede)){
        printOled("CONECTANDO AO WIFI");
        appendOled("SALVO",0,16);

        conectarRede(nomeRede,senhaRede);
    } else {
        printOled("CONECTANDO AO WIFI");
        appendOled("PROVISORIO",0,16);

        conectarRede("Somanex Casa 11","23280915");
    }

    if (carregarFlashValoresLimites()){
        printOled("CARREGANDO VALORES");
        appendOled("LIMITES DA FLASH",0,16);

        printf("\n===== VALORES LIMITES =====\n");
        printf("Temperatura:\n");
        printf("  Min: %d\n", limiteMinTemp);
        printf("  Max: %d\n", limiteMaxTemp);
                    
        printf("\nUmidade:\n");
        printf("  Min: %d\n", limiteMinUmidade);
        printf("  Max: %d\n", limiteMaxUmidade);
                    
        printf("\nLuminosidade:\n");
        printf("  Min: %d\n", limiteMinLuminosidade);
        printf("  Max: %d\n", limiteMaxLuminosidade);
                    
        printf("\npH:\n");
        printf("  Min: %d\n", limiteMinPh);
        printf("  Max: %d\n", limiteMaxPh);

    } else {
        printOled("DEFINA OS VALORES");
        appendOled("LIMITES NA DASHBOARD",0,16);
    }
}