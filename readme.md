Originally written for North Carolina State University class CSC 766: Code Optimization for Scalar and Parallel Programs in Spring 2023

--------------------------
CURRENTLY WORK_IN_PROGRESS
--------------------------

OBJECTIVES:
-----------
1. Build a branch-pointer tracing tool to insert instrumentation code that prints branch trace into the input C source file
2. Build an instruction counting tool to output the number of instructions executed by an input program

TODO:
-----
- examine libtooling/libastmatchers build tutorial; (I may have been barking up the wrong tree this past week....)
- learn how to:
    - get the location line (FullSourceLoc::getSpellingLineNumber() ) and target line of the branch
    - read in a program
    - write out a new source file
    - filter out code from #include directives
- use clang transformer to insert print statements into input code and write the code to a new c source file
    - take an input program
    - feed it to clang transformer
    - whenever it sees a branch:
        - add an instrumentation to the branch that outputs the branch id if the branch executes
        - write the branch id, its home source file, its line number, and its target line number to a dictionary file
- complete build-utils/get-llvm.sh

DONE:
-----
- get sample code from https://intel.github.io/llvm-docs/clang/RAVFrontendAction.html running
    - having issues with some sort of DT_TEXTREL error at link time 
    - DT_TEXTREL was not actually the issue; see docs/dev-notes for the solution
- learn how to:
    - find branch expressions (might be a generic branch category)
        - find function calls
        - find if-else
        - find switch
        - find loops
    - write dictionary file

COMPATIBILITY:
--------------
- Developed and tested on Kubuntu 22.04 
- Expected to work on Ubuntu, but untested