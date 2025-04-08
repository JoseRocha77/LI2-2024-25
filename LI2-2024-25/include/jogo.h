#ifndef JOGO_H
#define JOGO_H

typedef struct{
    char **tabuleiro;
    int linhas;
    int colunas;
} Jogo;

Jogo* carregarJogo (char *arquivo);

void desenhaJogo (Jogo *jogo);

int pintarBranco (Jogo *jogo, char *coordenada);

int riscar (Jogo *jogo, char *coordenada);

void freeJogo(Jogo *jogo);

int processarComandos(Jogo **jogo, char *comando);

#endif 


