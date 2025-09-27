#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "logging.h"

static Token *current_token = NULL;

/**
 * @brief Libera o espaço alocado para o token atual e obtém o próximo token do analisador léxico.
 */
static void token_advance()
{
    if (current_token)
    {
        free(current_token);
    }
    current_token = get_token();
}

/**
 * @brief Verifica se o token atual corresponde ao tipo e valor esperados.
 */
static bool token_check(TokenType type, const char *value)
{
    if (current_token == NULL)
        return false;

    if (current_token->type != type)
        return false;

    return value == NULL || strcmp(current_token->value, value) == 0;
}

/**
 * @brief Consome o token atual se ele corresponder ao tipo e valor esperados.
 */
static bool token_match(TokenType type, const char *value)
{
    if (token_check(type, value))
    {
        token_advance();
        return true;
    }
    return false;
}

/**
 * @brief Verifica se o token atual corresponde ao tipo e valor esperados.
 *        Se corresponder, avança para o próximo token.
 *        Caso contrário, registra um erro de sintaxe e termina o programa.
 */
static void token_expect(TokenType type, const char *value)
{
    if (!token_check(type, value))
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    token_advance();
}

// <type> ::= integer | boolean
void parser_parse_type()
{
    if (!token_check(TOKEN_KEYWORD, "integer") && !token_check(TOKEN_KEYWORD, "boolean"))
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }
    token_advance();
}

// <variable declaration> ::= <identifier > { , <identifier> } : <type>
void parser_parse_variable_declaration()
{
    token_expect(TOKEN_IDENTIFIER, NULL);

    while (token_match(TOKEN_DELIMITER, ","))
    {
        token_expect(TOKEN_IDENTIFIER, NULL);
    }

    token_expect(TOKEN_DELIMITER, ":");

    parser_parse_type();
}

// <variable declaration part> ::= <empty> | var <variable declaration> ; { <variable declaration part> ; }
void parser_parse_variable_declaration_part()
{
    if (token_match(TOKEN_KEYWORD, "var"))
    {
        parser_parse_variable_declaration();
        token_expect(TOKEN_DELIMITER, ";");

        while (token_match(TOKEN_KEYWORD, "var"))
        {
            parser_parse_variable_declaration();
            token_expect(TOKEN_DELIMITER, ";");
        }
    }
}

// <statement part> ::= begin <statement> { ; <statement> } end
void parser_parse_statement_part()
{
    token_expect(TOKEN_KEYWORD, "begin");
    parser_parse_statement();

    while (token_match(TOKEN_DELIMITER, ";"))
    {
        parser_parse_statement();
    }

    token_expect(TOKEN_KEYWORD, "end");
}

// <function declaration> ::= function < identifier > < formal parameters (variable declaration part) > : < type >; < block >
void parser_parse_function_declaration()
{
    token_expect(TOKEN_KEYWORD, "function");
    parser_parse_identifier();
    parser_parse_variable_declaration_part();
    token_expect(TOKEN_DELIMITER, ":");
    parser_parse_type();
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
}

// <procedure declaration> ::= procedure < identifier > < formal parameters (variable declaration part) > ; <block>
void parser_parse_procedure_declaration()
{
    token_expect(TOKEN_KEYWORD, "procedure");
    parser_parse_identifier();
    parser_parse_variable_declaration_part();
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
}

// <subroutine declaration part> ::= < procedure declaration | function declaration >
void parser_parse_subroutine_declaration_part()
{
    if (token_match(TOKEN_KEYWORD, "procedure"))
    {
        parser_parse_procedure_declaration();
    }
    else
    {
        parser_parse_function_declaration();
    }
}

// <block> ::= <variable declaration part> <subroutine declaration part> <statement part>
void parser_parse_block()
{
    parser_parse_variable_declaration_part();
    parser_parse_subroutine_declaration_part();
    parser_parse_statement_part();
}

//<program> ::= program <identifier> ; <block> .
void parser_parse_program()
{
    token_expect(TOKEN_KEYWORD, "program");
    parser_parse_identifier();
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
    token_expect(TOKEN_DELIMITER, ".");
}

void parser_parse()
{
    parser_parse_variable_declaration_part();
}

void parser_init()
{
    token_advance(); // Inicializa o primeiro token
}

void parser_parse()
{
    parser_parse_program();
}

void parser_cleanup()
{
    if (current_token)
    {
        free(current_token);
        current_token = NULL;
    }
}