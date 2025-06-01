#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/jogo.h"

// Função para exibir o menu inicial
void exibirMenuInicial(void) {
    printf("\n====================Bem-vindo ao jogo Puzzle Hitori!====================\n");
    printf("\nUse 'l <nome_arquivo>' para carregar um jogo.\n");
    printf("Comandos disponíveis:\n");
    printf("  l <arquivo.txt>   - Carregar jogo\n");
    printf("  g <arquivo.txt>   - Gravar jogo\n");
    printf("  b <posicao>       - Pintar de branco\n");
    printf("  r <posicao>       - Riscar\n");
    printf("  d                 - Desfazer último movimento\n");
    printf("  v                 - Verificar restrições\n");
    printf("  a                 - Ajudar (inferir próximos movimentos)\n");
    printf("  A                 - Ativar modo de ajuda automático\n");
    printf("  R                 - Resolver jogo automaticamente\n");
    printf("  s                 - Sair do jogo\n");
}

// Função para exibir a mensagem de vitória e aguardar ENTER
void exibirMensagemVitoria(void) {
    printf("\n\n");
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                              ║\n");
    printf("║         ██╗   ██╗██╗████████╗ ██████╗ ██████╗ ██╗ █████╗ ██╗██╗██╗           ║\n");
    printf("║         ██║   ██║██║╚══██╔══╝██╔═══██╗██╔══██╗██║██╔══██╗██║██║██║           ║\n");
    printf("║         ██║   ██║██║   ██║   ██║   ██║██████╔╝██║███████║██║██║██║           ║\n");
    printf("║         ╚██╗ ██╔╝██║   ██║   ██║   ██║██╔══██╗██║██╔══██║╚═╝╚═╝╚═╝           ║\n");
    printf("║          ╚████╔╝ ██║   ██║   ╚██████╔╝██║  ██║██║██║  ██║██╗██╗██╗           ║\n");
    printf("║           ╚═══╝  ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═╝╚═╝╚═╝           ║\n");
    printf("║                                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Pressiona ENTER para voltar ao menu principal...");

    int c;
    while ((c = getchar()) != '\n');
    getchar();
}


int main(void) {
    Jogo *jogo = NULL;

    exibirMenuInicial();

    char comando[100];  
    while (1) {
        printf("Digita um comando: ");
        if (!fgets(comando, sizeof(comando), stdin)) {
            printf("Erro ao ler comando.\n");
            continue;
        }
        
        // Remover newline
        comando[strcspn(comando, "\n")] = 0;
        
        int resultado = processarComandos(&jogo, comando);

        if (jogo != NULL && strcmp(comando, "v") != 0 && resultado != -1 && resultado != 1) {
            desenhaJogo(jogo);

            if ( verificarVitoria(jogo)== 1) {
                exibirMensagemVitoria();

                freeJogo(jogo);
                jogo = NULL;
                
                exibirMenuInicial();
            }
        }
        
        if (resultado == 1) {
            break;
        }
    }
    
    freeJogo(jogo);
    
    return 0;
}
