#ifndef JOGO_H
#define JOGO_H

typedef struct Movimento {
    int linha;
    int coluna;
    char estadoAnterior;
    struct Movimento *proximo;
    struct Movimento *grupoInterno; // Para armazenar movimentos agrupados
} Movimento;

typedef struct{
    char **tabuleiro;
    int linhas;
    int colunas;
    Movimento *historicoMovimentos;
    int modoAjudaAtiva;
    int agrupandoMovimentos;    // Nova flag para indicar agrupamento
    Movimento *grupoMovimentos; // Nova lista para movimentos temporários
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

void iniciarAgrupamentoMovimentos(Jogo *jogo);

void finalizarAgrupamentoMovimentos(Jogo *jogo);

int ajudar(Jogo *jogo);

int backtrackingResolver(Jogo *jogo);

Jogo* copiarJogo(Jogo* original);

void restaurarJogo(Jogo* destino, Jogo* origem);

int resolverJogo(Jogo *jogo);

int verificarVitoria(Jogo *jogo);

// Função principal
int processarComandos(Jogo **jogo, char *comando);


#endif
