#include <stdio.h>
#include "pico/stdlib.h"

#include "WIFI.h"
#include "MQTT.h"
#include "Oled.h"

int main(){
    stdio_init_all();
    sleep_ms(2000);

    teste();
    
    conectarRede();
    sleep_ms(5000);

    BrokerConectar(&brokerModel);
    sleep_ms(5000);

    // inicializarOled();
    
    while (true) {
        tight_loop_contents();
        printf("Hello, world!\n");
        // printf("%d\n",mqttConectado);
        sleep_ms(2000);

        if (mqttConectado){
            MQTTPublicar("alou","projeto/teste");
            // char* msg = ;
            sleep_ms(2000);
            continue;
        } else {
            printf("n tá conectado\n");
            continue;
        }
    }
}
