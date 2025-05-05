#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/jogo.h"

// Funções etapa 1 ===================================================================================

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
    jogo->modoAjudaAtiva = 0; // Desativado por padrão


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

        // Lê cada carater individualmente
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

    // Carrega o histórico de movimentos, se existir
    carregarHistoricoMovimentos(input, jogo);

    fclose(input);
    return jogo;
}

// Função auxiliar para carregar o histórico de movimentos
void carregarHistoricoMovimentos(FILE *input, Jogo *jogo) {
    int numMovimentos;
    if (fscanf(input, "%d\n", &numMovimentos) != 1) {
        return; // Não há histórico de movimentos no arquivo
    }
    
    printf("Carregando %d movimentos do histórico...\n", numMovimentos);
    
    Movimento **movimentosTemp = NULL;
    if (numMovimentos > 0) {
        movimentosTemp = malloc(numMovimentos * sizeof(Movimento*));
        if (!movimentosTemp) {
            printf("Erro na alocação de memória para movimentos temporários.\n");
            return;
        }

        for (int i = 0; i < numMovimentos; i++) {
            movimentosTemp[i] = NULL;
        }
    }
    
    // Lê cada movimento do histórico
    for (int i = 0; i < numMovimentos; i++) {
        int linha, coluna;
        char estadoAnterior;
        
        if (fscanf(input, "%d %d %c\n", &linha, &coluna, &estadoAnterior) == 3) {
            Movimento *novoMovimento = malloc(sizeof(Movimento));
            if (!novoMovimento) {
                printf("Erro na alocação de memória para o movimento %d.\n", i);
                continue;
            }
            
            novoMovimento->linha = linha;
            novoMovimento->coluna = coluna;
            novoMovimento->estadoAnterior = estadoAnterior;
            novoMovimento->proximo = NULL;
            
            movimentosTemp[i] = novoMovimento;
        } else {
            printf("Erro ao ler o movimento %d do histórico.\n", i);
            break;
        }
    }
    
    if (movimentosTemp) {
        for (int i = 0; i < numMovimentos; i++) {
            if (movimentosTemp[i]) {
                int idx = numMovimentos - 1 - i;
                if (idx >= 0 && idx < numMovimentos && movimentosTemp[idx]) {
                    movimentosTemp[idx]->proximo = jogo->historicoMovimentos;
                    jogo->historicoMovimentos = movimentosTemp[idx];
                }
            }
        }
        free(movimentosTemp);
    }
}



int gravarJogo(Jogo *jogo, char *arquivo) {
    if (!jogo || !arquivo) return -1;
    
    FILE *output = fopen(arquivo, "w");
    if (!output) {
        printf("Erro ao abrir arquivo %s para escrita\n", arquivo);
        return -1;
    }
    
    // Escreve as dimensões do tabuleiro
    fprintf(output, "%d %d\n", jogo->linhas, jogo->colunas);
    
    // Escreve o conteúdo do tabuleiro
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            fputc(jogo->tabuleiro[i][j], output);
        }
        fputc('\n', output);
    }
    
    // Conta quantos movimentos estão no histórico
    int numMovimentos = 0;
    Movimento *atual = jogo->historicoMovimentos;
    while (atual != NULL) {
        numMovimentos++;
        atual = atual->proximo;
    }
    
    // Escreve o número de movimentos
    fprintf(output, "%d\n", numMovimentos);
    
    // Escreve cada movimento do histórico (do mais recente para o mais antigo)
    atual = jogo->historicoMovimentos;
    while (atual != NULL) {
        fprintf(output, "%d %d %c\n", atual->linha, atual->coluna, atual->estadoAnterior);
        atual = atual->proximo;
    }
    
    fclose(output);
    printf("Jogo salvo com sucesso em '%s'\n", arquivo);
    return 0;
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

int pintarBranco(Jogo *jogo, char *coordenada){
    int coluna = coordenada[0] - 'a', linha = coordenada[1] - '1';
    
    // Validação importante para evitar buffer overflow
    if (coluna < 0 || coluna >= jogo->colunas || linha < 0 || linha >= jogo->linhas) {
        printf("Coordenadas inválidas: %s\n", coordenada);
        return -1;
    }
    
    // Regista o movimento antes de alterá-lo
    registarMovimento(jogo, linha, coluna, jogo->tabuleiro[linha][coluna]);
    
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
    
    // Regista o movimento antes de alterá-lo
    registarMovimento(jogo, linha, coluna, jogo->tabuleiro[linha][coluna]);
    
    jogo->tabuleiro[linha][coluna] = '#';
    return 0;
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
        freeHistoricoMovimentos(jogo->historicoMovimentos);
        
        free(jogo);
    }
}



// Funções etapa 2 ===================================================================================

void registarMovimento(Jogo *jogo, int linha, int coluna, char estadoAnterior) {
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


int desfazerMovimento(Jogo *jogo) {
    if (!jogo || !jogo->historicoMovimentos) {
        printf("Não há movimento para desfazer.\n");
        return -1;
    }

    Movimento *ultimoMovimento = jogo->historicoMovimentos;

    // Valor que será substituído (o valor atual do tabuleiro antes de desfazer)
    char valorAtual = jogo->tabuleiro[ultimoMovimento->linha][ultimoMovimento->coluna];

    // Restaura o estado anterior
    jogo->tabuleiro[ultimoMovimento->linha][ultimoMovimento->coluna] = ultimoMovimento->estadoAnterior;

    // Remove o movimento do histórico
    jogo->historicoMovimentos = ultimoMovimento->proximo;

    printf(
        "Movimento desfeito na posição (%c,%d): '%c' voltou para '%c'.\n",
        ultimoMovimento->coluna + 'a',
        ultimoMovimento->linha + 1 ,
        valorAtual,
        ultimoMovimento->estadoAnterior
    );

    free(ultimoMovimento);
    return 0;
}

int verificarRestricoes(Jogo *jogo) {
    if (!jogo) return -1;
    
    int violacoes = 0;
    
    // Verificação de símbolos únicos em linhas/colunas (NOVO)
    for (int i = 0; i < jogo->linhas; i++) {
        int contagem[26] = {0};
        for (int j = 0; j < jogo->colunas; j++) {
            char c = toupper(jogo->tabuleiro[i][j]);
            if (c >= 'A' && c <= 'Z') {
                if (contagem[c - 'A']++ > 0) {
                    printf("Violação: Múltiplos %c na linha %d\n", c, i+1);
                    violacoes++;
                }
            }
        }
    }
    
    for (int j = 0; j < jogo->colunas; j++) {
        int contagem[26] = {0};
        for (int i = 0; i < jogo->linhas; i++) {
            char c = toupper(jogo->tabuleiro[i][j]);
            if (c >= 'A' && c <= 'Z') {
                if (contagem[c - 'A']++ > 0) {
                    printf("Violação: Múltiplos %c na coluna %c\n", c, 'a'+j);
                    violacoes++;
                }
            }
        }
    }


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

                // Verifica se vizinhos ortogonais são brancos
                // Cima
                if (i > 0 && jogo->tabuleiro[i-1][j] != '#' && 
                    !(jogo->tabuleiro[i-1][j] >= 'A' && jogo->tabuleiro[i-1][j] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas o vizinho acima (%c%d) não é branco\n", 
                           'a' + j, i + 1, 'a' + j, i);
                    violacoes++;
                }

                // Baixo
                if (i < jogo->linhas - 1 && jogo->tabuleiro[i+1][j] != '#' && 
                    !(jogo->tabuleiro[i+1][j] >= 'A' && jogo->tabuleiro[i+1][j] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas o vizinho abaixo (%c%d) não é branco\n", 
                           'a' + j, i + 1, 'a' + j, i + 2);
                    violacoes++;
                }

                // Esquerda
                if (j > 0 && jogo->tabuleiro[i][j-1] != '#' && 
                    !(jogo->tabuleiro[i][j-1] >= 'A' && jogo->tabuleiro[i][j-1] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas o vizinho à esquerda (%c%d) não é branco\n", 
                           'a' + j, i + 1, 'a' + j - 1, i + 1);
                    violacoes++;
                }

                // Direita
                if (j < jogo->colunas - 1 && jogo->tabuleiro[i][j+1] != '#' && 
                    !(jogo->tabuleiro[i][j+1] >= 'A' && jogo->tabuleiro[i][j+1] <= 'Z')) {
                    printf("Violação: Casa (%c%d) riscada, mas o vizinho à direita (%c%d) não é branco\n", 
                           'a' + j, i + 1, 'a' + j + 1, i + 1);
                    violacoes++;
                }
            }
        }
    }

    // Verificação adicional: conectividade das casas brancas
    int conectividade = verificarConectividadeBrancas(jogo);
    if (conectividade != 0) {
        printf("Violação: As casas brancas não estão todas conectadas ortogonalmente.\n");
        violacoes++;
    } else {
        printf("Todas as casas brancas estão conectadas.\n");
    }

    if (violacoes == 0) {
        printf("Nenhuma violação de restrição foi encontrada.\n");
    } else {
        printf("Total de %d violações encontradas.\n", violacoes);
        printf("Use o comando 'd' se pretender desfazer o último movimento.\n");
    }
    

    return violacoes ? -1 : 0;
}


void freeHistoricoMovimentos(Movimento *historico) {
    while (historico != NULL) {
        Movimento *temp = historico;
        historico = historico->proximo;
        free(temp);
    }
}



// Funções etapa 3 ===================================================================================

// DFS fora da função principal
void dfs(Jogo *jogo, int **visitado, int *visitadas, int linha, int coluna) {
    if (linha < 0 || linha >= jogo->linhas || coluna < 0 || coluna >= jogo->colunas) return;
    if (visitado[linha][coluna]) return;
    if (!(jogo->tabuleiro[linha][coluna] >= 'A' && jogo->tabuleiro[linha][coluna] <= 'Z')) return;

    // Marca como visitado e incrementa o contador de casas brancas conectadas
    visitado[linha][coluna] = 1;
    (*visitadas)++;

    // Move-se recursivamente para casas brancas vizinhas ortogonais
    dfs(jogo, visitado, visitadas, linha - 1, coluna); // cima
    dfs(jogo, visitado, visitadas, linha + 1, coluna); // baixo
    dfs(jogo, visitado, visitadas, linha, coluna - 1); // esquerda
    dfs(jogo, visitado, visitadas, linha, coluna + 1); // direita
}

int verificarConectividadeBrancas(Jogo *jogo) {
    if (!jogo) return -1;

    int totalBrancas = 0;
    int visitadas = 0;

    int **visitado = malloc(jogo->linhas * sizeof(int *));
    if (!visitado) {
        printf("Erro ao alocar matriz de visitados.\n");
        return -1;
    }
    for (int i = 0; i < jogo->linhas; i++) {
        visitado[i] = calloc(jogo->colunas, sizeof(int));
        if (!visitado[i]) {
            for (int j = 0; j < i; j++) free(visitado[j]);
            free(visitado);
            printf("Erro ao alocar linha da matriz de visitados.\n");
            return -1;
        }
    }

    int inicioLinha = -1, inicioColuna = -1;
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] >= 'A' && jogo->tabuleiro[i][j] <= 'Z') {
                totalBrancas++;
                if (inicioLinha == -1) {
                    inicioLinha = i;
                    inicioColuna = j;
                }
            }
        }
    }

    if (totalBrancas == 0) {
        //printf("Não há casas brancas no tabuleiro.\n");
        for (int i = 0; i < jogo->linhas; i++) free(visitado[i]);
        free(visitado);
        return 0;
    }

    // Chama a função DFS com apontador para visitadas
    dfs(jogo, visitado, &visitadas, inicioLinha, inicioColuna);

    for (int i = 0; i < jogo->linhas; i++) free(visitado[i]);
    free(visitado);

    if (visitadas == totalBrancas) {
        //printf("Todas as casas brancas estão conectadas.\n");
        return 0;
    }

    return -1;
}



// Funções etapa 4 ==========================================================================================

int ajudar(Jogo *jogo) {
    // 1. Regra: riscar letras iguais a uma branca na linha ou coluna
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            char atual = jogo->tabuleiro[i][j];
            if (atual >= 'A' && atual <= 'Z') { // Letra branca
                // Verifica linha
                for (int k = 0; k < jogo->colunas; k++) {
                    if (jogo->tabuleiro[i][k] == atual + 32) {
                        char coord[3] = { 'a' + k, '1' + i, '\0' };
                        printf("Ajuda: riscar %s (igual a branca %c na linha)\n", coord, atual);
                        riscar(jogo, coord);
                        return 1;
                    }
                }
                // Verifica coluna
                for (int k = 0; k < jogo->linhas; k++) {
                    if (jogo->tabuleiro[k][j] == atual + 32) {
                        char coord[3] = { 'a' + j, '1' + k, '\0' };
                        printf("Ajuda: riscar %s (igual a branca %c na coluna)\n", coord, atual);
                        riscar(jogo, coord);
                        return 1;
                    }
                }
            }
        }
    }

    // 2. Regra: pintar de branco todas as casas vizinhas de uma casa riscada
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] == '#') {
                int alterou = 0;
                int di[] = {-1, 1, 0, 0};
                int dj[] = {0, 0, -1, 1};
                for (int d = 0; d < 4; d++) {
                    int ni = i + di[d], nj = j + dj[d];
                    if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
                        char viz = jogo->tabuleiro[ni][nj];
                        if (viz >= 'a' && viz <= 'z') {
                            char coord[3] = { 'a' + nj, '1' + ni, '\0' };
                            pintarBranco(jogo, coord);
                            alterou = 1;
                        }
                    }
                }
                if (alterou) {
                    printf("Ajuda: pintar casas vizinhas de (%c%d)\n", 'a'+j, i+1);
                    return 1;
                }
            }
        }
    }

    // 3. Regra: pintar de branco casas que isolariam brancas se fossem riscadas
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            char c = jogo->tabuleiro[i][j];
            if (c >= 'a' && c <= 'z') {
                // Simula riscar esta casa
                char original = jogo->tabuleiro[i][j];
                jogo->tabuleiro[i][j] = '#';
                int resultado = verificarConectividadeBrancas(jogo);
                jogo->tabuleiro[i][j] = original;

                if (resultado != 0) {
                    char coord[3] = { 'a' + j, '1' + i, '\0' };
                    printf("Ajuda: pintar de branco %s (evita isolamento)\n", coord);
                    pintarBranco(jogo, coord);
                    return 1;
                }
            }
        }
    }

    printf("Nenhuma jogada inferida disponível,faça uma jogada antes de usar o comando de ajuda.\n");
    return 0;
}



Jogo* copiarJogo(Jogo* original) {
    if (!original) return NULL;
    
    Jogo* copia = malloc(sizeof(Jogo));
    if (!copia) return NULL;
    
    copia->linhas = original->linhas;
    copia->colunas = original->colunas;
    copia->modoAjudaAtiva = original->modoAjudaAtiva;
    copia->historicoMovimentos = NULL;
    
    copia->tabuleiro = malloc(copia->linhas * sizeof(char*));
    if (!copia->tabuleiro) {
        free(copia);
        return NULL;
    }
    
    for (int i = 0; i < copia->linhas; i++) {
        copia->tabuleiro[i] = malloc((copia->colunas + 1) * sizeof(char));
        if (!copia->tabuleiro[i]) {
            for (int j = 0; j < i; j++) free(copia->tabuleiro[j]);
            free(copia->tabuleiro);
            free(copia);
            return NULL;
        }
        strcpy(copia->tabuleiro[i], original->tabuleiro[i]);
    }
    
    // Copiar histórico de movimentos
    Movimento *orig = original->historicoMovimentos;
    Movimento **dest = &(copia->historicoMovimentos);
    
    while (orig) {
        *dest = malloc(sizeof(Movimento));
        if (!*dest) {
            freeHistoricoMovimentos(copia->historicoMovimentos);
            for (int i = 0; i < copia->linhas; i++) free(copia->tabuleiro[i]);
            free(copia->tabuleiro);
            free(copia);
            return NULL;
        }
        (*dest)->linha = orig->linha;
        (*dest)->coluna = orig->coluna;
        (*dest)->estadoAnterior = orig->estadoAnterior;
        (*dest)->proximo = NULL;
        
        dest = &((*dest)->proximo);
        orig = orig->proximo;
    }
    
    return copia;
}


void restaurarJogo(Jogo* destino, Jogo* origem) {
    if (!destino || !origem) return;
    
    // Copiar tabuleiro
    for (int i = 0; i < destino->linhas; i++) {
        strcpy(destino->tabuleiro[i], origem->tabuleiro[i]);
    }
    
    // Copiar histórico
    freeHistoricoMovimentos(destino->historicoMovimentos);
    destino->historicoMovimentos = NULL;
    
    Movimento *orig = origem->historicoMovimentos;
    Movimento **dest = &(destino->historicoMovimentos);
    
    while (orig) {
        *dest = malloc(sizeof(Movimento));
        if (!*dest) break;
        
        (*dest)->linha = orig->linha;
        (*dest)->coluna = orig->coluna;
        (*dest)->estadoAnterior = orig->estadoAnterior;
        (*dest)->proximo = NULL;
        
        dest = &((*dest)->proximo);
        orig = orig->proximo;
    }
}

// Função auxiliar para backtracking melhorada
// Não imprime a solução, apenas resolve e retorna o status
int backtrackingResolver(Jogo *jogo) {
    // Aplicar todas as regras determinísticas primeiro
    int alteracoes;
    int maxIteracoes = 50;
    int iteracoes = 0;
    
    do {
        alteracoes = 0;
        
        // Aplicar Regra 1: Eliminar duplicados em linhas/colunas
        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (isupper(jogo->tabuleiro[i][j])) {
                    char atual = tolower(jogo->tabuleiro[i][j]);
                    
                    // Verificar linha e riscar duplicatas
                    for (int k = 0; k < jogo->colunas; k++) {
                        if (k != j && tolower(jogo->tabuleiro[i][k]) == atual) {
                            char coord[3] = {'a'+k, '1'+i, '\0'};
                            if (islower(jogo->tabuleiro[i][k]) && riscar(jogo, coord) == 0) {
                                alteracoes++;
                            }
                        }
                    }
                    
                    // Verificar coluna e riscar duplicatas
                    for (int k = 0; k < jogo->linhas; k++) {
                        if (k != i && tolower(jogo->tabuleiro[k][j]) == atual) {
                            char coord[3] = {'a'+j, '1'+k, '\0'};
                            if (islower(jogo->tabuleiro[k][j]) && riscar(jogo, coord) == 0) {
                                alteracoes++;
                            }
                        }
                    }
                }
            }
        }
        
        // Regra 2: Vizinhos de casas riscadas devem ser brancos
        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (jogo->tabuleiro[i][j] == '#') {
                    int di[] = {-1, 1, 0, 0};
                    int dj[] = {0, 0, -1, 1};
                    
                    for (int d = 0; d < 4; d++) {
                        int ni = i + di[d];
                        int nj = j + dj[d];
                        
                        if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
                            if (islower(jogo->tabuleiro[ni][nj])) {
                                char coord[3] = {'a'+nj, '1'+ni, '\0'};
                                if (pintarBranco(jogo, coord) == 0) {
                                    alteracoes++;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Regra 3: Se uma letra só aparece uma vez na linha ou coluna, deve ser branca
        // Linhas
        for (int i = 0; i < jogo->linhas; i++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0;
                int pos = -1;
                
                for (int j = 0; j < jogo->colunas; j++) {
                    if (tolower(jogo->tabuleiro[i][j]) == letra) {
                        count++;
                        pos = j;
                    }
                }
                
                if (count == 1 && pos != -1 && islower(jogo->tabuleiro[i][pos])) {
                    char coord[3] = {'a'+pos, '1'+i, '\0'};
                    if (pintarBranco(jogo, coord) == 0) {
                        alteracoes++;
                    }
                }
            }
        }
        
        // Colunas
        for (int j = 0; j < jogo->colunas; j++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0;
                int pos = -1;
                
                for (int i = 0; i < jogo->linhas; i++) {
                    if (tolower(jogo->tabuleiro[i][j]) == letra) {
                        count++;
                        pos = i;
                    }
                }
                
                if (count == 1 && pos != -1 && islower(jogo->tabuleiro[pos][j])) {
                    char coord[3] = {'a'+j, '1'+pos, '\0'};
                    if (pintarBranco(jogo, coord) == 0) {
                        alteracoes++;
                    }
                }
            }
        }
        
        iteracoes++;
    } while (alteracoes > 0 && iteracoes < maxIteracoes);
    
    // Verificar se o jogo atual tem violações evidentes
    // 1. Verificar casas riscadas adjacentes
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] == '#') {
                // Verifica vizinho à direita
                if (j < jogo->colunas - 1 && jogo->tabuleiro[i][j+1] == '#') {
                    return -1; // Violação: casas riscadas adjacentes
                }
                // Verifica vizinho abaixo
                if (i < jogo->linhas - 1 && jogo->tabuleiro[i+1][j] == '#') {
                    return -1; // Violação: casas riscadas adjacentes
                }
            }
        }
    }
    
    // Verifica se todas as células estão resolvidas
    int completo = 1;
    int linha_nao_resolvida = -1;
    int coluna_nao_resolvida = -1;
    
    for (int i = 0; i < jogo->linhas && completo; i++) {
        for (int j = 0; j < jogo->colunas && completo; j++) {
            if (islower(jogo->tabuleiro[i][j])) {
                completo = 0;
                linha_nao_resolvida = i;
                coluna_nao_resolvida = j;
            }
        }
    }
    
    // Se todas as células estão resolvidas, verifica se a solução é válida
    if (completo) {
        // Verificar conectividade das casas brancas
        if (verificarConectividadeBrancas(jogo) != 0) {
            return -1; // Violação: casas brancas não conectadas
        }
        
        // Verificar outras restrições
        return verificarRestricoes(jogo) == 0 ? 0 : -1;
    }
    
    // Criar uma cópia do jogo para tentar pintar de branco
    Jogo *jogoCopia = copiarJogo(jogo);
    if (!jogoCopia) {
        return -1; // Falha ao criar cópia
    }
    
    char coord[3] = {'a' + coluna_nao_resolvida, '1' + linha_nao_resolvida, '\0'};
    
    // Tenta pintar de branco primeiro
    if (pintarBranco(jogoCopia, coord) == 0) {
        // Aplicar regras determinísticas na cópia pintada de branco
        int result = backtrackingResolver(jogoCopia);
        if (result == 0) {
            // Copiar a solução encontrada de volta para o jogo original
            for (int i = 0; i < jogo->linhas; i++) {
                for (int j = 0; j < jogo->colunas; j++) {
                    jogo->tabuleiro[i][j] = jogoCopia->tabuleiro[i][j];
                }
            }
            freeJogo(jogoCopia);
            return 0;
        }
    }
    
    // Liberar a cópia e criar uma nova para tentar riscar
    freeJogo(jogoCopia);
    jogoCopia = copiarJogo(jogo);
    if (!jogoCopia) {
        return -1; // Falha ao criar cópia
    }

    // Tenta riscar a célula
    if (riscar(jogoCopia, coord) == 0) {
        // Aplicar regras determinísticas na cópia riscada
        int result = backtrackingResolver(jogoCopia);
        if (result == 0) {
            // Copiar a solução encontrada de volta para o jogo original
            for (int i = 0; i < jogo->linhas; i++) {
                for (int j = 0; j < jogo->colunas; j++) {
                    jogo->tabuleiro[i][j] = jogoCopia->tabuleiro[i][j];
                }
            }
            freeJogo(jogoCopia);
            return 0;
        }
    }

    freeJogo(jogoCopia);
    return -1;
}

int resolverJogo(Jogo *jogo) {
    if (!jogo) return -1;

    // Cria uma cópia do jogo original
    Jogo *jogoOriginal = copiarJogo(jogo);
    if (!jogoOriginal) {
        printf("Erro ao criar cópia do jogo original.\n");
        return -1;
    }

    int alteracoes, totalAlteracoes = 0, resolvido = 0;
    int iteracoes = 0, maxIteracoes = 100;

    do {
        alteracoes = 0;

        // Regra 1: Eliminar duplicatas
        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (isupper(jogo->tabuleiro[i][j])) {
                    char atual = tolower(jogo->tabuleiro[i][j]);

                    for (int k = 0; k < jogo->colunas; k++) {
                        if (k != j && tolower(jogo->tabuleiro[i][k]) == atual) {
                            char coord[3] = { 'a' + k, '1' + i, '\0' };
                            if (islower(jogo->tabuleiro[i][k]) && riscar(jogo, coord) == 0) {
                                alteracoes++;
                            }
                        }
                    }

                    for (int k = 0; k < jogo->linhas; k++) {
                        if (k != i && tolower(jogo->tabuleiro[k][j]) == atual) {
                            char coord[3] = { 'a' + j, '1' + k, '\0' };
                            if (islower(jogo->tabuleiro[k][j]) && riscar(jogo, coord) == 0) {
                                alteracoes++;
                            }
                        }
                    }
                }
            }
        }

        // Regra 2: Vizinhos de riscados devem ser brancos
        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (jogo->tabuleiro[i][j] == '#') {
                    int di[] = { -1, 1, 0, 0 };
                    int dj[] = { 0, 0, -1, 1 };

                    for (int d = 0; d < 4; d++) {
                        int ni = i + di[d];
                        int nj = j + dj[d];

                        if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
                            if (islower(jogo->tabuleiro[ni][nj])) {
                                char coord[3] = { 'a' + nj, '1' + ni, '\0' };
                                if (pintarBranco(jogo, coord) == 0) {
                                    alteracoes++;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Regra 3: Letras únicas na linha devem ser brancas
        for (int i = 0; i < jogo->linhas; i++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0, pos = -1;
                for (int j = 0; j < jogo->colunas; j++) {
                    if (tolower(jogo->tabuleiro[i][j]) == letra) {
                        count++;
                        pos = j;
                    }
                }
                if (count == 1 && pos != -1 && islower(jogo->tabuleiro[i][pos])) {
                    char coord[3] = { 'a' + pos, '1' + i, '\0' };
                    if (pintarBranco(jogo, coord) == 0) {
                        alteracoes++;
                    }
                }
            }
        }

        // Regra 3: Letras únicas na coluna devem ser brancas
        for (int j = 0; j < jogo->colunas; j++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0, pos = -1;
                for (int i = 0; i < jogo->linhas; i++) {
                    if (tolower(jogo->tabuleiro[i][j]) == letra) {
                        count++;
                        pos = i;
                    }
                }
                if (count == 1 && pos != -1 && islower(jogo->tabuleiro[pos][j])) {
                    char coord[3] = { 'a' + j, '1' + pos, '\0' };
                    if (pintarBranco(jogo, coord) == 0) {
                        alteracoes++;
                    }
                }
            }
        }

        // Regra 4: Prevenir desconexão de áreas brancas
        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (islower(jogo->tabuleiro[i][j])) {
                    char original = jogo->tabuleiro[i][j];
                    jogo->tabuleiro[i][j] = '#';
                    int resultado = verificarConectividadeBrancas(jogo);
                    jogo->tabuleiro[i][j] = original;

                    if (resultado != 0) {
                        char coord[3] = { 'a' + j, '1' + i, '\0' };
                        if (pintarBranco(jogo, coord) == 0) {
                            alteracoes++;
                        }
                    }
                }
            }
        }

        // Verifica se está resolvido (sem letras minúsculas)
        resolvido = 1;
        for (int i = 0; i < jogo->linhas && resolvido; i++) {
            for (int j = 0; j < jogo->colunas && resolvido; j++) {
                if (islower(jogo->tabuleiro[i][j])) {
                    resolvido = 0;
                }
            }
        }

        // Se o jogo está resolvido e é válido
        if (resolvido && verificarRestricoes(jogo) == 0) {
            printf("\nSolução encontrada com %d alterações:\n", totalAlteracoes + alteracoes);
            freeJogo(jogoOriginal);
            return 0;
        } else if (resolvido) {
            // Solução inválida
            resolvido = 0;
        }

        totalAlteracoes += alteracoes;
        iteracoes++;

    } while (!resolvido && alteracoes > 0 && iteracoes < maxIteracoes);

    // Se não resolveu com regras determinísticas
    printf("Não foi possível resolver apenas com regras determinísticas. Iniciando backtracking...\n");
    printf("Usando backtracking para tentar resolver...\n");
        
    restaurarJogo(jogo, jogoOriginal);
    freeJogo(jogoOriginal);
        
    if (backtrackingResolver(jogo) == 0) {
        return 0;
    }
        
    printf("Não foi possível encontrar uma solução.\n");
    return -1;
}




// Função principal ===========================================================================================



int processarComandos(Jogo **jogo, char *comando) {
    if (!jogo || !comando) return -1;


    // Comando para sair do jogo
    if (strcmp(comando, "s") == 0) {
        printf("Saindo do jogo...\n");
        return 1;
    }

    // Comando para gravar jogo
    if (comando[0] == 'g' && comando[1] == ' ') {
        char arquivo[100];
        if (sscanf(comando, "g %99s", arquivo) == 1) {
            return gravarJogo(*jogo, arquivo);
        } else {
            printf("Formato inválido. Use 'g <arquivo>'\n");
            return -1;
        }
    }

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
                freeHistoricoMovimentos((*jogo)->historicoMovimentos);
                
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

    
    // Comando para desfazer o último movimento
    if (strcmp(comando, "d") == 0) {
        return desfazerMovimento(*jogo);
    }
    
    // Comando para verificar restrições
    if (strcmp(comando, "v") == 0) {
        return verificarRestricoes(*jogo);
    }

    // Verifica o comando "a" (ajudar)
    if (strcmp(comando, "a") == 0) {
        if (!(*jogo)) {
            printf("Jogo não carregado. Use 'l <arquivo>' para carregar um jogo.\n");
            return -1;
        }
        ajudar(*jogo);
        return 0;
    }
    

    if (strcmp(comando, "A") == 0) {
        (*jogo)->modoAjudaAtiva = !((*jogo)->modoAjudaAtiva);
        if ((*jogo)->modoAjudaAtiva) {
            printf("Modo de ajuda automática ATIVADO.\n");
        } else {
            printf("Modo de ajuda automática DESATIVADO.\n");
        }
        return 0;
    }

    // Comando para resolver automaticamente o jogo
    if (strcmp(comando, "R") == 0) {
        if (!(*jogo)) {
            printf("Jogo não carregado. Use 'l <arquivo>' para carregar um jogo.\n");
            return -1;
        }
        return resolverJogo(*jogo);
    }
    
    

    // Verifica se o comando tem espaço entre a letra e o número
    char tipoComando;
    char posicao[3] = {0}; // Inicializa com zeros
    if (sscanf(comando, "%c %2s", &tipoComando, posicao) != 2) {
        printf("Comando inválido: %s\n", comando);
        printf("Comandos válidos:\n");
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
        int r = pintarBranco(*jogo, posicao);
        if ((*jogo)->modoAjudaAtiva) ajudar(*jogo);

        return r;
    }
    // Comando para riscar
    if (tipoComando == 'r') {
        int r = riscar(*jogo, posicao);
        if ((*jogo)->modoAjudaAtiva) ajudar(*jogo);

        return r;
    }
    
    

    // Se não corresponde a nenhum comando válido
    printf("Comando inválido: %s\n", comando);
    printf("Comandos válidos:\n");
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
    
    return -1;
}
