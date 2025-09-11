#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
Referências:
- https://medium.com/@garylin132/lexer-the-first-step-of-building-a-compiler-d5e70a84b49f
*/

typedef enum
{
    TOKEN_LETTER,
    TOKEN_DIGIT,
    TOKEN_NUMBER,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_COMMENT,
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
    int line;
} Token;

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

const char *keywords[] = {
    "program", "begin", "end", "procedure", "if", "then", "else", "while", "do",
    "and", "or", "not", "var", "integer", "boolean", "true", "false", "write", "div"};
const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

// <letra> ::= _ | a | ... | z | A | ... | Z
Token *recognize_letter(const char **input, int line)
{
    const char *start = *input;

    if (!(isalpha(**input) || **input == '_'))
    {
        return NULL;
    }

    while (isalpha(**input) || **input == '_')
    {
        (*input)++;
    }

    return create_token(TOKEN_LETTER, start, *input, line);
}

// <digito> ::= 0 | 1 | ... | 9
Token *recognize_digit(const char **input, int line)
{
    const char *start = *input;

    if (!isdigit(**input))
    {
        return NULL;
    }

    (*input)++; // consumir o token de dígito

    return create_token(TOKEN_DIGIT, start, *input, line);
}

// <numero> ::= <digito> {<digito>}
Token *recognize_number(const char **input, int line)
{
    const char *start = *input;

    if (!isdigit(**input))
    {
        return NULL;
    }

    while (isdigit(**input))
    {
        (*input)++;
    }

    return create_token(TOKEN_NUMBER, start, *input, line);
}

int is_keyword(const char *str)
{
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// <identificador> ::= <letra> {<letra> | <dígito>}
Token *recognize_identifier(const char **input, int line)
{
    const char *start = *input;

    if (!(isalpha(**input) || **input == '_'))
    {
        return NULL;
    }

    (*input)++; // consumir o primeiro caractere (letra ou '_')

    while (isalpha(**input) || isdigit(**input) || **input == '_')
    {
        (*input)++;
    }

    Token *token = create_token(TOKEN_IDENTIFIER, start, *input, line);

    if (is_keyword(token->value))
    {
        token->type = TOKEN_KEYWORD;
    }
    return token;
}

// TODO implementar reconhecimento de keyword, operadores, pontuação e comentários

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Source code file not specified. Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        perror("Source code file not found.");
        exit(EXIT_FAILURE);
    }

    // Ler todo o arquivo para um buffer
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    // lexer
    const char *input = buffer;
    int line = 1;

    printf("Tokens reconhecidos:\n");

    while (*input)
    {
        if (isspace(*input))
        {
            if (*input == '\n')
                line++;
            input++;
            continue;
        }

        Token *token = NULL;

        // Comentários

        // Letras
        token = recognize_letter(&input, line);
        if (token)
        {
            printf("LETTER: %s (linha %d)\n", token->value, token->line);
            free(token->value);
            free(token);
            continue;
        }

        // Dígitos
        token = recognize_digit(&input, line);
        if (token)
        {
            printf("DIGIT: %s (linha %d)\n", token->value, token->line);
            free(token->value);
            free(token);
            continue;
        }

        // Números
        token = recognize_number(&input, line);
        if (token)
        {
            printf("NUMBER: %s (linha %d)\n", token->value, token->line);
            free(token->value);
            free(token);
            continue;
        }

        // Identificadores ou keywords
        token = recognize_identifier(&input, line);
        if (token)
        {
            if (token->type == TOKEN_KEYWORD)
            {
                printf("KEYWORD: %s (linha %d)\n", token->value, token->line);
            }
            else
            {
                printf("IDENTIFIER: %s (linha %d)\n", token->value, token->line);
            }
            free(token->value);
            free(token);
            continue;
        }

        // Operadores

        // Pontuação

        // Desconhecido
        fprintf(stderr, "Erro léxico: caractere desconhecido '%c' na linha %d\n", *input, line);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Fim da análise léxica.\n");

    free(buffer);
    return EXIT_SUCCESS;
}
