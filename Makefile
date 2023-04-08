# Obligatory block comment
# I will fill this later once the project is more underway.
# currently just learning makefiles so the repo can be well-structured

VPATH = ./build:./include:./src

buildDir = ./build
objDir = ./obj
srcDir = ./src
#the install location of llvm
llvmDir = ../llvm-project
llvmBuildDir = $(llvmDir)/build
libDir = $(llvmDir)/build/lib

includeFlags = -I./include \
			  -I$(llvmDir)/clang/include \
			  -I$(llvmDir)/llvm/include \
			  -I$(llvmDir)/build/include \
			  -I$(llvmDir)/build/tools/clang/include \
			  -I$(llvmBuildDir)/tools/clang/include

cxxFlags = -fuse-ld=lld

#CLANG_LIBS := -L $(libDir) \
	-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangCrossTU \
	-lclangIndex \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group

.PHONY: all
all: $(objDir)/print_hello.o $(buildDir)/hellomain $(buildDir)/sample-frontend-action

$(objDir)/%.o: $(srcDir)/%.c
	g++ $^ -c -o $@ $(includeFlags) $(cxxFlags)

$(buildDir)/%: $(objDir)/%.o
	g++ $^ -o $@ $(includeFlags) $(cxxFlags)

$(objDir)/print_hello.o: $(srcDir)/print_hello.c
	g++ $^ -c -o $@ $(includeFlags)

$(buildDir)/hellomain: $(srcDir)/hellomain.c $(objDir)/print_hello.o
	g++ $^ -o $@ $(includeFlags)

.PHONY: clean
clean:
	rm -f $(buildDir)/* $(objDir)/*