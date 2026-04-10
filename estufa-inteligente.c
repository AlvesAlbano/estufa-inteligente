#include <stdio.h>
#include "pico/stdlib.h"

#include "WIFI.h"
#include "MQTT.h"
#include "Oled.h"
#include "joystick.h"
#include "botao.h"

#define BTN_A 5
#define BTN_B 6

const char* MODOS_DISPONIVEIS[] = {
    "MEDIR_UMIDADE",
    "MEDIR_TEMPERATURA",
    "MEDIR_LUMINOSIDADE",
    "MEDIR_PH"
};

const uint LIMITE_PONTEIRO = sizeof(MODOS_DISPONIVEIS) / sizeof(MODOS_DISPONIVEIS[0]);

uint ponteiro = 0;

void inicializarComponentes();
void modoInsercaoDados();

int main(){
    stdio_init_all();
    sleep_ms(2000);

    // conectarRede();
    // sleep_ms(5000);

    // BrokerConectar(&brokerModel);
    // sleep_ms(5000);

    inicializarComponentes();
    
    while (1) {
        // tight_loop_contents();
        // printf("Hello, world!\n");
        // printf("%d\n",mqttConectado);
        // printf("x = %d\n",joystick_x);
        // printf("y = %d\n",joystick_y);
        // printf("\n");
        // printOled(msg);
        
        if (!gpio_get(BTN_A)){
            sleep_ms(50);
            
            if (!gpio_get(BTN_A)){
                ponteiro = (ponteiro + 1) % LIMITE_PONTEIRO;
            }

            while(!gpio_get(BTN_A)){}
        }
        
        lerJoystick();
        char msg[16]; 
        snprintf(msg,sizeof(msg),"%d",joystick_x);
        printOled(MODOS_DISPONIVEIS[ponteiro]);
        appendOled("VALOR A SER ENVIADO",0,16);
        appendOled(msg,0,32);
        appendOled("APERTE B PARA ENVIAR",0,54);

        sleep_ms(200);

    //     if (mqttConectado){
    //         MQTTPublicar("alou","projeto/teste");
    //         // char* msg = ;
    //         sleep_ms(2000);
    //         continue;
    //     } else {
    //         printf("n tá conectado\n");
    //         continue;
    //     }
    }
}

void inicializarComponentes(){
    inicializarOled();
    inicializarJoystick();
    inicializarBtn(BTN_A);
    inicializarBtn(BTN_B);
}

void modoInsercaoDados(){
    
}