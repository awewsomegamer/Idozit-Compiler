# Idozit

## What
This is a JIT compiler designed to be embedded within projects. Originally the main aspects of the compiler (lexer, parser, semantic analyzer, code generator) were designed for compiling mathematical expressions, but later revised to adapt to open-source standards. This revision made it possible for all aspects, which affect compilation / execution, of the compiler to be switched out, and the compiler would work as long as the switched out aspects work with all other aspects (i.e. The original lexer, parser, and semantic analyzer, but differnt code generator, message handler, and runner function), see this <a href="https://github.com/awewsomegamer/Idozit-Compiler/blob/master/USERS.md#cusotmization">section</a> to see how the various aspects link together.

## Terminology
* `Lexer` - This aspect of the compiler is responsible for reading in the given string stream (program) and converting them into structures which the parser, semantic analyzer, and code generator can recognize.

* `Parser` - This aspect uses the output of the lexer and converts it into a binary tree structure based on the implemented grammer.

* `Semantics / Semantic Analyzer` - This aspect of the compiler is responsible for looking at the generated tree structure, and making sure that it contextually makes sense (i.e. variables cannot have children, operations must branch down to two other branches, etc...).

* `Code Generator` - This aspect of the compiler generates valid code for the target language.

* `Message Handler` - While preforming the above aspects, the compiler must have a way to report debug messages, errors, warnings, and fatal errors. **It is encouraged that the programmer writes this aspect specifically for the program**

* `Runner Function` - This function uses the output of the code generator, and runs it (whatever that may mean (print out the code or get the CPU to execute it)).

* `Cacher Function` - This function is used by the `Runner Function` to cache given code blocks.

## Documentation
Further documentation about how to program **for** the library is available at <a href="DEVELOPERS.md">DEVELOPERS.md</a>.<br>
Further documentation about how to **use** the library is available at <a href="USERS.md">USERS.md</a>.<br>
 
## Repository Contents
`/test/` - A directory containing the test files (used during development)<br>
`/src/` - Contains the source code of the library<br>
`/example_programs/` - Contains example programs / applications of the compiler<br>
`/bin/library/` - Contains the binary files for the library (`libcidozit.a` is the linkable library file (`c` stands for compiler))<br>
`./` - Contains .md files and ultimate Makefile

## Building
To build a debug version of the library, use:
```prompt
make debug
```
To build a final version of the library, use:
```prompt
make
```
For creating a new aspect of the compiler, the developer does not have to rebuild the Idozit library.

## Example programs
For information about the example programs, visit <a href="EXAMPLES.md">EXAMPLES.md</a>

## Contribution
Your code should consider the following:
* `Readability` - Is it easy to read?
* `Comments` - What is the developer thinking here? What do these lines mean?
* `Modification` - Is it easy to modify?

You must also provide a document following the lines of <a href="DEFAULTS.md">DEFAULTS.md</a>, to explain how your aspect(s) work(s), and what functions are provided. 

To create (a) new aspect(s) for the compiler, the developer does not need to build it into the library. Rather creating an external source file and implementation files (the user includes it in their project rather than it being built into than library).