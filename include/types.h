#ifndef TYPES_H
#define TYPES_H

// Valores lógicos para as proposições
typedef enum {
    BOOL_FALSE = 0,
    BOOL_TRUE  = 1
} BoolVal;

// Estado atual das variáveis lógicas
typedef struct {
    int p;  // 0 ou 1
    int q;  // 0 ou 1
    int r;  // 0 ou 1
} LogicState;

#endif // TYPES_H