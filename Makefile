# Obligatory block comment
# I will fill this later once the project is more underway.
# currently just learning makefiles so the repo can be well-structured

VPATH = ./build:./include:./src

BUILD_DIR = ./build
SRC_DIR = ./src
INCLUDE_DIR = ./include

hellomain: $(SRC_DIR)/hellomain.c $(SRC_DIR)/print_hello.c
	gcc $^ -o $(BUILD_DIR)/hellomain -I$(INCLUDE_DIR)

clean:
	rm -f $(BUILD_DIR)/*