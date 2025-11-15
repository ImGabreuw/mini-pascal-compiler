#include "intermediary_code_generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int temp_count = 0;
static int label_count = 0;

static FILE *output_file = NULL;
#define MAX_CODE_FILENAME 256
#define MAX_LINE_BUFFER 512

static char temp_name_buffer[20];
static char label_name_buffer[20];

void init_code_gen(const char *program_name) {
    temp_count = 0;
    label_count = 0;

    char code_filename[MAX_CODE_FILENAME];
    snprintf(code_filename, sizeof(code_filename), "%s.3ac", program_name);

    output_file = fopen(code_filename, "w");
    if (output_file == NULL) {
        perror("Error opening 3AC output file");
        exit(EXIT_FAILURE);
    }
}

char* new_temp() {
    sprintf(temp_name_buffer, "t%d", temp_count++);
    return strdup(temp_name_buffer); 
}

char* new_label() {
    sprintf(label_name_buffer, "L%d", label_count++);
    return strdup(label_name_buffer);
}

void emit_assignment(const char *target, const char *source) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];
    snprintf(line_buffer, sizeof(line_buffer), "    %s = %s\n", target, source);
    
    fprintf(output_file, "%s", line_buffer);
}

void emit_operation(const char *target, const char *op1, const char *op, const char *op2) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];

    if (op2 != NULL) {
        snprintf(line_buffer, sizeof(line_buffer), "    %s = %s %s %s\n", target, op1, op, op2);
    } else {
        snprintf(line_buffer, sizeof(line_buffer), "    %s = %s %s\n", target, op, op1);
    }
    
    fprintf(output_file, "%s", line_buffer);
}

void emit_label(const char *label) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];
    snprintf(line_buffer, sizeof(line_buffer), "%s:\n", label);
    
    fprintf(output_file, "%s", line_buffer);
}

void emit_goto(const char *label) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];
    snprintf(line_buffer, sizeof(line_buffer), "    goto %s\n", label);
    
    fprintf(output_file, "%s", line_buffer);
}

void emit_if_goto(const char *op1, const char *op, const char *op2, const char *label) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];
    snprintf(line_buffer, sizeof(line_buffer), "    if %s %s %s goto %s\n", op1, op, op2, label);
    
    fprintf(output_file, "%s", line_buffer);
}

void emit_write(const char *var) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];
    snprintf(line_buffer, sizeof(line_buffer), "    write %s\n", var);
    
    fprintf(output_file, "%s", line_buffer);
}

void emit_read(const char *var) {
    if (output_file == NULL) return;
    char line_buffer[MAX_LINE_BUFFER];
    snprintf(line_buffer, sizeof(line_buffer), "    read %s\n", var);
    
    fprintf(output_file, "%s", line_buffer);
}

void finalize_code_gen() {
    if (output_file == NULL) return;

     fclose(output_file);
     output_file = NULL;
}