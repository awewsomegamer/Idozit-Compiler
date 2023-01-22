# Developers

## What
This document outlines the specific parts of the compiler, the default files, and how a developer will be able to create their own aspects for the Idozit compiler. This can be imagined as the definition and structure of the Idozit compiler.

## Structures
`token_t`:
* int `type` - The type of the token
* double `value` - The double value of the token
* void *`custom` - A pointer to a custom structure (in case that the default `token_t` structure is not satisfactory)

`tree_code_t`:
* double `value` - The double value of the tree node
* uint8_t `type` - The type of the tree node
* struct tree_code *`left` - A pointer to the next left node
* struct tree_code *`right` - A pointer to the next right node
* struct tree_code *`parent` - A pointer to this node's parent
* void *`custom` - A pointer to a custom structure (in case that the default `tree_code_t` structure is not satisfactory)


`context_t`:
* struct tree_code *`head` - A pointer to the first node in the parse tree
* int `var_count` - The number of variables in the parse tree
* void *`custom` - A pointer to a custom structure (in case that the default `context_t` structure is not satisfactory)


`code_block_t`:
* uint8_t *code - A pointer to the code buffer
* uint8_t *data - A pointer to the data buffer
* size_t code_size - The size in bytes of the code buffer
* size_t data_size - The size in bytes of the data buffer
* int var_count - The number of variables in the program
* void *`custom` - A pointer to a custom structure (in case that the default `code_block_t` structure is not satisfactory)


The above structures share a common member, `void *custom`. This member can be used to point to a custom structure. In this example the developer has made it so that their `code_block_t` structure doesn't require anything the default one does, instead
```c
// Declare the strucutre of the custom code_block_t
struct my_code_block {
        uint32_t magic;
        char *code;
};

// ...

// When the code generator returns the generated code, embed it in the custom structure, and point the default code_block_t structure to it
code_block_t ret = { 0 };
struct my_code_block custom = { 0xABCDEF12, "print('Hello World')"};
ret.custom = &custom;
```

### Idozit Word

`idozit_word`:<br>
This is a very important structure that establishes general setting hooks between the developer and the user. Its structure is outlined in <a href="USERS.md#idozit-word">USERS.md</a>

## Functions
A developer may choose to address any of the following functions:
* The `Lexer`
* The `Parser`
* The `Semantic Analyzer`
* The `Code Generator`
* The `Run Function`
* The `Cacher Function`

To see more on these functions visit the <a href="DEFAULTS.md">DEFAULTS.md</a> file. It will provide explanation of the default compiler, and a description of the aforementioned functions.
