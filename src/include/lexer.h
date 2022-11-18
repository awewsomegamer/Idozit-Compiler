#ifndef LEXER_H
#define LEXER_H

#include <design1.h>

enum {
        T_ADD,
        T_SUB,
        T_MUL,
        T_DIV,
        T_LPAREN,
        T_RPAREN,
        T_INT,
        T_VAR,
        T_IDENT
};

struct token {
        int type;
        int64_t value;
        double dvalue;
};
typedef struct token token_t;

enum {
        T_FUNC_INTEGRAL,
        T_FUNC_DERIVATIVE,
        T_FUNC_MAX
};

static const char* T_FUNC_NAMES[] = {
        [T_FUNC_INTEGRAL] = "INTEGRAL",
        [T_FUNC_DERIVATIVE] = "DERIVATIVE"
};

int lex();

#endif