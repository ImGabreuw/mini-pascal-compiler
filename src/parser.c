#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "logging.h"

void match(Token *current_token, TokenType expected_type, const char *expected_value)
{
    if (current_token == NULL)
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    bool type_mismatch = current_token->type != expected_type;
    bool value_mismatch = expected_value != NULL && strcmp(current_token->value, expected_value) != 0;

    if (type_mismatch || value_mismatch)
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    free(current_token);
}

// <type> ::= integer | boolean
void parser_parse_type()
{
    Token *current_token = get_token();

    bool type_mismatch = current_token->type != TOKEN_KEYWORD;
    bool value_mismatch = strcmp(current_token->value, "integer") != 0 && strcmp(current_token->value, "boolean") != 0;

    if (type_mismatch || value_mismatch)
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    free(current_token);
}

// <variable declaration> ::= <identifier > { , <identifier> } : <type>
void parser_parse_variable_declaration()
{
    Token *current_token = get_token();
    match(current_token, TOKEN_IDENTIFIER, NULL);

    current_token = get_token();
    while (current_token->type == TOKEN_DELIMITER && strcmp(current_token->value, ",") == 0)
    {
        match(current_token, TOKEN_DELIMITER, ",");

        current_token = get_token();
        match(current_token, TOKEN_IDENTIFIER, NULL);

        current_token = get_token();
    }

    match(current_token, TOKEN_DELIMITER, ":");

    parser_parse_type();
}

// <variable declaration part> ::= <empty> | var <variable declaration> ; { <variable declaration part> ; }
void parser_parse_variable_declaration_part()
{
    Token *current_token = get_token();

    if (current_token == NULL)
    {
        return; // End of input, empty production
    }

    if (current_token->type == TOKEN_KEYWORD && strcmp(current_token->value, "var") == 0)
    {
        match(current_token, TOKEN_KEYWORD, "var");

        parser_parse_variable_declaration();

        current_token = get_token();
        match(current_token, TOKEN_DELIMITER, ";");

        parser_parse_variable_declaration_part();
    }
    else
    {
        // Produção vazia
        free(current_token);
    }
}

void parser_parse()
{
    parser_parse_variable_declaration_part();
}