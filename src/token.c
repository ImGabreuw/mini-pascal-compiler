#include <stdlib.h>
#include <string.h>

#include "token.h"

const char *keywords[] = {
    "program", "begin", "end", "procedure", "if", "then", "else", "while", "do",
    "and", "or", "not", "var", "integer", "boolean", "true", "false", "write", "div"};

const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

const char *arithmetic_operators[] = {"+", "-", "*", "div"};
const int num_arithmetic_operators = sizeof(arithmetic_operators) / sizeof(arithmetic_operators[0]);

const char *relational_operators[] = {"=", "<>", "<", "<=", ">", ">="};
const int num_relational_operators = sizeof(relational_operators) / sizeof(relational_operators[0]);

const char *logical_operators[] = {"and", "or", "not"};
const int num_logical_operators = sizeof(logical_operators) / sizeof(logical_operators[0]);

const char *delimiters[] = {"(", ")", ",", ":", ".", ";"};
const int num_delimiters = sizeof(delimiters) / sizeof(delimiters[0]);

const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case TOKEN_LETTER:
        return "LETTER";
    case TOKEN_DIGIT:
        return "DIGIT";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_BOOLEAN:
        return "BOOLEAN";
    case TOKEN_KEYWORD:
        return "KEYWORD";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_OPERATOR_ARITHMETIC:
        return "OPERATOR_ARITHMETIC";
    case TOKEN_OPERATOR_RELATIONAL:
        return "OPERATOR_RELATIONAL";
    case TOKEN_OPERATOR_LOGICAL:
        return "OPERATOR_LOGICAL";
    case TOKEN_OPERATOR_ASSIGNMENT:
        return "OPERATOR_ASSIGNMENT";
    case TOKEN_DELIMITER:
        return "DELIMITER";
    case TOKEN_COMMENT:
        return "COMMENT";
    default:
        return "UNKNOWN";
    }
}

Token *create_token(TokenType type, const char *start, const char *end, int line)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;

    token->value = (char *)malloc(end - start + 1);
    strncpy(token->value, start, end - start);
    token->value[end - start] = '\0';

    token->line = line;
    return token;
}