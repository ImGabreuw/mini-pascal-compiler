#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "logging.h"

static Token *current_token = NULL;

/**
 * @brief Libera o espaço alocado para o token atual e obtém o próximo token do analisador léxico.
 */
static void token_advance()
{
    if (current_token)
    {
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
 *        Se corresponder, avança para o próximo token.
 *        Caso contrário, registra um erro de sintaxe e termina o programa.
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

/* Números e identificadores */

// <constant> ::= <integer constant> | <constant identifier>
void parser_parse_constant()
{
    if (token_match(TOKEN_NUMBER, NULL))
        return;

    if (token_match(TOKEN_IDENTIFIER, NULL))
        return;

    log_syntax_error(current_token);
    exit(EXIT_FAILURE);
}

/* Expressões */

// <variable> ::= <identifier>
void parser_parse_variable()
{
    token_expect(TOKEN_IDENTIFIER, NULL);
}

// <multiplying operator> ::= * | div
void parser_parse_multiplying_operator()
{
    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "*"))
        return;

    if (token_match(TOKEN_KEYWORD, "div"))
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
void parser_parse_sign()
{
    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "+"))
        return;

    if (token_match(TOKEN_OPERATOR_ARITHMETIC, "-"))
        return;
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

// <factor> ::= <variable> | <constant> | ( <expression> ) | not <factor> | bool
void parser_parse_factor()
{
    if (token_match(TOKEN_DELIMITER, "("))
    {
        parser_parse_expression();
        token_expect(TOKEN_DELIMITER, ")");
        return;
    }

    if (token_match(TOKEN_KEYWORD, "not"))
    {
        parser_parse_factor();
        return;
    }

    if (token_check(TOKEN_BOOLEAN, NULL))
    {
        token_advance();
        return;
    }

    if (token_check(TOKEN_IDENTIFIER, NULL))
    {
        parser_parse_variable();
        return;
    }

    parser_parse_constant();
}

// <term> ::= <factor> { <multiplying operator> <factor> }
void parser_parse_term()
{
    parser_parse_factor();

    while (token_check(TOKEN_OPERATOR_ARITHMETIC, "*") || token_check(TOKEN_KEYWORD, "div"))
    {
        parser_parse_multiplying_operator();
        parser_parse_factor();
    }
}

// <simple expression> ::= <sign> <term> { <adding operator> <term> }
void parser_parse_simple_expression()
{
    parser_parse_sign();
    parser_parse_term();

    while (token_check(TOKEN_OPERATOR_ARITHMETIC, "+") || token_check(TOKEN_OPERATOR_ARITHMETIC, "-"))
    {
        parser_parse_adding_operator();
        parser_parse_term();
    }
}

// <expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
void parser_parse_expression()
{
    parser_parse_simple_expression();

    if (token_check(TOKEN_OPERATOR_RELATIONAL, NULL))
    {
        parser_parse_relational_operator();
        parser_parse_simple_expression();
    }
}

/* Comandos */

// <while statement> ::= while <expression> do <statement>
void parser_parse_while_statement()
{
    token_expect(TOKEN_KEYWORD, "while");
    parser_parse_expression();
    token_expect(TOKEN_KEYWORD, "do");
    parser_parse_statement();
}

// <if statement> ::= if <expression> then <statement> { else <statement> }
void parser_parse_if_statement()
{
    token_expect(TOKEN_KEYWORD, "if");
    parser_parse_expression();
    token_expect(TOKEN_KEYWORD, "then");
    parser_parse_statement();

    if (token_match(TOKEN_KEYWORD, "else"))
    {
        parser_parse_statement();
    }
}

/*
<read statement> ::=
read ( <variable> { , <variable> } )

<write statement> ::=
write ( <variable> { , <variable> } )
*/
void parser_parse_read_write_statement()
{
    bool result = token_match(TOKEN_KEYWORD, "write") || token_match(TOKEN_KEYWORD, "read");

    if (!result)
    {
        log_syntax_error(current_token);
        exit(EXIT_FAILURE);
    }

    token_expect(TOKEN_DELIMITER, "(");

    parser_parse_variable();

    while (token_match(TOKEN_DELIMITER, ","))
    {
        parser_parse_variable();
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
    token_expect(TOKEN_IDENTIFIER, NULL);
    token_expect(TOKEN_OPERATOR_ASSIGNMENT, NULL);
    parser_parse_expression();
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
        Token *lookahead = current_token;
        token_advance();

        if (token_check(TOKEN_DELIMITER, "("))
        {
            current_token = lookahead;
            parser_parse_function_procedure_statement();
            return;
        }
        else if (token_check(TOKEN_OPERATOR_ASSIGNMENT, NULL))
        {
            token_advance();

            if (token_check(TOKEN_IDENTIFIER, NULL))
            {
                current_token = lookahead;
                parser_parse_function_procedure_statement();
                return;
            }
            else
            {
                current_token = lookahead;
                parser_parse_assignment_statement();
                return;
            }
        }
        else
        {
            log_syntax_error(current_token);
            exit(EXIT_FAILURE);
        }
    }
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
void parser_parse_statement_v2(){

    if(token_check(TOKEN_KEYWORD,"read") || token_check(TOKEN_KEYWORD,"write")){
        parser_parse_read_write_statement();
        return;
    }

    if(token_check(TOKEN_KEYWORD,"if")){
        parser_parse_if_statement();
        return;
    }

    if(token_check(TOKEN_KEYWORD, "begin")){
        parser_parse_compound_statement();
        return;
    }

    if(token_check(TOKEN_KEYWORD,"while")){
        parser_parse_while_statement();
        return;

    }

    //assignment_statement ou function_procedure_statement
    token_expect(TOKEN_IDENTIFIER,NULL);

    //<function procedure identifier> (<parameters_list)
    if(token_match(TOKEN_DELIMITER,"(")){
        parser_parse_parameters_list();
        
        token_expect(TOKEN_DELIMITER,")");
        
        return;
        
    }

    //Comparando <variable> := <function procedure identifier> (<parameters_list)
    // e <variable> := <expression>
    //Em ambos, espera aqui o :=
    token_expect(TOKEN_OPERATOR_ASSIGNMENT,NULL);

    //Caso <function_procedure_identifier>(function_procedure) ou <expression>(do assignment, que pode produzir <identifier> aqui)
    if(token_check(TOKEN_IDENTIFIER,NULL)){
        int curr_line = current_token->line; //Salva linha atual

        token_advance(); // Olha para o proximo token
        
        //Se ainda nao acabou a linha, 
        // validar se corresponde ao <function_procedure_identifier> ( <parameters list) 
        //do <function_procedure_statement
        if(current_token->line == curr_line){ 
            token_expect(TOKEN_DELIMITER,"(");

            parser_parse_parameters_list();

            token_expect(TOKEN_DELIMITER,")");
            return;
        }

        //Se acabou a linha, so podia ser producao do assignment
        return;

    }
    else{ // Sera o assignment se passar em alguma das outras produções de expression
        parser_parse_expression();

    }


}

// <compound_statement> ::= begin <statement> { ; <statement> } end
void parser_parse_compound_statement()
{
    token_expect(TOKEN_KEYWORD, "begin");
    parser_parse_statement();

    while (token_match(TOKEN_DELIMITER, ";"))
    {
        parser_parse_statement();
    }

    token_expect(TOKEN_KEYWORD, "end");
}

/* Declarações */

// <formal parameters> ::= <variable declaration part>
void parser_parse_formal_parameters()
{
    parser_parse_variable_declaration_part();
}

// <function declaration> ::= function < identifier > < formal parameters (variable declaration part) > : < type >; < block >
void parser_parse_function_declaration()
{
    token_expect(TOKEN_KEYWORD, "function");
    token_expect(TOKEN_IDENTIFIER, NULL);
    parser_parse_variable_declaration_part();
    token_expect(TOKEN_DELIMITER, ":");
    parser_parse_type();
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
}

// <procedure declaration> ::= procedure < identifier > < formal parameters (variable declaration part) > ; <block>
void parser_parse_procedure_declaration()
{
    token_expect(TOKEN_KEYWORD, "procedure");
    token_expect(TOKEN_IDENTIFIER, NULL);
    parser_parse_variable_declaration_part();
    token_expect(TOKEN_DELIMITER, ";");
    parser_parse_block();
}

// <subroutine declaration part> ::= < procedure declaration | function declaration >
void parser_parse_subroutine_declaration_part()
{
    if (token_match(TOKEN_KEYWORD, "procedure"))
    {
        parser_parse_procedure_declaration();
    }
    else
    {
        parser_parse_function_declaration();
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
    token_expect(TOKEN_IDENTIFIER, NULL);

    while (token_match(TOKEN_DELIMITER, ","))
    {
        token_expect(TOKEN_IDENTIFIER, NULL);
    }

    token_expect(TOKEN_DELIMITER, ":");

    parser_parse_type();
}

// <variable declaration part> ::= <empty> | var <variable declaration> ; { <variable declaration part> ; }
void parser_parse_variable_declaration_part()
{
    if (token_match(TOKEN_KEYWORD, "var"))
    {
        parser_parse_variable_declaration();
        token_expect(TOKEN_DELIMITER, ";");

        while (token_match(TOKEN_KEYWORD, "var"))
        {
            parser_parse_variable_declaration();
            token_expect(TOKEN_DELIMITER, ";");
        }
    }
}

// <block> ::= <variable declaration part> <subroutine declaration part> <compound part>
void parser_parse_block()
{
    parser_parse_variable_declaration_part();
    parser_parse_subroutine_declaration_part();
    parser_parse_compound_statement();
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
    token_advance(); // Inicializa o primeiro token
}

void parser_parse()
{
    parser_parse_program();
}

void parser_cleanup()
{
    if (current_token)
    {
        free(current_token);
        current_token = NULL;
    }
}
