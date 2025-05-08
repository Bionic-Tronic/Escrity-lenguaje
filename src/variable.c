#include "variable.h"

void set_variable(const char* name, const char* type, void* value) {
    Variable* var = get_variable(name);
    if (var == NULL) {
        if (variable_count >= MAX_VARIABLES)
            show_errors ("Too many variables",ERROR_INTERPRETER);
        var = &variables[variable_count++];
        strcpy(var->name, name);
    }
    strcpy(var->type, type);
    var->is_wait = 0;
    if (strcmp(type, "int") == 0) {
        var->value.int_value = *(int*)value;
    } else if (strcmp(type, "float") == 0) {
        var->value.float_value = *(double*)value;
    } else if (strcmp(type, "string") == 0) {
        strcpy(var->value.string_value, (char*)value);
    } else if (strcmp(type, "char") == 0) {
        var->value.char_value = (char)value;
    } else if (strcmp(type, "function") == 0) {
        var->value.func_value = (Function*)value;
    } else if (strcmp(type, "array") == 0) {
        var->value.array_value = (Array*)value;
    } else if (strcmp(type, "bool") == 0) {
        int x = (int)value;
        if (x < 0 || x > 1)
        	x = 0;
        var->value.bool_value = x;
    } else if (strcmp(type, "wait") == 0) {
        var->is_wait = 1;
        var->value.wait_value = 0;
    } else if (strcmp(type, "empty") == 0) {
        var->value.int_value = 0;
    } else if (strcmp(type, "http_server") == 0) {
        var->value.int_value = 0;
    }
}

char * get_type_variable (const char * name){
        for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].type;
        }
    }
    return "undifined";
}

Variable* get_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            if (variables[i].is_wait == 1)
            	return NULL;
            return &variables[i];
        }
    }
    return NULL;
}
