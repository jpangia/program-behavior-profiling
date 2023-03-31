Originally written for North Carolina State University class CSC 766: Code Optimization for Scalar and Parallel Programs in Spring 2023

--------------------------
CURRENTLY WORK_IN_PROGRESS
--------------------------

OBJECTIVES:
-----------
1. Build a branch-pointer tracing tool to insert instrumentation code that prints branch trace into the input c source file
2. Build an instruction counting tool to output the number of instructions executed by an input program

TODO:
-----
- figure out how to access clang tools
- use clang transformer to insert print statements into input code and write the code to a new c source file
    - take an input program
    - feed it to clang transformer
    - whenever it sees a branch:
        - add an instrumentation to the branch that outputs the branch id if the branch executes
        - write the branch id, its home source file, its line number, and its target line number to a dictionary file
- complete build-utils/get-llvm.sh