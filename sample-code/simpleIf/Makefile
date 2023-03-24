# simple makefile to compile the toy program

.PHONY: all

all: simple_if

%: %.c
	gcc $^ -o $@

.PHONY: clean
clean:
	rm -r simple_if