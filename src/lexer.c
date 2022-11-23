#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *expression_string = NULL;
char **variables_list = NULL;
uint32_t variable_count = 0;

uint32_t expression_ptr = 0;

char next_char() {
    return *(expression_string + expression_ptr++);
}

char next_solid_char() {
    char c = next_char();

    while (c == ' ' || c == '\t')
        c = next_char();

    return c;
}

char* get_string(char c) {
    char* string = malloc(1);
    *string = c;
    int count = 1;

    while (isalnum(c) || c == '.') {
        c = next_char();

        if (!isalnum(c) && c != '.') break;

        count++;
        string = realloc(string, count);
        *(string + (count - 1)) = c;
    }

    expression_ptr--;

    return string;
}

int lex(token_t* token) {
    if (expression_ptr >= strlen(expression_string)) {
        token->type = T_EOF;
        return 1;
    }

    char c = next_solid_char();

    switch (c) {
    case '+':
        token->type = T_ADD;
        return 1;

    case '-':
        token->type = T_SUB;
        return 1;

    case '*':
        token->type = T_MUL;
        return 1;

    case '/':
        token->type = T_DIV;
        return 1;

    case '(':
        token->type = T_LPAREN;
        return 1;

    case ')':
        token->type = T_RPAREN;
        return 1;

    case '^':
        token->type = T_EXPONENT;
        return 1;

    default:
        char* string = get_string(c);

        for (int i = 0; i < variable_count; i++)
            if (strcmp(string, variables_list[i]) == 0) {
                token->type = T_VAR;
                token->value = i;
                return 1;
            }

        for (int i = 0; i < T_FUNC_MAX; i++)
            if (strcmp(string, T_FUNC_NAMES[i]) == 0) {
                token->type = T_IDENT;
                token->value = i;
                return 1;
            }

        // Number:
        // Look at the string, while you don't reach a . you are looking at a whole number
        // When you reach a . then flip over to the other integer, then put these two integers together
        // To form a single double value
        // .5213 is not allowed
        // 0.5213 is allowed
        if (isdigit(*string)) {
            printf("DIGIT\n");
            token->type = T_NUMBER;
            token->value = atof(string);
        
            return 1;
        }
    }

    return 0;
}