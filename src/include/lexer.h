#ifndef LEXER_H
#define LEXER_H

#include <design1.h>

extern char *expression_string;
extern char **variables_list;
extern uint32_t variable_count;

extern uint32_t expression_ptr;

enum {
        T_EOF,
        T_ADD,
        T_SUB,
        T_MUL,
        T_DIV,
        T_LPAREN,
        T_RPAREN,
        T_EXPONENT,
        T_NUMBER,
        T_VAR,
        T_IDENT
};

static const char* TOKEN_NAMES[] = {
        [T_EOF] =      "End Of File",
        [T_ADD] =      "Addition",
        [T_SUB] =      "Subtraction",
        [T_MUL] =      "Multiplication",
        [T_DIV] =      "Division",
        [T_LPAREN] =   "Left Parenthesis",
        [T_RPAREN] =   "Left Parenthesis",
        [T_EXPONENT] = "Exponent", 
        [T_NUMBER] =   "Number",
        [T_VAR] =      "Variable",
        [T_IDENT] =    "Identifier"
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