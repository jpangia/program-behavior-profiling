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
- use clang transformer to insert print statements into input code and write the code to a new c source file