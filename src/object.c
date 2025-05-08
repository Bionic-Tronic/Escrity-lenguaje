#include "../include/object.h"

Struct* get_struct(const char* name) {
    for (int i = 0; i < struct_count; i++) {
        if (strcmp(structs[i].name, name) == 0) {
            return &structs[i];
        }
    }
    return NULL;
}

void define_struct() {
    if (struct_count >= MAX_STRUCTS)
        show_errors ("Too many objects",ERROR_SINTAXIS);
    Struct* s = &structs[struct_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
        show_errors ("Expeced object name",ERROR_SINTAXIS);
    strcpy(s->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expeced 'then' after object name",ERROR_SINTAXIS);
    current_token++;
    s->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
            show_errors ("Expeced member name in object",ERROR_SINTAXIS);
        strcpy(s->members[s->member_count].name, tokens[current_token].value);
        current_token++;
        if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0)
            show_errors ("Expeced ':' after member name in object",ERROR_SINTAXIS);
        current_token++;
        if (current_token >= token_count)
            show_errors ("Expeced member value in object",ERROR_SINTAXIS);
        if (strcmp(tokens[current_token].type, "NUMBER") == 0) {
            strcpy(s->members[s->member_count].type, "int");
            s->members[s->member_count].value.int_value = atoi(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "FLOAT") == 0) {
            strcpy(s->members[s->member_count].type, "float");
            s->members[s->member_count].value.float_value = atof(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "STRING") == 0) {
            strcpy(s->members[s->member_count].type, "string");
            strcpy(s->members[s->member_count].value.string_value, tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "CHAR") == 0) {
            strcpy(s->members[s->member_count].type, "char");
            s->members[s->member_count].value.char_value = tokens[current_token].value[1];
        } else if (strcmp(tokens[current_token].type, "FUNC") == 0) {
            strcpy(s->members[s->member_count].type, "function");
            Function* func = &functions[function_count++];
            s->members[s->member_count].value.func_value = func;
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].value, "_") != 0)
                show_errors ("Expeced '_' for anonymous function in object",ERROR_SINTAXIS);
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0 || strcmp(tokens[current_token].value, "(") != 0)
                show_errors ("Expeced '(' after '_' in object",ERROR_SINTAXIS);
            current_token++;
            func->param_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                    show_errors ("Expeced parameter name in function a object",ERROR_SINTAXIS);
                strcpy(func->params[func->param_count++], tokens[current_token].value);
                current_token++;
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
                show_errors ("Expeced ')' after function parameter in function object",ERROR_SINTAXIS);
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
                show_errors ("Expeced then after function declaration a function object",ERROR_SINTAXIS);
            current_token++;
            func->body_token_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                func->body[func->body_token_count++] = tokens[current_token];
                current_token++;
            }
            if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
                show_errors ("Expected 'end' at then end of function a function object",ERROR_SINTAXIS);
        } else
            show_errors ("Invalid member value in object",ERROR_SINTAXIS);
        current_token++;
        s->member_count++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of object",ERROR_SINTAXIS);
    current_token++;
}
