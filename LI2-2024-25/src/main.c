#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/jogo.h"

int main(void) {
    Jogo *jogo = NULL;
    
    printf("\n====================Bem-vindo ao jogo Puzzle Hitori!====================\n");
    printf("\nUse 'l <nome_arquivo>' para carregar um jogo.\n");
    printf("Digite 's' para sair.\n\n");

    char comando[100];  
    while (1) {
        printf("Digite um comando: ");
        if (!fgets(comando, sizeof(comando), stdin)) {
            printf("Erro ao ler comando.\n");
            continue;
        }
        
        // Remover newline
        comando[strcspn(comando, "\n")] = 0;
        
        int resultado = processarComandos(&jogo, comando);

        if (jogo != NULL && resultado != -1 && resultado != 1) {
            desenhaJogo(jogo);
        }
        
        if (resultado == 1) {
            break;
        }
    }
    
    freeJogo(jogo);
    
    return 0;
}
