#include "../include/variable.h"

void set_variable(const char* name, const char* type, void* value) {
    Variable* var = get_variable(name);
    if (var == NULL) {
        if (variable_count >= MAX_VARIABLES)
            show_errors ("Too many variables",ERROR_INTERPRETER);
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
        var->value.char_value = (char)value;
    } else if (strcmp(type, "bool") == 0) {
        int x = (int)value;
        var->value.bool_value = x;
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

Variable * get_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            if (variables[i].is_wait == 1)
            	return NULL;
            return &variables[i];
        }
    }
    return NULL;
}

bool is_int (const char * name){
    Variable * var = get_variable(name);
    if (var == NULL)
        invalid_var(name, ERROR);
    if (strcmp(var->type, "int") == 0)
        return true;
    false;
}
bool is_string (const char * name){
    Variable * var = get_variable(name);
    if (var == NULL)
        invalid_var(name, ERROR);
    if (strcmp(var->type, "string"))
        return true;
    false;
}
bool is_float (const char * name){
    Variable * var = get_variable(name);
    if (var == NULL)
        invalid_var(name, ERROR);
    if (strcmp(var->type, "float"))
        return true;
    false;
}
bool is_bool (const char * name){
    Variable * var = get_variable(name);
    if (var == NULL)
        invalid_var(name, ERROR);
    if (strcmp(var->type, "bool"))
        return true;
    false;
}
bool is_char (const char * name){
    Variable * var = get_variable(name);
    if (var == NULL)
        invalid_var(name, ERROR);
    if (strcmp(var->type, "char"))
        return true;
    false;
}
