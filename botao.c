#include "pico/stdlib.h"

void inicializarBtn(uint gpioBtn);

void inicializarBtn(uint gpioBtn){
    gpio_init(gpioBtn);
    gpio_set_dir(gpioBtn,GPIO_IN);
    gpio_pull_up(gpioBtn);
}