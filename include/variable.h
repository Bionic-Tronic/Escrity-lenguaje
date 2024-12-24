#ifndef VARIABLE_H
#define VARIABLE_H

typedef struct Function Function;
typedef struct Array Array;
typedef char * string;
typedef const char * String;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char type[20];
    union {
        int int_value;
        double float_value;
        char string_value[MAX_TOKEN_LENGTH];
        char char_value;
        struct Struct* struct_value;
        Function* func_value;
        int* array_value;
        int array_size;
        int enum_value;
        FILE * fp;
    } value;
} Variable;

Variable variables[MAX_VARIABLES];
int variable_count = 0;

Variable* get_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

int get_type_variable (const char * name){
	for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            if(strcmp(variables[i].type,"int") == 0)
            	return 105;
            else if(strcmp(variables[i].type,"float") == 0)
                return 102;
            else if(strcmp(variables[i].type,"string") == 0)
                return 115;
            else if(strcmp(variables[i].type,"char") == 0)
                return 99;
            else if(strcmp(variables[i].type,"function") == 0)
                return 219;
            else if(strcmp(variables[i].type,"array") == 0)
                return 97;
        }
    }
    return -1;
}

int buscar_palabra (String word, String texto){
    string text_copy = strdup(word);
    if (text_copy == NULL)
        return -1;
    string line = strtok(text_copy, "\n");
    while (line != NULL){
        if (strstr(line, texto) != NULL){
            free(text_copy);
            return 1;
        }
        line = strtok(NULL, "\n");
    }
    free(text_copy);
    return -1;
}

void set_variable(const char* name, const char* type, void* value) {
    Variable* var = get_variable(name);
    if (var == NULL) {
        if (variable_count >= MAX_VARIABLES) {
            printf("Error: Too many variables\n");
            exit(1);
        }
        var = &variables[variable_count++];
        strcpy(var->name, name);
    }
    strcpy(var->type, type);
    if (strcmp(type, "int") == 0) {
        var->value.int_value = *(int*)value;
    } else if (strcmp(type, "float") == 0) {
        var->value.float_value = *(double*)value;
    } else if (strcmp(type, "string") == 0) {
        strcpy(var->value.string_value, (char*)value);
    } else if (strcmp(type, "char") == 0) {
        var->value.char_value = *(char*)value;
    } else if (strcmp(type, "function") == 0) {
        var->value.func_value = (Function*)value;
    } else if (strcmp(type, "array") == 0) {
        var->value.array_value = (Array*)value;
    } else if (strcmp(type, "file") == 0) {
        var->value.array_value = (FILE*)value;
    }
}

#endif
