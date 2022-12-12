#ifndef PARSER_H
#define PARSER_H

#include <design1.h>

extern tree_code_t * (*parse_function)();

/* tree_code_t *build_tree() :
 * Builds the parse tree after the
 * lexer has been setup to the right
 * input
 */
tree_code_t *build_tree();

void _set_parser_function(tree_code_t * (*)());

#endif