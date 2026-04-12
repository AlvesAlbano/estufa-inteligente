#ifndef TOMADA_DECISOES_H
#define TOMADA_DECISOES_H

extern int limiteMaxTemp;
extern int limiteMaxUmidade;
extern int limiteMaxLuminosidade;
extern int limiteMaxPh;
extern int limiteMinTemp;
extern int limiteMinUmidade;
extern int limiteMinLuminosidade;
extern int limiteMinPh;

typedef struct {
    const int limiteMaxTemp;
    const int limiteMaxUmidade;
    const int limiteMaxLuminosidade;
    const int limiteMaxPh;
    const int limiteMinTemp;
    const int limiteMinUmidade;
    const int limiteMinLuminosidade;
    const int limiteMinPh;
} ValoresLimites;

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

#endif