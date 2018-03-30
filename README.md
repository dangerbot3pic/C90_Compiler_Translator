# langproc-2017-cw-pie


EE2 Compiler Coursework completed during my Second Year of EIE at Imperial College

Project completed by me, Padmanaba Srinivasan and my partner Sam Stratton.

I say compiler, but in reality it's also a translator, able to convert C90 code into Python or compile into MIPS32 assembly.

Not all the C90 spec is met. The following was implemented:

- All arithmetic
- Binary ops || and &&
- Comparisons such as <, >, <=, >=, == and !=
- For loops (with nesting)
- While loops (with nesting)
- Do While loops (with nesting)
- Multiple function declarations
- Function calls
- Recursion
- More than four arguments in function calls
- Simple pointers and references (ie. no double or more pointers)
- Arrays (1D only)
- Probably some more stuff I can't recall at the moment. 

The code should be clear enough to follow, I have tried to comment as much as practically needed.

Testbenches for the translator and compiler are included along with some tests.

To run the compiler tests:
    make bin/c_compiler
    ./test_compiler.sh

Assembly code is printed in the temps file, along with any errors at the sub stages in further sub-directories. Compiler tests are stored in test_deliverable/test_cases

To run the translator tests:
    make bin/c_compiler (not needed if already run for compiler tests)
    ./test_translator.sh

The translator tests are different to those of the compiler tests as not all C code translates well to Python

Translator tests in pytest, and python files produced are stored in pyres

As an aside, python tests weren't part of the project requirement hence the reduced testing in this area

Feel free to message me if you have any questions! 
