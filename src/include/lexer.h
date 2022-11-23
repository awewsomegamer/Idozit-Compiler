#ifndef LEXER_H
#define LEXER_H

#include <design1.h>

extern char *expression_string;
extern char **variables_list;
extern uint32_t variable_count;

extern uint32_t expression_ptr;

enum {
        T_ADD,
        T_SUB,
        T_MUL,
        T_DIV,
        T_LPAREN,
        T_RPAREN,
        T_NUMBER,
        T_VAR,
        T_IDENT
};

struct token {
        int type;
        double value;
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