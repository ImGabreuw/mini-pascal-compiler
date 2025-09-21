#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#include "token.h"

#define MAX_LOG_FILENAME 256
#define MAX_LOG_LINE 512

void log_init(const char *program_name);

void log_token(const Token *token);

void log_cleanup();

#endif