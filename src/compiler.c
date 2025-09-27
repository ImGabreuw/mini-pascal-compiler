#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "scanner.h"
#include "parser.h"

/*
Referências:
- https://medium.com/@garylin132/lexer-the-first-step-of-building-a-compiler-d5e70a84b49f
*/

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Source code file not specified. Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *program_name = argv[0];

    log_init(program_name);
    scanner_init(argv[1]);
    parser_init();

    parser_parse();

    parser_cleanup();
    scanner_cleanup();
    log_cleanup();

    return EXIT_SUCCESS;
}
