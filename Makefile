INCLUDE_DIR="./include"
SRC_DIR="./src"

CC=gcc
CFLAGS=-Wall -Wno-unused-result -g -Og -I$(INCLUDE_DIR) -fsanitize=address

SRC_FILES=$(shell find $(SRC_DIR) -name "*.c")
OUTPUT=compiler

all: clean compile

clean:
	@rm -f $(OUTPUT)

compile: $(SRC_FILES)
	@$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC_FILES)

# "@" para nao exibir o comando executado em tela