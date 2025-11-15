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

 static void identifier_type_expect(const char* expected_type, const char* actual_type, int line, char* identifier){
    if(strcmp(actual_type,expected_type)){
        log_semantic_error_type_mismatch(line,identifier,expected_type,actual_type);
        exit(1);
    }
 }

/* Números e identificadores */

// <constant> ::= <integer constant>
// Modificado para retornar char*
char* parser_parse_constant()
{
    if (token_check(TOKEN_NUMBER, NULL)){
        
        if(is_assignment){
            identifier_type_expect(expected_id_type,"integer",current_token->line,current_token->value);
        }
        
        char* num_str = strdup(current_token->value); // Aloca
        token_advance();
        return num_str; // Retorna alocado
    }
    
    // <constant identifier> não é mais tratado aqui, mas em <factor>
    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
    return NULL; // Nunca alcançado
}

/* Expressões */

// <variable> ::= <identifier>
// Modificado para retornar char*
char* parser_parse_variable()
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

    char* var_name = strdup(current_token->value); // Aloca
    token_advance();
    return var_name; // Retorna alocado
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
// Modificado para retornar char* (o sinal) ou NULL
char* parser_parse_sign()
{
    if (token_check(TOKEN_OPERATOR_ARITHMETIC, "+"))
    {
        char* sign = strdup(current_token->value); // Aloca
        token_advance();
        return sign;
    }

    if (token_check(TOKEN_OPERATOR_ARITHMETIC, "-"))
    {
        char* sign = strdup(current_token->value); // Aloca
        token_advance();
        return sign;
    }
    
    return NULL; // <empty>
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
// Modificado para retornar char*
char* parser_parse_factor()
{
    if (token_match(TOKEN_DELIMITER, "("))
    {
        char* expr_result = parser_parse_expression(); // Retorna alocado
        token_expect(TOKEN_DELIMITER, ")");
        return expr_result; // Passa o ponteiro alocado
    }

    if (token_match(TOKEN_KEYWORD, "not"))
    {
        char* factor_result = parser_parse_factor(); // Retorna alocado
        char* temp_result = new_temp(); // Novo temporário (alocado)
        
        // Emite: t_result = not t_factor
        emit_operation(temp_result, factor_result, "not", NULL); 
        
        free(factor_result); // Libera o resultado interno
        return temp_result; // Retorna o novo temporário (alocado)
    }

    if (token_check(TOKEN_BOOLEAN, NULL))
    {
        if(is_assignment) {
             identifier_type_expect(expected_id_type,"boolean",current_token->line,current_token->value);
        }
        char* bool_val = strdup(current_token->value); // Aloca
        token_advance();
        return bool_val; // Retorna alocado
    }

    if (token_check(TOKEN_IDENTIFIER, NULL))
    {
        int line = current_token->line;
        const char* type_read = symbol_table_get_type(current_token->value);
        strcpy(last_identifier_found,current_token->value);
        if(!is_assignment) {
            strcpy(expected_id_type, type_read);
        }

        // parser_parse_variable agora retorna char* alocado
        char* var_name = parser_parse_variable(); 

        identifier_type_expect(expected_id_type,type_read,line,last_identifier_found);

        return var_name; // Retorna alocado
    }

    // Se for um número, parser_parse_constant é chamado
    if (token_check(TOKEN_NUMBER, NULL)) 
    {
        return parser_parse_constant(); // Retorna alocado
    }

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
    return NULL; // Nunca alcançado
}

// <term> ::= <factor> { <multiplying operator> <factor> }
// Modificado para retornar char*
char* parser_parse_term()
{
    char *left_result = parser_parse_factor(); // Retorna alocado

    while (token_check(TOKEN_OPERATOR_ARITHMETIC, "*") || token_check(TOKEN_OPERATOR_ARITHMETIC, "div"))
    {
        char op[5]; // Buffer para "div"
        strncpy(op, current_token->value, 4);
        op[4] = '\0';
        
        parser_parse_multiplying_operator();
        
        char *right_result = parser_parse_factor(); // Retorna alocado
        char *temp_result = new_temp(); // Novo temporário (alocado)

        // Emite: t_result = t_left * t_right
        emit_operation(temp_result, left_result, op, right_result);

        free(left_result); // Libera o resultado da esquerda anterior
        free(right_result); // Libera o resultado da direita
        
        left_result = temp_result; // O novo resultado se torna o da esquerda
    }

    return left_result; // Retorna o resultado final (alocado)
}

// <simple expression> ::= <sign> <term> { <adding operator> <term> }
// Modificado para retornar char*
char* parser_parse_simple_expression()
{
    char* sign = parser_parse_sign(); // Retorna alocado ou NULL
    char *left_result = parser_parse_term(); // Retorna alocado

    if (sign)
    {
        char *temp_result = new_temp(); // Novo temporário (alocado)
        
        // Emite: t_result = - t_left (operação unária)
        emit_operation(temp_result, left_result, sign, NULL);
        
        free(left_result); // Libera o resultado interno
        free(sign); // Libera o sinal
        left_result = temp_result; // O novo resultado se torna o da esquerda
    }

    while (token_check(TOKEN_OPERATOR_ARITHMETIC, "+") || token_check(TOKEN_OPERATOR_ARITHMETIC, "-"))
    {
        char op[2];
        strncpy(op, current_token->value, 1);
        op[1] = '\0';

        parser_parse_adding_operator();
        
        char *right_result = parser_parse_term(); // Retorna alocado
        char *temp_result = new_temp(); // Novo temporário (alocado)

        // Emite: t_result = t_left + t_right
        emit_operation(temp_result, left_result, op, right_result);

        free(left_result); // Libera o resultado da esquerda anterior
        free(right_result); // Libera o resultado da direita
        
        left_result = temp_result; // O novo resultado se torna o da esquerda
    }

    return left_result; // Retorna o resultado final (alocado)
}

// <expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
// Modificado para retornar char*
char* parser_parse_expression()
{
    char *left_result = parser_parse_simple_expression(); // Retorna alocado

    // Verifica operadores relacionais (incluindo 'or' e 'and' pela gramática)
    if (token_check(TOKEN_OPERATOR_RELATIONAL, NULL) || 
        token_check(TOKEN_KEYWORD, "or") || 
        token_check(TOKEN_KEYWORD, "and"))
    {
        char op[5]; // Para "<>" ou "and"
        strncpy(op, current_token->value, 4);
        op[4] = '\0';
        
        parser_parse_relational_operator();

        char *right_result = parser_parse_simple_expression(); // Retorna alocado
        char *temp_result = new_temp(); // Novo temporário (alocado)

        // Emite: t_result = t_left < t_right
        emit_operation(temp_result, left_result, op, right_result);

        free(left_result); // Libera o resultado da esquerda
        free(right_result); // Libera o resultado da direita
        
        return temp_result; // Retorna o novo temporário (alocado)
    }

    return left_result; // Retorna o resultado (alocado) da expressão simples
}

/* Comandos */

// <while statement> ::= while <expression> do <statement>
void parser_parse_while_statement()
{
    token_expect(TOKEN_KEYWORD, "while");
    
    char* label_start = new_label(); // Aloca L_start
    char* label_end = new_label();   // Aloca L_end

    emit_label(label_start); // L_start:

    is_assignment = false;
    char* expr_result = parser_parse_expression(); // Aloca t_expr
    is_assignment = true;

    // Emite: if t_expr == false goto L_end
    emit_if_goto(expr_result, "==", "false", label_end);
    free(expr_result); // Libera t_expr

    token_expect(TOKEN_KEYWORD, "do");
    parser_parse_statement();

    emit_goto(label_start); // goto L_start
    emit_label(label_end); // L_end:

    free(label_start); // Libera L_start
    free(label_end); // Libera L_end
}

// <if statement> ::= if <expression> then <statement> { else <statement> }
void parser_parse_if_statement()
{
    token_expect(TOKEN_KEYWORD, "if");
    is_assignment = false;
    char* expr_result = parser_parse_expression(); // Aloca t_expr
    is_assignment = true;

    token_expect(TOKEN_KEYWORD, "then");
    
    char* label_else = new_label(); // Aloca L_else

    // Emite: if t_expr == false goto L_else
    emit_if_goto(expr_result, "==", "false", label_else);
    free(expr_result); // Libera t_expr

    parser_parse_statement(); // Bloco 'then'
    
    if (token_match(TOKEN_KEYWORD, "else"))
    {
        char* label_end = new_label(); // Aloca L_end
        
        emit_goto(label_end); // goto L_end (pula o 'else')
        emit_label(label_else); // L_else:

        parser_parse_statement(); // Bloco 'else'
        
        emit_label(label_end); // L_end:
        
        free(label_end); // Libera L_end
    }
    else
    {
        // Se não houver 'else', L_else é o fim do 'if'
        emit_label(label_else); // L_else:
    }
    
    free(label_else); // Libera L_else
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

    char* var_name = parser_parse_variable(); // Aloca
    if (is_write) emit_write(var_name);
    else emit_read(var_name);
    free(var_name); // Libera

    while (token_match(TOKEN_DELIMITER, ","))
    {
        var_name = parser_parse_variable(); // Aloca
        if (is_write) emit_write(var_name);
        else emit_read(var_name);
        free(var_name); // Libera
    }

    token_expect(TOKEN_DELIMITER, ")");
}

// <parameters list> ::= ( <identifier> | <number> | <bool> ) {, ( <identifier> | <numero> | <bool> ) }
void parser_parse_parameters_list()
{
    // AINDA NÃO IMPLEMENTADO PARA 3AC (Chamadas de procedimento/função)
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
    // AINDA NÃO IMPLEMENTADO PARA 3AC
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

    strcpy(expected_id_type,var_type);
    
    strcpy(last_identifier_found,identifier);

    token_advance(); // Consome o identificador
    token_expect(TOKEN_OPERATOR_ASSIGNMENT, NULL);
    
    char* expr_result = parser_parse_expression(); // Aloca t_expr

    // Emite: identificador = t_expr
    emit_assignment(identifier, expr_result);
    
    free(expr_result); // Libera t_expr
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
        if (token_check(TOKEN_KEYWORD, "end")) {
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
    token_expect(TOKEN_DELIMITER,"(");
    parser_parse_formal_parameters();
    token_expect(TOKEN_DELIMITER,")");
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

        while (token_check(TOKEN_KEYWORD, "var")) // Modificado para aceitar 'var' repetido
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
    init_code_gen("program"); // Inicializa o gerador de código
    token_advance(); // Inicializa o primeiro token
}

void parser_parse()
{
    parser_parse_program();
    finalize_code_gen(); // Finaliza a impressão do 3AC
    symbol_table_print();
}

void parser_cleanup()
{
    if (current_token)
    {
        free(current_token->value); // Libera o valor do último token
        free(current_token); // Libera o último token
        current_token = NULL;
    }
    symbol_table_cleanup();
}