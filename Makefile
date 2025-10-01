INCLUDE_DIR="./include"
SRC_DIR="./src"

CC=gcc
CFLAGS=-Wall -Wno-unused-result -g -Og -I$(INCLUDE_DIR)

SRC_FILES=$(shell find $(SRC_DIR) -name "*.c")
OUTPUT=compiler

all: clean compile

clean:
	@rm -f $(OUTPUT)

compile: $(SRC_FILES)
	@$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC_FILES)

# "@" before a command suppresses the command output