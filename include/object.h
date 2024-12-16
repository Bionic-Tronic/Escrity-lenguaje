#ifndef OBJECT_H
#define OBJECT_H

typedef struct Struct {
    char name[MAX_TOKEN_LENGTH];
    Variable members[MAX_STRUCT_MEMBERS];
    int member_count;
} Struct;

Struct structs[MAX_STRUCTS];
int struct_count = 0;

Struct* get_struct(const char* name) {
    for (int i = 0; i < struct_count; i++) {
        if (strcmp(structs[i].name, name) == 0) {
            return &structs[i];
        }
    }
    return NULL;
}

void define_struct() {
    if (struct_count >= MAX_STRUCTS) {
        printf("Error: Too many structs\n");
        exit(1);
    }

    Struct* s = &structs[struct_count++];

    // Get struct name
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
        printf("Error: Expected struct name\n");
        exit(1);
    }
    strcpy(s->name, tokens[current_token].value);
    current_token++;

    // Expect 'declare'
    if (current_token >= token_count || strcmp(tokens[current_token].type, "DECLARE") != 0) {
        printf("Error: Expected 'declare' after struct name\n");
        exit(1);
    }
    current_token++;

    // Get struct members
    s->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
            printf("Error: Expected member name\n");
            exit(1);
        }
        strcpy(s->members[s->member_count].name, tokens[current_token].value);
        current_token++;

        if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0) {
            printf("Error: Expected ':' after member name\n");
            exit(1);
        }
        current_token++;

        if (current_token >= token_count) {
            printf("Error: Expected member value\n");
            exit(1);
        }

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
            
            // Parse anonymous function
            current_token++; // Skip 'func'
            if (current_token >= token_count || strcmp(tokens[current_token].value, "_anonima_") != 0) {
                printf("Error: Expected '_anonima_' for anonymous function\n");
                exit(1);
            }
            current_token++;

            // Get parameters
            if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0 || strcmp(tokens[current_token].value, "(") != 0) {
                printf("Error: Expected '(' after '_anonima_'\n");
                exit(1);
            }
            current_token++;

            func->param_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
                    printf("Error: Expected parameter name\n");
                    exit(1);
                }
                strcpy(func->params[func->param_count++], tokens[current_token].value);
                current_token++;
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }

            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0) {
                printf("Error: Expected ')' after function parameters\n");
                exit(1);
            }
            current_token++;

            // Expect 'then'
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
                printf("Error: Expected 'then' after function declaration\n");
                exit(1);
            }
            current_token++;

            // Get function body
            func->body_token_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                func->body[func->body_token_count++] = tokens[current_token];
                current_token++;
            }

            if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
                printf("Error: Expected 'end' at the end of function\n");
                exit(1);
            }
        } else {
            printf("Error: Invalid member value\n");
            exit(1);
        }

        current_token++;
        s->member_count++;

        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }

    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of struct\n");
        exit(1);
    }
    current_token++; // Skip 'end'
}

#endif
