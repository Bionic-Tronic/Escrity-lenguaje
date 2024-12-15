#ifndef EVALUATE_H
#define EVALUATE_H

void removeQuotes(const char *source, char *dest) {
    while (*source) {
        if (*source != '\"') {
            *dest++ = *source;
        }
        source++;
    }
    *dest = '\0'; // Terminar la cadena destino con null
}

int evaluate_factor() {
    Token token = tokens[current_token];
    current_token++;
    
    if (strcmp(token.type, "NUMBER") == 0) {
        return atoi(token.value);
    } else if (strcmp(token.type, "IDENTIFIER") == 0) {
        if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0) {
            // Function call
            Function* func = get_function(token.value);
            if (func == NULL) {
                printf("Error: Function '%s' not defined\n", token.value);
                exit(1);
            }
            current_token++; // Skip '('
            int args[MAX_PARAMS];
            int arg_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                args[arg_count++] = evaluate_expression();
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++; // Skip ','
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0) {
                printf("Error: Expected ')' after function arguments\n");
                exit(1);
            }
            current_token++; // Skip ')'
            return call_function(func, args, arg_count);
        } else {
            return get_variable_value(token.value);
        }
    } else if (strcmp(token.type, "STRING") == 0) {
        char result_string[1024];
        removeQuotes(token.value, result_string);
        printf("%s\n", result_string);
        return 0;
    } else if (strcmp(token.type, "PRINT") == 0 || strcmp(token.type, "RESOLVER") == 0) {
        int result = evaluate_expression();
        if (strcmp(token.type, "PRINT") == 0) {
            printf("%d\n", result);
        }
        return result;
    } else if (strcmp(token.type, "ASCII") == 0 ){
    	printf("%s\n", token.value);
    	return 0;
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
        if (op == '*') {
            left *= right;
        } else {
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
        set_variable_value(var_name, value);
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
