#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

#include "token.h" 

/**
 * @param source_filename Nome do arquivo do código-fonte
 */
void scanner_init(const char source_filename[]);

/**
 * @return O próximo token do código-fonte
 */
Token *get_token();

/**
 * Libera a memória usada pelo scanner
 */
void scanner_cleanup();

#endif