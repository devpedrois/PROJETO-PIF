#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define BALL_BASE_SPEED_X   1.5f
#define PADDLE_HEIGHT       3
#define RANKING_SIZE        5

// HUD ocupa essas linhas logo abaixo da borda superior
#define HUD_HEIGHT      7
#define SIDE_PANEL_X    (SCRSTARTX + 40)
#define SIDE_PANEL_Y    (SCRSTARTY + 1)

// linha onde começa o “andar de baixo” (campo)
static int g_gameTopY = 0;

// ---------- helpers internos ----------

// lê uma linha simples (nome do jogador) no próprio terminal
static void read_line(char *buffer, int maxLen, int x, int y) {
    int len = 0;
    buffer[0] = '\0';

    while (1) {
        if (keyhit()) {
            int ch = readch();

            if (ch == '\n' || ch == '\r') {
                buffer[len] = '\0';
                break;
            } else if (ch == 8 || ch == 127) { // backspace
                if (len > 0) {
                    len--;
                    buffer[len] = '\0';
                    screenGotoxy(x + len, y);
                    printf(" ");
                    screenGotoxy(x + len, y);
                }
            } else if (ch >= 32 && ch <= 126 && len < maxLen - 1) {
                buffer[len++] = (char)ch;
                screenGotoxy(x + len - 1, y);
                printf("%c", ch);
            }
            screenUpdate();
        }
    }
}

// define limites da área de jogo e inicializa lógica & scores
static void game_init_state(GameState *g, Ranking *ranking) {
    g->ranking = ranking;

    g_gameTopY = SCRSTARTY + HUD_HEIGHT;

    // Área de JOGO (tela inteira, sem ranking lateral)
    g->minX = SCRSTARTX + 1;
    g->maxX = SCRENDX   - 1;

    g->minY = g_gameTopY + 1;
    g->maxY = SCRENDY   - 1;

    // Raquete na direita da área de jogo
    g->paddleX      = g->maxX - 2;
    g->paddleHeight = PADDLE_HEIGHT;
    g->paddleY      = (g->minY + g->maxY) / 2 - g->paddleHeight / 2;
    g->prevPaddleY  = g->paddleY;

    // Lógica inicial
    logic_init_formulas();
    g->formula = logic_get_current_formula();

    // garante que comece FALSO
    logic_randomize_state_false(&g->logicState, g->formula);

    // Pontuação
    g->currentScore = 0;
    g->bestScore    = 0;
    g->running      = 1;
    g->speedMultiplier = 0.7f;

    // Linha horizontal separando HUD do campo
    for (int x = SCRSTARTX + 1; x < SCRENDX - 1; ++x) {
    screenGotoxy(x, g->minY - 1);
    printf("-");
    }
}

// randomiza posição e direção da bola
static void game_randomize_ball(GameState *g) {
    // posição X mais à esquerda pra dar tempo de reação
    g->ballX = g->minX + 2;

    // posição Y aleatória dentro da área de jogo
    int range = g->maxY - g->minY - 4;
    if (range < 1) range = 1;
    int offset = rand() % range;
    g->ballY = g->minY + 2 + offset;

    // velocidade X sempre para a DIREITA (em direção à raquete)
    g->ballVX = BALL_BASE_SPEED_X * g->speedMultiplier;

    // velocidade Y aleatória em um conjunto de ângulos
    float angles[] = { -0.8f, -0.5f, -0.3f, 0.3f, 0.5f, 0.8f };
    int   nAngles  = (int)(sizeof(angles) / sizeof(angles[0]));
    int   idx      = rand() % nAngles;
    g->ballVY = angles[idx];

    g->prevBallX = g->ballX;
    g->prevBallY = g->ballY;
}

static void game_reset_ball(GameState *g) {
    // Apaga a bola atual da tela (posição antiga)
    screenSetColor(BLACK, BLACK);
    screenGotoxy((int)g->ballX, (int)g->ballY);
    printf(" ");
    screenGotoxy((int)g->prevBallX, (int)g->prevBallY);
    printf(" ");

    // reposiciona aleatoriamente
    game_randomize_ball(g);
}

// movimenta bola e rebate nas paredes da ÁREA DE JOGO
static void game_update_ball(GameState *g) {
    // guarda posição antiga para poder apagar / detectar cruzamento
    g->prevBallX = g->ballX;
    g->prevBallY = g->ballY;

    // move
    g->ballX += g->ballVX;
    g->ballY += g->ballVY;

    // Rebote no topo da área de jogo
    if (g->ballY <= g->minY + 1) {
        g->ballY  = g->minY + 1;
        g->ballVY = -g->ballVY;
    }

    // Rebote no fundo da área de jogo
    if (g->ballY >= g->maxY - 1) {
        g->ballY  = g->maxY - 1;
        g->ballVY = -g->ballVY;
    }

    // Rebote na PAREDE ESQUERDA
    if (g->ballX <= g->minX + 1) {
        g->ballX  = g->minX + 1;
        g->ballVX = -g->ballVX;
    }
}

// colisão com raquete (usando cruzamento em X, não igualdade seca)
static int game_ball_hits_paddle(const GameState *g) {
    int currX = (int)g->ballX;
    int currY = (int)g->ballY;

    // só nos interessa quando a bola está indo pra DIREITA
    if (g->ballVX <= 0.0f) {
        return 0;
    }

    // Se a bola já alcançou ou passou a coluna da raquete,
    // e o Y está dentro da faixa, consideramos "acertou".
    if (currX >= g->paddleX &&
        currX <= g->paddleX + 1 &&
        currY >= g->paddleY &&
        currY <= g->paddleY + g->paddleHeight) {
        return 1;
    }

    return 0;
}

// trata colisões com raquete e borda direita
static void game_handle_collisions(GameState *g) {
    // colisão com raquete
    if (game_ball_hits_paddle(g)) {
        // Garante que a bola "pare" logo antes da raquete
        g->ballX = g->paddleX - 1;
        int currY = (int)g->ballY;

        // (opcional) se quiser alinhar o Y dentro da raquete:
        if (currY < g->paddleY) {
            g->ballY = g->paddleY;
        } else if (currY > g->paddleY + g->paddleHeight) {
            g->ballY = g->paddleY + g->paddleHeight;
        }

        BoolVal res = logic_eval(g->formula, &g->logicState);

        if (res == BOOL_TRUE) {
            // lógica verdadeira → rebate e soma ponto
            g->ballVX = -g->ballVX;
            g->currentScore += 1;
            if (g->currentScore > g->bestScore)
                g->bestScore = g->currentScore;

            // AUMENTA a velocidade da bola a cada acerto (bem de leve)
            g->speedMultiplier += 0.10f;      // +10% por acerto
            if (g->speedMultiplier > 2.5f)    // trava num máximo
                g->speedMultiplier = 2.5f;

            // re-aplica o multiplicador na velocidade atual
            float dirX = (g->ballVX >= 0.0f) ? 1.0f : -1.0f;
            g->ballVX = dirX * BALL_BASE_SPEED_X * g->speedMultiplier;

            // mantém o “ângulo” de VY mas escala a intensidade
            if (g->ballVY > 0.0f)
                g->ballVY = 0.5f * g->speedMultiplier;
            else
                g->ballVY = -0.5f * g->speedMultiplier;

            // próxima fórmula, sempre começando FALSA
            logic_cycle_formula();
            g->formula = logic_get_current_formula();
            logic_randomize_state_false(&g->logicState, g->formula);
        } else {
            // lógica falsa → atravessa: zera combo e reseta bola
            if (g->currentScore > g->bestScore)
                g->bestScore = g->currentScore;

            g->currentScore   = 0;
            g->speedMultiplier = 1.0f;        // volta pra velocidade padrão
            game_reset_ball(g);

            logic_cycle_formula();
            g->formula = logic_get_current_formula();
            logic_randomize_state_false(&g->logicState, g->formula);
        }
    }

    // passou muito da direita (errou feio, nem encostou na raquete)
    if ((int)g->ballX > g->maxX + 2) {
        if (g->currentScore > g->bestScore)
            g->bestScore = g->currentScore;

        g->currentScore    = 0;
        g->speedMultiplier = 1.0f;            // reset da velocidade
        game_reset_ball(g);

        logic_cycle_formula();
        g->formula = logic_get_current_formula();
        logic_randomize_state_false(&g->logicState, g->formula);
    }
}

// trata teclado (não bloqueante) durante o jogo
static void game_handle_input(GameState *g) {
    while (keyhit()) {
        int ch = readch();

        switch (ch) {
        case 'w':
        case 'W':
            if (g->paddleY > g->minY + 1) {
                g->prevPaddleY = g->paddleY;
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
            // zera combo atual, mas mantém melhor
            if (g->currentScore > g->bestScore) {
                g->bestScore = g->currentScore;
            }
            g->currentScore    = 0;
            g->speedMultiplier = 1.0f;     // reset da velocidade da bola
            game_reset_ball(g);
            break;
        case 'q':
        case 'Q':
            // antes de sair, atualiza melhor e grava no ranking
            if (g->currentScore > g->bestScore)
                g->bestScore = g->currentScore;

            if (g->bestScore > 0 && g->ranking) {
                ranking_add_or_update(g->ranking, g->playerName, g->bestScore);
            }
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
    printf("LOGICPONG - Jogador: %s", g->playerName);

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 1);
    printf("Formula %d/%d: %-30s",
           logic_get_current_formula_index() + 1,
           logic_get_formula_count(),
           logic_get_current_formula_string());

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 2);
    printf("P=%d  Q=%d  R=%d", g->logicState.p, g->logicState.q, g->logicState.r);

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 3);
    if (res == BOOL_TRUE) {
        screenSetColor(LIGHTGREEN, BLACK);
        printf("Resultado: VERDADEIRO   ");
    } else {
        screenSetColor(LIGHTRED, BLACK);
        printf("Resultado: FALSO        ");
    }

    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 4);
    printf("Score atual: %d", g->currentScore);

    screenGotoxy(SCRSTARTX + 1, SCRSTARTY + 5);
    printf("Melhor dessa partida: %d", g->bestScore);
    
    // Painel lateral (instruções)
    screenSetColor(WHITE, BLACK);

    // Y base do painel (mantém como está definido em cima)
    screenGotoxy(SIDE_PANEL_X, SIDE_PANEL_Y);
    printf("Controles:");

    screenGotoxy(SIDE_PANEL_X, SIDE_PANEL_Y + 1);
    printf("[W/S] Mover");

    screenGotoxy(SIDE_PANEL_X, SIDE_PANEL_Y + 2);
    printf("[1/2/3] Alternar");

    screenGotoxy(SIDE_PANEL_X + 2, SIDE_PANEL_Y + 3);
    printf("P, Q, R");

    screenGotoxy(SIDE_PANEL_X, SIDE_PANEL_Y + 4);
    printf("[R] Resetar  [Q] Sair");

    // --- APAGA BOLA ANTIGA ---
    screenSetColor(BLACK, BLACK);
    screenGotoxy((int)g->prevBallX, (int)g->prevBallY);
    printf(" ");

    // --- LIMPA COLUNA INTERA DA RAQUETE ---
    for (int y = g->minY; y <= g->maxY; ++y) {
        screenGotoxy(g->paddleX, y);
        printf(" ");
    }

    // --- DESENHA RAQUETE NOVA ---
    screenSetColor(CYAN, BLACK);
        for (int i = 0; i < g->paddleHeight; ++i) {
            screenGotoxy(g->paddleX, g->paddleY + i);
            printf("|");
        }

    // --- DESENHA BOLA NOVA ---
    screenSetColor(WHITE, BLACK);   // aqui você escolhe a cor
    screenGotoxy((int)g->ballX, (int)g->ballY);
    printf("O");

    screenUpdate();
}

// Loop principal só da PARTIDA (jogo rolando)
static void game_play(Ranking *ranking) {
    GameState g;

    // limpa qualquer tecla que sobrou do menu (ex: ENTER)
    while (keyhit()) {
        readch();
    }

    // tela pra digitar o nome
    screenInit(1);
    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCRSTARTX + 5, SCRSTARTY + 2);
    printf("LOGICPONG");

    screenGotoxy(SCRSTARTX + 5, SCRSTARTY + 4);
    printf("Digite seu nome (max %d caracteres):", RANKING_MAX_NAME);

    int nameX = SCRSTARTX + 5;
    int nameY = SCRSTARTY + 6;
    screenGotoxy(nameX, nameY);
    screenUpdate();

    char nome[RANKING_MAX_NAME + 1];
    read_line(nome, sizeof(nome), nameX, nameY);

    if (nome[0] == '\0') {
        strcpy(nome, "Jogador");
    }

    // limpa e inicia o jogo
    screenInit(1);
    memset(&g, 0, sizeof(GameState));
    strncpy(g.playerName, nome, RANKING_MAX_NAME);
    g.playerName[RANKING_MAX_NAME] = '\0';

    game_init_state(&g, ranking);
    game_randomize_ball(&g);
    timerInit(50);   // controla velocidade

    while (g.running) {
        game_handle_input(&g);

        if (timerTimeOver()) {
            game_update_ball(&g);
            game_handle_collisions(&g);
            game_draw(&g);
        }
    }

    timerDestroy();
    logic_free_formulas();
}

// ---------- TELAS DE MENU ----------

static int show_main_menu(void) {
    screenInit(1);

    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCRSTARTX + 10, SCRSTARTY + 2);
    printf("LOGICPONG");

    screenGotoxy(SCRSTARTX + 8, SCRSTARTY + 4);
    printf("1 - Jogar");

    screenGotoxy(SCRSTARTX + 8, SCRSTARTY + 5);
    printf("2 - Ver ranking");

    screenGotoxy(SCRSTARTX + 8, SCRSTARTY + 6);
    printf("3 - Sair");

    screenGotoxy(SCRSTARTX + 8, SCRSTARTY + 8);
    printf("Escolha uma opcao (1, 2 ou 3):");

    screenUpdate();

    while (1) {
        if (keyhit()) {
            int ch = readch();
            if (ch == '1' || ch == '2' || ch == '3') {
                return ch - '0'; // 1,2,3
            }
        }
    }
}

static void show_ranking_screen(Ranking *ranking) {
    screenInit(1);

    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCRSTARTX + 10, SCRSTARTY + 1);
    printf("RANKING - TOP %d JOGADORES", ranking->capacity);

    // desenha ranking mais centralizado
    ranking_draw(ranking, SCRSTARTX + 8, SCRSTARTY + 3);

    screenSetColor(LIGHTGRAY, BLACK);
    screenGotoxy(SCRSTARTX + 8, SCRENDY - 2);
    printf("[Q] Voltar ao menu");

    screenUpdate();

    while (1) {
        if (keyhit()) {
            int ch = readch();
            if (ch == 'q' || ch == 'Q' || ch == 27) { // ESC também volta
                break;
            }
        }
    }
}

// ---------- API pública ----------

void game_run(void) {
    screenInit(1);
    keyboardInit();
    srand((unsigned)time(NULL));   // aleatoriedade da bola

    Ranking *ranking = ranking_create(RANKING_SIZE);
    int runningProgram = 1;

    while (runningProgram) {
        int opcao = show_main_menu();

        switch (opcao) {
        case 1: // Jogar
            game_play(ranking);
            break;
        case 2: // Ver ranking isolado
            show_ranking_screen(ranking);
            break;
        case 3: // Sair
            runningProgram = 0;
            break;
        }
    }

    ranking_free(ranking);
    keyboardDestroy();
    screenDestroy();
}