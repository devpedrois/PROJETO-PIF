#include <stdlib.h>
#include "logic.h"

// Gera P, Q, R aleatórios até que a fórmula seja falsa
void logic_randomize_state_false(LogicState *state, Expr *expr) {
    if (!state || !expr) return;

    do {
        state->p = rand() % 2;
        state->q = rand() % 2;
        state->r = rand() % 2;
    } while (logic_eval(expr, state) == BOOL_TRUE);
}

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

// ---------- Sistema de múltiplas fórmulas ----------
#define MAX_FORMULAS 10

static Formula formulas[MAX_FORMULAS];
static int order[MAX_FORMULAS];       // ordem embaralhada das fórmulas
static int currentFormulaIndex = 0;   // posição 0..9
static int totalFormulas = 10;        

// ---------- helpers internos ----------

// embaralha a ordem das fórmulas (0..totalFormulas-1)
static void logic_shuffle_order(void) {
    // inicializa ordem sequencial
    for (int i = 0; i < totalFormulas; ++i) {
        order[i] = i;
    }

    // Fisher-Yates
    for (int i = totalFormulas - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp  = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }
}

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

// ---------- Implementação das fórmulas ----------

// Funções para criar cada fórmula (exemplo com 5 fórmulas)

static Expr* create_formula_1() {
    // P → Q
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *q = expr_new(OP_VAR_Q, NULL, NULL);
    return expr_new(OP_IMPLIES, p, q);
}

static Expr* create_formula_2() {
    // P ∧ Q
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *q = expr_new(OP_VAR_Q, NULL, NULL);
    return expr_new(OP_AND, p, q);
}

static Expr* create_formula_3() {
    // P ∨ Q
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *q = expr_new(OP_VAR_Q, NULL, NULL);
    return expr_new(OP_OR, p, q);
}

static Expr* create_formula_4() {
    // ¬P
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    return expr_new(OP_NOT, p, NULL);
}

static Expr* create_formula_5() {
    // (P ∧ Q) → R
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *q = expr_new(OP_VAR_Q, NULL, NULL);
    Expr *r = expr_new(OP_VAR_R, NULL, NULL);
    Expr *p_and_q = expr_new(OP_AND, p, q);
    return expr_new(OP_IMPLIES, p_and_q, r);
}

static Expr* create_formula_6() {
    // ¬P ∧ ¬Q
    Expr *not_p = expr_new(OP_NOT, expr_new(OP_VAR_P, NULL, NULL), NULL);
    Expr *not_q = expr_new(OP_NOT, expr_new(OP_VAR_Q, NULL, NULL), NULL);
    return expr_new(OP_AND, not_p, not_q);
}

static Expr* create_formula_7() {
    // P ∨ ¬Q
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *not_q = expr_new(OP_NOT, expr_new(OP_VAR_Q, NULL, NULL), NULL);
    return expr_new(OP_OR, p, not_q);
}

static Expr* create_formula_8() {
    // ¬(P ∨ R) ∧ Q
    Expr *p_or_r = expr_new(OP_OR, 
        expr_new(OP_VAR_P, NULL, NULL),
        expr_new(OP_VAR_R, NULL, NULL)
    );
    Expr *not_p_or_r = expr_new(OP_NOT, p_or_r, NULL);
    Expr *q = expr_new(OP_VAR_Q, NULL, NULL);
    return expr_new(OP_AND, not_p_or_r, q);
}

static Expr* create_formula_9() {
    // (P -> Q) /\ (Q -> R)
    Expr *p = expr_new(OP_VAR_P, NULL, NULL);
    Expr *q1 = expr_new(OP_VAR_Q, NULL, NULL);
    Expr *impl1 = expr_new(OP_IMPLIES, p, q1);

    Expr *q2 = expr_new(OP_VAR_Q, NULL, NULL);
    Expr *r  = expr_new(OP_VAR_R, NULL, NULL);
    Expr *impl2 = expr_new(OP_IMPLIES, q2, r);

    return expr_new(OP_AND, impl1, impl2);
}

static Expr* create_formula_10() {
    // (P /\ ~Q) V (~P /\ R)
    Expr *p1   = expr_new(OP_VAR_P, NULL, NULL);
    Expr *not_q = expr_new(OP_NOT, expr_new(OP_VAR_Q, NULL, NULL), NULL);
    Expr *p_and_not_q = expr_new(OP_AND, p1, not_q);

    Expr *not_p = expr_new(OP_NOT, expr_new(OP_VAR_P, NULL, NULL), NULL);
    Expr *r     = expr_new(OP_VAR_R, NULL, NULL);
    Expr *not_p_and_r = expr_new(OP_AND, not_p, r);

    return expr_new(OP_OR, p_and_not_q, not_p_and_r);
}

// ---------- API Pública do Sistema de Fórmulas ----------

void logic_init_formulas(void) {
    // Inicializa fórmula 1: P → Q
    formulas[0].expr = create_formula_1();
    formulas[0].str  = "P -> Q";
    
    // Inicializa fórmula 2: P ∧ Q
    formulas[1].expr = create_formula_2();
    formulas[1].str  = "P /\\ Q";
    
    // Inicializa fórmula 3: P ∨ Q
    formulas[2].expr = create_formula_3();
    formulas[2].str  = "P V Q";
    
    // Inicializa fórmula 4: ¬P
    formulas[3].expr = create_formula_4();
    formulas[3].str  = "~P";
    
    // Inicializa fórmula 5: (P ∧ Q) → R
    formulas[4].expr = create_formula_5();
    formulas[4].str  = "(P /\\ Q) -> R";
    
    // Nova fórmula 6: ¬P ∧ ¬Q
    formulas[5].expr = create_formula_6();
    formulas[5].str  = "~P /\\ ~Q";

    // Nova fórmula 7: P ∨ ¬Q
    formulas[6].expr = create_formula_7();
    formulas[6].str  = "P V ~Q";

    // Nova fórmula 8: ¬(P ∨ R) ∧ Q
    formulas[7].expr = create_formula_8();
    formulas[7].str  = "~(P V R) /\\ Q";

    // Nova fórmula 9: (P -> Q) /\ (Q -> R)
    formulas[8].expr = create_formula_9();
    formulas[8].str  = "(P -> Q) /\\ (Q -> R)";

    // Nova fórmula 10: (P -> Q) /\ (Q -> R)
    formulas[9].expr = create_formula_10();
    formulas[9].str  = "(P /\\ ~Q) V (~P /\\ R)";

    // começa sempre da posição 0 do ciclo
    currentFormulaIndex = 0;  
    // sorteia ordem inicial das fórmulas
    logic_shuffle_order();
}

Expr *logic_get_current_formula(void) {
    if (currentFormulaIndex >= 0 && currentFormulaIndex < totalFormulas) {
        int idx = order[currentFormulaIndex];
        return formulas[idx].expr;
    }
    return NULL;
}

const char *logic_get_current_formula_string(void) {
    if (currentFormulaIndex >= 0 && currentFormulaIndex < totalFormulas) {
        int idx = order[currentFormulaIndex];
        return formulas[idx].str;
    }
    return "Formula invalida";
}

void logic_cycle_formula(void) {
    currentFormulaIndex++;

    // terminou ciclo (fez as 10)
    if (currentFormulaIndex >= totalFormulas) {
        currentFormulaIndex = 0;  
        logic_shuffle_order();   // NOVA ORDEM
    }
}

void logic_set_formula(int index) {
    if (index >= 0 && index < totalFormulas) {
        currentFormulaIndex = index;
    }
}

int logic_get_formula_count(void) {
    return totalFormulas;
}

int logic_get_current_formula_index(void) {
    return currentFormulaIndex;
}

void logic_free_formulas(void) {
    for (int i = 0; i < totalFormulas; i++) {
        if (formulas[i].expr) {
            free_recursive(formulas[i].expr);
            formulas[i].expr = NULL;
        }
    }
}


BoolVal logic_eval(const Expr *expr, const LogicState *state) {
    return eval_recursive(expr, state);
}

const char *logic_get_formula_string(void) {
    return logic_get_current_formula_string();
} 

void logic_free(Expr *expr) {
    free_recursive(expr);
}