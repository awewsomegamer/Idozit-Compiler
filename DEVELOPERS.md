# Developers

## What
This document outlines the specific parts of the compiler, the default files, and how a developer will be able to create their own aspects for the Idozit compiler. This can be imagined as the definition and structure of the Idozit compiler.

## Structures
`token_t`:
* int `type` - The type of the token
* double `value` - The double value of the token
* void *`custom` - A pointer to a custom structure (in case that the default `token_t` structure is not satisfactory)

`tree_code_t`:
* int `type` - The type of the token
* double `value` - The double value of the token
* void *`custom` - A pointer to a custom structure (in case that the default `token_t` structure is not satisfactory)


`context_t`:
* int `type` - The type of the token
* double `value` - The double value of the token
* void *`custom` - A pointer to a custom structure (in case that the default `token_t` structure is not satisfactory)


`code_block_t`:
* int `type` - The type of the token
* double `value` - The double value of the token
* void *`custom` - A pointer to a custom structure (in case that the default `token_t` structure is not satisfactory)

`idozit_word`:<br>
This is a very important structure that establishes general hooks between the developer and the user. Its structure is outlined in <a href="USERS.md#Idozit_Word">USERS.md</a>

## Functions
