#include "logging.h"

#include <stdlib.h>

static FILE *token_file;

void log_init(const char *program_name) {
    char token_filename[MAX_LOG_FILENAME];
    snprintf(token_filename, sizeof(token_filename), "%s.tokens", program_name);

    token_file = fopen(token_filename, "w");
    if (token_file == NULL) {
        perror("Error opening token output file");
        exit(EXIT_FAILURE);
    }
}

void log_token(const Token *token) {
    if (token_file) {
        char log_line[MAX_LOG_LINE];
        snprintf(log_line, sizeof(log_line), "%02d # %-30s | %s\n", token->line, token_type_to_string(token->type), token->value);
        
        printf("%s", log_line);
        fprintf(token_file, "%s", log_line);
    }
}

void log_cleanup() {
    if (token_file) {
        fclose(token_file);
        token_file = NULL;
    }
}