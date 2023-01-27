#ifndef CACHER
#define CACHER

#include <idozit.h>

extern pthread_mutex_t cacher_mutex;

/* void *cahce_code_block(code_block_t *code) :
 * Allocates executable memory for the given block of code.

 * If the function returns NULL, then it has failed.
 * 
 * If the function returns not NULL, then that is the address
 * of the function.
 * 
 * The function will automatically handle everything related to caching.
 */
void *cache_code_block(code_block_t *code);

#endif