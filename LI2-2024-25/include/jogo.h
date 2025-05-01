#ifndef JOGO_H
#define JOGO_H

typedef struct Movimento {
    int linha;
    int coluna;
    char estadoAnterior;
    struct Movimento *proximo;
} Movimento;

typedef struct{
    char **tabuleiro;
    int linhas;
    int colunas;
    Movimento *historicoMovimentos;
    int modoAjudaAtiva;
} Jogo;

// Funções etapa 1
Jogo* carregarJogo (char *arquivo);

void carregarHistoricoMovimentos(FILE *input, Jogo *jogo);

int gravarJogo(Jogo *jogo, char *arquivo);

void desenhaJogo (Jogo *jogo);

int pintarBranco (Jogo *jogo, char *coordenada);

int riscar (Jogo *jogo, char *coordenada);

void freeJogo(Jogo *jogo);

// Funções etapa 2

void registarMovimento(Jogo *jogo, int linha, int coluna, char estadoAnterior);

int desfazerMovimento(Jogo *jogo);

int verificarRestricoes(Jogo *jogo);

void freeHistoricoMovimentos(Movimento *historico);

// Funções etapa 3

void dfs(Jogo *jogo, int **visitado, int *visitadas, int linha, int coluna);

int verificarConectividadeBrancas(Jogo *jogo);

// Funções etapa 4

int ajudar(Jogo *jogo);

// Função principal
int processarComandos(Jogo **jogo, char *comando);


#endif
