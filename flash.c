#include "lfs.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "cJSON.h"

#include <string.h>
#include <stdio.h>
#include "tomada-decisoes.h"
#include "WIFI.h"

#define FLASH_OFFSET (1024 * 1024)
#define FLASH_SIZE   (1024 * 1024)
#define BLOCK_SIZE 4096

char nomeRedeAtual[50];
char senhaRedeAtual[50];

static lfs_t lfs;
static struct lfs_config cfg;

void salvarCredenciais(const char* nomeRede, const char* senhaRede);
void salvarValoresLimites();
void carregarCredenciaisSalvas();
void atualizarCredenciais(const char* nomeRede, const char* senhaRede);
void limparFlash();

bool carregarFlashRede(char* nomeRede, char* senhaRede);
bool carregarFlashValoresLimites();

static int lfs_read(const struct lfs_config *c, lfs_block_t block,lfs_off_t off, void *buffer, lfs_size_t size) {

    const uint8_t *flash = (const uint8_t *)(XIP_BASE + FLASH_OFFSET);
    memcpy(buffer, flash + block * c->block_size + off, size);
    return 0;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block,lfs_off_t off, const void *buffer, lfs_size_t size) {

    uint32_t ints = save_and_disable_interrupts();

    flash_range_program(
        FLASH_OFFSET + block * c->block_size + off,
        buffer,
        size
    );

    restore_interrupts(ints);
    return 0;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block) {
    uint32_t ints = save_and_disable_interrupts();

    flash_range_erase(
        FLASH_OFFSET + block * c->block_size,
        c->block_size
    );

    restore_interrupts(ints);
    return 0;
}

static int lfs_sync(const struct lfs_config *c) {
    return 0;
}

void lfs_init() {
    cfg.read  = lfs_read;
    cfg.prog  = lfs_prog;
    cfg.erase = lfs_erase;
    cfg.sync  = lfs_sync;

    cfg.read_size = 256;
    cfg.prog_size = 256;
    cfg.block_size = BLOCK_SIZE;
    cfg.block_count = FLASH_SIZE / BLOCK_SIZE;
    cfg.cache_size = 256;
    cfg.lookahead_size = 256;
    cfg.block_cycles = 500;

    int err = lfs_mount(&lfs, &cfg);

    if (err) {
        printf("Formatando FS...\n");
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }
}

void salvarCredenciais(const char* nomeRede, const char* senhaRede) {
    lfs_file_t file;

    char json[128];

    snprintf(json, sizeof(json),
        "{\"nomeRede\":\"%s\",\"senhaRede\":\"%s\"}",
        nomeRede, senhaRede
    );

    if (lfs_file_open(&lfs, &file, "wifi.json",
        LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC) < 0) {
        printf("Erro ao abrir arquivo\n");
        return;
    }

    lfs_file_write(&lfs, &file, json, strlen(json));
    lfs_file_close(&lfs, &file);

    printf("Salvo na flash: %s\n", json);
}

void salvarValoresLimites() {
    lfs_file_t file;

    char json[256];

    snprintf(json, sizeof(json),
        "{"
        "\"tempMin\":%d,"
        "\"tempMax\":%d,"
        "\"umMin\":%d,"
        "\"umMax\":%d,"
        "\"lumMin\":%d,"
        "\"lumMax\":%d,"
        "\"phMin\":%d,"
        "\"phMax\":%d"
        "}",
        limiteMinTemp,
        limiteMaxTemp,
        limiteMinUmidade,
        limiteMaxUmidade,
        limiteMinLuminosidade,
        limiteMaxLuminosidade,
        limiteMinPh,
        limiteMaxPh
    );

    if (lfs_file_open(&lfs, &file, "limites.json",
        LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC) < 0) {
        printf("Erro ao abrir arquivo de limites\n");
        return;
    }

    lfs_file_write(&lfs, &file, json, strlen(json));
    lfs_file_close(&lfs, &file);

    printf("Valores Limites salvos: %s\n", json);
}

bool carregarFlashValoresLimites() {
    lfs_file_t file;

    if (lfs_file_open(&lfs, &file, "limites.json", LFS_O_RDONLY) < 0) {
        printf("Arquivo de limites nao encontrado\n");
        return false;
    }

    char buffer[256];
    int size = lfs_file_read(&lfs, &file, buffer, sizeof(buffer) - 1);
    
    if (size < 0) {
        printf("Erro ao ler arquivo\n");
        lfs_file_close(&lfs, &file);
        return false;
    }

    buffer[size] = '\0';

    lfs_file_close(&lfs, &file);

    printf("Lido da flash (limites): %s\n", buffer);

    ValoresLimites valores = getValoresLimites(buffer);

    parseValoresLimites(valores);

    return true;
}

bool carregarFlashRede(char* nomeRede, char* senhaRede) {
    lfs_file_t file;

    if (lfs_file_open(&lfs, &file, "wifi.json", LFS_O_RDONLY) < 0) {
        printf("Arquivo nao encontrado\n");
        return false;
    }

    char buffer[128];
    int size = lfs_file_read(&lfs, &file, buffer, sizeof(buffer)-1);
    buffer[size] = '\0';

    lfs_file_close(&lfs, &file);

    printf("Lido da flash: %s\n", buffer);

    cJSON *root = cJSON_Parse(buffer);
    if (!root) {
        printf("Erro JSON\n");
        return false;
    }

    cJSON *s = cJSON_GetObjectItem(root, "nomeRede");
    cJSON *p = cJSON_GetObjectItem(root, "senhaRede");

    if (cJSON_IsString(s)) strcpy(nomeRede, s->valuestring);
    if (cJSON_IsString(p)) strcpy(senhaRede, p->valuestring);

    cJSON_Delete(root);
    return true;
}

void carregarCredenciaisSalvas() {
    if (carregarFlashRede(nomeRedeAtual, senhaRedeAtual)) {
        printf("Credenciais carregadas: %s\n", nomeRedeAtual);
    } else {
        printf("Sem credenciais salvas\n");
        nomeRedeAtual[0] = '\0';
        senhaRedeAtual[0] = '\0';
    }
}

void atualizarCredenciais(const char* nomeRede, const char* senhaRede) {

    strcpy(nomeRedeAtual, nomeRede);
    strcpy(senhaRedeAtual, senhaRede);

    salvarCredenciais(nomeRede, senhaRede);

    reconectarWiFi(nomeRede, senhaRede);
}

void limparFlash() {
    printf("Limpando sistema de arquivos (LittleFS)...\n");

    int err = lfs_format(&lfs, &cfg);
    if (err) {
        printf("Erro ao formatar flash: %d\n", err);
        return;
    }

    err = lfs_mount(&lfs, &cfg);
    if (err) {
        printf("Erro ao remontar filesystem: %d\n", err);
        return;
    }

    nomeRedeAtual[0] = '\0';
    senhaRedeAtual[0] = '\0';

    printf("Flash (filesystem) limpa com sucesso!\n");
}