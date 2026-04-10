#include "pico/stdlib.h"
#include "hardware/adc.h"

#define JOYSTICK_X 26
#define JOYSTICK_Y 27

uint16_t joystick_x = 0;
uint16_t joystick_y = 0;

uint fds = 12;
void lerJoystick();
void inicializarJoystick();

void lerJoystick(){
    adc_select_input(0);
    joystick_x = adc_read();

    adc_select_input(1);
    joystick_y = adc_read();
}

void inicializarJoystick(){
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);
}