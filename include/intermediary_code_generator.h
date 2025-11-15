#ifndef INTERMEDIATE_CODE_H
#define INTERMEDIATE_CODE_H

/**
 * @brief Inicializa o módulo de geração de código.
 * @param program_name O nome do executável (usado para criar o nome do arquivo .3ac)
 */
void init_code_gen(const char *program_name);

/**
 * @brief Gera um novo nome de variável temporária (ex: "t0").
 * @return Um ponteiro para uma string alocada (deve ser liberada pelo chamador).
 */
char* new_temp();

/**
 * @brief Gera um novo nome de rótulo (label) (ex: "L0").
 * @return Um ponteiro para uma string alocada (deve ser liberada pelo chamador).
 */
char* new_label();

/**
 * @brief Emite uma instrução de atribuição: target = source
 */
void emit_assignment(const char *target, const char *source);

/**
 * @brief Emite uma operação: target = op1 op op2 (binária) ou target = op op1 (unária)
 * @param op2 Pode ser NULL para operações unárias.
 */
void emit_operation(const char *target, const char *op1, const char *op, const char *op2);

/**
 * @brief Emite um rótulo (label): label:
 */
void emit_label(const char *label);

/**
 * @brief Emite um desvio incondicional: goto label
 */
void emit_goto(const char *label);

/**
 * @brief Emite um desvio condicional: if op1 op op2 goto label
 */
void emit_if_goto(const char *op1, const char *op, const char *op2, const char *label);

/**
 * @brief Emite uma instrução de escrita: write var
 */
void emit_write(const char *var);

/**
 * @brief Emite uma instrução de leitura: read var
 */
void emit_read(const char *var);

/**
 * @brief Finaliza a impressão do código intermediário.
 */
void finalize_code_gen();

#endif // INTERMEDIATE_CODE_H