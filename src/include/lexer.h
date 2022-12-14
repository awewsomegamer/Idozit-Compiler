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
        T_INT,
        T_NUMBER,
        T_VAR,
        T_IDENT
};

static const char *TOKEN_NAMES[] = {
        [T_EOF] =      "End Of File",
        [T_ADD] =      "Addition",
        [T_SUB] =      "Subtraction",
        [T_MUL] =      "Multiplication",
        [T_DIV] =      "Division",
        [T_LPAREN] =   "Left Parenthesis",
        [T_RPAREN] =   "Left Parenthesis",
        [T_EXPONENT] = "Exponent", 
        [T_NUMBER] =   "Number",
        [T_INT] =      "Integer",
        [T_VAR] =      "Variable",
        [T_IDENT] =    "Identifier"
};

enum {
        T_FUNC_INTEGRAL,
        T_FUNC_DERIVATIVE,
        T_FUNC_MAX
};

static const char *T_FUNC_NAMES[] = {
        [T_FUNC_INTEGRAL] = "INTEGRAL",
        [T_FUNC_DERIVATIVE] = "DERIVATIVE"
};

struct symbol {
        char* name;
        double value;
};
typedef struct symbol symbol_t;

static const symbol_t STANDARD_SYMBOLS[] = {
        {"PI", PI},
        {"e", EULERS_CONSTANT},
        {"E", EULERS_NUMBER},
};

extern int (*lex_function)(token_t *);

/* int lex(token_t*) :
 * This function will write to the token
 * given, and return either a 1 or a 0
 * 
 * 1: New token found
 * 0: No new token found
 */
int lex(token_t *token);

void _set_lexer_function(int (*)(token_t *));

#endif