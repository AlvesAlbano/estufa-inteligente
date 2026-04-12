#ifndef FLASH_H
#define FLASH_H

extern char nomeRedeAtual[50];
extern char senhaRedeAtual[50];

void lfs_init();
void salvarCredenciais(const char* nomeRede, const char* senhaRede);
void salvarValoresLimites();
void carregarCredenciaisSalvas();
void atualizarCredenciais(const char* nomeRede, const char* senhaRede);
void limparFlash();

bool carregarFlashRede(char* nomeRede, char* senhaRede);
bool carregarFlashValoresLimites();
#endif