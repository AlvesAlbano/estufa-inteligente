#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "WIFI.h"
#include "MQTT.h"
#include "Oled.h"
#include "joystick.h"
#include "botao.h"

#define BTN_A 5
#define BTN_B 6
#define CENTRO 1932
#define DEADZONE 300
#define STEP_THRESHOLD 200

bool mudou_modo = false;
const char* MODOS_DISPONIVEIS[] = {
    "MEDIR_UMIDADE",
    "MEDIR_TEMPERATURA",
    "MEDIR_LUMINOSIDADE",
    "MEDIR_PH"
};

const char* TOPICOS_MQTT[] = {
    "projeto/teste",
    "projeto/teste",
    "projeto/teste",
    "projeto/teste"
};

const uint LIMITE_PONTEIRO = sizeof(MODOS_DISPONIVEIS) / sizeof(MODOS_DISPONIVEIS[0]);
static uint valor_anterior_joystick = 0;

uint ponteiro = 0;

void inicializarComponentes();
void alterarModo();
char* uintToString(uint valor);
void enviarValor(uint valor);
void simularValores();

int valor_simulado = 0;
int main(){
    stdio_init_all();
    inicializarComponentes();

    printOled("CONECTANDO A INTERNET");
    conectarRede();
    sleep_ms(5000);

    printOled("CONECTANDO AO BROKER");
    BrokerConectar(&brokerModel);
    sleep_ms(5000);

    printOled("AGUARDANDO ENTRADA");
    
    while (1) {
        // tight_loop_contents();
        // printf("Hello, world!\n");
        // printf("%d\n",mqttConectado);
        // printf("x = %d\n",joystick_x);
        // printf("y = %d\n",joystick_y);
        // printf("\n");
        // printOled(msg);
        alterarModo();
        lerJoystick();
        // printf("valor anterior: %d\n",valor_anterior_joystick);
        // printf("joystick: %d\n",joystick_x);
        printf("%d\n",CENTRO);
        printf("%d\n",DEADZONE);
        printf("%d\n",joystick_y);

        printf("valor controlado: %d\n",valor_simulado);
        simularValores();
        if (mudou_modo){
            char msg[16]; 
            snprintf(msg,sizeof(msg),"%d",valor_simulado);
            printOled(MODOS_DISPONIVEIS[ponteiro]);
            appendOled("VALOR A SER ENVIADO",0,16);
            appendOled(msg,0,32);
            appendOled("APERTE B PARA ENVIAR",0,54);
            
            mudou_modo = false;
        }

        if (!gpio_get(BTN_B)){
            sleep_ms(50);

            if (!gpio_get(BTN_B)){
                enviarValor(valor_simulado);
            }
            while(!gpio_get(BTN_B)){}
        }

        // sleep_ms(1000);
    }
}

void inicializarComponentes(){
    inicializarOled();
    inicializarJoystick();
    inicializarBtn(BTN_A);
    inicializarBtn(BTN_B);
}

void alterarModo(){
    if (!gpio_get(BTN_A)){
        mudou_modo = true;
        sleep_ms(50);
        
        if (!gpio_get(BTN_A)){
            ponteiro = (ponteiro + 1) % LIMITE_PONTEIRO;
        }
        while(!gpio_get(BTN_A)){}
    }
}

void enviarValor(uint valor){
    char* payload = uintToString(valor);

    if (mqttConectado){
        
        switch (ponteiro) {
            case 0:
                printf("%s\n",payload);
                MQTTPublicar(payload,TOPICOS_MQTT[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT[ponteiro],0,32);
                
                sleep_ms(2000);
            break;
            case 1:
                printf("%s\n",payload);
                MQTTPublicar(payload,TOPICOS_MQTT[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT[ponteiro],0,32);
                
                sleep_ms(2000);
            break;
            case 2:
                printf("%s\n",payload);
                MQTTPublicar(payload,TOPICOS_MQTT[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT[ponteiro],0,32);
                
                sleep_ms(2000);
            break;
            case 3:
                printf("%s\n",payload);
                MQTTPublicar(payload,TOPICOS_MQTT[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT[ponteiro],0,32);
                
                sleep_ms(2000);
            break;
            case 4:
                printf("%s\n",payload);
                MQTTPublicar(payload,TOPICOS_MQTT[ponteiro]);
                printOled("VALOR ENVIADO PARA O");
                appendOled("TOPICO:",0,16);
                appendOled(TOPICOS_MQTT[ponteiro],0,32);
                
                sleep_ms(2000);
            break;
            
            default:
                break;
        }

    } else {
        printf("n tá conectado\n");
    }

    limparOled();
    free(payload);
}

char* uintToString(uint valor){
    // "{\"valor\": %d}"
    const int TAMANHO_MEMORIA = 16;
    char *string = malloc(TAMANHO_MEMORIA);
    snprintf(string,TAMANHO_MEMORIA,"%u}",valor);
    
    return string;
}

void simularValores(){
    if (joystick_y > CENTRO + DEADZONE || joystick_x > CENTRO + DEADZONE) {
        valor_simulado++;
        mudou_modo = true;
        sleep_ms(100);
    } else if (joystick_y < CENTRO - DEADZONE || joystick_x < CENTRO - DEADZONE) {
        valor_simulado--;
        mudou_modo = true;
        sleep_ms(100);
    }
}