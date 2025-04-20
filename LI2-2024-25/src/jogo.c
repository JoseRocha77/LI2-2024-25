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
    
    // Inicializa o histórico de movimentos
    jogo->historicoMovimentos = NULL;

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

        // Lê cada caractere individualmente
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

void registrarMovimento(Jogo *jogo, int linha, int coluna, char estadoAnterior) {
    Movimento *novoMovimento = malloc(sizeof(Movimento));
    if (!novoMovimento) {
        printf("Erro na alocação de memória para o histórico.\n");
        return;
    }
    
    novoMovimento->linha = linha;
    novoMovimento->coluna = coluna;
    novoMovimento->estadoAnterior = estadoAnterior;
    novoMovimento->proximo = jogo->historicoMovimentos;
    
    jogo->historicoMovimentos = novoMovimento;
}

int pintarBranco(Jogo *jogo, char *coordenada){
    int coluna = coordenada[0] - 'a', linha = coordenada[1] - '1';
    
    // Validação importante para evitar buffer overflow
    if (coluna < 0 || coluna >= jogo->colunas || linha < 0 || linha >= jogo->linhas) {
        printf("Coordenadas inválidas: %s\n", coordenada);
        return -1;
    }
    
    // Registra o movimento antes de alterá-lo
    registrarMovimento(jogo, linha, coluna, jogo->tabuleiro[linha][coluna]);
    
    if (jogo->tabuleiro[linha][coluna] >= 'a' && jogo->tabuleiro[linha][coluna] <= 'z') {
        jogo->tabuleiro[linha][coluna] = jogo->tabuleiro[linha][coluna] - 32; //converter para maiscula
    }
    
    return 0;
}

int riscar(Jogo *jogo, char *coordenada) {
    if (!jogo || !coordenada || strlen(coordenada) < 2) return -1;
    
    int coluna = coordenada[0] - 'a';
    int linha = coordenada[1] - '1';
    
    // Validação importante para evitar buffer overflow
    if (coluna < 0 || coluna >= jogo->colunas || linha < 0 || linha >= jogo->linhas) {
        printf("Coordenadas inválidas: %s\n", coordenada);
        return -1;
    }
    
    // Registra o movimento antes de alterá-lo
    registrarMovimento(jogo, linha, coluna, jogo->tabuleiro[linha][coluna]);
    
    jogo->tabuleiro[linha][coluna] = '#';
    return 0;
}

int desfazerMovimento(Jogo *jogo) {
    if (!jogo || !jogo->historicoMovimentos) {
        printf("Não há movimento para desfazer.\n");
        return -1;
    }
    
    Movimento *ultimoMovimento = jogo->historicoMovimentos;
    
    // Restaura o estado anterior
    jogo->tabuleiro[ultimoMovimento->linha][ultimoMovimento->coluna] = ultimoMovimento->estadoAnterior;
    
    // Remove o movimento do histórico
    jogo->historicoMovimentos = ultimoMovimento->proximo;
    free(ultimoMovimento);
    
    printf("Movimento desfeito.\n");
    return 0;
}

int verificarRestricoes(Jogo *jogo) {
    if (!jogo) return -1;
    
    int violacoes = 0;
    
    // Verifica a restrição: casas riscadas não podem ser adjacentes
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] == '#') {
                // Verifica vizinho à direita
                if (j < jogo->colunas - 1 && jogo->tabuleiro[i][j+1] == '#') {
                    printf("Violação: Casas riscadas adjacentes em (%c%d) e (%c%d)\n", 
                           'a' + j, i + 1, 'a' + j + 1, i + 1);
                    violacoes++;
                }
                
                // Verifica vizinho abaixo
                if (i < jogo->linhas - 1 && jogo->tabuleiro[i+1][j] == '#') {
                    printf("Violação: Casas riscadas adjacentes em (%c%d) e (%c%d)\n", 
                           'a' + j, i + 1, 'a' + j, i + 2);
                    violacoes++;
                }
                
                // Verificar se vizinhos ortogonais estão pintados de branco
                // Vizinho acima
                if (i > 0 && jogo->tabuleiro[i-1][j] != '#' && 
                    !(jogo->tabuleiro[i-1][j] >= 'A' && jogo->tabuleiro[i-1][j] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas seu vizinho (%c%d) não está pintado de branco\n", 
                           'a' + j, i + 1, 'a' + j, i);
                    violacoes++;
                }
                
                // Vizinho abaixo
                if (i < jogo->linhas - 1 && jogo->tabuleiro[i+1][j] != '#' && 
                    !(jogo->tabuleiro[i+1][j] >= 'A' && jogo->tabuleiro[i+1][j] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas seu vizinho (%c%d) não está pintado de branco\n", 
                           'a' + j, i + 1, 'a' + j, i + 2);
                    violacoes++;
                }
                
                // Vizinho à esquerda
                if (j > 0 && jogo->tabuleiro[i][j-1] != '#' && 
                    !(jogo->tabuleiro[i][j-1] >= 'A' && jogo->tabuleiro[i][j-1] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas seu vizinho (%c%d) não está pintado de branco\n", 
                           'a' + j, i + 1, 'a' + j - 1, i + 1);
                    violacoes++;
                }
                
                // Vizinho à direita
                if (j < jogo->colunas - 1 && jogo->tabuleiro[i][j+1] != '#' && 
                    !(jogo->tabuleiro[i][j+1] >= 'A' && jogo->tabuleiro[i][j+1] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas seu vizinho (%c%d) não está pintado de branco\n", 
                           'a' + j, i + 1, 'a' + j + 1, i + 1);
                    violacoes++;
                }
            }
        }
    }
    
    if (violacoes == 0) {
        printf("Nenhuma violação de restrição foi encontrada.\n");
    } else {
        printf("Total de %d violações encontradas.\n", violacoes);
    }
    
    return violacoes;
}

void liberarHistoricoMovimentos(Movimento *historico) {
    while (historico != NULL) {
        Movimento *temp = historico;
        historico = historico->proximo;
        free(temp);
    }
}

void freeJogo(Jogo *jogo) {
    if (jogo != NULL) {
        if (jogo->tabuleiro != NULL) {
            for (int i = 0; i < jogo->linhas; i++) {
                free(jogo->tabuleiro[i]);
            }
            free(jogo->tabuleiro);
        }
        
        // Libera a memória do histórico de movimentos
        liberarHistoricoMovimentos(jogo->historicoMovimentos);
        
        free(jogo);
    }
}

int processarComandos(Jogo **jogo, char *comando) {
    if (!jogo || !comando) return -1;

    // Comando para carregar arquivo
    if (comando[0] == 'l' && comando[1] == ' ') {
        char arquivo[100];
        if (sscanf(comando, "l %99s", arquivo) == 1) {
            // Libera o jogo anterior se existir
            if (*jogo) {
                for (int i = 0; i < (*jogo)->linhas; i++) {
                    free((*jogo)->tabuleiro[i]);
                }
                free((*jogo)->tabuleiro);
                
                // Libera a memória do histórico de movimentos
                liberarHistoricoMovimentos((*jogo)->historicoMovimentos);
                
                free(*jogo);
                *jogo = NULL;
            }
            
            // Carrega o novo jogo
            *jogo = carregarJogo(arquivo);
            return (*jogo != NULL) ? 0 : -1;
        }
    }

    // Para os demais comandos, precisamos verificar se o jogo existe
    if (!(*jogo)) {
        printf("Jogo não carregado. Use 'l <arquivo>' para carregar um jogo.\n");
        return -1;
    }

    // Comando para sair do jogo
    if (strcmp(comando, "s") == 0) {
        printf("Saindo do jogo...\n");
        return 1;
    }
    
    // Comando para desfazer o último movimento
    if (strcmp(comando, "d") == 0) {
        return desfazerMovimento(*jogo);
    }
    
    // Comando para verificar restrições
    if (strcmp(comando, "v") == 0) {
        return verificarRestricoes(*jogo);
    }

    // Verifica se o comando tem espaço entre a letra e o número
    char tipoComando;
    char posicao[3] = {0}; // Inicializa com zeros
    if (sscanf(comando, "%c %2s", &tipoComando, posicao) != 2) {
        printf("Comando inválido: %s\n", comando);
        printf("Comandos válidos:\n");
        printf("  l <arquivo>   - Carregar jogo\n");
        printf("  b <posicao>   - Pintar de branco\n");
        printf("  r <posicao>   - Riscar\n");
        printf("  d             - Desfazer último movimento\n");
        printf("  v             - Verificar restrições\n");
        printf("  s             - Sair do jogo\n");
        return -1;
    }

    // Valida formato da posição
    if (!isalpha(posicao[0]) || !isdigit(posicao[1]) || posicao[2] != '\0') {
        printf("Formato de posição inválido: %s\n", posicao);
        return -1;
    }

    // Converte para minúsculo
    posicao[0] = tolower(posicao[0]);
    
    // Valida os limites das coordenadas
    int coluna = posicao[0] - 'a';
    int linha = posicao[1] - '1';
    
    if (coluna < 0 || coluna >= (*jogo)->colunas || linha < 0 || linha >= (*jogo)->linhas) {
        printf("Coordenadas inválidas: %s\n", posicao);
        return -1;
    }
    
    // Comando para pintar de branco
    if (tipoComando == 'b') {
        return pintarBranco(*jogo, posicao);
    }
    
    // Comando para riscar
    if (tipoComando == 'r') {
        return riscar(*jogo, posicao);
    }

    // Se não corresponde a nenhum comando válido
    printf("Comando inválido: %s\n", comando);
    printf("Comandos válidos:\n");
    printf("  l <arquivo>   - Carregar jogo\n");
    printf("  b <posicao>   - Pintar de branco\n");
    printf("  r <posicao>   - Riscar\n");
    printf("  d             - Desfazer último movimento\n");
    printf("  v             - Verificar restrições\n");
    printf("  s             - Sair do jogo\n");
    
    return -1;
}