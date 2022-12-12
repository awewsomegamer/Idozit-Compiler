# Idozit

## What
This is a JIT compiler designed to be used within projects. Originally the aspects of the compiler (lexer, parser, semantic analyzer, code generator, and message handler) were designed for compiling mathematical expressions, but later revised to adapt to open-source standards. This revision made it possible for all aspects of the compiler to be switched out, and the compiler would work as long as it has been given the correct aspects (i.e. The original lexer, parser, and semantic analyzer, but a differnt code generator and message handler).

## Terminology
`Lexer` - This aspect of the compiler is responsible for reading in the given string stream (program) and converting them into structures which the parser, semantic analyzer, and code generator can recognize.<br>
`Parser` - This aspect uses the output of the lexer and converts it into a binary tree structure based on the implemented grammer (the default grammer can be found at the top of the "design1.h" header file)<br>
`Semantics / Semantic Analyzer` - This aspect of the compiler is responsible for looking at the generated tree structure, and making sure that it contextually makes sense (i.e. variables cannot have children, operations must branch down to two other branches, etc...)<br>
`Code Generator` - This aspect of the compiler generates valid code for the target language (the default code generator writes x86-64 machine code)<br>
`Message Handler` - While preforming the above aspects, the compiler must have a way to report debug messages, errors, warnings, and errors which cannot be recovered from. **It is encouraged that the programmer writes this aspect specifically for the program**.<br>

## 
