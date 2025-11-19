#include <stdlib.h>
#include "logic.h"

// Tipos de nó
typedef enum {
    OP_VAR_P,
    OP_VAR_Q,
    OP_VAR_R,
    OP_NOT,
    OP_AND,
    OP_OR,
    OP_IMPLIES
} OpType;

// Definição da árvore
struct Expr {
    OpType op;
    struct Expr *left;
    struct Expr *right;
};

// ---------- helpers internos ----------

static Expr *expr_new(OpType op, Expr *left, Expr *right) {
    Expr *e = (Expr *)malloc(sizeof(Expr));
    if (!e) return NULL;
    e->op = op;
    e->left = left;
    e->right = right;
    return e;
}

// avaliação recursiva
static BoolVal eval_recursive(const Expr *expr, const LogicState *st) {
    if (!expr || !st) return BOOL_FALSE;

    switch (expr->op) {
        case OP_VAR_P: return st->p ? BOOL_TRUE : BOOL_FALSE;
        case OP_VAR_Q: return st->q ? BOOL_TRUE : BOOL_FALSE;
        case OP_VAR_R: return st->r ? BOOL_TRUE : BOOL_FALSE;

        case OP_NOT: {
            BoolVal v = eval_recursive(expr->left, st);
            return (v == BOOL_TRUE) ? BOOL_FALSE : BOOL_TRUE;
        }

        case OP_AND: {
            BoolVal a = eval_recursive(expr->left, st);
            BoolVal b = eval_recursive(expr->right, st);
            return (a == BOOL_TRUE && b == BOOL_TRUE) ? BOOL_TRUE : BOOL_FALSE;
        }

        case OP_OR: {
            BoolVal a = eval_recursive(expr->left, st);
            BoolVal b = eval_recursive(expr->right, st);
            return (a == BOOL_TRUE || b == BOOL_TRUE) ? BOOL_TRUE : BOOL_FALSE;
        }

        case OP_IMPLIES: {
            BoolVal a = eval_recursive(expr->left, st);
            BoolVal b = eval_recursive(expr->right, st);
            if (a == BOOL_TRUE && b == BOOL_FALSE) return BOOL_FALSE;
            return BOOL_TRUE;
        }

        default:
            return BOOL_FALSE;
    }
}

// liberação recursiva
static void free_recursive(Expr *expr) {
    if (!expr) return;
    free_recursive(expr->left);
    free_recursive(expr->right);
    free(expr);
}

// ---------- API pública ----------

Expr *logic_create_example_formula(void) {
    // Fórmula: P -> Q
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *q = expr_new(OP_VAR_Q, NULL, NULL);
    Expr *impl = expr_new(OP_IMPLIES, p, q);
    return impl;
}

BoolVal logic_eval(const Expr *expr, const LogicState *state) {
    return eval_recursive(expr, state);
}

const char *logic_get_formula_string(void) {
    // Por enquanto fixa; depois vocês podem trocar pra algo dinâmico
    return "P -> Q";
}

void logic_free(Expr *expr) {
    free_recursive(expr);
}