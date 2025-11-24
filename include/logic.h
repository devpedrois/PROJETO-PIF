#ifndef LOGIC_H
#define LOGIC_H

#include "types.h"

typedef struct Expr Expr;

// Estrutura para representar uma fórmula
typedef struct {
    Expr *expr;        // Árvore da expressão para cálculo
    const char *str;   // String para exibição
} Formula;

// Sistema de múltiplas fórmulas
void logic_init_formulas(void);
Expr *logic_get_current_formula(void);
const char *logic_get_current_formula_string(void);
void logic_cycle_formula(void);
void logic_set_formula(int index);
int logic_get_formula_count(void);
int logic_get_current_formula_index(void);
void logic_free_formulas(void);

// Funções de avaliação
BoolVal logic_eval(const Expr *expr, const LogicState *state);

// Libera a árvore recursivamente
void logic_free(Expr *expr);

#endif // LOGIC_H