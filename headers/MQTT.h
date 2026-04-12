#ifndef MQTT_H
#define MQTT_H

typedef struct {
    const char *NAME;
    const char *HOST;
    const unsigned int PORT;
    const char *CLIENT_ID;
    const char *USERNAME;
    const char *PASSWORD;
} BrokerModel;

extern BrokerModel brokerModel;
extern bool mqttConectado;
extern char ultimoTopico[100];

void BrokerConectar(BrokerModel *config);
void MQTTPublicar(const char *payload, const char *topico);
void MQTTInscrever(const char *topico);
void MQTTInscreverMultiplos(const char *topicos[],int tamanho);
const char* MQTTReceber();
#endif