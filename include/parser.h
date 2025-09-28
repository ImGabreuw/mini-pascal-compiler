#ifndef PARSER_H
#define PARSER_H

#include "token.h"

void parser_init();

void parser_parse();

void parser_cleanup();

void parser_parse_constant();

void parser_parse_variable();

void parser_parse_multiplying_operator();

void parser_parse_adding_operator();

void parser_parse_sign();

void parser_parse_relational_operator();

void parser_parse_factor();

void parser_parse_term();

void parser_parse_simple_expression();

void parser_parse_expression();

void parser_parse_while_statement();

void parser_parse_if_statement();

void parser_parse_read_write_statement();

void parser_parse_parameters_list();

void parser_parse_function_procedure_statement();

void parser_parse_assignment_statement();

void parser_parse_statement();

void parser_parse_compound_statement();

void parser_parse_formal_parameters();

void parser_parse_function_declaration();

void parser_parse_procedure_declaration();

void parser_parse_subroutine_declaration_part();

void parser_parse_type();

void parser_parse_variable_declaration();

void parser_parse_variable_declaration_part();

void parser_parse_block();

void parser_parse_program();

#endif // PARSER_H