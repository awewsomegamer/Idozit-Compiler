#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>

char *expression_string = NULL;
char **variables_list = NULL;
uint32_t variable_count = 0;

uint32_t expression_ptr = 0;

inline char next_char() {
    return *(expression_string + expression_ptr++);
}

char next_solid_char() {
    char c = next_char();

    while (c == ' ' || c == '\t')
        c = next_char();

    return c;
}

int lex(token_t* token) {
    char c = next_solid_char();

    switch (c) {
    case '+':
        return 1;

    case '-':
        return 1;

    case '*':
        return 1;

    case '/':
        return 1;

    case '(':
        return 1;

    case ')':
        return 1;

    default:
        return 1;
    }

    return 0;
}
