#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#include "token.h"

#define MAX_LOG_FILENAME 256
#define MAX_LOG_LINE 512

void log_init(const char *program_name);

void log_token(const Token *token);

void log_lexical_error(int line, char invalid_char);

void log_syntax_error(const Token *token);

void log_semantic_error_duplicate(int line, const char *identifier);

void log_semantic_error_undeclared(int line, const char *identifier);

void log_semantic_error_type_mismatch(int line, const char *identifier, const char *expected_type, const char *actual_type);

void log_cleanup();

#endif