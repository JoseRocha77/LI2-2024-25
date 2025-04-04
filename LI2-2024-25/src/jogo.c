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

    // Aloca memória para cada linha do tabuleiro e lê cada caractere
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

        // Lê cada caractere individualmente para garantir que não há problemas
        for (int j = 0; j < jogo->colunas; j++) {
            int ch = fgetc(input);
            if (ch == EOF || ch == '\n') {
                if (j < jogo->colunas - 1) {
                    j--;
                    continue;
                }
            } else {
                jogo->tabuleiro[i][j] = (char)ch;
            }
        }
        jogo->tabuleiro[i][jogo->colunas] = '\0';

        int ch;
        while ((ch = fgetc(input)) != EOF && ch != '\n');
    }

    fclose(input);
    return jogo;
}

void desenhaJogo (Jogo *jogo) {
    //Desenhar letras correspondente às colunas
    printf("  ");
    for (int c = 0; c < jogo->colunas; c++) {
        printf("%c ", 'A' + c);
    }
    printf("\n");
    //Desenhar número da linha e desenhar linhas
    for (int l = 0; l < jogo->linhas; l++) {
        printf("%d ", l +1);

        for (int c = 0; c < jogo->colunas; c++) {
            printf("%c ", jogo->tabuleiro[l][c]);
        }
        printf("\n");
    }
}


int pintarBranco (Jogo *jogo, char *coordenada){
    int coluna = coordenada[0] -'a', linha = coordenada[1] - '1';
    jogo->tabuleiro[linha][coluna] = (jogo->tabuleiro[linha][coluna]) - ' ';

    return 0;
}

int riscar (Jogo *jogo, char *coordenada){
    int coluna = coordenada[0] -'a', linha = coordenada[1] - '1';
    jogo->tabuleiro[linha][coluna] = '#';

    return 0;
}

int processarComandos(Jogo *jogo, char *comando) {
    if (!jogo || !comando) return -1;

    // Comando para pintar de branco
    if (comando[0] == 'b' && isalpha(comando[1]) && isdigit(comando[2]) && comando[3] == '\0') {
        comando[1] = tolower(comando[1]);
        return pintarBranco(jogo, &comando[1]);
    }
    
    // Comando para riscar
    if (comando[0] == 'r' && isalpha(comando[1]) && isdigit(comando[2]) && comando[3] == '\0') {
        comando[1] = tolower(comando[1]);
        return riscar(jogo, &comando[1]);
    }
    
    // Comando para sair do jogo
    if (strcmp(comando, "s") == 0) {
        printf("Saindo do jogo...\n");
        return 1;
    }
    
    printf("Comando inválido: %s\n", comando);
    return -1;
}
