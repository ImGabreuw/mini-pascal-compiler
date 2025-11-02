#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

#define MAX_IDENTIFIER_LENGTH 16
#define MAX_TYPE_LENGTH 8

typedef struct SymbolNode {
    char identifier[MAX_IDENTIFIER_LENGTH];
    int address;
    char type[MAX_TYPE_LENGTH];
    struct SymbolNode *next;
} SymbolNode;

void symbol_table_init();

/**
 * @brief Insere um novo símbolo na tabela de símbolos
 * @param identifier Nome do identificador
 * @param type Tipo do identificador (integer ou boolean)
 * @return true se a inserção foi bem-sucedida, false se o identificador já existe
 */
bool symbol_table_insert(const char *identifier, const char *type);

/**
 * @brief Busca um símbolo na tabela de símbolos
 * @param identifier Nome do identificador a ser buscado
 * @return Ponteiro para o nó encontrado ou NULL se não existir
 */
SymbolNode *symbol_table_find(const char *identifier);

/**
 * @brief Verifica se um identificador já foi declarado
 * @param identifier Nome do identificador
 * @return true se o identificador existe, false caso contrário
 */
bool symbol_table_exists(const char *identifier);

/**
 * @brief Obtém o tipo de um identificador
 * @param identifier Nome do identificador
 * @return String com o tipo ou NULL se não encontrado
 */
const char *symbol_table_get_type(const char *identifier);

/**
 * @brief Obtém o endereço de um identificador
 * @param identifier Nome do identificador
 * @return Endereço do identificador ou -1 se não encontrado
 */
int symbol_table_get_address(const char *identifier);

void symbol_table_print();

void symbol_table_cleanup();

#endif // SYMBOL_TABLE_H
