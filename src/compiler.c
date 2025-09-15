#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

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

    scanner_init(argv[1]);
    
    Token *token;
    while ((token = get_token()) != NULL)
    {
        printf("Token: Type=%s, Value='%s', Line=%d\n", token_type_to_string(token->type), token->value, token->line);
        free(token);
    }
    
    scanner_cleanup();

    return EXIT_SUCCESS;
}
