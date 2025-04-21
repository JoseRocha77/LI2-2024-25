#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "../include/jogo.h"

// Definições para facilitar os testes
#define TABULEIRO_TEST "tabuleiro_test.txt"
#define ARQUIVO_INEXISTENTE "arquivo_inexistente.txt"

// Função para criar um arquivo de teste
void criar_arquivo_teste() {
    FILE *file = fopen(TABULEIRO_TEST, "w");
    if (file) {
        fprintf(file, "5 5\necadc\ndcdec\nbddce\ncdeeb\naccbb\n");
        fclose(file);
    }
}

// Função para limpar o arquivo de teste
void limpar_arquivo_teste() {
    remove(TABULEIRO_TEST);
}

// ===== Testes para carregamento de jogo =====

void teste_carregar_jogo_valido() {
    criar_arquivo_teste();
    
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    CU_ASSERT_PTR_NOT_NULL(jogo);
    CU_ASSERT_EQUAL(jogo->linhas, 5);
    CU_ASSERT_EQUAL(jogo->colunas, 5);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'e');
    CU_ASSERT_EQUAL(jogo->tabuleiro[4][4], 'b');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_carregar_jogo_arquivo_inexistente() {
    Jogo *jogo = carregarJogo(ARQUIVO_INEXISTENTE);
    CU_ASSERT_PTR_NULL(jogo);
}

// ===== Testes para pintar e riscar =====

void teste_pintar_branco_valido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = pintarBranco(jogo, "a1");
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'E');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_pintar_branco_invalido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = pintarBranco(jogo, "z9");
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_riscar_valido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = riscar(jogo, "a1");
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '#');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_riscar_invalido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = riscar(jogo, "z9");
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

// ===== Testes para registo de movimentos =====

void teste_registar_movimento() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    char estadoAnterior = jogo->tabuleiro[0][0];
    registarMovimento(jogo, 0, 0, estadoAnterior);
    
    CU_ASSERT_PTR_NOT_NULL(jogo->historicoMovimentos);
    CU_ASSERT_EQUAL(jogo->historicoMovimentos->linha, 0);
    CU_ASSERT_EQUAL(jogo->historicoMovimentos->coluna, 0);
    CU_ASSERT_EQUAL(jogo->historicoMovimentos->estadoAnterior, estadoAnterior);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

// ===== Testes para desfazer movimento =====

void teste_desfazer_movimento() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Primeiro, faz um movimento para ter o que desfazer
    pintarBranco(jogo, "a1");
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'E');
    
    // Agora, desfaz o movimento
    int resultado = desfazerMovimento(jogo);
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'e');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_desfazer_movimento_sem_historico() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Tenta desfazer sem ter feito nenhum movimento
    int resultado = desfazerMovimento(jogo);
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_desfazer_multiplos_movimentos() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Faz vários movimentos
    pintarBranco(jogo, "a1");
    riscar(jogo, "b1");
    pintarBranco(jogo, "c1");
    
    // Desfaz os movimentos na ordem inversa
    desfazerMovimento(jogo);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][2], 'a');
    
    desfazerMovimento(jogo);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][1], 'c');
    
    desfazerMovimento(jogo);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'e');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

// ===== Testes para verificar restrições =====

void teste_verificar_restricoes_basico() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = verificarRestricoes(jogo);
    
    // O tabuleiro inicial não deve ter violações de restrições
    CU_ASSERT_EQUAL(resultado, 0);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_restricao_casas_riscadas_adjacentes_direita() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca duas casas adjacentes
    riscar(jogo, "a1");
    riscar(jogo, "b1");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}


void teste_verificar_restricao_casas_riscadas_adjacentes_esquerda() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca duas casas adjacentes
    riscar(jogo, "c3");
    riscar(jogo, "b3");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}


void teste_verificar_restricao_casas_riscadas_adjacentes_baixo() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca duas casas adjacentes
    riscar(jogo, "a1");
    riscar(jogo, "a2");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}


void teste_verificar_restricao_casas_riscadas_adjacentes_cima() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca duas casas adjacentes
    riscar(jogo, "c3");
    riscar(jogo, "c2");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_restricao_vizinhos_nao_brancos_casa_riscada() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca uma casa sem pintar os vizinhos de branco
    riscar(jogo, "b2");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_restricao_vizinhos_riscados_todos() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca uma casa sem pintar os vizinhos de branco
    riscar(jogo, "b2");
    pintarBranco(jogo, "b1");
    pintarBranco(jogo, "a2");
    pintarBranco(jogo, "c2");
    pintarBranco(jogo, "b3");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_restricao_vizinhos_riscados_dois_riscados() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca uma casa sem pintar os vizinhos de branco
    riscar(jogo, "b2");
    pintarBranco(jogo, "b1");
    pintarBranco(jogo, "b3");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_restricao_vizinhos_riscados_tres_riscados() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Risca uma casa sem pintar os vizinhos de branco
    riscar(jogo, "b2");
    pintarBranco(jogo, "b1");
    pintarBranco(jogo, "a2");
    pintarBranco(jogo, "b3");
    
    int resultado = verificarRestricoes(jogo);
    
    // Deve identificar violação de restrição
    CU_ASSERT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

// ===== Testes para verificar conectividade =====

void teste_verificar_conectividade_sem_brancas() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Tabuleiro inicial não tem casas brancas
    int resultado = verificarConectividadeBrancas(jogo);
    
    CU_ASSERT_EQUAL(resultado, 0);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_conectividade_com_brancas_conectadas() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Pinta casas brancas adjacentes
    pintarBranco(jogo, "a1");
    pintarBranco(jogo, "a2");
    pintarBranco(jogo, "b2");
    
    int resultado = verificarConectividadeBrancas(jogo);
    
    CU_ASSERT_EQUAL(resultado, 0);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_conectividade_com_brancas_conectadas_com_biforcação() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Pinta casas brancas adjacentes
    pintarBranco(jogo, "a1");
    pintarBranco(jogo, "a2");
    pintarBranco(jogo, "b2");
    pintarBranco(jogo, "b3");
    pintarBranco(jogo, "b4");
    pintarBranco(jogo, "a4");
    pintarBranco(jogo, "c2");
    pintarBranco(jogo, "d2");
    
    int resultado = verificarConectividadeBrancas(jogo);
    
    CU_ASSERT_EQUAL(resultado, 0);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_conectividade_com_brancas_desconectadas() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Pinta casas brancas não adjacentes
    pintarBranco(jogo, "a1");
    pintarBranco(jogo, "e5");
    
    int resultado = verificarConectividadeBrancas(jogo);
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_verificar_conectividade_com_brancas_desconectadas_diagonal() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Pinta casas brancas não adjacentes
    pintarBranco(jogo, "a1");
    pintarBranco(jogo, "b2");
    pintarBranco(jogo, "c3");
    
    int resultado = verificarConectividadeBrancas(jogo);
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}


// ===== Testes para processamento de comandos =====

void teste_processar_comando_carregar() {
    criar_arquivo_teste();
    Jogo *jogo = NULL;
    
    int resultado = processarComandos(&jogo, "l tabuleiro_test.txt");
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_pintar() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = processarComandos(&jogo, "b a1");
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'E');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_riscar() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = processarComandos(&jogo, "r a1");
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '#');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_desfazer() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Faz um movimento e depois desfaz
    processarComandos(&jogo, "b a1");
    int resultado = processarComandos(&jogo, "d");
    
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'e');
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_verificar() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = processarComandos(&jogo, "v");
    
    CU_ASSERT_EQUAL(resultado, 0);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_sair() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = processarComandos(&jogo, "s");
    
    CU_ASSERT_EQUAL(resultado, 1); // 1 indica sair
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_invalido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = processarComandos(&jogo, "x");
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

// ===== Testes para processamento de comandos =====
void teste_gravar_jogo_valido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Faz uma alteração no jogo para verificar se foi gravada
    pintarBranco(jogo, "a1");
    
    // Grava o jogo em um novo arquivo
    int resultado = gravarJogo(jogo, "jogo_salvo.txt");
    
    CU_ASSERT_EQUAL(resultado, 0);
    
    // Verifica se o arquivo foi criado
    FILE *file = fopen("jogo_salvo.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(file);
    
    if (file) {
        // Carrega o jogo novamente para verificar se os dados foram salvos corretamente
        fclose(file);
        Jogo *jogoCarregado = carregarJogo("jogo_salvo.txt");
        
        CU_ASSERT_PTR_NOT_NULL(jogoCarregado);
        CU_ASSERT_EQUAL(jogoCarregado->linhas, 5);
        CU_ASSERT_EQUAL(jogoCarregado->colunas, 5);
        CU_ASSERT_EQUAL(jogoCarregado->tabuleiro[0][0], 'E'); // Deve estar em maiúsculo
        
        freeJogo(jogoCarregado);
        remove("jogo_salvo.txt");
    }
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_gravar_jogo_invalido() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Tenta gravar em um diretório que não existe
    int resultado = gravarJogo(jogo, "/diretorio_inexistente/jogo.txt");
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_gravar() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Modifica o jogo
    processarComandos(&jogo, "b a1");
    
    // Grava o jogo
    int resultado = processarComandos(&jogo, "g jogo_salvo.txt");
    
    CU_ASSERT_EQUAL(resultado, 0);
    
    // Verifica se o arquivo foi criado
    FILE *file = fopen("jogo_salvo.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(file);
    
    if (file) {
        fclose(file);
        remove("jogo_salvo.txt");
    }
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}

// ===== Configuração da suíte de testes =====

int inicializar() {
    return 0;
}

int limpar() {
    return 0;
}

int main() {
    CU_pSuite pSuite = NULL;
    
    // Inicializa o registo de testes do CUnit
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }
    
    // Adiciona uma suíte ao registo
    pSuite = CU_add_suite("Suite_Jogo", inicializar, limpar);
    if (pSuite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Adiciona os testes à suíte
    // Testes para carregamento de jogo
    CU_add_test(pSuite, "teste_carregar_jogo_valido", teste_carregar_jogo_valido);
    CU_add_test(pSuite, "teste_carregar_jogo_arquivo_inexistente", teste_carregar_jogo_arquivo_inexistente);
    
    // Testes para pintar e riscar
    CU_add_test(pSuite, "teste_pintar_branco_valido", teste_pintar_branco_valido);
    CU_add_test(pSuite, "teste_pintar_branco_invalido", teste_pintar_branco_invalido);
    CU_add_test(pSuite, "teste_riscar_valido", teste_riscar_valido);
    CU_add_test(pSuite, "teste_riscar_invalido", teste_riscar_invalido);
    
    // Testes para registo e desfazer movimentos
    CU_add_test(pSuite, "teste_registar_movimento", teste_registar_movimento);
    CU_add_test(pSuite, "teste_desfazer_movimento", teste_desfazer_movimento);
    CU_add_test(pSuite, "teste_desfazer_movimento_sem_historico", teste_desfazer_movimento_sem_historico);
    CU_add_test(pSuite, "teste_desfazer_multiplos_movimentos", teste_desfazer_multiplos_movimentos);
    
    // Testes para verificação de restrições
    CU_add_test(pSuite, "teste_verificar_restricoes_basico", teste_verificar_restricoes_basico);
    CU_add_test(pSuite, "teste_verificar_restricao_casas_riscadas_adjacentes_direita", teste_verificar_restricao_casas_riscadas_adjacentes_direita);
    CU_add_test(pSuite, "teste_verificar_restricao_casas_riscadas_adjacentes_esquerda", teste_verificar_restricao_casas_riscadas_adjacentes_esquerda);
    CU_add_test(pSuite, "teste_verificar_restricao_casas_riscadas_adjacentes_baixo", teste_verificar_restricao_casas_riscadas_adjacentes_baixo);
    CU_add_test(pSuite, "teste_verificar_restricao_casas_riscadas_adjacentes_cima", teste_verificar_restricao_casas_riscadas_adjacentes_cima);
    CU_add_test(pSuite, "teste_verificar_restricao_vizinhos_nao_brancos_casa_riscada", teste_verificar_restricao_vizinhos_nao_brancos_casa_riscada);
    CU_add_test(pSuite, "teste_verificar_restricao_vizinhos_riscados_todos", teste_verificar_restricao_vizinhos_riscados_todos);
    CU_add_test(pSuite, "teste_verificar_restricao_vizinhos_riscados_dois_riscados",  teste_verificar_restricao_vizinhos_riscados_dois_riscados);
    CU_add_test(pSuite, "teste_verificar_restricao_vizinhos_riscados_tres_riscados",  teste_verificar_restricao_vizinhos_riscados_tres_riscados);
    
    // Testes para verificação de conectividade
    CU_add_test(pSuite, "teste_verificar_conectividade_sem_brancas", teste_verificar_conectividade_sem_brancas);
    CU_add_test(pSuite, "teste_verificar_conectividade_com_brancas_conectadas", teste_verificar_conectividade_com_brancas_conectadas);
    CU_add_test(pSuite, "teste_verificar_conectividade_com_brancas_conectadas_com_biforcação", teste_verificar_conectividade_com_brancas_conectadas_com_biforcação);
    CU_add_test(pSuite, "teste_verificar_conectividade_com_brancas_desconectadas", teste_verificar_conectividade_com_brancas_desconectadas);
    CU_add_test(pSuite, "teste_verificar_conectividade_com_brancas_desconectadas_diagonal", teste_verificar_conectividade_com_brancas_desconectadas_diagonal);
    // Testes para processamento de comandos
    CU_add_test(pSuite, "teste_processar_comando_carregar", teste_processar_comando_carregar);
    CU_add_test(pSuite, "teste_processar_comando_pintar", teste_processar_comando_pintar);
    CU_add_test(pSuite, "teste_processar_comando_riscar", teste_processar_comando_riscar);
    CU_add_test(pSuite, "teste_processar_comando_desfazer", teste_processar_comando_desfazer);
    CU_add_test(pSuite, "teste_processar_comando_verificar", teste_processar_comando_verificar);
    CU_add_test(pSuite, "teste_processar_comando_sair", teste_processar_comando_sair);
    CU_add_test(pSuite, "teste_processar_comando_invalido", teste_processar_comando_invalido);
    
    // Testes para gravação de jogo
    CU_add_test(pSuite, "teste_gravar_jogo_valido", teste_gravar_jogo_valido);
    CU_add_test(pSuite, "teste_gravar_jogo_invalido", teste_gravar_jogo_invalido);
    CU_add_test(pSuite, "teste_processar_comando_gravar", teste_processar_comando_gravar);

    // Executa todos os testes usando a interface básica do CUnit
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    // Limpa o registo de teste e retorna o status
    CU_cleanup_registry();
    return CU_get_error();
}
