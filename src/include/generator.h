#ifndef GENERATOR_H
#define GENERATOR_H

#include <idozit.h>

/* code_block_t default_x86_64_generator(tree_code_t *tree) :
 * This will generate the given tree into a code buffer and a data buffer 
 * which is wrapped in the code_block_t structure
 */
code_block_t default_x86_64_generator(tree_code_t *tree);

#endif