#include "pico/stdlib.h"
#include "ssd1306.h"

#define OLED_SDA 14
#define OLED_SCL 15
#define I2C_PORT i2c1

#define WIDTH 128
#define HEIGHT 64

// #define WIDTH 64
// #define HEIGHT 128

#define HERTZ 400000
#define ENDERECO 0x3C

ssd1306_t OLED_SSD;

void inicializarOled();
void printOled(const char* texto);
void appendOled(const char* texto, int x, int y);
void limparOled();

void inicializarOled(){
    i2c_init(I2C_PORT,HERTZ);
    
    gpio_set_function(OLED_SDA,GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL,GPIO_FUNC_I2C);

    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    ssd1306_init(&OLED_SSD,WIDTH,HEIGHT,ENDERECO,I2C_PORT);
    ssd1306_clear(&OLED_SSD);
    ssd1306_show(&OLED_SSD);
}

void printOled(const char* texto){
    ssd1306_clear(&OLED_SSD);
    ssd1306_draw_string(&OLED_SSD,0,0,1,texto);
    ssd1306_show(&OLED_SSD);
}

void appendOled(const char* texto, int x, int y){
    ssd1306_draw_string(&OLED_SSD,x,y,1,texto);
    ssd1306_show(&OLED_SSD);

}

void limparOled(){
    ssd1306_clear(&OLED_SSD);
}