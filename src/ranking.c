#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ranking.h"
#include "screen.h"

// insere uma entrada em ordem decrescente (recursivo)
static void insert_sorted_recursive(RankingEntry *arr, int index, RankingEntry e) {
    if (index < 0 || arr[index].score >= e.score) {
        arr[index + 1] = e;
        return;
    }

    arr[index + 1] = arr[index];
    insert_sorted_recursive(arr, index - 1, e);
}

// insertion sort recursivo para ordenar o vetor inteiro
static void sort_recursive(RankingEntry *arr, int n) {
    if (n <= 1) return;

    // ordena os n-1 primeiros
    sort_recursive(arr, n - 1);

    // insere o último na posição correta
    RankingEntry last = arr[n - 1];
    insert_sorted_recursive(arr, n - 2, last);
}

Ranking *ranking_create(int capacity) {
    Ranking *r = (Ranking *)malloc(sizeof(Ranking));
    if (!r) return NULL;

    r->entries = (RankingEntry *)malloc(sizeof(RankingEntry) * capacity);
    if (!r->entries) {
        free(r);
        return NULL;
    }

    r->capacity = capacity;
    r->count    = 0;

    for (int i = 0; i < capacity; ++i) {
        r->entries[i].name[0] = '\0';
        r->entries[i].score   = 0;
    }

    return r;
}

void ranking_add_or_update(Ranking *ranking, const char *name, int score) {
    if (!ranking || !name || score <= 0) return;

    // procura se o jogador já está no ranking
    int existingIndex = -1;
    for (int i = 0; i < ranking->count; ++i) {
        if (strncmp(ranking->entries[i].name, name, RANKING_MAX_NAME) == 0) {
            existingIndex = i;
            break;
        }
    }

    if (existingIndex >= 0) {
        // já existe: só atualiza se o novo score for maior
        if (score > ranking->entries[existingIndex].score) {
            ranking->entries[existingIndex].score = score;
            sort_recursive(ranking->entries, ranking->count);
        }
        return;
    }

    // monta nova entrada
    RankingEntry e;
    strncpy(e.name, name, RANKING_MAX_NAME);
    e.name[RANKING_MAX_NAME] = '\0';
    e.score = score;

    if (ranking->count < ranking->capacity) {
        // ainda há espaço: insere e ordena
        ranking->entries[ranking->count] = e;
        ranking->count++;
        sort_recursive(ranking->entries, ranking->count);
    } else {
        // ranking cheio: só entra se for maior que o último
        int lastIdx = ranking->capacity - 1;
        if (score > ranking->entries[lastIdx].score) {
            ranking->entries[lastIdx] = e;
            sort_recursive(ranking->entries, ranking->capacity);
        }
    }
}

void ranking_draw(const Ranking *ranking, int startX, int startY) {
    if (!ranking) return;

    screenSetColor(YELLOW, BLACK);
    screenGotoxy(startX, startY);
    printf("TOP %d JOGADORES:", ranking->capacity);

    if (ranking->count == 0) {
        screenGotoxy(startX, startY + 1);
        printf("Nenhum registro ainda :(");
        return;
    }

    for (int i = 0; i < ranking->count; ++i) {
        screenGotoxy(startX, startY + 1 + i);
        printf("%2d) %-20s  %d",
               i + 1,
               ranking->entries[i].name,
               ranking->entries[i].score);
    }
}

void ranking_free(Ranking *ranking) {
    if (!ranking) return;
    free(ranking->entries);
    free(ranking);
}