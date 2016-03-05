# NCTU Intro to Compiler Design
===

### About

This is a repo for my homework in Compiler Design, NCTU. It contains four directories.
The lab is seperated into 4 parts, only `hw4` is a complete work.

+ HW1 contains only the lex scanner
+ HW2 has the lex scanner and the parser by yacc
+ HW3 is extended from HW2, a symbol table and the ability to find semantic error is added.
+ HW4 is the complete work, compiles the code and generated java bytecode

### Ability

The compiler is for the pascal-like **P language**.

+ varible: integer, real. Supports global, local and constant varible
+ loop: for, while
+ I/O: print, read
+ function: supports function define and invocation.
+ expression: arithmetic, boolean expressions
+ branch: if...else

### Dependency

+ `lex` or `flex`
+ `yacc` or `bison`
+ java runtime
+ gcc or clang to compile the compiler

### Run the compiler

Goto **HW4**

+ `Patterns` is for full functionality test
+ `test` is for small conponent test

Use `bash ./compile_and_run [filename]` to compile and run the code
for example: `bash ./compile_and_run Patterns/general.p`

### Demo screenshots

This is a game of guessing number

The code of the program

![code](https://raw.githubusercontent.com/daniel0076/NCTU-Compiler/master/hw4/screenshots/code.png)

result

![result](https://raw.githubusercontent.com/daniel0076/NCTU-Compiler/master/hw4/screenshots/result.png)

