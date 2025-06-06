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
    jogo->agrupandoMovimentos = 0;
    jogo->grupoMovimentos = NULL;


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

    for (int j = 0; j < jogo->colunas; j++) {
        int ch = fgetc(input);
        if (ch == EOF || ch == '\n') {
            if (j < jogo->colunas - 1) {
                j--;
            }
        } else {
            jogo->tabuleiro[i][j] = (char)ch;
        }
    }
    jogo->tabuleiro[i][jogo->colunas] = '\0';

    int ch;
    do {
        ch = fgetc(input);
    } while (ch != EOF && ch != '\n');
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
    int erroLeitura = 0;
    for (int i = 0; i < numMovimentos && !erroLeitura; i++) {
        int linha, coluna;
        char estadoAnterior;

        if (fscanf(input, "%d %d %c\n", &linha, &coluna, &estadoAnterior) == 3) {
            Movimento *novoMovimento = malloc(sizeof(Movimento));
            if (!novoMovimento) {
                printf("Erro na alocação de memória para o movimento %d.\n", i);
            } else {
                novoMovimento->linha = linha;
                novoMovimento->coluna = coluna;
                novoMovimento->estadoAnterior = estadoAnterior;
                novoMovimento->proximo = NULL;
                movimentosTemp[i] = novoMovimento;
            }
        } else {
            printf("Erro ao ler o movimento %d do histórico.\n", i);
            erroLeitura = 1;
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
        jogo->tabuleiro[linha][coluna] = jogo->tabuleiro[linha][coluna] - 32; //converter para maiuscula
    }
    
    return 0;
}

int riscar(Jogo *jogo, char *coordenada) {
    if (!jogo || !coordenada || strlen(coordenada) < 2) return -1;
    
    int coluna = coordenada[0] - 'a';
    int linha = coordenada[1] - '1';
    
    // Validação das coordenadas
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
        
        // Liberta a memória do histórico de movimentos
        freeHistoricoMovimentos(jogo->historicoMovimentos);
        
        free(jogo);
    }
}



// Funções etapa 2 ===================================================================================

void registarMovimento(Jogo *jogo, int linha, int coluna, char estadoAnterior) {
    if (!jogo) return;
    
    Movimento *novoMovimento = malloc(sizeof(Movimento));
    if (!novoMovimento) {
        printf("Erro na alocação de memória para o histórico.\n");
        return;
    }
    
    novoMovimento->linha = linha;
    novoMovimento->coluna = coluna;
    novoMovimento->estadoAnterior = estadoAnterior;
    novoMovimento->proximo = NULL;
    novoMovimento->grupoInterno = NULL; // Inicializa o campo de grupo interno
    
    if (jogo->agrupandoMovimentos) {
        // Adiciona ao grupo de movimentos temporário
        novoMovimento->proximo = jogo->grupoMovimentos;
        jogo->grupoMovimentos = novoMovimento;
    } else {
        // Adiciona diretamente ao histórico
        novoMovimento->proximo = jogo->historicoMovimentos;
        jogo->historicoMovimentos = novoMovimento;
    }
}


int desfazerMovimento(Jogo *jogo) {
    if (!jogo || !jogo->historicoMovimentos) {
        printf("Não há movimento para desfazer.\n");
        return -1;
    }

    Movimento *ultimoMovimento = jogo->historicoMovimentos;
    
    // Verifica se é um movimento de grupo (gerado pelo comando 'A')
    if (ultimoMovimento->linha == -1 && ultimoMovimento->coluna == -1 && 
        ultimoMovimento->estadoAnterior == 'A') {
        
        printf("A desfazer todos os movimentos da ajuda automática...\n");
        
        // Remove o movimento de grupo do histórico principal
        jogo->historicoMovimentos = ultimoMovimento->proximo;
        
        // Conta quantos movimentos serão desfeitos
        int contadorMovimentos = 0;
        Movimento *temp = ultimoMovimento->grupoInterno;
        while (temp != NULL) {
            contadorMovimentos++;
            temp = temp->proximo;
        }
        
        // Desfaz todos os movimentos no grupo (que já estão na ordem correta)
        Movimento *movimentoGrupo = ultimoMovimento->grupoInterno;
        
        while (movimentoGrupo != NULL) {
            // Restaura o estado anterior da célula
            jogo->tabuleiro[movimentoGrupo->linha][movimentoGrupo->coluna] = movimentoGrupo->estadoAnterior;
            
            printf("  Desfeito: (%c,%d) voltou para '%c'\n", 
                   movimentoGrupo->coluna + 'a',
                   movimentoGrupo->linha + 1,
                   movimentoGrupo->estadoAnterior);
            
            // Passa para o próximo movimento do grupo
            Movimento *tempMovimento = movimentoGrupo;
            movimentoGrupo = movimentoGrupo->proximo;
            free(tempMovimento);
        }
        
        free(ultimoMovimento); // Liberta o movimento de grupo
        printf("Todos os %d movimentos da ajuda automática foram desfeitos.\n", contadorMovimentos);
        printf("Tabuleiro restaurado ao estado anterior ao comando 'A'.\n");
        return 0;
    }
    
    // Caso seja um movimento normal individual
    char valorAtual = jogo->tabuleiro[ultimoMovimento->linha][ultimoMovimento->coluna];
    jogo->tabuleiro[ultimoMovimento->linha][ultimoMovimento->coluna] = ultimoMovimento->estadoAnterior;
    
    printf("Movimento desfeito na posição (%c,%d): '%c' voltou para '%c'.\n",
           ultimoMovimento->coluna + 'a',
           ultimoMovimento->linha + 1,
           valorAtual,
           ultimoMovimento->estadoAnterior);

    // Remove o movimento do histórico
    jogo->historicoMovimentos = ultimoMovimento->proximo;
    free(ultimoMovimento);
    return 0;
}

// Verifica se há duplicados de letras brancas (A-Z) numa linha
int verificarDuplicadosLinha(Jogo *jogo, int linha) {
    int contagem[26] = {0};

    for (int j = 0; j < jogo->colunas; j++) {
        char c = toupper(jogo->tabuleiro[linha][j]);
        if (c >= 'A' && c <= 'Z') {
            if (contagem[c - 'A']++ > 0) {
                return 1; // Duplicado encontrado
            }
        }
    }
    return 0; // Sem duplicados
}

// Verifica se há duplicados de letras brancas (A-Z) numa coluna
int verificarDuplicadosColuna(Jogo *jogo, int coluna) {
    int contagem[26] = {0};

    for (int i = 0; i < jogo->linhas; i++) {
        char c = toupper(jogo->tabuleiro[i][coluna]);
        if (c >= 'A' && c <= 'Z') {
            if (contagem[c - 'A']++ > 0) {
                return 1; // Duplicado encontrado
            }
        }
    }
    return 0; // Sem duplicados
}

// Devolve o número de adjacências de casas riscadas à posição (i,j)
int contarRiscadasAdjacentes(Jogo *jogo, int i, int j) {
    int count = 0;
    int di[] = {0, 1};  // direita e abaixo
    int dj[] = {1, 0};

    for (int d = 0; d < 2; d++) {
        int ni = i + di[d];
        int nj = j + dj[d];

        if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
            if (jogo->tabuleiro[ni][nj] == '#') {
                count++;
            }
        }
    }

    return count;
}

void aplicarAosVizinhos(Jogo *jogo, int i, int j, void (*acao)(Jogo *, int, int, void *), void *extra) {
    int di[] = {-1, 1, 0, 0};
    int dj[] = {0, 0, -1, 1};

    for (int d = 0; d < 4; d++) {
        int ni = i + di[d];
        int nj = j + dj[d];

        if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
            acao(jogo, ni, nj, extra);
        }
    }
}

void verificarVizinhoBranco(Jogo *jogo, int i, int j, void *violacoesPtr) {
    int *violacoes = (int *)violacoesPtr;
    char c = jogo->tabuleiro[i][j];
    if (c != '#' && !(c >= 'A' && c <= 'Z')) {
        printf("Violação: Vizinho (%c%d) de casa riscada não é branco\n", 'a'+j, i+1);
        (*violacoes)++;
    }
}

void pintarVizinhoSeMinuscula(Jogo *jogo, int i, int j, void *alteracoesPtr) {
    int *alteracoes = (int *)alteracoesPtr;
    if (islower(jogo->tabuleiro[i][j])) {
        char coord[3] = {'a'+j, '1'+i, '\0'};
        if (pintarBranco(jogo, coord) == 0) {
            printf("Ajuda: pintado %s (vizinho de riscada)\n", coord);
            (*alteracoes)++;
        }
    }
}




int verificarRestricoes(Jogo *jogo) {
    if (!jogo) return -1;
    
    int violacoes = 0;
    
    // Verificação de símbolos únicos em linhas/colunas
    for (int i = 0; i < jogo->linhas; i++) {
    if (verificarDuplicadosLinha(jogo, i)) {
        printf("Violação: Duplicados na linha %d\n", i+1);
        violacoes++;
    }
}
    
    for (int j = 0; j < jogo->colunas; j++) {
    if (verificarDuplicadosColuna(jogo, j)) {
        printf("Violação: Duplicados na coluna %c\n", 'a'+j);
        violacoes++;
    }
}


    // Verifica a restrição: casas riscadas não podem ser adjacentes
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] == '#') {
                int adjacentes = contarRiscadasAdjacentes(jogo, i, j);
                aplicarAosVizinhos(jogo, i, j, verificarVizinhoBranco, &violacoes);
                if (adjacentes > 0) {
                    printf("Violação: Casas riscadas adjacentes a (%c%d)\n", 'a'+j, i+1);
                    violacoes += adjacentes;
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
        
        // Se for um movimento de grupo, liberta os movimentos internos primeiro
        if (temp->linha == -1 && temp->coluna == -1 && temp->estadoAnterior == 'G') {
            freeHistoricoMovimentos(temp->grupoInterno);
        }
        
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

void iniciarAgrupamentoMovimentos(Jogo *jogo) {
    if (!jogo) return;
    jogo->agrupandoMovimentos = 1;
    jogo->grupoMovimentos = NULL;
}

int simulaRiscarEVerificaConectividade(Jogo *jogo, int i, int j) {
    char original = jogo->tabuleiro[i][j];
    jogo->tabuleiro[i][j] = '#';
    int resultado = verificarConectividadeBrancas(jogo);
    jogo->tabuleiro[i][j] = original;
    return resultado;
}

int existeDuplicadoNaLinha(Jogo *jogo, int linha, int colunaIgnorar, char celula) {
    for (int j = 0; j < jogo->colunas; j++) {
        if (j != colunaIgnorar && jogo->tabuleiro[linha][j] == celula)
            return 1;
    }
    return 0;
}

int existeDuplicadoNaColuna(Jogo *jogo, int coluna, int linhaIgnorar, char celula) {
    for (int i = 0; i < jogo->linhas; i++) {
        if (i != linhaIgnorar && jogo->tabuleiro[i][coluna] == celula)
            return 1;
    }
    return 0;
}

void riscarDuplicadosLinha(Jogo *jogo, int linha, char letra, int *alteracoes) {
    for (int j = 0; j < jogo->colunas; j++) {
        if (tolower(jogo->tabuleiro[linha][j]) == letra && islower(jogo->tabuleiro[linha][j])) {
            char coord[3] = {'a'+j, '1'+linha, '\0'};
            if (riscar(jogo, coord) == 0) {
                printf("Ajuda: riscado %s (duplicado na linha)\n", coord);
                (*alteracoes)++;
            }
        }
    }
}

void riscarDuplicadosColuna(Jogo *jogo, int coluna, char letra, int *alteracoes) {
    for (int i = 0; i < jogo->linhas; i++) {
        if (tolower(jogo->tabuleiro[i][coluna]) == letra && islower(jogo->tabuleiro[i][coluna])) {
            char coord[3] = {'a'+coluna, '1'+i, '\0'};
            if (riscar(jogo, coord) == 0) {
                printf("Ajuda: riscado %s (duplicado na coluna)\n", coord);
                (*alteracoes)++;
            }
        }
    }
}


void finalizarAgrupamentoMovimentos(Jogo *jogo) {
    if (!jogo || !jogo->agrupandoMovimentos) return;
    
    // Se não houver movimentos no grupo, apenas desativa o agrupamento
    if (!jogo->grupoMovimentos) {
        jogo->agrupandoMovimentos = 0;
        printf("Agrupamento finalizado (nenhum movimento realizado).\n");
        return;
    }
    
    // Criar um movimento especial para representar o grupo
    Movimento *movimentoGrupo = malloc(sizeof(Movimento));
    if (!movimentoGrupo) {
        printf("Erro na alocação de memória para agrupamento.\n");
        return;
    }
    
    movimentoGrupo->linha = -1;  // Valor especial para indicar que é um grupo
    movimentoGrupo->coluna = -1;
    movimentoGrupo->estadoAnterior = 'A';  // 'A' de ajuda automática
    movimentoGrupo->proximo = jogo->historicoMovimentos;
    movimentoGrupo->grupoInterno = NULL; // Inicializar como NULL
    movimentoGrupo->grupoInterno = jogo->grupoMovimentos;
    
    // Adiciona o movimento especial ao histórico
    jogo->historicoMovimentos = movimentoGrupo;
    
    // Conta os movimentos para feedback
    int numMovimentos = 0;
    Movimento *temp = jogo->grupoMovimentos;
    while (temp != NULL) {
        numMovimentos++;
        temp = temp->proximo;
    }
    
    printf("Agrupamento finalizado: %d movimentos registados como grupo.\n", numMovimentos);
    printf("Use 'd' para desfazer todos os movimentos deste grupo de uma vez.\n");
    
    // Reinicializa o estado de agrupamento
    jogo->agrupandoMovimentos = 0;
    jogo->grupoMovimentos = NULL;
}

int ajudar(Jogo *jogo) {
    if (!jogo) return -1;
    
    int alteracoesFeitas = 0;
    
    // 1. Regra: riscar letras iguais a uma branca na linha ou coluna
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            char atual = jogo->tabuleiro[i][j];
            if (atual >= 'A' && atual <= 'Z') { // Letra branca (maiúscula)
                char letraMinuscula = atual + 32; // Converte para minúscula correspondente
                
                // Verifica linha - riscar todas as letras minúsculas iguais na mesma linha
                for (int k = 0; k < jogo->colunas; k++) {
                    if (k != j && jogo->tabuleiro[i][k] == letraMinuscula) {
                        char coord[3] = { 'a' + k, '1' + i, '\0' };
                        printf("Ajuda: riscar %s (igual a branca %c na linha %d)\n", coord, atual, i+1);
                        riscar(jogo, coord);
                        alteracoesFeitas++;
                    }
                }
                
                // Verifica coluna - riscar todas as letras minúsculas iguais na mesma coluna
                for (int k = 0; k < jogo->linhas; k++) {
                    if (k != i && jogo->tabuleiro[k][j] == letraMinuscula) {
                        char coord[3] = { 'a' + j, '1' + k, '\0' };
                        printf("Ajuda: riscar %s (igual a branca %c na coluna %c)\n", coord, atual, 'a'+j);
                        riscar(jogo, coord);
                        alteracoesFeitas++;
                    }
                }
            }
        }
    }

    // Se já fizemos alterações, retornar para não aplicar outras regras na mesma iteração
    if (alteracoesFeitas > 0) {
        return alteracoesFeitas;
    }

    // 2. Regra: pintar de branco todas as casas vizinhas de uma casa riscada
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] == '#') {
                int di[] = {-1, 1, 0, 0};
                int dj[] = {0, 0, -1, 1};
                for (int d = 0; d < 4; d++) {
                    int ni = i + di[d], nj = j + dj[d];
                    if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
                        char viz = jogo->tabuleiro[ni][nj];
                        if (viz >= 'a' && viz <= 'z') {
                            char coord[3] = { 'a' + nj, '1' + ni, '\0' };
                            printf("Ajuda: pintar %s (vizinho de casa riscada em %c%d)\n", coord, 'a'+j, i+1);
                            pintarBranco(jogo, coord);
                            alteracoesFeitas++;
                        }
                    }
                }
            }
        }
    }

    // Se já fizemos alterações, retornar
    if (alteracoesFeitas > 0) {
        return alteracoesFeitas;
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
                    alteracoesFeitas++;
                    // Retorna imediatamente após encontrar uma casa que evita isolamento
                    return alteracoesFeitas;
                }
            }
        }
    }
    
    if (alteracoesFeitas == 0) {
        printf("Nenhuma jogada inferida disponível no momento.\n");
    }
    
    return alteracoesFeitas;
}


Jogo* copiarJogo(Jogo* original) {
    if (!original) return NULL;
    
    Jogo* copia = malloc(sizeof(Jogo));
    if (!copia) return NULL;
    
    copia->linhas = original->linhas;
    copia->colunas = original->colunas;
    copia->modoAjudaAtiva = original->modoAjudaAtiva;
    copia->historicoMovimentos = NULL;
    copia->agrupandoMovimentos = original->agrupandoMovimentos;
    copia->grupoMovimentos = NULL;
    
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
    int falhaAloc = 0;

    while (orig && !falhaAloc) {
        Movimento *novo = malloc(sizeof(Movimento));
        if (!novo) {
            falhaAloc = 1;
        } else {
            novo->linha = orig->linha;
            novo->coluna = orig->coluna;
            novo->estadoAnterior = orig->estadoAnterior;
            novo->proximo = NULL;
            *dest = novo;
            dest = &((*dest)->proximo);
        }
        orig = orig->proximo;
    }
}

// Função auxiliar para verificar se um movimento é válido
int movimentoValido(Jogo *jogo, int linha, int coluna) {
    if (!jogo) return 0;

    char celula = jogo->tabuleiro[linha][coluna];

    if (celula >= 'A' && celula <= 'Z') {
        if (existeDuplicadoNaLinha(jogo, linha, coluna, celula)) return 0;
        if (existeDuplicadoNaColuna(jogo, coluna, linha, celula)) return 0;
    } else if (celula == '#') {
        int vizinhos[][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
        for (int v = 0; v < 4; v++) {
            int ni = linha + vizinhos[v][0];
            int nj = coluna + vizinhos[v][1];

            if (ni >= 0 && ni < jogo->linhas && nj >= 0 && nj < jogo->colunas) {
                char vizinho = jogo->tabuleiro[ni][nj];
                int ignorar = (vizinho >= 'a' && vizinho <= 'z');
                if (!ignorar && vizinho == '#') {
                    return 0;
                }
            }
        }
    }

    return 1;
}

int resolverJogo(Jogo *jogo) {
    if (!jogo) {
        printf("Erro: Jogo inválido.\n");
        return -1;
    }
    
    printf("Iniciando resolução do jogo...\n");
    
    // Fase 1: Resetar o tabuleiro para o estado inicial
    printf("Resetando tabuleiro para o estado inicial...\n");
    int movimentosDesfeitos = 0;
    
    // Desfazer todos os movimentos até voltar ao estado inicial
    while (jogo->historicoMovimentos != NULL) {
        if (desfazerMovimento(jogo) == 0) {
            movimentosDesfeitos++;
        } else {
            printf("Erro ao desfazer movimento.\n");
            return -1;
        }
    }
    
    printf("Total de movimentos desfeitos: %d\n", movimentosDesfeitos);
    printf("Tabuleiro resetado para o estado inicial.\n\n");
    
    // Verificar se realmente está no estado inicial (todas minúsculas)
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->tabuleiro[i][j] != tolower(jogo->tabuleiro[i][j]) && 
                jogo->tabuleiro[i][j] != '#') {
                printf("Erro: Tabuleiro não está no estado inicial após reset.\n");
                return -1;
            }
        }
    }
    
    // Fase 2: Resolver usando backtracking
    printf("Iniciando resolução por backtracking...\n");
    
    // Criar uma cópia do jogo para não afetar o original durante tentativas
    Jogo *jogoTentativa = copiarJogo(jogo);
    if (!jogoTentativa) {
        printf("Erro ao criar cópia do jogo.\n");
        return -1;
    }
    
    // Chamar função de backtracking
    int resultado = backtrackingResolver(jogoTentativa);
    
    if (resultado == 1) {
        // Sucesso: copiar solução de volta para o jogo original
        printf("Solução encontrada! Aplicando ao jogo...\n");
        
        // Aplicar cada movimento encontrado ao jogo original
        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                char estadoOriginal = jogo->tabuleiro[i][j];
                char estadoSolucao = jogoTentativa->tabuleiro[i][j];
                
                // Se o estado mudou, registar o movimento
                if (estadoOriginal != estadoSolucao) {
                    jogo->tabuleiro[i][j] = estadoSolucao;
                    registarMovimento(jogo, i, j, estadoOriginal);
                }
            }
        }
        
        printf("Jogo resolvido com sucesso!\n");
        
        // Verificar se a solução está correta
        if (verificarVitoria(jogo)) {
            printf("Verificação: Solução válida!\n");
            freeJogo(jogoTentativa);
            return 0; // Retorna 0 para que a main desenhe o jogo e verifique vitória
        } else {
            printf("Aviso: Solução pode não estar completamente correta.\n");
            freeJogo(jogoTentativa);
            return -1; // Retorna -1 para indicar erro
        }
        
    } else if (resultado == 0) {
        printf("Nenhuma solução encontrada para este tabuleiro.\n");
        freeJogo(jogoTentativa);
        return -1; 
    } else {
        printf("Erro durante a resolução do jogo.\n");
        freeJogo(jogoTentativa);
        return -1; // Retorna -1 para indicar erro
    }
}

// Função auxiliar melhorada para backtracking
int backtrackingResolver(Jogo *jogo) {
    if (!jogo) return -1;
    
    // Verificar se o jogo já está resolvido
    if (verificarVitoria(jogo)) {
        return 1; // Solução encontrada
    }
    
    // Encontrar a próxima casa vazia (minúscula) para processar
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            char celula = jogo->tabuleiro[i][j];
            
            // Se é uma letra minúscula, podemos tentar pintá-la ou riscá-la
            if (celula >= 'a' && celula <= 'z') {
                
                // Tentar pintar de branco (maiúscula)
                char estadoOriginal = jogo->tabuleiro[i][j];
                jogo->tabuleiro[i][j] = toupper(celula);
                
                // Verificar se este movimento é válido
                if (movimentoValido(jogo, i, j)) {
                    // Recursão: tentar resolver o resto
                    int resultado = backtrackingResolver(jogo);
                    if (resultado == 1) {
                        return 1; // Solução encontrada
                    }
                }
                
                // Desfazer movimento (pintar de branco)
                jogo->tabuleiro[i][j] = estadoOriginal;
                
                // Tentar riscar (#)
                jogo->tabuleiro[i][j] = '#';
                
                // Verificar se este movimento é válido
                if (movimentoValido(jogo, i, j)) {
                    // Recursão: tentar resolver o resto
                    int resultado = backtrackingResolver(jogo);
                    if (resultado == 1) {
                        return 1; // Solução encontrada
                    }
                }
                
                // Desfazer movimento (riscar)
                jogo->tabuleiro[i][j] = estadoOriginal;
                
                // Se nenhuma das opções funcionou, retornar falha
                return 0;
            }
        }
    }
    
    // Se chegamos aqui, todas as casas foram processadas
    // Verificar se é uma solução válida
    return verificarVitoria(jogo) ? 1 : 0;
}


// função para verificar se o jogo está completamente resolvido
int verificarVitoria(Jogo *jogo) {
    if (!jogo) return 0;
    
    // Verifica se todas as células estão resolvidas (sem letras minúsculas)
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (islower(jogo->tabuleiro[i][j])) {
                return 0; // Ainda existem células não resolvidas
            }
        }
    }
    
    // Verifica se o tabuleiro está em um estado válido
    if (verificarRestricoes(jogo) != 0) {
        return 0; // Há violações de restrições
    }
    
    return 1; // Jogo resolvido e válido
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
            // Liberta o jogo anterior se existir
            if (*jogo) {
                for (int i = 0; i < (*jogo)->linhas; i++) {
                    free((*jogo)->tabuleiro[i]);
                }
                free((*jogo)->tabuleiro);
                
                // Liberta a memória do histórico de movimentos
                freeHistoricoMovimentos((*jogo)->historicoMovimentos);
                
                free(*jogo);
                *jogo = NULL;
            }
            
            // Carrega o novo jogo
            *jogo = carregarJogo(arquivo);
            return (*jogo != NULL) ? 0 : -1;
        }
    }

    // Para os demais comandos, é necessário verificar se o jogo existe
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

    // comando "a" (ajudar)
    if (strcmp(comando, "a") == 0) {
        if (!(*jogo)) {
            printf("Jogo não carregado. Use 'l <arquivo>' para carregar um jogo.\n");
            return -1;
        }
        ajudar(*jogo);
        return 0;
    }



    // comando "A" (ajuda automatica)
    if (strcmp(comando, "A") == 0) {
        if (!(*jogo)) {
            printf("Jogo não carregado. Use 'l <arquivo>' para carregar um jogo.\n");
            return -1;
        }
        
        printf("Executando ajuda automática contínua...\n");
        int totalAlteracoes = 0;
        int iteracao = 1;
        int alteracoesNaIteracao;
        
        // Inicia o agrupamento de movimentos
        iniciarAgrupamentoMovimentos(*jogo);
        
        int resolvido = 0;
        do {
            printf("\n--- Iteração %d ---\n", iteracao);
            
            alteracoesNaIteracao = ajudar(*jogo);
            
            if (alteracoesNaIteracao > 0) {
                totalAlteracoes += alteracoesNaIteracao;
                printf("Alterações feitas nesta iteração: %d\n", alteracoesNaIteracao);

                printf("\nTabuleiro após iteração %d:\n", iteracao);
                desenhaJogo(*jogo);

                if (verificarVitoria(*jogo)) {
                    printf("Jogo completamente resolvido!\n");
                    resolvido = 1;  // Encerrar o loop
                }

                iteracao++;
            } else {
                printf("Nenhuma alteração possível nesta iteração.\n");
            }

        } while (alteracoesNaIteracao > 0 && !resolvido);
        
        // Finaliza o agrupamento de movimentos
        finalizarAgrupamentoMovimentos(*jogo);
        
        printf("\n=== Resumo da Ajuda Automática ===\n");
        printf("Total de iterações executadas: %d\n", iteracao);
        printf("Total de alterações realizadas: %d\n", totalAlteracoes);
        
        if (totalAlteracoes > 0) {
            printf("Processo de ajuda automática concluído.\n");
            
            // Verifica o estado final do jogo
            printf("\nA verificar estado final...\n");
            if (verificarVitoria(*jogo)) {
                printf("Parabéns! O jogo foi completamente resolvido!\n");
            } else {
                // Mostra se há violações
                int violacoes = verificarRestricoes(*jogo);
                if (violacoes == 0) {
                    printf("Não há violações, mas o jogo ainda não está completo.\n");
                }
            }
        } else {
            printf("Nenhuma alteração foi possível. O tabuleiro permanece inalterado.\n");
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
