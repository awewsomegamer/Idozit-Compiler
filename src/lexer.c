#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <messages.h>

char *expression_string = NULL;
char **variables_list = NULL;
uint32_t variable_count = 0;
uint32_t expression_ptr = 0;

/* char next_char() :
 * Returns the next character in the
 * expreesion_string character stream
 */
char next_char()
{
	return *(expression_string + expression_ptr++);
}

/* char next_solid_char() :
 * Returns the next non-whitespace character
 * within the given expression_string character
 * stream
 */
char next_solid_char()
{
	char c = next_char();

	while (c == ' ' || c == '\t')
		c = next_char();

	return c;
}

/* char* get_string(char) :
 * Will return a string of characters.
 * The given char is the starting character
 * And from there any alpha-numeric or
 * '.' character will be returned in the string
 */
char* get_string(char c)
{
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

/* int lex(token_t*) :
 * This function will write to the token
 * given, and return either a 1 or a 0
 * 
 * 1: New token found
 * 0: No new token found
 */
int lex(token_t *token)
{
	token->type = 0;
	token->value = 0;

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

		// Is this string one of the given variables?
		for (int i = 0; i < variable_count; i++)
			if (strcmp(string, variables_list[i]) == 0) {
			// If so, return its index and a var token
			token->type = T_VAR;
			token->value = i;
			return 1;
			}

		// Is this string one of the functions
		for (int i = 0; i < T_FUNC_MAX; i++)
			if (strcmp(string, T_FUNC_NAMES[i]) == 0) {
				// If so, return its index and a identifier token
				token->type = T_IDENT;
				token->value = i;
				return 1;
			}

		// If this string is representing a number,
		// return a number token, with the double
		// value of the string
		if (isdigit(*string) || *string == '.') {
			message(MESSAGE_DEBUG, "DIGIT\n");
			token->type = T_NUMBER;
			token->value = atof(string);

			return 1;
		}
	}

	return 0;
}