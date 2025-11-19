#include <stdlib.h>
#include <stdio.h>
#include "ranking.h"
#include "screen.h"

// insere score em ordem decrescente (recursivo)
static void insert_sorted_recursive(int *scores, int index, int score) {
    if (index < 0 || scores[index] >= score) {
        scores[index + 1] = score;
        return;
    }

    scores[index + 1] = scores[index];
    insert_sorted_recursive(scores, index - 1, score);
}

Ranking *ranking_create(int capacity) {
    Ranking *r = (Ranking *)malloc(sizeof(Ranking));
    if (!r) return NULL;

    r->scores = (int *)malloc(sizeof(int) * capacity);
    if (!r->scores) {
        free(r);
        return NULL;
    }

    r->capacity = capacity;
    r->count = 0;

    for (int i = 0; i < capacity; ++i)
        r->scores[i] = 0;

    return r;
}

void ranking_add_score(Ranking *ranking, int score) {
    if (!ranking || score <= 0) return;

    if (ranking->count < ranking->capacity) {
        insert_sorted_recursive(ranking->scores, ranking->count - 1, score);
        ranking->count++;
    } else {
        int lastIdx = ranking->capacity - 1;
        if (score > ranking->scores[lastIdx]) {
            insert_sorted_recursive(ranking->scores, lastIdx - 1, score);
        }
    }
}

void ranking_draw(const Ranking *ranking, int startX, int startY) {
    if (!ranking) return;

    screenSetColor(YELLOW, BLACK);
    screenGotoxy(startX, startY);
    printf("TOP %d PONTUACOES:", ranking->capacity);

    for (int i = 0; i < ranking->count; ++i) {
        screenGotoxy(startX, startY + 1 + i);
        printf("%2d) %d", i + 1, ranking->scores[i]);
    }

    if (ranking->count == 0) {
        screenGotoxy(startX, startY + 1);
        printf("Nenhuma ainda :(");
    }
}

void ranking_free(Ranking *ranking) {
    if (!ranking) return;
    free(ranking->scores);
    free(ranking);
}
