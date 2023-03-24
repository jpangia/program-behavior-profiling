# Obligatory block comment
# I will fill this later once the project is more underway.
# currently just learning makefiles so the repo can be well-structured

VPATH = ./build:./include:./src

buildDir = ./build
objDir = ./obj
srcDir = ./src
includeDir = ./include

.PHONY: all
all: $(objDir)/print_hello.o $(buildDir)/hellomain

$(objDir)/print_hello.o: $(srcDir)/print_hello.c
	gcc $^ -c -o $@ -I$(includeDir)

$(buildDir)/hellomain: $(srcDir)/hellomain.c $(objDir)/print_hello.o
	gcc $^ -o $@ -I$(includeDir)

.PHONY: clean
clean:
	rm -f $(buildDir)/* $(objDir)/*