#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "token.h"
#include "logging.h"

static FILE *source_file;
static int current_line = 1;

Token symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

static bool is_letter(const char ch)
{
    return isalpha(ch) || ch == '_';
}

// <letra> ::= _ | a | ... | z | A | ... | Z
bool recognize_letter(const char *buffer)
{
    if (buffer[0] == '\0')
    {
        return false; // Empty string
    }

    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (!is_letter(buffer[i]))
        {
            return false;
        }
    }

    return true;
}

// <digito> ::= 0 | 1 | ... | 9
bool recognize_digit(const char *buffer)
{
    if (buffer[0] == '\0')
    {
        return false; // Empty string
    }

    if (buffer[1] != '\0')
    {
        return false; // More than one character
    }

    return isdigit(buffer[0]);
}

// <numero> ::= <digito> {<digito>}
bool recognize_number(const char *buffer)
{
    if (buffer[0] == '\0')
    {
        return false; // Empty string
    }

    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (!isdigit(buffer[i]))
        {
            return false;
        }
    }

    return true;
}

// <boolean> ::= true | false
bool recognize_boolean(const char *buffer)
{
    return strcmp(buffer, "true") == 0 || strcmp(buffer, "false") == 0;
}

// <keyword> ::= program | begin | ...
bool recognize_keyword(const char *buffer)
{
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(buffer, keywords[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// <identificador> ::= <letra> {<letra> | <dígito>}
bool recognize_identifier(const char *buffer)
{
    if (buffer[0] == '\0')
    {
        return false; // Empty string
    }

    if (!is_letter(buffer[0]))
    {
        return false;
    }

    for (int i = 1; buffer[i] != '\0'; i++)
    {
        if (!(is_letter(buffer[i]) || isdigit(buffer[i])))
        {
            return false;
        }
    }

    return true;
}

// <operador aritmético> ::= + | - | * | div
bool recognize_arithmetic_operator(const char *buffer)
{
    for (int i = 0; i < num_arithmetic_operators; i++)
    {
        if (strcmp(buffer, arithmetic_operators[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// <operador relacional> ::= = | <> | < | <= | > | >=
bool recognize_relational_operator(const char *buffer)
{
    for (int i = 0; i < num_relational_operators; i++)
    {
        if (strcmp(buffer, relational_operators[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// <operador lógico> ::= and | or | not
bool recognize_logical_operator(const char *buffer)
{
    for (int i = 0; i < num_logical_operators; i++)
    {
        if (strcmp(buffer, logical_operators[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// <operador de atribuição> ::= :=
bool recognize_assignment_operator(const char *buffer)
{
    return strcmp(buffer, ":=") == 0;
}

// <delimitador> ::= ( | ) | , | : | . | ;
bool recognize_delimiter(const char *buffer)
{
    for (int i = 0; i < num_delimiters; i++)
    {
        if (strcmp(buffer, delimiters[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// <comentario> ::= /* { qualquer caractere } */
bool recognize_comment(const char *buffer)
{
    int len = strlen(buffer);

    if (len < 4)
    {
        return false; // Minimum length for a comment is 4: "/* */"
    }

    return buffer[0] == '/' && buffer[1] == '*' && buffer[len - 2] == '*' && buffer[len - 1] == '/';
}

void scanner_init(const char source_filename[])
{
    source_file = fopen(source_filename, "r");

    if (source_file == NULL)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    current_line = 1;
}

Token *get_token()
{
    char buffer[MAX_TOKEN_LENGTH];
    int buffer_index = 0;
    int ch;

    // 1. Pular espaços em branco
    while ((ch = fgetc(source_file)) != EOF && isspace(ch))
    {
        if (ch == '\n')
        {
            current_line++;
        }
    }

    if (ch == EOF)
    {
        return NULL;
    }

    // Reconhecer comentários
    if (ch == '/')
    {
        int next_ch = fgetc(source_file);
        if (next_ch == '*')
        {
            // Processar comentário completo
            char comment_buffer[MAX_COMMENT_LENGTH];
            int comment_index = 0;

            comment_buffer[comment_index++] = ch;
            comment_buffer[comment_index++] = next_ch;

            bool comment_closed = false;
            while ((ch = fgetc(source_file)) != EOF)
            {
                if (ch == '\n')
                {
                    current_line++;
                }

                if (comment_index < MAX_COMMENT_LENGTH - 1)
                {
                    comment_buffer[comment_index++] = ch;
                }

                if (ch == '*')
                {
                    int potential_end = fgetc(source_file);
                    if (potential_end == '/')
                    {
                        if (comment_index < MAX_COMMENT_LENGTH - 1)
                        {
                            comment_buffer[comment_index++] = potential_end;
                        }
                        comment_closed = true;
                        break;
                    }
                    else
                    {
                        ungetc(potential_end, source_file);
                    }
                }
            }

            if (!comment_closed)
            {
                fprintf(stderr, "Lexical Error: Unterminated comment starting at line %d\n", current_line);
                exit(EXIT_FAILURE);
            }

            comment_buffer[comment_index] = '\0';

            Token *token = create_token(TOKEN_COMMENT, comment_buffer, comment_buffer + comment_index, current_line);
            log_token(token);

            free(token->value);
            free(token);
            
            return get_token(); // Pegar o próximo token após o comentário
        }
        else
        {
            ungetc(next_ch, source_file);
        }
    }

    // Colocar o primeiro caractere válido de volta
    ungetc(ch, source_file);

    // 2. Encontrar a maior correspondência
    int last_match_length = 0;
    TokenType last_match_type = TOKEN_IDENTIFIER;

    while (1)
    {
        ch = fgetc(source_file);

        if (ch == EOF)
            break;

        buffer[buffer_index++] = ch;
        buffer[buffer_index] = '\0';

        // Verifica quais reconhecedores ainda correspondem ao buffer atual
        int current_match = 0;

        if (recognize_number(buffer))
        {
            last_match_type = TOKEN_NUMBER;
            current_match = 1;
        }

        if (recognize_identifier(buffer))
        {
            last_match_type = TOKEN_IDENTIFIER;
            current_match = 1;
        }

        if (recognize_keyword(buffer))
        {
            last_match_type = TOKEN_KEYWORD;
            current_match = 1;
        }

        if (recognize_boolean(buffer))
        {
            last_match_type = TOKEN_BOOLEAN;
            current_match = 1;
        }

        if (recognize_arithmetic_operator(buffer))
        {
            last_match_type = TOKEN_OPERATOR_ARITHMETIC;
            current_match = 1;
        }

        if (recognize_relational_operator(buffer))
        {
            last_match_type = TOKEN_OPERATOR_RELATIONAL;
            current_match = 1;
        }

        if (recognize_logical_operator(buffer))
        {
            last_match_type = TOKEN_OPERATOR_LOGICAL;
            current_match = 1;
        }

        if (recognize_assignment_operator(buffer))
        {
            last_match_type = TOKEN_OPERATOR_ASSIGNMENT;
            current_match = 1;
        }

        if (recognize_delimiter(buffer))
        {
            last_match_type = TOKEN_DELIMITER;
            current_match = 1;
        }

        if (current_match)
        {
            last_match_length = buffer_index;
        }
        else
        {
            // O caractere quebrou todas as correspondências do buffer
            ungetc(ch, source_file); // Voltar uma posição no arquivo
            break;
        }
    }

    // 3. Criar o token com base na maior correspondência encontrada
    if (last_match_length > 0)
    {
        buffer[last_match_length] = '\0';
        Token *token = create_token(last_match_type, buffer, buffer + last_match_length, current_line);
        log_token(token);

        if (token->type == TOKEN_IDENTIFIER)
        {
            token->simbols_table_idx = symbol_count;
            symbol_table[symbol_count++] = *token;
        }

        return token;
    }

    //TODO avaliar necessidade desse bloco
    // Se o caractere atual for EOF (acabou o arquivo)
    if (ch == EOF)
    {
        return NULL;
    }

    // Nenhum token foi reconhecido
    ch = fgetc(source_file);
    log_lexical_error(current_line, ch);
    exit(EXIT_FAILURE);
}

void scanner_cleanup()
{
    if (source_file)
    {
        fclose(source_file);
        source_file = NULL;
    }
}