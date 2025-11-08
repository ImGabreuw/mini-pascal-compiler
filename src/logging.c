#include "logging.h"

#include <stdlib.h>

static FILE *token_file;

void log_init(const char *program_name)
{
    char token_filename[MAX_LOG_FILENAME];
    snprintf(token_filename, sizeof(token_filename), "%s.tokens", program_name);

    token_file = fopen(token_filename, "w");
    if (token_file == NULL)
    {
        perror("Error opening token output file");
        exit(EXIT_FAILURE);
    }
}

void log_token(const Token *token)
{
    if (token == NULL)
    {
        return;
    }

    if (token_file)
    {
        char log_line[MAX_LOG_LINE];
        snprintf(log_line,sizeof(log_line),"%02d # %-30s | %s\n",token->line, (token->type == TOKEN_KEYWORD ? token->value : token_type_to_string(token->type)),token->value);

        printf("%s", log_line);
        fprintf(token_file, "%s", log_line);
    }
}

void log_lexical_error(int line, char invalid_char)
{
    if (token_file)
    {
        char log_line[MAX_LOG_LINE];
        snprintf(log_line, sizeof(log_line), "Lexical Error at line %02d: invalid character '%c' (ASCII code: %d)\n", line, invalid_char, invalid_char);

        printf("%s", log_line);
    }
}

void log_syntax_error(const Token *token)
{
    if (token_file)
    {
        char log_line[MAX_LOG_LINE];

        if (token == NULL)
        {
            snprintf(log_line, sizeof(log_line), "Syntax Error: Unexpected end of file\n");
        }
        else
        {
            snprintf(log_line, sizeof(log_line), "Syntax Error at line %02d: Unexpected token '%s' of type %s\n", token->line, token->value, token_type_to_string(token->type));
        }

        printf("%s", log_line);
    }
}

void log_semantic_error_duplicate(int line, const char *identifier)
{
    char log_line[MAX_LOG_LINE];
    snprintf(log_line, sizeof(log_line), "Semantic Error at line %02d: Variable '%s' already declared\n", line, identifier);
    printf("%s", log_line);
}

void log_semantic_error_undeclared(int line, const char *identifier)
{
    char log_line[MAX_LOG_LINE];
    snprintf(log_line, sizeof(log_line), "Semantic Error at line %02d: Variable '%s' not declared\n", line, identifier);
    printf("%s", log_line);
}

void log_semantic_error_type_mismatch(int line, const char *identifier, const char *expected_type, const char *actual_type)
{
    char log_line[MAX_LOG_LINE];
    snprintf(log_line, sizeof(log_line), "Semantic Error at line %02d: Type mismatch for variable '%s' (expected: %s, actual: %s)\n", 
             line, identifier, expected_type, actual_type);
    printf("%s", log_line);
}

void log_cleanup()
{
    if (token_file)
    {
        fclose(token_file);
        token_file = NULL;
    }
}