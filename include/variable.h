#ifndef VARIABLE_H
#define VARIABLE_H

typedef struct Function Function;
typedef struct Array Array;

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
    }
}

#endif
