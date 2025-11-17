#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "logging.h"
#include "symbol_table.h"
#include "intermediary_code_generator.h"

static Token *current_token = NULL;

static bool is_assignment = true;

static char expected_id_type[MAX_TYPE_LENGTH];

static char last_identifier_found[MAX_IDENTIFIER_LENGTH];

/**
 * @brief Libera o espaço alocado para o token atual e obtém o próximo token do analisador léxico.
 */
static void token_advance()
{
    if (current_token)
    {
        free(current_token->value);
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
 * Se corresponder, avança para o próximo token.
 * Caso contrário, registra um erro de sintaxe e termina o programa.
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

/**
 * @brief Avalia se o tipo lido equivale ao tipo esperado
 */

static void identifier_type_expect(const char *expected_type, const char *actual_type, int line, char *identifier)
{
    if (strcmp(actual_type, expected_type))
    {
        log_semantic_error_type_mismatch(line, identifier, expected_type, actual_type);
        exit(1);
    }
}

/* Números e identificadores */

// <constant> ::= <integer constant>
char *parser_parse_constant()
{
    if (token_check(TOKEN_NUMBER, NULL))
    {

        if (is_assignment)
        {
            identifier_type_expect(expected_id_type, "integer", current_token->line, current_token->value);
        }

        char *num_str = strdup(current_token->value); 
        token_advance();
        return num_str;
    }

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
    return NULL; 
}

/* Expressões */

// <variable> ::= <identifier>
char *parser_parse_variable()
{
    if (!token_check(TOKEN_IDENTIFIER, NULL))
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    if (!symbol_table_exists(current_token->value))
    {
        log_semantic_error_undeclared(current_token->line, current_token->value);
        exit(EXIT_FAILURE);
    }

    char *var_name = strdup(current_token->value); 
    token_advance();
    return var_name; 
}

// <multiplying operator> ::= * | div
void parser_parse_multiplying_operator()
{
    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "*"))
        return;

    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "div"))
        return;

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
}

// <adding operator> ::= + | -
void parser_parse_adding_operator()
{
    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "+"))
        return;

    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "-"))
        return;

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
}

// <sign> ::= + | - | <empty>
char *parser_parse_sign()
{
    if (token_check(TOKEN_OPERATOR_ARITHMETIC, "+"))
    {
        char *sign = strdup(current_token->value); 
        token_advance();
        return sign;
    }

    if (token_check(TOKEN_OPERATOR_ARITHMETIC, "-"))
    {
        char *sign = strdup(current_token->value); 
        token_advance();
        return sign;
    }

    return NULL;
}

// <relational operator> ::= = | <> | < | <= | >= | > | or | and
void parser_parse_relational_operator()
{
    if (token_match(TOKEN_OPERATOR_RELATIONAL, NULL))
        return;

    if (token_match(TOKEN_KEYWORD, "or"))
        return;

    if (token_match(TOKEN_KEYWORD, "and"))
        return;

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
}

// <factor> ::= <variable> | <constant> | ( <expression> ) | not <factor> | <bool>
char *parser_parse_factor()
{
    if (token_match(TOKEN_DELIMITER, "("))
    {
        char *expr_result = parser_parse_expression();
        token_expect(TOKEN_DELIMITER, ")");
        return expr_result; 
    }

    if (token_match(TOKEN_KEYWORD, "not"))
    {
        char *factor_result = parser_parse_factor(); 
        char *temp_result = new_temp();             

        emit_operation(temp_result, factor_result, "not", NULL);

        free(factor_result);
        return temp_result; 
    }

    if (token_check(TOKEN_BOOLEAN, NULL))
    {
        if (is_assignment)
        {
            identifier_type_expect(expected_id_type, "boolean", current_token->line, current_token->value);
        }
        char *bool_val = strdup(current_token->value); 
        token_advance();
        return bool_val; 
    }

    if (token_check(TOKEN_IDENTIFIER, NULL))
    {
        int line = current_token->line;
        const char *type_read = symbol_table_get_type(current_token->value);
        strcpy(last_identifier_found, current_token->value);
        if (!is_assignment)
        {
            strcpy(expected_id_type, type_read);
        }

        char *var_name = parser_parse_variable();

        identifier_type_expect(expected_id_type, type_read, line, last_identifier_found);

        return var_name; 
    }

    if (token_check(TOKEN_NUMBER, NULL))
    {
        return parser_parse_constant(); 
    }

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
    return NULL; 
}

// <term> ::= <factor> { <multiplying operator> <factor> }
char *parser_parse_term()
{
    char *left_result = parser_parse_factor(); 

    while (token_check(TOKEN_OPERATOR_ARITHMETIC, "*") || token_check(TOKEN_OPERATOR_ARITHMETIC, "div"))
    {
        char operator_buffer[5];
        strncpy(operator_buffer, current_token->value, 4);
        operator_buffer[4] = '\0';

        parser_parse_multiplying_operator();

        char *right_result = parser_parse_factor(); 
        char *temp_result = new_temp();             

        emit_operation(temp_result, left_result, operator_buffer, right_result);

        free(left_result);  
        free(right_result); 

        left_result = temp_result; 
    }

    return left_result; 
}

// <simple expression> ::= <sign> <term> { <adding operator> <term> }
char *parser_parse_simple_expression()
{
    char *sign = parser_parse_sign();       
    char *left_result = parser_parse_term();

    if (sign)
    {
        char *temp_result = new_temp(); 

        emit_operation(temp_result, left_result, sign, NULL);

        free(left_result);         
        free(sign);               
        left_result = temp_result;
    }

    while (token_check(TOKEN_OPERATOR_ARITHMETIC, "+") || token_check(TOKEN_OPERATOR_ARITHMETIC, "-"))
    {
        char operator_buffer[2];
        strncpy(operator_buffer, current_token->value, 1);
        operator_buffer[1] = '\0';

        parser_parse_adding_operator();

        char *right_result = parser_parse_term(); 
        char *temp_result = new_temp();           

        emit_operation(temp_result, left_result, operator_buffer, right_result);

        free(left_result);  
        free(right_result); 

        left_result = temp_result; 
    }

    return left_result; 
}

// <expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
char *parser_parse_expression()
{
    char *left_result = parser_parse_simple_expression();

    if (token_check(TOKEN_OPERATOR_RELATIONAL, NULL) ||
        token_check(TOKEN_KEYWORD, "or") ||
        token_check(TOKEN_KEYWORD, "and"))
    {
        char operator_buffer[5]; 
        strncpy(operator_buffer, current_token->value, 4);
        operator_buffer[4] = '\0';

        parser_parse_relational_operator();

        char *right_result = parser_parse_simple_expression(); 
        char *temp_result = new_temp();                        

        emit_operation(temp_result, left_result, operator_buffer, right_result);

        free(left_result);  
        free(right_result); 

        return temp_result; 
    }

    return left_result; 
}

/* Comandos */

// <while statement> ::= while <expression> do <statement>
void parser_parse_while_statement()
{
    token_expect(TOKEN_KEYWORD, "while");

    char *label_start = new_label(); 
    char *label_end = new_label();   

    emit_label(label_start); 

    is_assignment = false;
    char *expr_result = parser_parse_expression(); 
    is_assignment = true;

    emit_if_goto(expr_result, "==", "false", label_end);
    free(expr_result); 

    token_expect(TOKEN_KEYWORD, "do");
    parser_parse_statement();

    emit_goto(label_start); 
    emit_label(label_end);  

    free(label_start); 
    free(label_end);   
}

// <if statement> ::= if <expression> then <statement> { else <statement> }
void parser_parse_if_statement()
{
    token_expect(TOKEN_KEYWORD, "if");
    is_assignment = false;
    char *expr_result = parser_parse_expression();
    is_assignment = true;

    token_expect(TOKEN_KEYWORD, "then");

    char *label_else = new_label(); 

    emit_if_goto(expr_result, "==", "false", label_else);
    free(expr_result);

    parser_parse_statement();

    if (token_match(TOKEN_KEYWORD, "else"))
    {
        char *label_end = new_label();

        emit_goto(label_end);   
        emit_label(label_else); 

        parser_parse_statement(); 

        emit_label(label_end); 

        free(label_end); 
    }
    else
    {
        emit_label(label_else); 
    }

    free(label_else);
    is_assignment = true;
}

/*
<read statement> ::=
read ( <variable> { , <variable> } )

<write statement> ::=
write ( <variable> { , <variable> } )
*/
void parser_parse_read_write_statement()
{
    bool is_write = token_check(TOKEN_KEYWORD, "write");
    bool result = token_match(TOKEN_KEYWORD, "write") || token_match(TOKEN_KEYWORD, "read");

    if (!result)
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    token_expect(TOKEN_DELIMITER, "(");

    char *var_name = parser_parse_variable(); 
    if (is_write)
        emit_write(var_name);
    else
        emit_read(var_name);
    free(var_name); 

    while (token_match(TOKEN_DELIMITER, ","))
    {
        var_name = parser_parse_variable();
        if (is_write)
            emit_write(var_name);
        else
            emit_read(var_name);
        free(var_name); // Libera
    }

    token_expect(TOKEN_DELIMITER, ")");
}

// <parameters list> ::= ( <identifier> | <number> | <bool> ) {, ( <identifier> | <numero> | <bool> ) }
void parser_parse_parameters_list()
{
    token_expect(TOKEN_DELIMITER, "(");

    bool result = token_match(TOKEN_IDENTIFIER, NULL) || token_match(TOKEN_NUMBER, NULL) || token_match(TOKEN_BOOLEAN, NULL);

    if (!result)
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    token_expect(TOKEN_DELIMITER, ")");

    while (token_match(TOKEN_DELIMITER, ","))
    {
        token_expect(TOKEN_DELIMITER, "(");

        bool result = token_match(TOKEN_IDENTIFIER, NULL) || token_match(TOKEN_NUMBER, NULL) || token_match(TOKEN_BOOLEAN, NULL);

        if (!result)
        {
            log_syntax_error(current_token);
            exit(EXIT_FAILURE);
        }

        token_expect(TOKEN_DELIMITER, ")");
    }
}

/*
<function_procedure statement> ::=
<function_procedure identifier> ( <parameters list> ) | <variable> := <function_procedure identifier> ( <parameters list>)
*/
void parser_parse_function_procedure_statement()
{
    if (token_match(TOKEN_IDENTIFIER, NULL))
    {
        if (token_match(TOKEN_OPERATOR_ASSIGNMENT, NULL))
        {
            // <variable> := <function_procedure identifier> ( <parameters list>)

            token_expect(TOKEN_IDENTIFIER, NULL);
            parser_parse_parameters_list();
            return;
        }

        // <function_procedure identifier> ( <parameters list> )

        token_expect(TOKEN_IDENTIFIER, NULL);
        parser_parse_parameters_list();
        return;
    }

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
}

// <assignment statement> ::= <variable> := <expression>
void parser_parse_assignment_statement()
{
    if (!token_check(TOKEN_IDENTIFIER, NULL))
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    char identifier[MAX_IDENTIFIER_LENGTH];
    strncpy(identifier, current_token->value, MAX_IDENTIFIER_LENGTH - 1);
    identifier[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    int line = current_token->line;

    if (!symbol_table_exists(identifier))
    {
        log_semantic_error_undeclared(line, identifier);
        exit(EXIT_FAILURE);
    }

    const char *var_type = symbol_table_get_type(identifier);

    strcpy(expected_id_type, var_type);

    strcpy(last_identifier_found, identifier);

    token_advance(); // Consome o identificador
    token_expect(TOKEN_OPERATOR_ASSIGNMENT, NULL);

    char *expr_result = parser_parse_expression(); 
    emit_assignment(identifier, expr_result);
    free(expr_result);
}

/*
<statement> ::=
<assignment statement>
| <function_procedure statement>
| <read statement>
| <write statement>
| <compound statement>
| <if statement>
| <while statement>
*/
void parser_parse_statement()
{
    if (token_check(TOKEN_KEYWORD, "read") || token_check(TOKEN_KEYWORD, "write"))
    {
        parser_parse_read_write_statement();
        return;
    }

    if (token_check(TOKEN_KEYWORD, "if"))
    {
        parser_parse_if_statement();
        return;
    }

    if (token_check(TOKEN_KEYWORD, "begin"))
    {
        parser_parse_compound_statement();
        return;
    }

    if (token_check(TOKEN_KEYWORD, "while"))
    {
        parser_parse_while_statement();
        return;
    }

    if (token_check(TOKEN_IDENTIFIER, NULL))
    {
        parser_parse_assignment_statement();
        // FIXME <function_procedure statement> removido por enquanto
        return;
    }
}

// <compound_statement> ::= begin <statement> { ; <statement> } end
void parser_parse_compound_statement()
{
    token_expect(TOKEN_KEYWORD, "begin");
    parser_parse_statement();

    while (token_match(TOKEN_DELIMITER, ";"))
    {
        // Ignora ponto e vírgula extras antes do 'end'
        if (token_check(TOKEN_KEYWORD, "end"))
        {
            break;
        }
        parser_parse_statement();
    }

    token_expect(TOKEN_KEYWORD, "end");
}

/* Declarações (sem geração de código 3AC, apenas análise) */

// <formal parameters> ::= <empty> | var <variable declaration> { ; var <variable declaration> }
void parser_parse_formal_parameters()
{
    if (token_match(TOKEN_KEYWORD, "var"))
    {
        parser_parse_variable_declaration();

        while (token_match(TOKEN_DELIMITER, ";"))
        {
            token_expect(TOKEN_KEYWORD, "var");
            parser_parse_variable_declaration();
        }
    }
}

// <function declaration> ::= function < identifier > ( < formal parameters > ) : < type > ; < block > ;
void parser_parse_function_declaration()
{
    token_expect(TOKEN_KEYWORD, "function");
    token_expect(TOKEN_IDENTIFIER, NULL);
    token_expect(TOKEN_DELIMITER, "(");
    parser_parse_formal_parameters();
    token_expect(TOKEN_DELIMITER, ")");
    token_expect(TOKEN_DELIMITER, ":");
    parser_parse_type();
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
    token_expect(TOKEN_DELIMITER, ";");
}

// <procedure declaration> ::= procedure < identifier > ( < formal parameters > ) ; <block> ;
void parser_parse_procedure_declaration()
{
    token_expect(TOKEN_KEYWORD, "procedure");
    token_expect(TOKEN_IDENTIFIER, NULL);

    if (token_match(TOKEN_DELIMITER, "("))
    {
        parser_parse_formal_parameters();
        token_expect(TOKEN_DELIMITER, ")");
    }

    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
    token_expect(TOKEN_DELIMITER, ";");
}

// <subroutine declaration part> ::= <empty> | < procedure declaration | function declaration >
void parser_parse_subroutine_declaration_part()
{
    while (token_check(TOKEN_KEYWORD, "procedure") || token_check(TOKEN_KEYWORD, "function"))
    {
        if (token_check(TOKEN_KEYWORD, "procedure"))
        {
            parser_parse_procedure_declaration();
        }
        else if (token_check(TOKEN_KEYWORD, "function"))
        {
            parser_parse_function_declaration();
        }
    }
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
    char identifiers[100][MAX_IDENTIFIER_LENGTH];
    int identifier_count = 0;
    int first_line = current_token->line;

    // Primeiro identificador
    if (!token_check(TOKEN_IDENTIFIER, NULL))
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    strncpy(identifiers[identifier_count], current_token->value, MAX_IDENTIFIER_LENGTH - 1);
    identifiers[identifier_count][MAX_IDENTIFIER_LENGTH - 1] = '\0';
    identifier_count++;

    token_advance();

    // Identificadores adicionais
    while (token_match(TOKEN_DELIMITER, ","))
    {
        if (!token_check(TOKEN_IDENTIFIER, NULL))
        {
            log_syntax_error(current_token);
            exit(EXIT_FAILURE);
        }

        strncpy(identifiers[identifier_count], current_token->value, MAX_IDENTIFIER_LENGTH - 1);
        identifiers[identifier_count][MAX_IDENTIFIER_LENGTH - 1] = '\0';
        identifier_count++;
        token_advance();
    }

    token_expect(TOKEN_DELIMITER, ":");

    if (!token_check(TOKEN_KEYWORD, "integer") && !token_check(TOKEN_KEYWORD, "boolean"))
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    char type[MAX_TYPE_LENGTH];
    strncpy(type, current_token->value, MAX_TYPE_LENGTH - 1);
    type[MAX_TYPE_LENGTH - 1] = '\0';

    parser_parse_type();

    for (int i = 0; i < identifier_count; i++)
    {
        if (!symbol_table_insert(identifiers[i], type))
        {
            log_semantic_error_duplicate(first_line, identifiers[i]);
            exit(EXIT_FAILURE);
        }
    }
}

// <variable declaration part> ::= <empty> | var <variable declaration> ; { <variable declaration part> ; }
void parser_parse_variable_declaration_part()
{
    if (token_match(TOKEN_KEYWORD, "var"))
    {
        parser_parse_variable_declaration();
        token_expect(TOKEN_DELIMITER, ";");

        while (token_check(TOKEN_KEYWORD, "var")) 
        {
            token_match(TOKEN_KEYWORD, "var");
            parser_parse_variable_declaration();
            token_expect(TOKEN_DELIMITER, ";");
        }
    }
}

// <statement part> ::= <compound statement>
void parser_parse_statement_part()
{
    parser_parse_compound_statement();
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
    token_expect(TOKEN_IDENTIFIER, NULL);
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
    token_expect(TOKEN_DELIMITER, ".");
}

void parser_init()
{
    symbol_table_init();
    token_advance();         
}

void parser_parse()
{
    parser_parse_program();
    symbol_table_print();
}

void parser_cleanup()
{
    if (current_token)
    {
        free(current_token->value); 
        free(current_token);        
        current_token = NULL;
    }
    symbol_table_cleanup();
}