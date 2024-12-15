#ifndef INTERPRET_H
#define INTERPRET_H

void interpret_if_statement() {
    current_token++; // Skip 'if'
    int condition = evaluate_expression();
    
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        printf("Error: Expected 'then' after if condition\n");
        exit(1);
    }
    current_token++; // Skip 'then'

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
    current_token++; // Skip 'end'
}

void interpret_for_loop() {
    current_token++; // Skip 'for'
    
    // Check if it's a traditional for loop or the new syntax
    if (strcmp(tokens[current_token].type, "PAREN") == 0) {
        // Traditional for loop
        current_token++; // Skip '('
        evaluate_expression(); // Initialize
        if (current_token >= token_count || strcmp(tokens[current_token].type, "SEMICOLON") != 0) {
            printf("Error: Expected ';' in for loop\n");
            exit(1);
        }
        current_token++; // Skip ';'
        
        while (evaluate_expression()) { // Condition
            if (current_token >= token_count || strcmp(tokens[current_token].type, "SEMICOLON") != 0) {
                printf("Error: Expected ';' in for loop\n");
                exit(1);
            }
            current_token++; // Skip ';'
            
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
            current_token++; // Skip 'then'
            
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                evaluate_expression();
            }
            
            int saved_current_token = current_token;
            current_token = loop_start;
            evaluate_expression(); // Increment
            current_token = saved_current_token;
        }
    } else if (strcmp(tokens[current_token].type, "SET") == 0) {
        // New for loop syntax: for(set i in_range i < 10) then
        current_token++; // Skip 'set'
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
        current_token++; // Skip 'in_range'
        
        set_variable_value(var_name, 0); // Initialize loop variable
        while (evaluate_expression()) { // Condition
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
                printf("Error: Expected 'then' after for loop condition\n");
                exit(1);
            }
            current_token++; // Skip 'then'
            
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                evaluate_expression();
            }
            
            set_variable_value(var_name, get_variable_value(var_name) + 1); // Increment
        }
    } else {
        printf("Error: Invalid for loop syntax\n");
        exit(1);
    }
    
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of for loop\n");
        exit(1);
    }
    current_token++; // Skip 'end'
}

void interpret_while_loop() {
    current_token++; // Skip 'while'
    
    while (evaluate_expression()) {
        if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
            printf("Error: Expected 'then' after while condition\n");
            exit(1);
        }
        current_token++; // Skip 'then'
        
        while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
            evaluate_expression();
        }
        
        if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
            printf("Error: Expected 'end' at the end of while loop\n");
            exit(1);
        }
        current_token = current_token - (token_count - 1); // Reset to start of while loop
    }
    
    // Skip to end of while loop
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        current_token++;
    }
    
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of while loop\n");
        exit(1);
    }
    current_token++; // Skip 'end'
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
        } else {
            evaluate_expression();
        }
    }
}

#endif
