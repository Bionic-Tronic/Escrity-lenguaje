#ifndef INTERPRET_H
#define INTERPRET_H

void interpret_if_statement() {
    current_token++;
    int condition = evaluate_expression();
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        printf("Error: Expected 'then' after if condition\n");
        exit(1);
    }
    current_token++;
    if (condition) {
        while (current_token < token_count && 
               strcmp(tokens[current_token].type, "ELSE") != 0 && 
               strcmp(tokens[current_token].type, "END") != 0) {
            evaluate_expression();
        }
    } else {
        int paren_count = 0;
        while (current_token < token_count) {
            if (strcmp(tokens[current_token].type, "IF") == 0) paren_count++;
            if (strcmp(tokens[current_token].type, "END") == 0) {
                if (paren_count == 0) break;
                paren_count--;
            }
            if (paren_count == 0 && strcmp(tokens[current_token].type, "ELSE") == 0) {
                current_token++;
                while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                    evaluate_expression();
                }
                break;
            }
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of if statement\n");
        exit(1);
    }
    current_token++;
}

void interpret_for_loop() {
    current_token++;
    if (strcmp(tokens[current_token].type, "PAREN") == 0) {
        current_token++;
        evaluate_expression();
        if (current_token >= token_count || strcmp(tokens[current_token].type, "SEMICOLON") != 0) {
            printf("Error: Expected ';' in for loop\n");
            exit(1);
        }
        current_token++;
        while (evaluate_expression()) {
            if (current_token >= token_count || strcmp(tokens[current_token].type, "SEMICOLON") != 0) {
                printf("Error: Expected ';' in for loop\n");
                exit(1);
            }
            current_token++;
            int loop_start = current_token;
            int paren_count = 1;
            while (paren_count > 0 && current_token < token_count) {
                if (strcmp(tokens[current_token].type, "PAREN") == 0) {
                    if (strcmp(tokens[current_token].value, "(") == 0) paren_count++;
                    else if (strcmp(tokens[current_token].value, ")") == 0) paren_count--;
                }
                current_token++;
            }
            
            if (current_token >= token_count || strcmp(tokens[current_token-1].type, "PAREN") != 0) {
                printf("Error: Expected ')' at the end of for loop condition\n");
                exit(1);
            }
            
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
                printf("Error: Expected 'then' after for loop condition\n");
                exit(1);
            }
            current_token++;
            /*while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0)
                evaluate_expression();
             */
            while (current_token < token_count){
                evaluate_expression();
                if( strcmp(tokens[current_token].type, "END") == 0){
                	return;
                }
            }
            int saved_current_token = current_token;
            current_token = loop_start;
            evaluate_expression();
            current_token = saved_current_token;
        }
    } else if (strcmp(tokens[current_token].type, "SET") == 0) {
        current_token++;
        char var_name[MAX_TOKEN_LENGTH];
        if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
            printf("Error: Expected variable name in for loop\n");
            exit(1);
        }
        strcpy(var_name, tokens[current_token].value);
        current_token++;
        
        if (current_token >= token_count || strcmp(tokens[current_token].type, "IN_RANGE") != 0) {
            printf("Error: Expected 'in_range' in for loop\n");
            exit(1);
        }
        current_token++;
        set_variable(var_name, "int", 0);
        while (evaluate_expression()) {
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
                printf("Error: Expected 'then' after for loop condition\n");
                exit(1);
            }
            current_token++;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                evaluate_expression();
            }
            set_variable(var_name, "int", get_variable(var_name) + 1);
        }
    } else {
        printf("Error: Invalid for loop syntax\n");
        exit(1);
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of for loop\n");
        exit(1);
    }
    current_token++;
}

void interpret_while_loop() {
    current_token++;
    while (evaluate_expression()) {
        if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
            printf("Error: Expected 'then' after while condition\n");
            exit(1);
        }
        current_token++;
        while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0)
            evaluate_expression();
        if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
            printf("Error: Expected 'end' at the end of while loop\n");
            exit(1);
        }
        current_token = current_token - (token_count - 1);
    }
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0)
        current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of while loop\n");
        exit(1);
    }
    current_token++;
}

void interpret() {
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "FUNC") == 0) {
            current_token++;
            define_function();
        } else if (strcmp(tokens[current_token].type, "IF") == 0) {
            interpret_if_statement();
        } else if (strcmp(tokens[current_token].type, "FOR") == 0) {
            interpret_for_loop();
        } else if (strcmp(tokens[current_token].type, "WHILE") == 0) {
            interpret_while_loop();
        } else if (strcmp(tokens[current_token].type, "STRUCT") == 0) {
            current_token++;
            define_struct();
        } else {
            evaluate_expression();
        }
    }
}

#endif
