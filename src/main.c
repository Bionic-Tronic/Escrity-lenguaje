#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/interpret.h"

#define MAX_LINE_LENGTH 1024

void read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: No se pudo abrir el  archivo: '%s'\n", filename);
        exit(1);
    }
    char content_file[MAX_LINE_LENGTH];
    size_t bytes_read = fread(content_file, sizeof(char), MAX_LINE_LENGTH - 1, file);
    fclose(file);
    if (bytes_read == 0) {
        fprintf(stderr, "Error: El archivo '%s' está vacío o no se pudo leer\n", filename);
        exit(1);
    }
    content_file[bytes_read] = '\0';
    tokenize(content_file);
    interpret();
}

void print_help() {
    printf("Uso: Escrity [opciones]\n");
    printf("Opciones:\n");
    printf("  -i <archivo>    Especifica el archivo de entrada a procesar\n");
    printf("  -h              Muestra este mensaje de ayuda\n");
    printf("  -v              Muestra la versión del interprete\n");
}

void print_version () {
    printf("Escrity 0.0.2a\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char* filename = NULL;
    while ((opt = getopt(argc, argv, "i:hv")) != -1) {
        switch (opt) {
            case 'i':
                filename = optarg;
                break;
            case 'h':
                print_help();
                exit(0);
            case 'v':
                print_version();
                exit(0);
            default:
                fprintf(stderr, "Uso: %s -i <archivo> o %s -h\n", argv[0], argv[0]);
                exit(1);
        }
    }
    if (filename == NULL) {
        fprintf(stderr, "Error: No se especificó un archivo de entrada.\n");
        fprintf(stderr, "Uso: %s -i <archivo> o %s -h\n", argv[0], argv[0]);
        exit(1);
    }
    read_file(filename);
    return 0;
}