#include "../include/enums.h"

Values * get_values (const char * name){
	for (int i = 0; i < values_count; i++) {
        if (strcmp(values[i].name, name) == 0) {
            return &values[i];
        }
    }
    return NULL;
}

void define_values () {
	char * type;
	if (values_count >= MAX_VALUES)
	    show_errors("Too many values",-1);
	Values * v = &values[values_count++];
	if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
	   show_errors ("Expeced value name",-1);
	strcpy(v->name, tokens[current_token].value);
	current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0)
        show_errors ("Expeced ':' after value name",-1);
    current_token++;
    if (strcmp(tokens[current_token].type,"INTEGER_PARAM") == 0){
    	type = "int";
    } else if (strcmp(tokens[current_token].type,"STRING_PARAM") == 0){
    	type = "string";
    } else if (strcmp(tokens[current_token].type,"FLOAT_PARAM") == 0){
    	type = "float";
    } else if (strcmp(tokens[current_token].type,"CHAR_PARAM") == 0){
    	type = "char";
    } else if (strcmp(tokens[current_token].type,"BOOL_PARAM") == 0){
    	type = "bool";
    } else
        show_errors ("Expeced type of value after name",-1);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expeced 'then' after object name",-1);
    current_token++;
    v->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
            show_errors ("Expeced member name in values",-1);
        strcpy(v->members[v->member_count].name, tokens[current_token].value);
        current_token++;
        if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0)
            show_errors ("Expeced '=' after member name in values",-1);
        if (tokens[current_token].value[0] != '=')
            show_errors("Expected '=' after member name in values",-1);
        current_token++;
        if (strcmp(type, "int") == 0) {
            strcpy(v->members[v->member_count].type, "int");
            v->members[v->member_count].value.int_value = atoi(tokens[current_token].value);
        } else if (strcmp(type, "float") == 0) {
            strcpy(v->members[v->member_count].type, "float");
            v->members[v->member_count].value.float_value = atof(tokens[current_token].value);
        } else if (strcmp(type, "string") == 0) {
            strcpy(v->members[v->member_count].type, "string");
            strcpy(v->members[v->member_count].value.string_value, tokens[current_token].value);
        } else if (strcmp(type, "char") == 0) {
            strcpy(v->members[v->member_count].type, "char");
            v->members[v->member_count].value.char_value = tokens[current_token].value[1];
        } else
            show_errors ("Invalid member in values structure",-1);
        current_token++;
        v->member_count++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
            current_token++;
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of values",-1);
    current_token++;
}

Enum* get_enum(const char* name) {
    for (int i = 0; i < enum_count; i++) {
        if (strcmp(enums[i].name, name) == 0) {
            return &enums[i];
        }
    }
    return NULL;
}

void define_enum() {
    if (enum_count >= MAX_ENUMS) {
        printf("Error: Too many enums\n");
        exit(1);
    }
    Enum* e = &enums[enum_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
        printf("Error: Expected enum name\n");
        exit(1);
    }
    strcpy(e->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        printf("Error: Expected 'then' after enum name\n");
        exit(1);
    }
    current_token++;
    e->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
            printf("Error: Expected enum member name\n");
            exit(1);
        }
        strcpy(e->members[e->member_count], tokens[current_token].value);
        e->member_count++;
        current_token++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of enum\n");
        exit(1);
    }
    current_token++;
}
