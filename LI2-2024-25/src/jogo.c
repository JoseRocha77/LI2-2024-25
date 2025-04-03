#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/jogo.h"

Jogo* carregarJogo(char *arquivo) {
    FILE *input = fopen(arquivo, "r");
    if (!input) {
        printf("Erro ao abrir arquivo %s\n", arquivo);
        return NULL;
    }

    Jogo *jogo = malloc(sizeof(Jogo));
    if (!jogo) {
        printf("Erro na alocação de memória para o jogo.\n");
        fclose(input);
        return NULL;
    }

    // Lê as dimensões do tabuleiro
    if (fscanf(input, "%d %d\n", &jogo->linhas, &jogo->colunas) != 2) {
        printf("Erro ao ler dimensões do tabuleiro.\n");
        free(jogo);
        fclose(input);
        return NULL;
    }

    // Aloca memória para o tabuleiro
    jogo->tabuleiro = malloc(jogo->linhas * sizeof(char *));
    if (!jogo->tabuleiro) {
        printf("Erro na alocação de memória para o tabuleiro.\n");
        free(jogo);
        fclose(input);
        return NULL;
    }

    // Aloca memória para cada linha do tabuleiro
    for (int i = 0; i < jogo->linhas; i++) {
        jogo->tabuleiro[i] = malloc((jogo->colunas + 1) * sizeof(char)); // +1 para '\0'
        if (!jogo->tabuleiro[i]) {
            printf("Erro na alocação de memória para a linha %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(jogo->tabuleiro[j]);
            }
            free(jogo->tabuleiro);
            free(jogo);
            fclose(input);
            return NULL;
        }

        // Lê a linha do tabuleiro (considerando que cada linha tem colunas caracteres)
        if (fgets(jogo->tabuleiro[i], jogo->colunas + 1, input) == NULL) {
            printf("Erro ao ler linha %d do tabuleiro.\n", i);
            for (int j = 0; j <= i; j++) {
                free(jogo->tabuleiro[j]);
            }
            free(jogo->tabuleiro);
            free(jogo);
            fclose(input);
            return NULL;
        }

        // Remover o caractere de nova linha, se presente
        jogo->tabuleiro[i][jogo->colunas] = '\0';
    }

    fclose(input);
    return jogo;
}
