#!/bin/bash

# this script builds llvm in the folder it is run in
# TODO: add mechanism to set build location

script_dir= $(pwd)

# TODO: cd to user-selected llvm install folder

git clone --depth=1 https://github.com/llvm/llvm-project.git
cd llvm-project/
mkdir build
cd build/
cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
make

echo "***get-llvm.sh says:***"

#add llvm/build/bin to $PATH, assuming the script is in llvm-project/build directory
echo -e "\n#add llvm to PATH" >> ~/.bashrc
echo "export PATH=\$PATH:$(pwd)/bin" >> ~/.bashrc