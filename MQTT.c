#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"

#define MQTT_MSG_MAX 100

char ultimaMensagem[MQTT_MSG_MAX];
bool novaMensagem = false;

mqtt_client_t *client;
ip_addr_t broker_ip;

typedef struct {
    const char *NAME;
    const char *HOST;
    const unsigned int PORT;
    const char *CLIENT_ID;
    const char *USERNAME;
    const char *PASSWORD;
} BrokerModel;

BrokerModel brokerModel = {
    .NAME = "embarcatech-projeto",
    .HOST = "broker.emqx.io",
    .PORT = 1883,
    .CLIENT_ID = "pico_w",
    .USERNAME = NULL,
    .PASSWORD = NULL
};

bool mqttConectado = false;
void BrokerConectar(BrokerModel *config);
void MQTTPublicar(const char *msg, const char *topico);
void MQTTInscrever(const char *topico);
void MQTTInscreverMultiplos(const char *topicos[],int tamanho);

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    // printf("Topico: %s\n", topic);
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {

    if (len >= MQTT_MSG_MAX) len = MQTT_MSG_MAX - 1;

    memcpy(ultimaMensagem, data, len);
    ultimaMensagem[len] = '\0';
    // printf("Mensagem: %s\n",ultimaMensagem);

    novaMensagem = true;
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker!\n");

        BrokerModel *config = (BrokerModel *)arg;

        mqttConectado = true;

    } else {
        mqttConectado = false;
        printf("Erro na conexao: %d\n", status);
    }
}

static void dns_found(const char *name, const ip_addr_t *ipaddr, void *arg) {
    BrokerModel *config = (BrokerModel *)arg;

    if (!ipaddr) {
        printf("Erro DNS\n");
        return;
    }

    broker_ip = *ipaddr;

    struct mqtt_connect_client_info_t ci = {
        .client_id = config->CLIENT_ID,
        .client_user = config->USERNAME,
        .client_pass = config->PASSWORD
    };

    client = mqtt_client_new();

    mqtt_set_inpub_callback(client,
        mqtt_incoming_publish_cb,
        mqtt_incoming_data_cb,
        NULL
    );

    mqtt_client_connect(client, &broker_ip, config->PORT,
                        mqtt_connection_cb, config, &ci);
}

static void mqtt_pub_request_cb(void *arg, err_t err) {
    if (err == ERR_OK) {
        printf("Mensagem enviada com sucesso!\n");

    } else {
        printf("Erro ao publicar: %d\n", err);
    }
}


void BrokerConectar(BrokerModel *config) {
    dns_gethostbyname(config->HOST, &broker_ip, dns_found, config);
}

void MQTTPublicar(const char *payload, const char *topico) {
    mqtt_publish(client, topico, payload, strlen(payload), 0, 0, mqtt_pub_request_cb, NULL);
}

void MQTTInscrever(const char *topico){
    mqtt_subscribe(client, topico, 0, NULL, NULL);
}

void MQTTInscreverMultiplos(const char *topicos[],int tamanho){

    for(int i = 0;i < tamanho;i++){
        const char* topicoAtual = topicos[i];
        mqtt_subscribe(client, topicoAtual, 0, NULL, NULL);
        printf("%s\n",topicoAtual);
        sleep_ms(500);
    }
    printf("inscrito em todos os topicos!\n");

}

const char* MQTTReceber() {
    if (novaMensagem) {
        novaMensagem = false;
        return ultimaMensagem;
    }
    return NULL;
}