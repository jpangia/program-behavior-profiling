#!/bin/bash

# this script builds llvm in the folder it is run in
# TODO: add mechanism to set build location

script_dir=$(pwd)
#uncomment to use the ninja build system (recommended)
#ninja=true
# TODO: cd to user-selected llvm install folder

echo "script work in progress; it won't do anything but exit right now"
exit

git clone --depth=1 https://github.com/llvm/llvm-project.git
cd llvm-project/
mkdir build
cd build/

#TODO: remove the if-else and just remove the ninja stuff
if [ $ninja ]
then
    #cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DLLVM_BUILD_TESTS=ON  # Enable tests; default is off.
    cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DLLVM_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
    ninja
    # this will require entering root password
    sudo ninja install
else
then
cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
make
fi

echo "***get-llvm.sh says:***"

#add llvm/build/bin to $PATH, assuming the script is in llvm-project/build directory
echo -e "\n#add llvm to PATH" >> ~/.bashrc
echo "export PATH=\$PATH:$(pwd)/bin" >> ~/.bashrc