#include <stdio.h>
#include "game.h"
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define BALL_SPEED_X   -0.8f
#define BALL_SPEED_Y    0.4f
#define PADDLE_HEIGHT   5
#define RANKING_SIZE    5

// ---------- helpers internos ----------

static void game_init_state(GameState *g) {
    g->minX = SCRSTARTX;
    g->maxX = SCRENDX;
    g->minY = SCRSTARTY;
    g->maxY = SCRENDY;

    // Raquete na direita
    g->paddleX = g->maxX - 2;
    g->paddleHeight = PADDLE_HEIGHT;
    g->paddleY = (g->minY + g->maxY) / 2 - g->paddleHeight / 2;
    g->prevPaddleY = g->paddleY;

    // Lógica inicial
    g->logicState.p = 1;
    g->logicState.q = 1;
    g->logicState.r = 0;

    // Sistema de fórmulas
    logic_init_formulas();
    g->formula = logic_get_current_formula();

    g->ranking = ranking_create(RANKING_SIZE);
    g->currentScore = 0;
    g->running = 1;

    // Bola começa no centro
    g->ballX = (g->minX + g->maxX) / 2.0f;
    g->ballY = (g->minY + g->maxY) / 2.0f;
    g->ballVX = BALL_SPEED_X;
    g->ballVY = BALL_SPEED_Y;
    g->prevBallX = g->ballX;
    g->prevBallY = g->ballY;
}

static void game_reset_ball(GameState *g) {
    // NÃO mexe em prevBallX/prevBallY aqui!
    g->ballX = (g->minX + g->maxX) / 2.0f;
    g->ballY = (g->minY + g->maxY) / 2.0f;
    g->ballVX = BALL_SPEED_X;
    g->ballVY = BALL_SPEED_Y;
}

// movimenta bola e rebate em cima/baixo
static void game_update_ball(GameState *g) {
    // guarda posição antiga para apagar depois
    g->prevBallX = g->ballX;
    g->prevBallY = g->ballY;

    // move
    g->ballX += g->ballVX;
    g->ballY += g->ballVY;

   // Rebote no topo
    if (g->ballY <= g->minY + 1) {
        g->ballY = g->minY + 1;
        g->ballVY = -g->ballVY;
    }

    // Rebote no fundo
    if (g->ballY >= g->maxY - 1) {
        g->ballY = g->maxY - 1;
        g->ballVY = -g->ballVY;
    }

    // Rebote na PAREDE ESQUERDA
    if (g->ballX <= g->minX + 1) {
        g->ballX = g->minX + 1;
        g->ballVX = -g->ballVX;
    }
}

// colisão com raquete
static int game_ball_hits_paddle(const GameState *g) {
    int xi = (int)g->ballX;
    int yi = (int)g->ballY;

    if (xi == g->paddleX) {
        if (yi >= g->paddleY && yi <= g->paddleY + g->paddleHeight) {
            return 1;
        }
    }
    return 0;
}

// trata colisões com raquete e bordas laterais + lógica P->Q
static void game_handle_collisions(GameState *g) {
    // colisão com raquete
    if (game_ball_hits_paddle(g)) {
        BoolVal res = logic_eval(g->formula, &g->logicState);
        if (res == BOOL_TRUE) {
            g->ballVX = -g->ballVX;
            g->currentScore += 1;
            logic_cycle_formula();
            g->formula = logic_get_current_formula();
        } else {
            ranking_add_score(g->ranking, g->currentScore);
            g->currentScore = 0;
            logic_cycle_formula();
            g->formula = logic_get_current_formula();
            game_reset_ball(g);
        }
    }

    // passou muito da direita (errou feio)
    if ((int)g->ballX > g->maxX + 1) {
        ranking_add_score(g->ranking, g->currentScore);
        g->currentScore = 0;
        logic_cycle_formula();
        g->formula = logic_get_current_formula();
        game_reset_ball(g);
    }
}

// trata teclado (não bloqueante)
static void game_handle_input(GameState *g) {
    while (keyhit()) {
        int ch = readch();

        switch (ch) {
        case 'w': // TODO: mapear setas ↑/↓ depois
        case 'W':
            if (g->paddleY > g->minY + 1) {
                g->prevPaddleY = g->paddleY;   // NOVO
                g->paddleY -= 1;
            }
            break;
        case 's':
        case 'S':
            if (g->paddleY + g->paddleHeight < g->maxY - 1) {
                g->prevPaddleY = g->paddleY;
                g->paddleY += 1;
            }
            break;
        case '1':
            g->logicState.p = !g->logicState.p;
            break;
        case '2':
            g->logicState.q = !g->logicState.q;
            break;
        case '3':
            g->logicState.r = !g->logicState.r;
            break;
        case 'r':
        case 'R':
            // reseta score e bola
            g->currentScore = 0;
            game_reset_ball(g);
            break;
        case 'q':
        case 'Q':
            g->running = 0;
            break;
        default:
            break;
        }
    }
}

// desenha HUD + campo + bola + raquete
static void game_draw(const GameState *g) {

    // HUD    
    BoolVal res = logic_eval(g->formula, &g->logicState);

    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCRSTARTX + 1, SCRSTARTY);
    printf("PONG LOGICO");

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 1);
    printf("Formula %d/%d: %s", 
           logic_get_current_formula_index() + 1, 
           logic_get_formula_count(),
           logic_get_current_formula_string());

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 2);
    printf("P=%d  Q=%d  R=%d", g->logicState.p, g->logicState.q, g->logicState.r);

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 3);
    if (res == BOOL_TRUE) {
        screenSetColor(LIGHTGREEN, BLACK);
        printf("Resultado: VERDADEIRO");
    } else {
        screenSetColor(LIGHTRED, BLACK);
        printf("Resultado: FALSO");
    }

    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 4);
    printf("Score atual: %d", g->currentScore);

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 5);
    printf("[W/S] Mover  [1/2/3] Alternar P,Q,R");

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 6);
    printf("[R] Reset  [Q] Sair");

   // Ranking (lado esquerdo inferior)
    ranking_draw(g->ranking, SCRSTARTX + 1, SCRSTARTY + 8);

// --- LIMPA COLUNA DA RAQUETE INTEIRA ---
    screenSetColor(BLACK, BLACK);
    for (int y = g->minY + 1; y < g->maxY; ++y) {
        screenGotoxy(g->paddleX, y);
        printf(" ");
    }

   // --- APAGA BOLA ANTIGA ---
    screenGotoxy((int)g->prevBallX, (int)g->prevBallY);
    printf(" ");

    // --- DESENHA RAQUETE NOVA ---
    screenSetColor(CYAN, BLACK);
    for (int i = 0; i < g->paddleHeight; ++i) {
        screenGotoxy(g->paddleX, g->paddleY + i);
        printf("|");
    }

    // --- DESENHA BOLA NOVA ---
    screenSetColor(WHITE, BLACK);
    screenGotoxy((int)g->ballX, (int)g->ballY);
    printf("O");

    screenUpdate();
}

// ---------- API pública ----------

void game_run(void) {
    GameState g;

    screenInit(1);
    keyboardInit();
    timerInit(50);

    game_init_state(&g);

    while (g.running) {
        game_handle_input(&g);

        if (timerTimeOver()) {
            game_update_ball(&g);
            game_handle_collisions(&g);
            game_draw(&g);
        }
    }

    logic_free_formulas();
    ranking_free(g.ranking);
    keyboardDestroy();
    screenDestroy();
    timerDestroy();
}