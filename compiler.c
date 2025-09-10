#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Source code file not specified. Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        perror("Source code file not found.");
        exit(EXIT_FAILURE);
    }

    printf("--- Conteúdo do arquivo: %s ---\n", argv[1]);

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        putchar(c);
    }

    printf("\n--- Fim do arquivo ---\n");

    fclose(file);
    return EXIT_SUCCESS;
}
