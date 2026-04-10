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

void BrokerConectar(BrokerModel *config);
void MQTTPublicar(const char *payload, const char *topico);
void MQTTInscrever(const char *topico);

#endif