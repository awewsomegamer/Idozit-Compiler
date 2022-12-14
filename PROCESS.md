# Process
The outline to the process of the library is very simple. The programmer can call upon these core functions:
* expression
* compile
* run
* set

### Expression function
The expression function takes three parameters:
* `form` - The string containing the program
* `var_count` - The number of variables used within the program string
* `...` - A list of parameters that specify the name of the variables *\**

*\** The order in which the programmer puts them in is the order they must be put in the run function ("x", "y" -> run(15.0, 16.0) -> x = 15, y = 16).<br>

This expression function returns a `context_t` structure, which specifies the number of variables and the head of the `tree_code_t` structure in which the program is.


### Compile function
The compile function uses a single operand, the `context_t` structure that the expression function returns. The compile function uses the given code generator to recursively evaluate the tree and generate code. By default the code generated is x86-64 machine code.


### Run function
The run function takes the below parameters:
* `code` - This operand is the code_block_t structure to run
* `...` - This series of operands specifies the doubles that will be the variables within the program (see the note in the expression function)

This function simply puts the code and data of the program into an executable buffer, pushes all variable value's onto the stack, runs the program, and returns its value.

### Set functions
These functions use one argument, a function pointer, to set the functions responsible for the following parts of the compiler (definitions for items of the list can be found in the Terminology section of README.md):
* Lexer (`set_lexer`)
* Parser (`set_parser`)
* Semantic analyzer (`set_semantic_analyzer`)
* Code Generator (`set_code_generator`)
* Message Handler (`set_message_handler`)

## Structures
`token_t`:
* `type` - Type of the token
* `value` - The floating point value of the token

`tree_code_t`:
* `value` - Floating point value of the node
* `type` - The type of the node
* `parser_mark` - The a value for the parser \*
* `left`, `right`, `parent` - Pointers to the respective nodes

`context_t`:
* `head` - A pointer to the head of the parse tree
* `var_count` - The number of variables contained in the program

`code_block_t`:
* `func` - A pointer to the buffer which contains the executable code
* `data` - A pointer to the buffer which contains non1-executable initalized data used by the program
* `code_size` - The number of bytes within the code buffer
* `data_size` - The number of bytes within the data buffer
* `var_count` - The number of variables contained in the program

The above sections list the core parts of the compiler, the next section specifies some functions to be implemented

## To-do
* `tree_to` - This function is designed to convert the parse tree to a given language, this may be removed as the code generator is swapable

* `objectify` - This function takes a file and a seties of code_block_t structures and converts them into an object file which can be linked with a regular compiler to other programs

