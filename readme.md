Originally written for North Carolina State University class CSC 766: Code Optimization for Scalar and Parallel Programs in Spring 2023
---

# COMPLETENESS STATE:
The branch-trace tool is only guaranteed to run correctly on programs that are self-contained in a single file. If the input program #includes external headers, particularly external headers that define macros, the `branch-trace` is likely to fail.

# OBJECTIVES:
1. Build a branch-pointer tracing tool to insert branch-pointer instrumentation code into the input C source file
    This is implemented as a clang-tooling-extra program. Using clang::Rewriter and AST matching, a single input source file is traversed 
    and modified. 
2. Build an instruction counting tool to output the number of instructions executed by an input program
    This is implemented as a script that uses valgrind's cachegrind tool, filters the output, and writes the instruction count to a file
    data.csv. The file is written to the location of the script file.

# DEPENDENCIES:
Ensure the following packages (for Ubuntu) are installed before attempting installation:
* `build-essential`
* `cmake`
* `ninja-build`
* `valgrind`

# INSTALLATION:
1. Clone this repository with: 
```console
git clone https://github.com/jpangia/program-behavior-profiling
```

2. Install llvm in a directory of your choice, (using a shallow clone to speed up the download)

Assuming the current working directory is the directory you want to install llvm in, execute the following:
```console
git clone --depth=1 https://github.com/llvm/llvm-project.git
cd llvm-project/
mkdir build
cd build/
cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DLLVM_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
ninja
```
(building llvm may take several hours, depending on system specs)

3. Copy the directory `$path_to_repo/program-behavior-profiling/src/branch-track` and its contents into `$path_to_llvm/llvm-project/clang-tools-extra/`
    There should now be a new directory `$path_to_llvm/llvm-project/clang-tools-extra/branch-track` containing `BranchTrack.cpp` and `CMakeLists.txt`

4. change directory into `$path_to_llvm/llvm-project/clang-tools-extra`

5. add the line `add_subdirectory(branch-track)` to the `CMakeLists.txt` file in `clang-tools-extra`

quick commandline way:
```console
echo 'add_subdirectory(branch-track)' >> CMakeLists.txt
```

6. change directory to `$path_to_llvm/llvm-project/build` and run:
```console
ninja
```
There might be one warning about an unused variable. You can ignore this.

7. There should be a binary `branch-track` in `$path_to_llvm/llvm-project/build/bin`. If so, installation is complete, if not, something went wrong.

# USAGE:
Run the branch trace tool like below:
```console
$path_to_llvm/llvm-project/build/bin/branch-track <path to input C source file> -- [path to file to write instrumented C file to [path to file to dump dictionary to]]
```
IMPORTANT: don't forget the `--` in the command. Clang behaves funny without it, and the program may either fail, or attempt to modify standard C include files. As a related precaution, avoid running this program with root privileges whenever possible

Specifying the input file is mandatory, specifying the file to write output to is optional. Only one file may be processed at a time.

If the output file is specified, a dictionary file can also be specified. If no dictionary file is specified, then the information will be written to `$path_to_llvm/llvm-project/build/bin/dictionary.txt`, overwriting any previous data in the file.

This dictionary file gives information on the branches as following:
```
branch n <source file branch lives in> <line branch originates from> <line branch goes to>
```
note: the source file is written with all the path information with which it was passed to the `branch-track` binary.

for example, The below line would describe the second branch that is recorded for an input. It lives in the source file `simpleMain.c`, at line 5, and jumps to line 18
```
branch 2 simpleMain.c 5 18
```

If no output destination is specified, the file is written to stdout.

Compile the resultant instrumented code with your favorite C compiler (tested with gcc).

Run the instruction counter like below:
```console
$path_to_repo/utils/ins-count.sh <relative path to input program> [arguments for the program]
```
note: the path to the input program must be relative, otherwise you will get a file not found from valgrind

# NOTES & ASSUMPTIONS:
- branch-track instruments its input source code by inserting fprintf calls to print to stderr. If the input source cannot call fprintf, this tool will not be able to work
- branch-track only considers break, continue, do-while, for loops, function calls, if-else, switch, and while loops. Return statements and goto statements were excluded due to complexity and time constraints

# TESTING:
Sample source files are provided in the `/sample-code` folder.
The originals are `$name.c`, the expected outputs are `$name.expected.c`. A `$name-dict.txt` file is also included that describes the branches for the associated sample case

# COMPATIBILITY:
- Developed and tested on Kubuntu 22.04 
- Tested on Ubuntu 22.04 LTS