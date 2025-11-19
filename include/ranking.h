#ifndef RANKING_H
#define RANKING_H

// Estrutura do ranking (top 5)
typedef struct {
    int  capacity; // capacidade máxima
    int  count;    // elementos válidos
    int *scores;   // vetor dinâmico
} Ranking;

// Cria ranking com "capacity" posições (ex.: 5)
Ranking *ranking_create(int capacity);

// Adiciona um score ao ranking (ordem decrescente usando recursão)
void ranking_add_score(Ranking *ranking, int score);

// Desenha o ranking numa área da tela
void ranking_draw(const Ranking *ranking, int startX, int startY);

// Libera memória
void ranking_free(Ranking *ranking);

#endif // RANKING_H