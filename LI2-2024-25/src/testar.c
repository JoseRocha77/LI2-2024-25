#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "../include/jogo.h"

// Variáveis globais para os testes
Jogo *jogo = NULL;
FILE *temp_file = NULL;

// Função para criar um arquivo de teste temporário
void criar_arquivo_teste() {
    temp_file = fopen("temp_test.txt", "w");
    if (temp_file) {
        // Cria um tabuleiro 3x3 para testes
        fprintf(temp_file, "3 3\n");
        fprintf(temp_file, "...\n");
        fprintf(temp_file, ".o.\n");
        fprintf(temp_file, "...\n");
        fclose(temp_file);
    }
}

// Configuração inicial antes de todos os testes
int inicializar() {
    criar_arquivo_teste();
    return 0;
}

// Limpeza após todos os testes
int finalizar() {
    remove("temp_test.txt");
    return 0;
}

// Configuração antes de cada teste
void preparar_teste() {
    // Carrega um jogo novo para cada teste
    jogo = carregarJogo("temp_test.txt");
}

// Limpeza após cada teste
void limpar_teste() {
    if (jogo) {
        for (int i = 0; i < jogo->linhas; i++) {
            free(jogo->tabuleiro[i]);
        }
        free(jogo->tabuleiro);
        free(jogo);
        jogo = NULL;
    }
}

// Testes para a função carregarJogo
void testar_CarregaJogo() {
    Jogo *jogo_teste = carregarJogo("temp_test.txt");
    
    // Verifica se o jogo foi carregado corretamente
    CU_ASSERT_PTR_NOT_NULL(jogo_teste);
    
    if (jogo_teste) {
        // Verifica dimensões do tabuleiro
        CU_ASSERT_EQUAL(jogo_teste->linhas, 3);
        CU_ASSERT_EQUAL(jogo_teste->colunas, 3);
        
        // Verifica se o tabuleiro foi alocado
        CU_ASSERT_PTR_NOT_NULL(jogo_teste->tabuleiro);
        
        // Verifica conteúdo do tabuleiro
        if (jogo_teste->tabuleiro) {
            CU_ASSERT_EQUAL(jogo_teste->tabuleiro[0][0], '.');
            CU_ASSERT_EQUAL(jogo_teste->tabuleiro[1][1], 'o');
            CU_ASSERT_EQUAL(jogo_teste->tabuleiro[2][2], '.');
        }
        
        // Limpa memória alocada para este teste específico
        for (int i = 0; i < jogo_teste->linhas; i++) {
            free(jogo_teste->tabuleiro[i]);
        }
        free(jogo_teste->tabuleiro);
        free(jogo_teste);
    }
    
    // Teste com arquivo inexistente
    Jogo *jogo_invalido = carregarJogo("arquivo_inexistente.txt");
    CU_ASSERT_PTR_NULL(jogo_invalido);
}

// Teste para desenhaJogo - verifica que não causa falha de segmentação
void testar_desenhaJogo() {
    // Redirecionamos stdout para um arquivo temporário para não poluir a saída de teste
    FILE *stdout_original = stdout;
    FILE *temp_output = fopen("temp_output.txt", "w");
    stdout = temp_output;
    
    // Executa a função (verificamos apenas que não causa crash)
    desenhaJogo(jogo);
    
    // Restaura stdout
    fclose(temp_output);
    stdout = stdout_original;
    
    // Verifica se o arquivo de saída foi criado
    FILE *verify = fopen("temp_output.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(verify);
    if (verify) {
        fclose(verify);
        remove("temp_output.txt");
    }
}

// Teste para pintarBranco
void testar_pintarBranco() {
    char coordenada[] = "b2";  // Corresponde à posição (1,1) no array
    
    // Verifica que a posição tem 'o' antes de pintá-la
    CU_ASSERT_EQUAL(jogo->tabuleiro[1][1], 'o');
    
    // Pinta de branco
    int resultado = pintarBranco(jogo, coordenada);
    
    // Verifica que a função retornou sucesso
    CU_ASSERT_EQUAL(resultado, 0);
    
    // Verifica que a posição foi alterada corretamente (remove 32 do valor ASCII)
    CU_ASSERT_EQUAL(jogo->tabuleiro[1][1], 'O');  // 'o' - ' ' = 'O'
    
    // Testa outra posição
    char coordenada2[] = "a1";
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '.');
    pintarBranco(jogo, coordenada2);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '.');  // '.' - ' ' não muda o caractere
}

// Teste para riscar
void testar_riscar() {
    char coordenada[] = "c3";  // Corresponde à posição (2,2) no array
    
    // Verifica o estado inicial
    CU_ASSERT_EQUAL(jogo->tabuleiro[2][2], '.');
    
    // Executa riscar
    int resultado = riscar(jogo, coordenada);
    
    // Verifica que a função retornou sucesso
    CU_ASSERT_EQUAL(resultado, 0);
    
    // Verifica que a posição foi alterada para '#'
    CU_ASSERT_EQUAL(jogo->tabuleiro[2][2], '#');
}

// Teste para processarComandos
void testar_processarComandos() {
    // Teste de comando de pintar
    int resultado = processarComandos(jogo, "b a1");
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[0][0], '.');  // '.' - ' ' não muda
    
    // Teste de comando de riscar
    resultado = processarComandos(jogo, "r c3");
    CU_ASSERT_EQUAL(resultado, 0);
    CU_ASSERT_EQUAL(jogo->tabuleiro[2][2], '#');
    
    // Teste de comando de sair
    resultado = processarComandos(jogo, "s");
    CU_ASSERT_EQUAL(resultado, 1);
    
    // Teste de comando inválido
    resultado = processarComandos(jogo, "x a1");
    CU_ASSERT_EQUAL(resultado, -1);
    
    // Teste com ponteiro nulo
    resultado = processarComandos(NULL, "b a1");
    CU_ASSERT_EQUAL(resultado, -1);
    
    resultado = processarComandos(jogo, NULL);
    CU_ASSERT_EQUAL(resultado, -1);
}

int main() {
    // Inicializa a suíte de testes CUnit
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }
    
    // Adiciona a suíte ao registro
    CU_pSuite suite = CU_add_suite("Suite de testes do jogo", inicializar, finalizar);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Adiciona os testes à suíte
    if (
        (CU_add_test(suite, "Teste de carregarJogo", testar_CarregaJogo) == NULL) ||
        (CU_add_test(suite, "Teste de desenhaJogo", testar_desenhaJogo) == NULL) ||
        (CU_add_test(suite, "Teste de pintarBranco", testar_pintarBranco) == NULL) ||
        (CU_add_test(suite, "Teste de riscar", testar_riscar) == NULL) ||
        (CU_add_test(suite, "Teste de processarComandos", testar_processarComandos) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Configura os callbacks para preparar e limpar cada teste
    CU_set_test_active(NULL, CU_TRUE);
    CU_set_test_start_handler(preparar_teste);
    CU_set_test_complete_handler(limpar_teste);
    
    // Executa os testes no modo básico
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    // Limpeza final
    CU_cleanup_registry();
    return CU_get_error();
}