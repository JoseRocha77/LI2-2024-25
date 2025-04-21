#ifndef TESTAR_H
#define TESTAR_H

#include "../include/jogo.h"

// Funções auxiliares
void criar_arquivo_teste();
void limpar_arquivo_teste();

// Testes para carregamento de jogo
void teste_carregar_jogo_valido();
void teste_carregar_jogo_arquivo_inexistente();

// Testes para pintar e riscar
void teste_pintar_branco_valido();
void teste_pintar_branco_invalido();
void teste_riscar_valido();
void teste_riscar_invalido();

// Testes para registo e desfazer movimentos
void teste_registar_movimento();
void teste_desfazer_movimento();
void teste_desfazer_movimento_sem_historico();
void teste_desfazer_multiplos_movimentos();

// Testes para verificação de restrições
void teste_verificar_restricoes_basico();
void teste_verificar_restricao_casas_riscadas_adjacentes_direita();
void teste_verificar_restricao_casas_riscadas_adjacentes_esquerda();
void teste_verificar_restricao_casas_riscadas_adjacentes_baixo();
void teste_verificar_restricao_casas_riscadas_adjacentes_cima();
void teste_verificar_restricao_vizinhos_nao_brancos_casa_riscada();
void teste_verificar_restricao_vizinhos_riscados_todos();
void teste_verificar_restricao_vizinhos_riscados_dois_riscados();
void teste_verificar_restricao_vizinhos_riscados_tres_riscados();

// Testes para conectividade
void teste_verificar_conectividade_sem_brancas();
void teste_verificar_conectividade_com_brancas_conectadas();
void teste_verificar_conectividade_com_brancas_conectadas_com_biforcação();
void teste_verificar_conectividade_com_brancas_desconectadas();
void teste_verificar_conectividade_com_brancas_desconectadas_diagonal();
void teste_verificar_conectividade_com_brancas_desconectadas_e_conectadas();

// Testes para comandos
void teste_processar_comando_carregar();
void teste_processar_comando_pintar();
void teste_processar_comando_riscar();
void teste_processar_comando_desfazer();
void teste_processar_comando_verificar();
void teste_processar_comando_sair();
void teste_processar_comando_gravar();
void teste_processar_comando_invalido();

// Testes para gravação de jogo
void teste_gravar_jogo_valido();
void teste_gravar_jogo_invalido();
void teste_processar_comando_gravar();

// Inicialização da suíte
int inicializar();
int limpar();


#endif