#include <stdio.h>
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
    // Verifica se o jogo existe antes de testar
    CU_ASSERT_PTR_NOT_NULL(jogo);
    
    if (jogo != NULL && jogo->tabuleiro != NULL) {
        // Guarda o valor original
        char valorOriginal = jogo->tabuleiro[1][1]; // Célula central (b2)
        
        // Testa comando de pintar
        int resultado = processarComandos(jogo, "b b2");
        CU_ASSERT_EQUAL(resultado, 0);
        CU_ASSERT_EQUAL(jogo->tabuleiro[1][1], valorOriginal - ' ');
        
        // Testa comando de riscar
        resultado = processarComandos(jogo, "r a3");
        CU_ASSERT_EQUAL(resultado, 0);
        CU_ASSERT_EQUAL(jogo->tabuleiro[2][0], '#');
        
        // Testa comando inválido
        resultado = processarComandos(jogo, "x a1");
        CU_ASSERT_EQUAL(resultado, -1);
        
        // Testa comando de sair
        resultado = processarComandos(jogo, "s");
        CU_ASSERT_EQUAL(resultado, 1);
    }
}

int main() {
    // Inicializa o registro do CUnit
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Adiciona a suite ao registro
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
    
    // Limpa o registro do CUnit
    CU_cleanup_registry();
    
    return CU_get_error();
}
