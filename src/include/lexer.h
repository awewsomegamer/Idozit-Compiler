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
        T_IDENT
};

struct token {
        int type;
        int64_t value;
        double dvalue;
};
typedef struct token token_t;

int lex();

#endif