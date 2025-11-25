#ifndef RANKING_H
#define RANKING_H

// tamanho máximo do nome do jogador (sem o '\0')
#define RANKING_MAX_NAME 20

// Uma entrada do ranking: nome + melhor score
typedef struct {
    char name[RANKING_MAX_NAME + 1];
    int  score;
} RankingEntry;

// Estrutura do ranking (top N jogadores)
typedef struct {
    int capacity;        // capacidade máxima (ex.: 5)
    int count;           // quantos estão ocupados
    RankingEntry *entries;
} Ranking;

// Cria ranking com "capacity" posições (ex.: 5)
Ranking *ranking_create(int capacity);

// Registra/atualiza a melhor pontuação de um jogador.
// - Se o jogador já existe e o novo score for maior, atualiza.
// - Se não existe, tenta inseri-lo mantendo ordem decrescente.
// - Usa recursão pra ordenar internamente.
void ranking_add_or_update(Ranking *ranking, const char *name, int score);

// Desenha o ranking numa área da tela
void ranking_draw(const Ranking *ranking, int startX, int startY);

// Libera memória
void ranking_free(Ranking *ranking);

#endif // RANKING_H