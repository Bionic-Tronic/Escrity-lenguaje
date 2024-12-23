#ifndef ARRAY_H
#define ARRAY_H

struct Array {
    char name[MAX_TOKEN_LENGTH];
    Variable members[MAX_ARRAY_MEMBERS];
    int member_count;
};

Array arrays[MAX_ARRAY_SIZE];
int array_count = 0;

Array* get_array(const char* name) {
    for (int i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) {
            return &arrays[i];
        }
    }
    return NULL;
}

void define_array() {
    if (array_count >= MAX_ARRAY_SIZE) {
        printf("Error: Too many arrays\n");
        exit(1);
    }
    Array* arr = &arrays[array_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
        printf("Error: Expected array name\n");
        exit(1);
    }
    strcpy(arr->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "INCLUDE") != 0) {
        printf("Error: Expected 'include' after array name\n");
        exit(1);
    }
    current_token++;
    arr->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
            printf("Error: Expected member name\n");
            exit(1);
        }
        strcpy(arr->members[arr->member_count].name, tokens[current_token].value);
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
            strcpy(arr->members[arr->member_count].type, "int");
            arr->members[arr->member_count].value.int_value = atoi(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "FLOAT") == 0) {
            strcpy(arr->members[arr->member_count].type, "float");
            arr->members[arr->member_count].value.float_value = atof(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "STRING") == 0) {
            strcpy(arr->members[arr->member_count].type, "string");
            strcpy(arr->members[arr->member_count].value.string_value, tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "CHAR") == 0) {
            strcpy(arr->members[arr->member_count].type, "char");
            arr->members[arr->member_count].value.char_value = tokens[current_token].value[1];
        } else if (strcmp(tokens[current_token].type, "BRACKET") == 0 && strcmp(tokens[current_token].value, "[") == 0) {
            strcpy(arr->members[arr->member_count].type, "array");
            int nested_array[MAX_ARRAY_SIZE];
            int nested_size = 0;
            current_token++;
            while (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") != 0) {
                nested_array[nested_size++] = evaluate_expression();
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, "]") != 0) {
                printf("Error: Expected ']' after nested array\n");
                exit(1);
            }
            current_token++;
            arr->members[arr->member_count].value.array_value = malloc(nested_size * sizeof(int));
            memcpy(arr->members[arr->member_count].value.array_value, nested_array, nested_size * sizeof(int));
            arr->members[arr->member_count].value.array_size = nested_size;
        } else if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0 && strcmp(tokens[current_token].value, "Array") == 0) {
            strcpy(arr->members[arr->member_count].type, "array");
            Array* nested_arr = &arrays[array_count++];
            arr->members[arr->member_count].value.array_value = nested_arr;
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0 || strcmp(tokens[current_token].value, "_anonima_") != 0) {
                printf("Error: Expected '_anonima_' for anonymous array\n");
                exit(1);
            }
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "INCLUDE") != 0) {
                printf("Error: Expected 'include' after '_anonima_'\n");
                exit(1);
            }
            current_token++;
            nested_arr->member_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
                    printf("Error: Expected member name in nested array\n");
                    exit(1);
                }
                strcpy(nested_arr->members[nested_arr->member_count].name, tokens[current_token].value);
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0) {
                    printf("Error: Expected ':' after member name in nested array\n");
                    exit(1);
                }
                current_token++;
                if (current_token >= token_count) {
                    printf("Error: Expected member value in nested array\n");
                    exit(1);
                }
                if (strcmp(tokens[current_token].type, "NUMBER") == 0) {
                    strcpy(nested_arr->members[nested_arr->member_count].type, "int");
                    nested_arr->members[nested_arr->member_count].value.int_value = atoi(tokens[current_token].value);
                } else if (strcmp(tokens[current_token].type, "FLOAT") == 0) {
                    strcpy(nested_arr->members[nested_arr->member_count].type, "float");
                    nested_arr->members[nested_arr->member_count].value.float_value = atof(tokens[current_token].value);
                } else if (strcmp(tokens[current_token].type, "STRING") == 0) {
                    strcpy(nested_arr->members[nested_arr->member_count].type, "string");
                    strcpy(nested_arr->members[nested_arr->member_count].value.string_value, tokens[current_token].value);
                } else if (strcmp(tokens[current_token].type, "CHAR") == 0) {
                    strcpy(nested_arr->members[nested_arr->member_count].type, "char");
                    nested_arr->members[nested_arr->member_count].value.char_value = tokens[current_token].value[1];
                } else {
                    printf("Error: Invalid member value in nested array\n");
                    exit(1);
                }
                current_token++;
                nested_arr->member_count++;
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
                printf("Error: Expected 'end' at the end of nested array\n");
                exit(1);
            }
            current_token++;
        } else if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0 && strcmp(tokens[current_token].value, "enum") == 0) {
            strcpy(arr->members[arr->member_count].type, "enum");
            Enum* e = &enums[enum_count++];
            arr->members[arr->member_count].value.struct_value = e;
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0 || strcmp(tokens[current_token].value, "_anonima_") != 0) {
                printf("Error: Expected '_anonima_' for anonymous enum\n");
                exit(1);
            }
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
                printf("Error: Expected 'then' after '_anonima_'\n");
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
        } else {
            printf("Error: Invalid member value\n");
            exit(1);
        }
        current_token++;
        arr->member_count++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of array\n");
        exit(1);
    }
    current_token++;
    set_variable(arr->name, "array", arr);
}

#endif
