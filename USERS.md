# Users

## What

This document outlines how a user of the Idozit compiler will be able to interface with the library. This document can be thought of as the process to use the compiler.

## Functions
### expression(const char *form, int var_count, ...)
The expression function encompasses three aspects of the compiler, which are the following:
* Lexical Analysis
* Parsing
* Semantic Analysis

It asks for the following arguments:
* `const char *form` - The source code given to the lexer
* `int var_count` - Mandated by default compiler (see <a href="DEFAULTS.md">DEFAULTS.md</a>)
* `...` - Mandated by default compiler (see <a href="DEFAULTS.md">DEFAULTS.md</a>)

The expression function returns a `context_t` structure.

### compile(context_t context);
The compile function takes a context structure, and uses the given code generator to convert it into the target language.

It asks for the following arguments:
* `context_t context` - Required context structure which gives the code generator what it needs.


The compile function returns a `code_block_t` structure.

### run(code_block_t *code, ...)
This function will run the code (whatever "run" means in the user's context - could be printing the generated code or running it on the CPU).

It asks for the following arguments:
* `code_block_t *code` - A pointer to the generatedd `code_block_t` structure
* `...` - Any number of variables

The run function returns a double which is the return value of the executed code.

## Process
The process of using the Idozit compiler is as follows:
* Expression
* Compile
* Run

An example program using the default compiler is made as such:
```c
#include <stdlib.h>
#include <stdio.h>
#include <idozit.h>

int main() {
        // Compile the expression "5 + 5" which contains 0 variables and express it in the variable named "program"
        code_block_t program = compile(expression("5 + 5", 0));

        // Run the program in the variable "program"
        printf("%f\n", run(&program));
}
```
To compile the above code into a executable use the following command:
```console
$ gcc program.c -o executable.out -L./ -lcidozit
```
Where -L points to the folder which contains `libcidozit.a`.

## Cusotmization
As described in the `What` section of <a href="README.md">README.md</a>, the Idozit library has the ability to be changed (as in using a different lexer and code generator). All that needs to be ensured by the user is that the aspects which are changed are compatible with the other aspects (`lexer` is compatible with `parser`, `parser` is compatible with `semantic analyzer`, `parser` is compatible with the `code generator` and so on).

The various customization functions are listed below:
* `set_lexer`
* `set_parser`
* `set_semantic_analyzer`
* `set_code_generator`
* `set_message_handler`
* `set_run_function`
* `set_cacher_function`

All of the functions listed above only take a single operand which is a pointer to a function of the proper type.

The part of the user is to ensure that the developer of the function has listed that the function is compatible with the other functions.

The arrow denotes that the left side must be compatible with the right side:
* `Lexer Function` -> `Parser Function`
* `Parser Function` -> `Semantic Analyzer Function`
* `Parser Function` -> `Code Generator Function`
* `Code Generator Function` -> `Run Function`

### Idozit Word
The `idozit_word` variable is globally available, and it outlines some general configuration. As of writing it contains the following:
* `caching` - Should programs be cached when run? (1: yes, 0: no)
* `available_caches` - If caching is enabled, how many programs can be cached at a time?
* `cache_cut_off` - After what time (in microseconds) should a cache be free'd?
