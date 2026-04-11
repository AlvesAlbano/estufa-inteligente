#include "pico/stdlib.h"
#include "hardware/adc.h"

void inicializarTemperatura();
float pegarTemperatura();
float converterCelcius(float voltage);

float pegarTemperatura(){
    inicializarTemperatura();
    uint16_t raw = adc_read();
    // Convert raw value to voltage (assuming 3.3V ref)
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = raw * conversion_factor;


    float temperatura = converterCelcius(voltage);

    return temperatura;
}


float converterCelcius(float voltage){
    float temp = 27.0f - (voltage - 0.706f) / 0.001721f;
    
    return temp;
}

void inicializarTemperatura(){
    adc_init();
    
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}