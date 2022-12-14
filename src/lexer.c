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

// Custom lex function
int (*lex_function)(token_t *) = NULL;

// Get the next cdhar in the stream (expression_string)
#define NEXT_CHAR *(expression_string + expression_ptr++)

/* char next_solid_char() :
 * Returns the next non-whitespace character
 * within the given expression_string character
 * stream
 */
char next_solid_char()
{
	char c = NEXT_CHAR;

	while (c == ' ' || c == '\t')
		c = NEXT_CHAR;

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
		c = NEXT_CHAR;

		if (!isalnum(c) && c != '.')
			break;

		count++;
		string = realloc(string, count);
		*(string + (count - 1)) = c;
	}

	expression_ptr--;

	return string;
}

int lex(token_t *token)
{
	if (lex_function != NULL)
		return (*lex_function)(token);
	
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
		char *string = get_string(c);

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
		uint8_t decimal = 0;
		if (isdigit(*string) || (decimal = ((*string == '.') || (*string + strlen(string) == '.')))) {
			for (int i = 0; i < strlen(string); i++)
				if (*(string + i) == '.'|| decimal) {
					decimal = 1;
					break;
				}
			
			if (decimal) {
				message(MESSAGE_DEBUG, "DECIMAL NUMBER\n");
				token->type = T_NUMBER;
				token->value = atof(string);
				
				return 1;
			}

			message(MESSAGE_DEBUG, "WHOLE NUMBER\n");
			token->type = T_INT;
			token->value = atoi(string);

			return 1;
		}

		// Check if the string is apart of the constants list, if so
		// substitute in the number for it
		for (int i = 0; i < sizeof(STANDARD_SYMBOLS) / sizeof(STANDARD_SYMBOLS[0]); i++) {
			if (strcmp(string, STANDARD_SYMBOLS[i].name) == 0) {
				message(MESSAGE_DEBUG, "Found mathematical symbol %s\n", string);

				token->type = T_NUMBER;
				token->value = STANDARD_SYMBOLS[i].value;
				return 1;
			}
		}

		message(MESSAGE_FATAL, "Unidentified string \"%s\"\n", string);
	}

	return 0;
}

void _set_lexer_function(int (*func)(token_t *))
{
	lex_function = func;
}