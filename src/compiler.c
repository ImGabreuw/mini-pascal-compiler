#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "scanner.h"
#include "parser.h"
#include "intermediary_code_generator.h"

/**
 *Integrantes:
 *Enzo B. Proença - 10418579 - 06D
 *Gabriel Ken Kazama Geronazzo - 10418247 - 06D
 *Lucas Pires de Camargo Sarai - 10418013 - 06D
 */

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Source code file not specified. Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *program_name = argv[0];

    log_init(program_name);
    scanner_init(argv[1]);
    init_code_gen(program_name);
    parser_init();

    parser_parse();

    parser_cleanup();
    scanner_cleanup();
    finalize_code_gen();
    log_cleanup();

    return EXIT_SUCCESS;
}
