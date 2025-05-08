#include "../include/properties.h"

Properties * get_properties (const char* name){
	for (int i = 0; i < struct_count; i++) {
        if (strcmp(pro[i].name, name) == 0) {
            return &pro[i];
        }
    }
    return NULL;
}

void define_properties() {
    if (properties_count >= MAX_PROPERTIES)
        show_errors ("Too many properties",-1);
    Properties * s = &pro[properties_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
        show_errors ("Expected properties name",-1);
    strcpy(s->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expected 'then' after properties name",-1);
    current_token++;
    s->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type,"ACESS") == 0) {
        	if (strcmp(tokens[current_token].value,"priv") == 0)
        		s->visible[s->member_count] = 0;
        	else if (strcmp(tokens[current_token].value,"pub") == 0)
        		s->visible[s->member_count] = 1;
        	else
        		show_errors ("Expected 'priv' or 'pub' a member properties",-1);
        	current_token++;
        } else
        	show_errors ("Expected a visivility member of properties",-1);
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
            show_errors ("Expected member name in properties",-1);
        strcpy(s->members[s->member_count].name, tokens[current_token].value);
        current_token++;
        if (current_token >= token_count || strcmp(tokens[current_token].type, "AT") != 0)
            show_errors ("Expected 'at' after member name in properties",-1);
        current_token++;
        if (current_token >= token_count)
            show_errors ("Expected member value in properties",-1);
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
        } else if (strcmp(tokens[current_token].type, "PAREN") == 0) {
            strcpy(s->members[s->member_count].type, "function");
            Function* func = &functions[function_count++];
            s->members[s->member_count].value.func_value = func;
            current_token++;
            func->param_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                    show_errors ("Expected parameter name in properties",-1);
                strcpy(func->params[func->param_count++], tokens[current_token].value);
                current_token++;
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
                show_errors ("Expected ')' after function parameters in properties",-1);
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
                show_errors ("Exoected 'then' after fubction declaration in properties",-1);
            current_token++;
            func->body_token_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                func->body[func->body_token_count++] = tokens[current_token];
                current_token++;
            }
            if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
                show_errors ("Expected 'end' at the end of function in properties",-1);
        } else {
            strcpy(s->members[s->member_count].type, "int");
            s->members[s->member_count].value.char_value = 0;
        }
        current_token++;
        s->member_count++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of properties",-1);
    current_token++;
}
