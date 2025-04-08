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
        printf("_%c", 'a' + c);
    }
    printf("\n");
    //Desenhar número da linha e desenhar linhas
    for (int l = 0; l < jogo->linhas; l++) {
        printf("%d| ", l +1);

        for (int c = 0; c < jogo->colunas; c++) {
            printf("%c ", jogo->tabuleiro[l][c]);
        }
        printf("\n");
    }
}


int pintarBranco (Jogo *jogo, char *coordenada){
    int coluna = coordenada[0] -'a', linha = coordenada[1] - '1';

    // Verificar se as coordenadas estão dentro dos limites do tabuleiro
    if (coluna < 0 || coluna >= jogo->colunas || linha < 0 || linha >= jogo->linhas) {
        printf("Coordenada inválida!\n");
        return -1;
    }

    // Verificar se o local já está pintado 
    if (jogo->tabuleiro[linha][coluna] >= 'A' && jogo->tabuleiro[linha][coluna] <= 'Z') {
        printf("Esta célula já está pintada de branco!\n");
        return -1;
    }

    // Verificar se o local já está riscado
    if (jogo->tabuleiro[linha][coluna] == '#') {
        printf("Esta célula já está riscada e não pode ser pintada de branco!\n");
        return -1;
    }

    jogo->tabuleiro[linha][coluna] = (jogo->tabuleiro[linha][coluna]) - ' ';

    return 0;
}

int riscar (Jogo *jogo, char *coordenada){
    int coluna = coordenada[0] -'a', linha = coordenada[1] - '1';

    // Verificar se as coordenadas estão dentro dos limites do tabuleiro
    if (coluna < 0 || coluna >= jogo->colunas || linha < 0 || linha >= jogo->linhas) {
        printf("Coordenada inválida!\n");
        return -1;
    }
    
    // Verificar se o local já está riscado
    if (jogo->tabuleiro[linha][coluna] == '#') {
        printf("Esta célula já está riscada!\n");
        return -1;
    }
    
    // Verificar se o local já está pintado 
    if (jogo->tabuleiro[linha][coluna] >= 'A' && jogo->tabuleiro[linha][coluna] <= 'Z') {
        printf("Esta célula está pintada de branco, volta atrás para poderes riscar!\n");
        return -1;
    }

    jogo->tabuleiro[linha][coluna] = '#';

    return 0;
}

// int processarComandos(Jogo *jogo, char *comando) {
//     if (!jogo || !comando) return -1;

//     // Comando para sair do jogo
//     if (strcmp(comando, "s") == 0) {
//         printf("Saindo do jogo...\n");
//         return 1;
//     }

//     // Verifica se o comando tem espaço entre a letra e o número
//     char tipoComando;
//     char posicao[3];
//     if (sscanf(comando, "%c %2s", &tipoComando, posicao) != 2) {
//         printf("Comando inválido: %s\n", comando);
//         printf("Comandos válidos:\n");
//         printf("  b <posicao>   - Pintar de branco\n");
//         printf("  r <posicao>   - Riscar\n");
//         printf("  s             - Sair do jogo\n");
//         return -1;
//     }

//     // Comando para pintar de branco
//     if (tipoComando == 'b' && isalpha(posicao[0]) && isdigit(posicao[1]) && posicao[2] == '\0') {
//         posicao[0] = tolower(posicao[0]);
//         return pintarBranco(jogo, posicao);
//     }
    
//     // Comando para riscar
//     if (tipoComando == 'r' && isalpha(posicao[0]) && isdigit(posicao[1]) && posicao[2] == '\0') {
//         posicao[0] = tolower(posicao[0]);
//         return riscar(jogo, posicao);
//     }

//     // Se não corresponde a nenhum comando válido
//     printf("Comando inválido: %s\n", comando);
//     printf("Comandos válidos:\n");
//     printf("  b <posicao>   - Pintar de branco\n");
//     printf("  r <posicao>   - Riscar\n");
//     printf("  s             - Sair do jogo\n");
    
//     return -1;
// }

void freeJogo(Jogo *jogo) {
    if (jogo != NULL) {
        if (jogo->tabuleiro != NULL) {
            for (int i = 0; i < jogo->linhas; i++) {
                free(jogo->tabuleiro[i]);
            }
            free(jogo->tabuleiro);
        }
        free(jogo);
    }
}

int processarComandos(Jogo **jogo, char *comando) {
    if (!comando) return -1;

    // Comando para sair do jogo
    if (strcmp(comando, "s") == 0) {
        printf("Saindo do jogo...\n");
        return 1;
    }
    
    // Comando para carregar jogo (novo)
    if (comando[0] == 'l' && comando[1] == ' ') {
        char arquivo[95];
        if (sscanf(comando, "l %94s", arquivo) != 1) {
            printf("Formato inválido. Use: l <nome_arquivo>\n");
            return -1;
        }
        
        // Libera a memória do jogo atual, se existir
        freeJogo(*jogo);
        *jogo = NULL;
        
        // Carrega o novo jogo
        *jogo = carregarJogo(arquivo);
        if (*jogo == NULL) {
            printf("Falha ao carregar o jogo de %s\n", arquivo);
            return -1;
        }
        
        printf("Jogo carregado com sucesso de %s\n", arquivo);
        return 0;
    }

    // Verifica se há um jogo carregado para os outros comandos
    if (*jogo == NULL) {
        printf("Nenhum jogo carregado. Use 'l <nome_arquivo>' para carregar um jogo.\n");
        return -1;
    }

    // Verifica se o comando tem espaço entre a letra e o número
    char tipoComando;
    char posicao[3];
    if (sscanf(comando, "%c %2s", &tipoComando, posicao) != 2) {
        printf("Comando inválido: %s\n\n", comando);
        printf("          Comandos válidos:\n");
        printf("  l <arquivo>    - Carregar jogo\n");
        printf("  b <posicao>    - Pintar de branco\n");
        printf("  r <posicao>    - Riscar\n");
        printf("  s              - Sair do jogo\n");
        return -1;
    }

    // Comando para pintar de branco
    if (tipoComando == 'b' && isalpha(posicao[0]) && isdigit(posicao[1]) && posicao[2] == '\0') {
        posicao[0] = tolower(posicao[0]);
        return pintarBranco(*jogo, posicao);
    }
    
    // Comando para riscar
    if (tipoComando == 'r' && isalpha(posicao[0]) && isdigit(posicao[1]) && posicao[2] == '\0') {
        posicao[0] = tolower(posicao[0]);
        return riscar(*jogo, posicao);
    }

    // Se não corresponde a nenhum comando válido
    printf("Comando inválido: %s\n\n", comando);
    printf("          Comandos válidos:\n");
    printf("  l <arquivo>    - Carregar jogo\n");
    printf("  b <posicao>    - Pintar de branco\n");
    printf("  r <posicao>    - Riscar\n");
    printf("  s              - Sair do jogo\n");
    
    return -1;
}
