#ifndef LOGIC_H
#define LOGIC_H

#include "types.h"

// Árvore de expressão lógica
typedef struct Expr Expr;

// Cria a fórmula de exemplo: P -> Q
Expr *logic_create_example_formula(void);

// Avalia ϕ(P,Q,R) retornando BOOL_TRUE / BOOL_FALSE
BoolVal logic_eval(const Expr *expr, const LogicState *state);

// Retorna uma string legível da fórmula (para HUD)
const char *logic_get_formula_string(void);

// Libera a árvore recursivamente
void logic_free(Expr *expr);

#endif // LOGIC_H