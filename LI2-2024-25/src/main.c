#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/jogo.h"

int main(void) {
    Jogo *jogo = carregarJogo("input.txt");
    desenhaJogo(jogo);
    return 0;
}
