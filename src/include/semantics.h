#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <design1.h>

extern void (*validate_function)(tree_code_t *);

/* void validate(tree_code_t *head)
 * Recursively validate the given tree, head
 */
void validate(tree_code_t *head);

void _set_validate_function(void (*)(tree_code_t *));

#endif