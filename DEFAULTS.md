# Defaults

## What
This document outlines how the default compiler interface with the library. This document builds on top of <a href="DEVELOPERS.md">DEVELOPERS.md</a> and <a href="USERS.md">USERS.md</a>. Ensure the previously mentioned resources have been read and understood.

## Overall
The default compiler is able to take equations such as:
`x + y * x / INTEGRAL 1 x ( x^2 + 2*x + 1 ) - DERIVATIVE 1 x ( x ) * E` and convert them into x86-64 machine code.

*\** Note: The power of an exponent is truncated to an integer.

The default run function then executes the generated x86-64 machine code buffer.

## Lexer
The lexer recognizes the following:
* Number literals (Integer: 123, Number: 1237.786) - Constant values
* String literals - Names of variables (x, y, z, ...), mathematical symbols (PI, E, e), and function names (INTEGRAL, DERIVATIVE)
* `+`
* `-`
* `*`
* `/`
* `(` and `)`
* `^` - Exponent

The lexer provides two functions to call:
* int `lex`(token_t *token) - This function returns the next recognized token in the character stream, if a token is found it returns 1, if no token is found it returns 0
* void `_set_lexer_function`(int (*)(token_t *)) - This function sets the function pointer of the lexer function, if set to `NULL`, it resets to the default lexer function


## Parser
The parser builds the parse tree using the following grammar:
```
NUMBER = 0123456789 | mathematical_symbol
IDENTIFIER = string of characters that do not include numbers

program: addition T_EOF

addition: multiplication '+' multiplication
        | multiplication '-' multiplication
        ;

multiplication: term '*' term
              | term '/' term
              ;

term: ( expression )
    | NUMBER
    | variable
    | function
    ;

variable: IDENTIFIER != reserved

function: IDENTIFIER == reserved
        & IDENTIFIER NUMBER variable ( addition )
```

Functions follow the following format:
* `Function name` ( IDENTIFIER ) - The name function call
* `Degree` ( NUMBER ) - How many times the function is applied to the `statement`
* `Respected variable` ( variable ) - The variable which is respected during the application of the function (`INTEGRAL 1 x ( y )` = `x*y`)
* `Statement` ( ( addition ) ) - The statement the function is applied to

The parser provides two functions to call:
* tree_code_t *`build_tree`() - Returns the head of the built parse tree
* void `_set_parser_function`(tree_code_t * (*)()) - This function sets the function pointer of the parser function, if set to `NULL`, it resets to the default parser function

## Semantic Analyzer
Fairly simple aspect of the default compiler, it ensures that two criteria are met by the given parse tree:
* `Variables`, `Numbers`, and `Integers` do not have any children (`left` and `right` nodes are NULL pointers).
* Operations (`Addition`, `Subtraction`, `Division`, `Multplication`, and `Exponents`) all have two children of the following types: `Variable`, `Number`, or `Integer`

The semanitc analyzer provides two functions to call:
* void `validate`(tree_code_t *head) - Recurisevly scans tree to ensure that it meets the aforementioned requirements 
* void `_set_validate_function`(void (*)(tree_code_t *)) - This function sets the function pointer of the validate function, if set to `NULL`, it resets to the default validation function

## Code Generator
The code generator writes code for x86-64 machines and uses SSE instructions to complete arithmetic operations.

It assumes the following about the runtime enviornment:
* The stack is loaded with the required variables
* The stack grows down
* The RCX register contains the address of the start of the data buffer in memory

There is only one available function to call which is: code_block_t `default_x86_64_generator`(tree_code_t *tree, int var_count). This function simply converts the given tree into a buffer of machine code and a buffer of IEEE floats.

The code generator can be swapped out with the use of: void `set_code_generator`(code_block_t (*)(tree_code_t *, int)); if set to NULL, it resets to the default code generator function.

## Run Function
The run function simply loads the given `code_block_t` into an executable buffer: 
* If `caching` is `enabled`, this is done through the cacher function.
* If `caching` is `disabled`, the run function requests the size of the program (`code` + `data`) at `virtual` memory address `0x0` to be allocated to it, and does book-keeping to ensure that if the same function is called as last time it will not allocate a new buffer for it at `0x0`.

After loading the code into an executable buffer, it pushes `RCX` to the stack and writes the pointer to the `data` buffer to it (`buffer.base` + `code.code_size`).

It will also push given variables onto the stack in the order they were given to it.

The executable buffer is then called, and its return value stored in a local variable.

The variables are then popped off the stack, and `RCX` restored.

The local variable is then returned.

The run function can be swapped out with the use of: void `set_run_function`(double (*)(code_block_t *, va_list)); if set to NULL, it resets to the default run function.

## Cacher Function
The caher function serves to allocate and manage multiple executable buffers, so that they do not need to be reallocated and have that time wasted. 

The default cacher function accounts for these cases:
* The code to cache is already cached - Return the cache in question
* If the number of available caches are changed in the Idozit Word - Update the number of slots there are in the cache `blocks` array
* Make sure caches are sorted (lowest to highest base address) - Simple bubble sort ( O(N^2) complexity, rather slow )
* Mark caches that are older than the `cache_cut_off` time as free
* If there is a free space available use it, address cases:
  * The block that is free is just the right size - Load the new code and data in there and return it
  * The block is too small - Free the block's memory, return a `miss`
* If the cacher reached the maximum number of usable blocks, find the oldest amongst them and:
  * If it is big enough to fit the to cache code and data - Load it with the new code and data
  * If it is too small, free the block's memory, and return a `miss`
* If the code hasn't returned by now, return a `miss` - Allocate a new block and load it with the code and data

The cacher is very simple and is most definitely not a water tight solution.

The cacher function can be swapped out with the use of: void `set_cacher_function`(void * (*)(code_block_t *)); if set to NULL, it resets to the default cacher function.
