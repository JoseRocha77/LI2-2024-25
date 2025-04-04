#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/jogo.h"

int main(void) {
    Jogo *jogo = carregarJogo("input.txt");
    if (!jogo) {
        return 1;
    }

    desenhaJogo(jogo);

    char comando[10];
    while (1) {
        printf("Digite um comando: ");
        if (!fgets(comando, sizeof(comando), stdin)) {
            printf("Erro ao ler comando.\n");
            continue;
        }
        
        // Remover newline
        comando[strcspn(comando, "\n")] = 0;
        
        int resultado = processarComandos(jogo, comando);
        desenhaJogo(jogo);
        if (resultado == 1) {
            break;
        }
    }
    
    return 0;
}
