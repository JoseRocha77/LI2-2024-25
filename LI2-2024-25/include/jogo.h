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
} Jogo;

Jogo* carregarJogo (char *arquivo);

void desenhaJogo (Jogo *jogo);

int pintarBranco (Jogo *jogo, char *coordenada);

int riscar (Jogo *jogo, char *coordenada);

void freeJogo(Jogo *jogo);

int processarComandos(Jogo **jogo, char *comando);

// Funções novas para a etapa 2
int desfazerMovimento(Jogo *jogo);

int verificarRestricoes(Jogo *jogo);

void registrarMovimento(Jogo *jogo, int linha, int coluna, char estadoAnterior);

void liberarHistoricoMovimentos(Movimento *historico);

#endif