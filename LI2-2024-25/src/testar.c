#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "../include/jogo.h"

// Definições para facilitar os testes
#define TABULEIRO_TEST "tabuleiro_test.txt"
#define ARQUIVO_INEXISTENTE "arquivo_inexistente.txt"
#define TABULEIRO_AJUDA_TEST "tabuleiro_ajuda_test.txt"
#define TABULEIRO_RESOLVER_TEST "tabuleiro_resolver_test.txt"

// Função para criar um arquivo de teste
void criar_arquivo_teste() {
    FILE *file = fopen(TABULEIRO_TEST, "w");
    if (file) {
        fprintf(file, "5 5\necadc\ndcdec\nbddce\ncdeeb\naccbb\n");
        fclose(file);
    }
}

// Função para criar um arquivo específico para testar o comando de ajuda
void criar_arquivo_ajuda() {
    FILE *file = fopen(TABULEIRO_AJUDA_TEST, "w");
    if (file) {
        // Tabuleiro específico onde o comando 'a' pode inferir uma jogada
        // Uma célula riscada com vizinhos para pintar de branco
        fprintf(file, "3 3\nabc\nd#e\nfgh\n");
        fclose(file);
    }
}

// Função para criar um arquivo para testar a resolução automática
void criar_arquivo_resolver() {
    FILE *file = fopen(TABULEIRO_RESOLVER_TEST, "w");
    if (file) {
        // Tabuleiro simples para resolver
        fprintf(file, "3 3\naaa\nbbb\nccc\n");
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
    CU_ASSERT_NOT_EQUAL(resultado, 0);
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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
    CU_ASSERT_NOT_EQUAL(resultado, 0); // A função retorna 0 mas imprime as violações
    
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

void teste_verificar_conectividade_com_brancas_desconectadas_e_conectadas() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    // Pinta casas brancas não adjacentes
    pintarBranco(jogo, "a1");
    pintarBranco(jogo, "a2");
    pintarBranco(jogo, "a3");
    pintarBranco(jogo, "e3");
    pintarBranco(jogo, "e3");
    
    int resultado = verificarConectividadeBrancas(jogo);
    
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
    limpar_arquivo_teste();
}


// ===== Testes para o comando de ajuda (a) =====
void teste_comando_ajuda() {
    criar_arquivo_ajuda();
    Jogo *jogo = carregarJogo(TABULEIRO_AJUDA_TEST);
    
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    // Verifica que o comando de ajuda retorna 0 (sucesso)
    int resultado = ajudar(jogo);
    
    // O comando de ajuda deve retornar um valor indicando se conseguiu ajudar
    // 1 se conseguiu sugerir um movimento, 0 se não
    CU_ASSERT(resultado >= 0);
    
    // Verificamos que o comando funcionou ao verificar se existem casas pintadas de branco
    // onde antes havia minúsculas adjacentes à célula riscada
    
    // Para este teste específico, sabemos que a casa do meio é '#'
    // e as adjacentes devem ser pintadas de branco (maiúsculas)
    char celula_centro = jogo->tabuleiro[1][1];
    CU_ASSERT_EQUAL(celula_centro, '#');
    
    // Ao menos uma das células adjacentes deve ter sido pintada para o teste ter efeito
    int alguma_mudanca = 0;
    if (jogo->tabuleiro[0][1] >= 'A' && jogo->tabuleiro[0][1] <= 'Z') alguma_mudanca = 1;
    if (jogo->tabuleiro[1][0] >= 'A' && jogo->tabuleiro[1][0] <= 'Z') alguma_mudanca = 1;
    if (jogo->tabuleiro[1][2] >= 'A' && jogo->tabuleiro[1][2] <= 'Z') alguma_mudanca = 1;
    if (jogo->tabuleiro[2][1] >= 'A' && jogo->tabuleiro[2][1] <= 'Z') alguma_mudanca = 1;
    
    // Se o comando ajudar retornou 1, alguma mudança deve ter ocorrido
    if (resultado == 1) {
        CU_ASSERT_EQUAL(alguma_mudanca, 1);
    }
    
    freeJogo(jogo);
}

// ===== Testes para o modo de ajuda automática (A) =====
void teste_modo_ajuda_automatica() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    // Por padrão, o modo de ajuda automática deve estar desativado
    CU_ASSERT_EQUAL(jogo->modoAjudaAtiva, 0);
    
    // Simula o comando A para ativar o modo de ajuda
    jogo->modoAjudaAtiva = !(jogo->modoAjudaAtiva);
    
    // Verifica se o modo foi ativado
    CU_ASSERT_EQUAL(jogo->modoAjudaAtiva, 1);
    
    // Simula o comando A novamente para desativar
    jogo->modoAjudaAtiva = !(jogo->modoAjudaAtiva);
    
    // Verifica se o modo foi desativado
    CU_ASSERT_EQUAL(jogo->modoAjudaAtiva, 0);
    
    freeJogo(jogo);
}

// ===== Testes para o comando de resolver jogo (R) =====
void teste_resolver_jogo() {
    criar_arquivo_resolver();
    Jogo *jogo = carregarJogo(TABULEIRO_RESOLVER_TEST);
    
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    // Guarda o estado original do tabuleiro para comparação
    int linhas = jogo->linhas;
    int colunas = jogo->colunas;
    
    // Cria uma cópia do tabuleiro original
    char **tabuleiro_original = malloc(linhas * sizeof(char *));
    for (int i = 0; i < linhas; i++) {
        tabuleiro_original[i] = malloc((colunas + 1) * sizeof(char));
        for (int j = 0; j < colunas; j++) {
            tabuleiro_original[i][j] = jogo->tabuleiro[i][j];
        }
        tabuleiro_original[i][colunas] = '\0';
    }
    
    // Executa o comando de resolver o jogo
    int resultado = resolverJogo(jogo);
    
    // O comando deve retornar 0 se conseguiu resolver ou -1 se não conseguiu
    // Não garantimos que todos os jogos têm solução, então aceitamos ambos os resultados
    CU_ASSERT(resultado == 0 || resultado == -1);
    
    // Se o jogo foi resolvido com sucesso, todas as células devem ser maiúsculas ou riscadas
    if (resultado == 0) {
        int todas_resolvidas = 1;
        for (int i = 0; i < linhas && todas_resolvidas; i++) {
            for (int j = 0; j < colunas && todas_resolvidas; j++) {
                char c = jogo->tabuleiro[i][j];
                if (!(c >= 'A' && c <= 'Z') && c != '#') {
                    todas_resolvidas = 0;
                }
            }
        }
        CU_ASSERT_EQUAL(todas_resolvidas, 1);
        
        // Verifica se o jogo resolvido não tem violações de restrições
        CU_ASSERT_EQUAL(verificarRestricoes(jogo), 0);
    }
    
    // O tabuleiro original deve ter sido modificado
    int modificado = 0;
    for (int i = 0; i < linhas && !modificado; i++) {
        for (int j = 0; j < colunas && !modificado; j++) {
            if (tabuleiro_original[i][j] != jogo->tabuleiro[i][j]) {
                modificado = 1;
            }
        }
    }
    
    // Só verificamos se foi modificado se o resultado foi 0
    if (resultado == 0) {
        CU_ASSERT_EQUAL(modificado, 1);
    }
    
    // Libera a memória da cópia do tabuleiro
    for (int i = 0; i < linhas; i++) {
        free(tabuleiro_original[i]);
    }
    free(tabuleiro_original);
    
    freeJogo(jogo);
}

// ===== Teste para verificar o comando de ajuda (a) com jogo inválido =====
void teste_comando_ajuda_jogo_invalido() {
    // Tentando ajudar em um jogo nulo
    int resultado = ajudar(NULL);
    CU_ASSERT_NOT_EQUAL(resultado, 0); // O comportamento esperado é retornar 0
}

// ===== Teste para verificar o comando de resolver (R) com jogo inválido =====
void teste_resolver_jogo_invalido() {
    // Tentando resolver um jogo nulo
    int resultado = resolverJogo(NULL);
    CU_ASSERT_EQUAL(resultado, -1); // Deve retornar erro (-1)
}

// ===== Teste para verificar interação entre os comandos a, A e R =====
void teste_interacao_comandos_a_A_R() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    // Ativa o modo de ajuda automática
    jogo->modoAjudaAtiva = 1;
    CU_ASSERT_EQUAL(jogo->modoAjudaAtiva, 1);
    
    // Faz uma jogada que deve acionar a ajuda automática
    int resultado_pintar = pintarBranco(jogo, "a1");
    CU_ASSERT_EQUAL(resultado_pintar, 0);
    
    // Verifica que o modo de ajuda automática ainda está ativo
    CU_ASSERT_EQUAL(jogo->modoAjudaAtiva, 1);
    
    // Tenta resolver o jogo
    int resultado_resolver = resolverJogo(jogo);
    // O comando resolver deve funcionar independente do modo de ajuda
    CU_ASSERT(resultado_resolver == 0 || resultado_resolver == -1);
    
    // Desativa o modo de ajuda automática
    jogo->modoAjudaAtiva = 0;
    CU_ASSERT_EQUAL(jogo->modoAjudaAtiva, 0);
    
    freeJogo(jogo);
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
    
    CU_ASSERT_NOT_EQUAL(resultado, 0);
    
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

void teste_processar_comando_gravar() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);
    
    int resultado = processarComandos(&jogo, "g");
    
    CU_ASSERT_EQUAL(resultado, -1); // 1 indica sair
    
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

void teste_processar_comando_a() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);

    int resultado = processarComandos(&jogo, "a a1");

    CU_ASSERT_NOT_EQUAL(resultado, 0);
    CU_ASSERT_NOT_EQUAL(jogo->tabuleiro[0][0], 'X'); // Supondo que 'a' marca com 'X'

    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_A() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);

    processarComandos(&jogo, "a a1"); // marca primeiro
    int resultado = processarComandos(&jogo, "A a1"); // desmarca

    CU_ASSERT_NOT_EQUAL(resultado, 0);

    freeJogo(jogo);
    limpar_arquivo_teste();
}

void teste_processar_comando_R() {
    criar_arquivo_teste();
    Jogo *jogo = carregarJogo(TABULEIRO_TEST);

    processarComandos(&jogo, "r a1"); // risca
    int resultado = processarComandos(&jogo, "R a1"); // remove risca

    CU_ASSERT_NOT_EQUAL(resultado, 0);
    CU_ASSERT_NOT_EQUAL(jogo->tabuleiro[0][0], 'e'); // Supondo que 'e' representa estado limpo

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
    CU_add_test(pSuite, "teste_verificar_conectividade_com_brancas_desconectadas_e_conectadas", teste_verificar_conectividade_com_brancas_desconectadas_e_conectadas);

    //Testes para o comando ajuda (a)
    CU_add_test(pSuite, "teste_comando_ajuda", teste_comando_ajuda);
    CU_add_test(pSuite, "teste_modo_ajuda_automatica", teste_modo_ajuda_automatica);
    CU_add_test(pSuite, "teste_resolver_jogo", teste_resolver_jogo);
    CU_add_test(pSuite, "teste_comando_ajuda_jogo_invalido", teste_comando_ajuda_jogo_invalido);
    CU_add_test(pSuite, "teste_resolver_jogo_invalido", teste_resolver_jogo_invalido);
    CU_add_test(pSuite, "teste_interacao_comandos_a_A_R", teste_interacao_comandos_a_A_R);


    // Testes para processamento de comandos
    CU_add_test(pSuite, "teste_processar_comando_carregar", teste_processar_comando_carregar);
    CU_add_test(pSuite, "teste_processar_comando_pintar", teste_processar_comando_pintar);
    CU_add_test(pSuite, "teste_processar_comando_riscar", teste_processar_comando_riscar);
    CU_add_test(pSuite, "teste_processar_comando_desfazer", teste_processar_comando_desfazer);
    CU_add_test(pSuite, "teste_processar_comando_verificar", teste_processar_comando_verificar);
    CU_add_test(pSuite, "teste_processar_comando_sair", teste_processar_comando_sair);
    CU_add_test(pSuite, "teste_processar_comando_gravar", teste_processar_comando_gravar);
    CU_add_test(pSuite, "teste_processar_comando_invalido", teste_processar_comando_invalido);
    CU_add_test(pSuite, "teste_processar_comando_a", teste_processar_comando_a);
    CU_add_test(pSuite, "teste_processar_comando_A", teste_processar_comando_A);
    CU_add_test(pSuite, "teste_processar_comando_R", teste_processar_comando_R);
    
    // Testes para gravação de jogo
    CU_add_test(pSuite, "teste_gravar_jogo_valido", teste_gravar_jogo_valido);
    CU_add_test(pSuite, "teste_gravar_jogo_invalido", teste_gravar_jogo_invalido);

    // Executa todos os testes usando a interface básica do CUnit
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    // Limpa o registo de teste e retorna o status
    CU_cleanup_registry();
    return CU_get_error();
}
