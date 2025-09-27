#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct Parser
{
    Token *current_token;
} Parser;

/**
 * @brief Cria e inicializa um analisador sintático.
 */
Parser *parser_init();

/**
 * @brief Inicia o processo de análise sintática.
 */
void parser_parse(Parser *parser);

/**
 * @brief Libera os recursos alocados pelo analisador sintático.
 */
void parser_cleanup(Parser *parser);

#endif // PARSER_H