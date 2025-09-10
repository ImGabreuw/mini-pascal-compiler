CC=gcc
CFLAGS=-Wall -Wno-unused-result -g -Og

EXECUTABLE=compiler

all: compile

clean:
	@rm -f $(EXECUTABLE)

compile: compiler.c
	@$(CC) $(CFLAGS) -o $(EXECUTABLE) compiler.c

run: compile
	@./$(EXECUTABLE)

# "@" before a command suppresses the command output