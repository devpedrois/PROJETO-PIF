#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "logic.h"
#include "ranking.h"

// Estado completo do jogo
typedef struct {
    // área de jogo (usando constantes da screen.h)
    int minX, maxX;
    int minY, maxY;

    // Raquete
    int paddleX;
    int paddleY;
    int paddleHeight;
    int prevPaddleY; 

    // Bola
    float ballX;
    float ballY;
    float ballVX;
    float ballVY;
    float prevBallX;
    float prevBallY;
    float speedMultiplier;

    // Lógica proposicional
    LogicState logicState;
    Expr      *formula;

    // Pontuação
    int  currentScore;       // score atual (combo)
    int  bestScore;          // melhor score dessa "sessão" do jogador

    // Jogador
    char    playerName[RANKING_MAX_NAME + 1];
    Ranking *ranking;        // ranking global (top 5)

    int running; // 1 enquanto jogo ativo
} GameState;

// Inicializa libs, estado do jogo e roda o loop principal
void game_run(void);

#endif // GAME_H