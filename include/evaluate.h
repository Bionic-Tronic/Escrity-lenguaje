#ifndef EVALUATE_H
#define EVALUATE_H

void removeQuotes(const char *source, char *dest) {
    while (*source) {
        if (*source != '\"') {
            *dest++ = *source;
        }
        source++;
    }
    *dest = '\0';
}

int evaluate_array_access(Variable* var) {
    if (strcmp(var->type, "array") != 0) {
        printf("Error: Variable '%s' is not an array\n", var->name);
        exit(1);
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "BRACKET") != 0 || strcmp(tokens[current_token].value, "[") != 0) {
        printf("Error: Expected '[' for array access\n");
        exit(1);
    }
    current_token++;
    int index = evaluate_expression();
    if (current_token >= token_count || strcmp(tokens[current_token].type, "BRACKET") != 0 || strcmp(tokens[current_token].value, "]") != 0) {
        printf("Error: Expected ']' after array index\n");
        exit(1);
    }
    current_token++;
    if (index < 0 || index >= var->value.array_size) {
        printf("Error: Array index out of bounds\n");
        exit(1);
    }
    return var->value.array_value[index];
}

int evaluate_factor() {
    Token token = tokens[current_token];
    current_token++;
    if (strcmp(token.type, "NUMBER") == 0) {
        return atoi(token.value);
    } else if (strcmp(token.type, "FLOAT") == 0) {
        return (int)(atof(token.value) * 1000);
    } else if (strcmp(token.type, "IDENTIFIER") == 0) {
        if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0) {
            Function* func = get_function(token.value);
            if (func == NULL) {
                printf("Error: Function '%s' not defined\n", token.value);
                exit(1);
            }
            current_token++;
            int args[MAX_PARAMS];
            int arg_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                args[arg_count++] = evaluate_expression();
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0) {
                printf("Error: Expected ')' after function arguments\n");
                exit(1);
            }
            current_token++;
            return call_function(func, args, arg_count);
        } else if (current_token < token_count && strcmp(tokens[current_token].type, "DOT") == 0) {
            Struct* s = get_struct(token.value);
            if (s == NULL) {
                printf("Error: Struct '%s' not defined\n", token.value);
                exit(1);
            }
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
                printf("Error: Expected member name after '.'\n");
                exit(1);
            }
            char* member_name = tokens[current_token].value;
            current_token++;
            for (int i = 0; i < s->member_count; i++) {
                if (strcmp(s->members[i].name, member_name) == 0) {
                    if (strcmp(s->members[i].type, "int") == 0) {
                        return s->members[i].value.int_value;
                    } else if (strcmp(s->members[i].type, "float") == 0) {
                        return (int)(s->members[i].value.float_value * 1000); // Convert to int (cents)
                    } else if (strcmp(s->members[i].type, "char") == 0) {
                        return s->members[i].value.char_value;
                    } else if (strcmp(s->members[i].type, "function") == 0) {
                        Function* func = s->members[i].value.func_value;
                        int args[MAX_PARAMS];
                        int arg_count = 0;
                        if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0) {
                            current_token++;
                            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                                args[arg_count++] = evaluate_expression();
                                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                                    current_token++;
                                }
                            }
                            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0) {
                                printf("Error: Expected ')' after function arguments\n");
                                exit(1);
                            }
                            current_token++;
                        }
                        return call_function(func, args, arg_count);
                    } else {
                        printf("Error: Cannot evaluate member '%s' of struct '%s'\n", member_name, token.value);
                        exit(1);
                    }
                }
            }
            printf("Error: Member '%s' not found in struct '%s'\n", member_name, token.value);
            exit(1);
        } else {
            Variable* var = get_variable(token.value);
            if (var == NULL) {
                printf("Error: Variable '%s' not defined\n", token.value);
                exit(1);
            }
            if (strcmp(var->type, "int") == 0) {
                return var->value.int_value;
            } else if (strcmp(var->type, "float") == 0) {
                return (int)(var->value.float_value * 1000);
            } else if (strcmp(var->type, "char") == 0) {
                return var->value.char_value;
            } else if (strcmp(var->type, "array") == 0) {
                return evaluate_array_access(var);
            } else {
                printf("Error: Cannot evaluate variable '%s' of type '%s'\n", token.value, var->type);
                exit(1);
            }
        }
    } else if (strcmp(token.type, "CHAR") == 0) {
        return token.value[1];
    } else if (strcmp(token.type, "STRING") == 0) {
        char result_string[1024];
        removeQuotes(token.value, result_string);
        printf("%s", result_string);
        return 0;
    } else if (strcmp(token.type, "PRINT") == 0 || strcmp(token.type, "RESOLVER") == 0) {
        int result = evaluate_expression();
        if (strcmp(token.type, "RESOLVER") == 0) {
            printf("%d", result);
        }
        return result;
    } else if(strcmp(token.type, "GET_INT") == 0){
    	int v;
    	fflush(stdin);
    	scanf("%d",&v);
    	fflush(stdin);
    	return v;
    } else if(strcmp(token.type, "GET_STR") == 0){
    	char v[1024];
    	fflush(stdin);
    	fgets(v,1024,stdin);
    	fflush(stdin);
    	return * v;
    } else if(strcmp(token.type, "RANDOM") == 0){
    	srand(time(NULL));
    	int r = rand()%atoi(token.value);
    	return r;
    } else if(strcmp(token.type, "PI") == 0) {
    	return 3.14159;
    } else if(strcmp(token.type, "EXIT") == 0) {
    	exit(1);
    } else if (strcmp(token.type, "PUTS") == 0){
    	printf("\n");
    	return 0;
    } else if (strcmp(token.type, "GETCHAR") == 0){
    	char x = getch();
    	return x;
    } else if (strcmp(token.type, "BRACKET") == 0 && strcmp(token.value, "[") == 0) {
        int array[MAX_ARRAY_SIZE];
        int size = 0;
        while (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") != 0) {
            array[size++] = evaluate_expression();
            if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                current_token++;
            }
        }
        if (current_token >= token_count || strcmp(tokens[current_token].value, "]") != 0) {
            printf("Error: Expected ']' after array literal\n");
            exit(1);
        }
        current_token++;
        char array_name[MAX_TOKEN_LENGTH];
        sprintf(array_name, "_array_%d", variable_count);
        Variable* var = &variables[variable_count++];
        strcpy(var->name, array_name);
        strcpy(var->type, "array");
        var->value.array_value = malloc(size * sizeof(int));
        memcpy(var->value.array_value, array, size * sizeof(int));
        var->value.array_size = size;
        return 0;
    } else {
        printf("Error: Unexpected token %s\n", token.type);
        exit(1);
    }
}

int evaluate_term() {
    int left = evaluate_factor();
    while (current_token < token_count && 
           strcmp(tokens[current_token].type, "OPERATOR") == 0 && 
           (tokens[current_token].value[0] == '*' || tokens[current_token].value[0] == '/')) {
        char op = tokens[current_token].value[0];
        current_token++;
        int right = evaluate_factor();
        if (op == '*')
            left *= right;
        else {
            if (right == 0) {
                printf("Error: Division by zero\n");
                exit(1);
            }
            left /= right;
        }
    }
    return left;
}

int evaluate_expression() {
    if (current_token + 2 < token_count &&
        strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
        strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
        tokens[current_token + 1].value[0] == '=') {
        char var_name[MAX_TOKEN_LENGTH];
        strcpy(var_name, tokens[current_token].value);
        current_token += 2;
        int value = evaluate_expression();
        set_variable(var_name, "int", &value);
        return value;
    }

    int left = evaluate_term();
    
    while (current_token < token_count && 
           strcmp(tokens[current_token].type, "OPERATOR") == 0 && 
           (tokens[current_token].value[0] == '+' || tokens[current_token].value[0] == '-' ||
            strcmp(tokens[current_token].value, "==") == 0 || strcmp(tokens[current_token].value, "<") == 0 ||
            strcmp(tokens[current_token].value, ">") == 0 || strcmp(tokens[current_token].value, "<=") == 0 ||
            strcmp(tokens[current_token].value, ">=") == 0)) {
        char op[3];
        strcpy(op, tokens[current_token].value);
        current_token++;
        int right = evaluate_term();
        if (strcmp(op, "+") == 0) {
            left += right;
        } else if (strcmp(op, "-") == 0) {
            left -= right;
        } else if (strcmp(op, "==") == 0) {
            left = (left == right);
        } else if (strcmp(op, "<") == 0) {
            left = (left < right);
        } else if (strcmp(op, ">") == 0) {
            left = (left > right);
        } else if (strcmp(op, "<=") == 0) {
            left = (left <= right);
        } else if (strcmp(op, ">=") == 0) {
            left = (left >= right);
        }
    }
    
    return left;
}

#endif
