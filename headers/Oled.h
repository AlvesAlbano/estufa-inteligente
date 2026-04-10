#ifndef OLED_H
#define OLED_H

void inicializarOled();
void printOled(const char* texto);
void appendOled(const char* texto, int x, int y);
void limparOled();

#endif