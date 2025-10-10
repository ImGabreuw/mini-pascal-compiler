#ifndef TOKEN_H
#define TOKEN_H

#define MAX_TOKEN_LENGTH 50
#define MAX_COMMENT_LENGTH 250

extern const char *keywords[];
extern const int num_keywords;

extern const char *arithmetic_operators[];
extern const int num_arithmetic_operators;

extern const char *relational_operators[];
extern const int num_relational_operators;

extern const char *logical_operators[];
extern const int num_logical_operators;

extern const char *delimiters[];
extern const int num_delimiters;

typedef enum
{
    TOKEN_LETTER,
    TOKEN_DIGIT,
    TOKEN_NUMBER,
    TOKEN_BOOLEAN,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR_ARITHMETIC,
    TOKEN_OPERATOR_RELATIONAL,
    TOKEN_OPERATOR_LOGICAL,
    TOKEN_OPERATOR_ASSIGNMENT,
    TOKEN_DELIMITER,
    TOKEN_COMMENT,
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
    int line;
    int simbols_table_idx;
} Token;

/**
 * Converte o tipo de token para uma string legível.
 */
const char *token_type_to_string(TokenType type);

/**
 * Cria um novo token.
 * @param type O tipo do token.
 * @param start O ponteiro para o início do valor do token na fonte.
 * @param end O ponteiro para o fim do valor do token na fonte.
 * @param line O número da linha onde o token foi encontrado.
 * @return Um ponteiro para o token criado.
 */
Token *create_token(TokenType type, const char *start, const char *end, int line);

#endif // TOKEN_H