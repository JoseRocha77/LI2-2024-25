/*#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "../include/jogo.h"

// Variável global para usar nos testes
Jogo *jogo = NULL;

// Criar um arquivo de teste temporário simples
void criar_arquivo_teste() {
    FILE *f = fopen("teste_jogo.txt", "w");
    if (f != NULL) {
        fprintf(f, "3 3\n");
        fprintf(f, "...\n");
        fprintf(f, ".o.\n");
        fprintf(f, "...\n");
        fclose(f);
    } else {
        printf("Erro ao criar arquivo de teste temporário\n");
    }
}

// Inicialização da suite de testes
int init_suite() {
    criar_arquivo_teste();
    return 0;
}

// Limpeza da suite de testes
int clean_suite() {
    if (jogo != NULL) {
        if (jogo->tabuleiro != NULL) {
            for (int i = 0; i < jogo->linhas; i++) {
                if (jogo->tabuleiro[i] != NULL) {
                    free(jogo->tabuleiro[i]);
                }
            }
            free(jogo->tabuleiro);
        }
        free(jogo);
        jogo = NULL;
    }
    remove("teste_jogo.txt");
    return 0;
}

// Teste para carregarJogo
void teste_carregarJogo() {
    // Primeiro limpa qualquer jogo existente
    if (jogo != NULL) {
        if (jogo->tabuleiro != NULL) {
            for (int i = 0; i < jogo->linhas; i++) {
                if (jogo->tabuleiro[i] != NULL) {
                    free(jogo->tabuleiro[i]);
                }
            }
            free(jogo->tabuleiro);
        }
        free(jogo);
        jogo = NULL;
    }
    
    // Tenta carregar um arquivo que não existe
    Jogo *jogoInexistente = carregarJogo("arquivo_inexistente.txt");
    CU_ASSERT_PTR_NULL(jogoInexistente);
    
    // Carrega o jogo de teste
    jogo = carregarJogo("teste_jogo.txt");
    
    // Verifica se o jogo foi carregado
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    // Se o jogo foi carregado, verifica suas propriedades
    if (jogo != NULL) {
        CU_ASSERT_EQUAL(jogo->linhas, 3);
        CU_ASSERT_EQUAL(jogo->colunas, 3);
        CU_ASSERT_PTR_NOT_NULL(jogo->tabuleiro);
    }
}

// Teste para desenhaJogo
void teste_desenhaJogo() {
    // Verifica se o jogo existe antes de desenhar
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    if (jogo != NULL) {
        // Como desenhaJogo apenas imprime no stdout, não há muito o que testar além
        // de garantir que não quebra quando chamado
        desenhaJogo(jogo);
        // Se chegou aqui, passou no teste
        CU_PASS("desenhaJogo executou sem erros");
    }
}

// Teste para pintarBranco
void teste_pintarBranco() {
    // Verifica se o jogo existe antes de testar
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    if (jogo != NULL && jogo->tabuleiro != NULL) {
        // Guarda o valor original
        char valorOriginal = jogo->tabuleiro[1][1]; // Célula central (b2)
        
        // Testa a função pintarBranco
        char coordenada[] = "b2";  // Posição central com 'o'
        int resultado = pintarBranco(jogo, coordenada);
        
        // Verifica o resultado
        CU_ASSERT_EQUAL(resultado, 0);
        
        // 'o' deve se tornar 'O' (maiúsculo) pela lógica da função pintarBranco
        // que subtrai o valor de um espaço ' ' (ASCII 32)
        CU_ASSERT_EQUAL(jogo->tabuleiro[1][1], valorOriginal - ' ');
    }
}

// Teste para riscar
void teste_riscar() {
    // Verifica se o jogo existe antes de testar
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    if (jogo != NULL && jogo->tabuleiro != NULL) {
        // Testa a função riscar
        char coordenada[] = "a1";  // Posição superior esquerda
        int resultado = riscar(jogo, coordenada);
        
        // Verifica o resultado
        CU_ASSERT_EQUAL(resultado, 0);
        CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '#');
    }
}

// Teste para processarComandos
void teste_processarComandos() {
    // Teste de comando de pintar
    int resultado = processarComandos(&jogo, "b b2");  // Agora passando &jogo
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[1][1], 'O');  // 'o' - ' ' = 'O'
    
    // Teste de comando de riscar
    resultado = processarComandos(&jogo, "r c3");  // Agora passando &jogo
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[2][2], '#');
    
    // Teste de comando inválido
    resultado = processarComandos(&jogo, "x a1");  // Agora passando &jogo
    CU_ASSERT_EQUAL(resultado, -1);
    
    // Teste de comando de sair
    resultado = processarComandos(&jogo, "s");  // Agora passando &jogo
    CU_ASSERT_EQUAL(resultado, 1);
    
    // Teste com ponteiro nulo
    resultado = processarComandos(NULL, "b a1");
    CU_ASSERT_EQUAL(resultado, -1);
    
    resultado = processarComandos(&jogo, NULL);
    CU_ASSERT_EQUAL(resultado, -1);
}

int main() {
    // Inicializa o registo do CUnit
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Adiciona a suite ao registo
    CU_pSuite suite = CU_add_suite("Suite de testes do jogo", init_suite, clean_suite);
    if (NULL == suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Adiciona os testes à suite
    if ((NULL == CU_add_test(suite, "Teste de carregarJogo", teste_carregarJogo)) ||
        (NULL == CU_add_test(suite, "Teste de desenhaJogo", teste_desenhaJogo)) ||
        (NULL == CU_add_test(suite, "Teste de pintarBranco", teste_pintarBranco)) ||
        (NULL == CU_add_test(suite, "Teste de riscar", teste_riscar)) ||
        (NULL == CU_add_test(suite, "Teste de processarComandos", teste_processarComandos))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Executa todos os testes
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    // Limpa o registo do CUnit
    CU_cleanup_registry();
    
    return CU_get_error();
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "../include/jogo.h"

// Função auxiliar para criar um jogo de teste
Jogo* criarJogoTeste() {
    Jogo *jogo = malloc(sizeof(Jogo));
    jogo->linhas = 3;
    jogo->colunas = 3;
    jogo->historicoMovimentos = NULL;
    
    jogo->tabuleiro = malloc(jogo->linhas * sizeof(char*));
    for (int i = 0; i < jogo->linhas; i++) {
        jogo->tabuleiro[i] = malloc((jogo->colunas + 1) * sizeof(char));
        for (int j = 0; j < jogo->colunas; j++) {
            jogo->tabuleiro[i][j] = 'a' + i;
        }
        jogo->tabuleiro[i][jogo->colunas] = '\0';
    }
    
    return jogo;
}

// Teste para a função desfazerMovimento
void testDesfazerMovimento() {
    Jogo *jogo = criarJogoTeste();
    char coordenada[] = "a1";
    
    // Estado inicial
    char estadoInicial = jogo->tabuleiro[0][0];
    
    // Riscar uma casa
    riscar(jogo, coordenada);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '#');
    
    // Desfazer o movimento
    desfazerMovimento(jogo);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], estadoInicial);
    
    // Verificar que não há mais movimentos para desfazer
    CU_ASSERT_EQUAL(jogo->historicoMovimentos, NULL);
    
    // Testar desfazer sem movimentos
    int resultado = desfazerMovimento(jogo);
    CU_ASSERT_EQUAL(resultado, -1);
    
    freeJogo(jogo);
}

// Teste para a função verificarRestricoes com casas riscadas adjacentes
void testVerificarRestricoesRiscadasAdjacentes() {
    Jogo *jogo = criarJogoTeste();
    
    // Riscar duas casas adjacentes
    char coord1[] = "a1";
    char coord2[] = "a2";
    riscar(jogo, coord1);
    riscar(jogo, coord2);
    
    // Deve encontrar uma violação
    int violacoes = verificarRestricoes(jogo);
    CU_ASSERT_TRUE(violacoes > 0);
    
    freeJogo(jogo);
}

// Teste para a função verificarRestricoes com vizinhos não pintados de branco
void testVerificarRestricoesVizinhosNaoBrancos() {
    Jogo *jogo = criarJogoTeste();
    
    // Riscar uma casa no meio
    char coord[] = "b2";
    riscar(jogo, coord);
    
    // Deve encontrar violações (vizinhos não pintados de branco)
    int violacoes = verificarRestricoes(jogo);
    CU_ASSERT_TRUE(violacoes > 0);
    
    // Agora vamos pintar todos os vizinhos de branco
    char vizinho1[] = "a2";
    char vizinho2[] = "b1";
    char vizinho3[] = "b3";
    char vizinho4[] = "c2";
    pintarBranco(jogo, vizinho1);
    pintarBranco(jogo, vizinho2);
    pintarBranco(jogo, vizinho3);
    pintarBranco(jogo, vizinho4);
    
    // Não deve encontrar violações
    violacoes = verificarRestricoes(jogo);
    CU_ASSERT_EQUAL(violacoes, 0);
    
    freeJogo(jogo);
}

// Teste para a função verificarRestricoes sem violações
void testVerificarRestricoesNenhumaViolacao() {
    Jogo *jogo = criarJogoTeste();
    
    // Riscar uma casa
    char coord[] = "a1";
    riscar(jogo, coord);
    
    // Pintar todos os vizinhos de branco
    char vizinho1[] = "a2";
    char vizinho2[] = "b1";
    pintarBranco(jogo, vizinho1);
    pintarBranco(jogo, vizinho2);
    
    // Não deve encontrar violações
    int violacoes = verificarRestricoes(jogo);
    CU_ASSERT_EQUAL(violacoes, 0);
    
    freeJogo(jogo);
}

// Teste para a função registarMovimento
void testRegist§arMovimento() {
    Jogo *jogo = criarJogoTeste();
    
    // Estado inicial do histórico
    CU_ASSERT_PTR_NULL(jogo->historicoMovimentos);
    
    // Riscar uma casa
    char coord[] = "a1";
    char estadoInicial = jogo->tabuleiro[0][0];
    riscar(jogo, coord);
    
    // Verificar se o movimento foi registado corretamente
    CU_ASSERT_PTR_NOT_NULL(jogo->historicoMovimentos);
    CU_ASSERT_EQUAL(jogo->historicoMovimentos->linha, 0);
    CU_ASSERT_EQUAL(jogo->historicoMovimentos->coluna, 0);
    CU_ASSERT_EQUAL(jogo->historicoMovimentos->estadoAnterior, estadoInicial);
    
    freeJogo(jogo);
}

// Teste para verificar a liberação correta da memória do histórico
void testLiberarHistoricoMovimentos() {
    Jogo *jogo = criarJogoTeste();
    
    // Fazer alguns movimentos
    char coord1[] = "a1";
    char coord2[] = "a2";
    riscar(jogo, coord1);
    riscar(jogo, coord2);
    
    // Verificar se o histórico não é nulo
    CU_ASSERT_PTR_NOT_NULL(jogo->historicoMovimentos);
    
    // Liberar o jogo deve liberar também o histórico
    freeJogo(jogo);
    
    // Não podemos testar diretamente se a memória foi liberada,
    // mas pelo menos garantimos que a função foi chamada sem erros
}

// Teste para processarComandos - comando "d"
void testProcessarComandosDesfazer() {
    Jogo *jogo = criarJogoTeste();
    Jogo **jogoPtr = &jogo;
    
    // Riscar uma casa
    char comando1[] = "r a1";
    processarComandos(jogoPtr, comando1);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '#');
    
    // Desfazer o movimento
    char comando2[] = "d";
    processarComandos(jogoPtr, comando2);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], 'a');
    
    freeJogo(jogo);
}

// Teste para processarComandos - comando "v"
void testProcessarComandosVerificar() {
    Jogo *jogo = criarJogoTeste();
    Jogo **jogoPtr = &jogo;
    
    // Riscar duas casas adjacentes
    char comando1[] = "r a1";
    char comando2[] = "r a2";
    processarComandos(jogoPtr, comando1);
    processarComandos(jogoPtr, comando2);
    
    // Verificar restrições
    char comando3[] = "v";
    int resultado = processarComandos(jogoPtr, comando3);
    CU_ASSERT_NOT_EQUAL(resultado, -1); // O comando deve ser aceito
    
    freeJogo(jogo);
}

int main() {
    CU_initialize_registry();
    
    CU_pSuite suite = CU_add_suite("Suite_Jogo", NULL, NULL);
    
    CU_add_test(suite, "testDesfazerMovimento", testDesfazerMovimento);
    CU_add_test(suite, "testVerificarRestricoesRiscadasAdjacentes", testVerificarRestricoesRiscadasAdjacentes);
    CU_add_test(suite, "testVerificarRestricoesVizinhosNaoBrancos", testVerificarRestricoesVizinhosNaoBrancos);
    CU_add_test(suite, "testVerificarRestricoesNenhumaViolacao", testVerificarRestricoesNenhumaViolacao);
    CU_add_test(suite, "testRegistarMovimento", testRegistarMovimento);
    CU_add_test(suite, "testLiberarHistoricoMovimentos", testLiberarHistoricoMovimentos);
    CU_add_test(suite, "testProcessarComandosDesfazer", testProcessarComandosDesfazer);
    CU_add_test(suite, "testProcessarComandosVerificar", testProcessarComandosVerificar);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    int failures = CU_get_number_of_failures();
    
    CU_cleanup_registry();
    
    return failures;
}