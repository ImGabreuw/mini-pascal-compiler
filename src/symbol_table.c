#include "symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SymbolNode *symbol_table_head = NULL;
static int next_address = 0;

void symbol_table_init()
{
    symbol_table_head = NULL;
    next_address = 0;
}

bool symbol_table_insert(const char *identifier, const char *type)
{
    if (symbol_table_exists(identifier))
    {
        return false;
    }

    SymbolNode *new_node = (SymbolNode *)malloc(sizeof(SymbolNode));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed in symbol_table_insert\n");
        exit(EXIT_FAILURE);
    }

    strncpy(new_node->identifier, identifier, MAX_IDENTIFIER_LENGTH - 1);
    new_node->identifier[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    
    strncpy(new_node->type, type, MAX_TYPE_LENGTH - 1);
    new_node->type[MAX_TYPE_LENGTH - 1] = '\0';
    
    new_node->address = next_address++;
    new_node->next = NULL;

    if (symbol_table_head == NULL)
    {
        symbol_table_head = new_node;
    }
    else
    {
        SymbolNode *current = symbol_table_head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }

    return true;
}

SymbolNode *symbol_table_find(const char *identifier)
{
    SymbolNode *current = symbol_table_head;
    
    while (current != NULL)
    {
        if (strcmp(current->identifier, identifier) == 0)
        {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

bool symbol_table_exists(const char *identifier)
{
    return symbol_table_find(identifier) != NULL;
}

const char *symbol_table_get_type(const char *identifier)
{
    SymbolNode *node = symbol_table_find(identifier);
    if (node != NULL)
    {
        return node->type;
    }
    return NULL;
}

int symbol_table_get_address(const char *identifier)
{
    SymbolNode *node = symbol_table_find(identifier);
    if (node != NULL)
    {
        return node->address;
    }
    return -1;
}

void symbol_table_print()
{
    printf("\n========== Tabela de Símbolos ==========\n");
    printf("%-16s | %-8s | %-7s\n", "Identificador", "Endereço", "Tipo");
    printf("----------------------------------------\n");
    
    SymbolNode *current = symbol_table_head;
    while (current != NULL)
    {
        printf("%-16s | %-8d | %-7s\n", 
               current->identifier, 
               current->address, 
               current->type);
        current = current->next;
    }
    printf("========================================\n\n");
}

void symbol_table_cleanup()
{
    SymbolNode *current = symbol_table_head;
    
    while (current != NULL)
    {
        SymbolNode *next = current->next;
        free(current);
        current = next;
    }
    
    symbol_table_head = NULL;
    next_address = 0;
}
