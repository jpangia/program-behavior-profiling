# Obligatory block comment
# I will fill this later once the project is more underway.
# currently just learning makefiles so the repo can be well-structured

buildDir = ./build
objDir = ./obj
srcDir = ./src
#the install location of llvm
llvmDir = ../llvm-project
llvmBuildDir = $(llvmDir)/build
libDir = $(llvmDir)/build/lib


includeFlags != llvm-config --cxxflags
includeFlags += -I./include \
			  -I$(llvmDir)/clang/include \
			  -I$(llvmDir)/build/tools/clang/include \
			  -I$(llvmBuildDir)/tools/clang/include

# cxxFlags = -fuse-ld=lld

llvmLibs != llvm-config --ldflags --libs --system-libs
CLANG_LIBS := \
	-Wl,--start-group \
	-lclang \
	-lclangAnalysis \
	-lclangARCMigrate \
	-lclangAST \
	-lclangASTMatchers \
	-lclangBasic \
	-lclangCodeGen \
	-lclangCrossTU \
	-lclangDriver \
	-lclangEdit \
	-lclangFormat \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangIndex \
	-lclangLex \
	-lclangParse \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangSema \
	-lclangSerialization \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSupport \
	-lclangToolingCore \
	-lclangTooling \
	-Wl,--end-group

llvmLibFlags = -L$(libDir) $(CLANG_LIBS) $(llvmLibs)

.PHONY: all
all: $(objDir)/print_hello.o $(buildDir)/hellomain $(buildDir)/sample-frontend-action

$(objDir)/%.o: $(srcDir)/%.c
	g++ $^ -c -o $@ -fPIC $(includeFlags) $(llvmLibFlags)

$(buildDir)/%: $(objDir)/%.o
	g++ $^ -o $@ -fPIC $(includeFlags) $(llvmLibFlags)

$(objDir)/print_hello.o: $(srcDir)/print_hello.c
	g++ $^ -c -o $@ $(includeFlags)

$(buildDir)/hellomain: $(srcDir)/hellomain.c $(objDir)/print_hello.o
	g++ $^ -o $@ $(includeFlags)

.PHONY: clean
clean:
	rm -f $(buildDir)/* $(objDir)/*