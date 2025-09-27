#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

Parser *parser_init()
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));

    if (!parser)
    {
        fprintf(stderr, "Memory allocation failed for Parser\n");
        exit(EXIT_FAILURE);
    }

    parser->current_token = NULL;
    return parser;
}

void parser_parse(Parser *parser)
{
    // TODO
}

void parser_cleanup(Parser *parser)
{
    if (parser)
    {
        free(parser);
    }
}