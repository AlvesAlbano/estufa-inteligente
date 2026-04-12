#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "pico/stdlib.h"

#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "Oled.h"

#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12

int limiteMaxTemp = 0;
int limiteMaxUmidade = 0;
int limiteMaxLuminosidade = 0;
int limiteMaxPh = 0;
int limiteMinTemp = 0;
int limiteMinUmidade = 0;
int limiteMinLuminosidade = 0;
int limiteMinPh = 0;

float DIVIDER_PWM = 10.0;
uint32_t PERIOD;

typedef struct {
    int LIMITE_MAX_TEMP;
    int LIMITE_MAX_UMIDADE;
    int LIMITE_MAX_LUMINOSIDADE;
    int LIMITE_MAX_PH;
    int LIMITE_MIN_TEMP;
    int LIMITE_MIN_UMIDADE;
    int LIMITE_MIN_LUMINOSIDADE;
    int LIMITE_MIN_PH;
} ValoresLimites;


// key:{
//         msg:msg
//     }

ValoresLimites getValoresLimites(const char* json);

void estabilizarTemp(float tempAtual);
void estabilizarPh(int phAtual);
void estabilizarUmid(int umidadeAtual);
void estabilizarLum(int luminosidadeAtual);
void inicializarLed();
void switchLed(int pinoLed);
void setPWM(int pinoLed);
void aumentarBrilhoLed(int pinoLed);
void escurecerLed(int pinoLed);
void apagarLed(int pinoLed);
void parseValoresLimites(ValoresLimites valoreLimites);
void apagarTodos();


ValoresLimites getValoresLimites(const char* json){
    // {"tempMin":1,"tempMax":1,"umMin":1,"umMax":1,"lumMin":1,"lumMax":1,"phMin":1,"phMax":1}
    ValoresLimites valoresLimites = {0};

    cJSON *root = cJSON_Parse(json);
    if (!root) {
        printf("Erro ao parsear json\n");
        cJSON_Delete(root);

        return valoresLimites;
    }

    cJSON *tmin = cJSON_GetObjectItem(root, "tempMin");
    cJSON *tmax = cJSON_GetObjectItem(root, "tempMax");

    cJSON *umin = cJSON_GetObjectItem(root, "umMin");
    cJSON *umax = cJSON_GetObjectItem(root, "umMax");

    cJSON *lmin = cJSON_GetObjectItem(root, "lumMin");
    cJSON *lmax = cJSON_GetObjectItem(root, "lumMax");

    cJSON *pmin = cJSON_GetObjectItem(root, "phMin");
    cJSON *pmax = cJSON_GetObjectItem(root, "phMax");

    if (cJSON_IsNumber(tmin)) {
        valoresLimites.LIMITE_MIN_TEMP = tmin->valueint;
    }

    if (cJSON_IsNumber(tmax)) {
        valoresLimites.LIMITE_MAX_TEMP = tmax->valueint;
    }

    if (cJSON_IsNumber(umin)) {
        valoresLimites.LIMITE_MIN_UMIDADE = umin->valueint;
    }

    if (cJSON_IsNumber(umax)) {
        valoresLimites.LIMITE_MAX_UMIDADE = umax->valueint;
    }

    if (cJSON_IsNumber(lmin)) {
        valoresLimites.LIMITE_MIN_LUMINOSIDADE = lmin->valueint;
    }

    if (cJSON_IsNumber(lmax)) {
        valoresLimites.LIMITE_MAX_LUMINOSIDADE = lmax->valueint;
    }

    if (cJSON_IsNumber(pmin)) {
        valoresLimites.LIMITE_MIN_PH = pmin->valueint;
    }

    if (cJSON_IsNumber(pmax)) {
        valoresLimites.LIMITE_MAX_PH = pmax->valueint;
    }

    cJSON_Delete(root);
    return valoresLimites;
}

void parseValoresLimites(ValoresLimites valoreLimites){
    limiteMinTemp = valoreLimites.LIMITE_MIN_TEMP;
    limiteMaxTemp = valoreLimites.LIMITE_MAX_TEMP;

    limiteMinUmidade = valoreLimites.LIMITE_MIN_UMIDADE;
    limiteMaxUmidade = valoreLimites.LIMITE_MAX_UMIDADE;

    limiteMinLuminosidade = valoreLimites.LIMITE_MIN_LUMINOSIDADE;
    limiteMaxLuminosidade = valoreLimites.LIMITE_MAX_LUMINOSIDADE;

    limiteMinPh = valoreLimites.LIMITE_MIN_PH;
    limiteMaxPh = valoreLimites.LIMITE_MAX_PH;
}

void estabilizarTemp(float tempAtual){
    apagarTodos();
    if (tempAtual > limiteMaxTemp){
        // printOled("CONECTANDO AO BROKER");
        printOled("TEMPERATURA ACIMA DO NORMAL");
        appendOled("DIMINUINDO TEMPERATURA",0,16);
        escurecerLed(LED_VERMELHO);
        
        sleep_ms(2000);
        
    } else if(tempAtual < limiteMinTemp){
        printOled("TEMPERATURA ACIMA DO NORMAL");
        appendOled("AUMENTANDO TEMPERATURA",0,16);

        aumentarBrilhoLed(LED_VERMELHO);
        sleep_ms(2000);
    }
}

void estabilizarPh(int phAtual){
    apagarTodos();

    if (phAtual > limiteMaxPh){
        // printOled("CONECTANDO AO BROKER");
        printOled("PH ACIMA DO NORMAL");
        appendOled("DIMINUINDO PH",0,16);
        escurecerLed(LED_AZUL);
        
        sleep_ms(2000);
        
    } else if(phAtual < limiteMinPh){
        printOled("PH ABAIXO DO NORMAL");
        appendOled("AUMENTANDO PH",0,16);
        
        aumentarBrilhoLed(LED_AZUL);
        sleep_ms(2000);
    }
}

void estabilizarUmid(int umidadeAtual){
    apagarTodos();

    if (umidadeAtual > limiteMaxUmidade){
        // printOled("CONECTANDO AO BROKER");
        printOled("UMIDADE ACIMA DO NORMAL");
        appendOled("DIMINUINDO UMIDADE",0,16);
        escurecerLed(LED_VERDE);
        
        sleep_ms(2000);
        
    } else if(umidadeAtual < limiteMinUmidade){
        printOled("UMIDADE ABAIXO DO NORMAL");
        appendOled("AUMENTANDO UMIDADE",0,16);
        
        aumentarBrilhoLed(LED_VERDE);
        sleep_ms(2000);
    }
}

void estabilizarLum(int luminosidadeAtual){
    apagarTodos();

    if (luminosidadeAtual > limiteMaxLuminosidade){
        // printOled("CONECTANDO AO BROKER");
        printOled("LUMINOSIDADE ACIMA DO NORMAL");
        appendOled("DIMINUINDO LUMINOSIDADE",0,16);
        escurecerLed(LED_AZUL);
        
        sleep_ms(2000);
        
    } else if(luminosidadeAtual < limiteMinLuminosidade){
        printOled("LUMINOSIDADE ABAIXO DO NORMAL");
        appendOled("AUMENTANDO LUMINOSIDADE",0,16);
        aumentarBrilhoLed(LED_AZUL);
        
        sleep_ms(2000);
    }
}

void switchLed(int pinoLed){
    gpio_put(pinoLed,1);
    sleep_ms(1000);
    gpio_put(pinoLed,0);
}

void inicializarLed(){
    // gpio_init(LED_VERMELHO);
    // gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    
    // gpio_init(LED_VERDE);
    // gpio_set_dir(LED_VERDE, GPIO_OUT);
    
    // gpio_init(LED_AZUL);
    // gpio_set_dir(LED_AZUL, GPIO_OUT);

    uint32_t clock = clock_get_hz(clk_sys);
    
    PERIOD = (clock / (DIVIDER_PWM * 1000)) - 1;
    
    setPWM(LED_AZUL);
    setPWM(LED_VERDE);
    setPWM(LED_VERMELHO);
}

void aumentarBrilhoLed(int pinoLed){
    for(int i = 0;i <= PERIOD;i+=50){
        pwm_set_gpio_level(pinoLed,i);
        sleep_ms(10);
    }
}

void escurecerLed(int pinoLed) {
    for(int i = PERIOD;i >= 0;i-=50){
        pwm_set_gpio_level(pinoLed,i);
        sleep_ms(10);
    }
}

void setPWM(int pinoLed){
    uint slice;

    gpio_set_function(pinoLed, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(pinoLed);

    pwm_set_clkdiv(slice, DIVIDER_PWM);
    pwm_set_wrap(slice, PERIOD);
    pwm_set_enabled(slice, true);
}

void apagarLed(int pinoLed){
    pwm_set_gpio_level(pinoLed,0);
}

void apagarTodos(){
    pwm_set_gpio_level(LED_VERMELHO, 0);
    pwm_set_gpio_level(LED_VERDE, 0);
    pwm_set_gpio_level(LED_AZUL, 0);
}