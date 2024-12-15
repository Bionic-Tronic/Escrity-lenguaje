#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKEN_LENGTH 100
#define MAX_TOKENS 1000
#define MAX_VARIABLES 100
#define MAX_FUNCTIONS 50
#define MAX_PARAMS 10
#define MAX_BODY_TOKENS 100
#define MAX_INPUT_LENGTH 1000

typedef struct {
    char type[20];
    char value[MAX_TOKEN_LENGTH];
} Token;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    int value;
} Variable;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char params[MAX_PARAMS][MAX_TOKEN_LENGTH];
    int param_count;
    Token body[MAX_BODY_TOKENS];
    int body_token_count;
} Function;

Token tokens[MAX_TOKENS];
int token_count = 0;
int current_token = 0;

Variable variables[MAX_VARIABLES];
int variable_count = 0;

Function functions[MAX_FUNCTIONS];
int function_count = 0;

void add_token(const char* type, const char* value) {
    strcpy(tokens[token_count].type, type);
    strcpy(tokens[token_count].value, value);
    token_count++;
}

void tokenize(const char* code) {
    char buffer[MAX_TOKEN_LENGTH];
    int buffer_pos = 0;
    for (int i = 0; code[i] != '\0'; i++) {
        if (isalpha(code[i]) || code[i] == '_') {
            buffer[buffer_pos++] = code[i];
            while (isalnum(code[i+1]) || code[i+1] == '_') {
                buffer[buffer_pos++] = code[++i];
            }
            buffer[buffer_pos] = '\0';
            if (strcmp(buffer, "text") == 0 || strcmp(buffer, "print") == 0) {
                add_token("PRINT", buffer);
            } else if (strcmp(buffer, "resolver") == 0) {
                add_token("RESOLVER", buffer);
            } else if (strcmp(buffer, "func") == 0) {
                add_token("FUNC", buffer);
            } else if (strcmp(buffer, "then") == 0) {
                add_token("THEN", buffer);
            } else if (strcmp(buffer, "end") == 0) {
                add_token("END", buffer);
            } else if (strcmp(buffer, "return") == 0) {
                add_token("RETURN", buffer);
            } else if (strcmp(buffer, "if") == 0) {
                add_token("IF", buffer);
            } else if (strcmp(buffer, "else") == 0) {
                add_token("ELSE", buffer);
            } else {
                add_token("IDENTIFIER", buffer);
            }
            buffer_pos = 0;
        } else if (isdigit(code[i])) {
            buffer[buffer_pos++] = code[i];
            while (isdigit(code[i+1])) {
                buffer[buffer_pos++] = code[++i];
            }
            buffer[buffer_pos] = '\0';
            add_token("NUMBER", buffer);
            buffer_pos = 0;
        } else if (code[i] == '"') {
            buffer[buffer_pos++] = code[i];
            i++;
            while (code[i] != '"' && code[i] != '\0') {
                buffer[buffer_pos++] = code[i++];
            }
            if (code[i] == '"') {
                buffer[buffer_pos++] = code[i];
            }
            buffer[buffer_pos] = '\0';
            add_token("STRING", buffer);
            buffer_pos = 0;
        } else if (strchr("+-*/=(),<>", code[i]) != NULL) {
            buffer[0] = code[i];
            buffer[1] = '\0';
            if (code[i] == '=' && code[i+1] == '=') {
                buffer[1] = '=';
                buffer[2] = '\0';
                i++;
            } else if ((code[i] == '<' || code[i] == '>') && code[i+1] == '=') {
                buffer[1] = '=';
                buffer[2] = '\0';
                i++;
            }
            add_token(code[i] == '(' || code[i] == ')' ? "PAREN" :
                      code[i] == ',' ? "COMMA" : "OPERATOR", buffer);
        } else if (code[i] == ' ' || code[i] == '\t' || code[i] == '\n') {
        } else {
            printf("Error: Unexpected character '%c'\n", code[i]);
            exit(1);
        }
    }
}

int get_variable_value(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    printf("Error: Variable '%s' not defined\n", name);
    exit(1);
}

void set_variable_value(const char* name, int value) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            variables[i].value = value;
            return;
        }
    }
    if (variable_count < MAX_VARIABLES) {
        strcpy(variables[variable_count].name, name);
        variables[variable_count].value = value;
        variable_count++;
    } else {
        printf("Error: Too many variables\n");
        exit(1);
    }
}

int evaluate_expression();

Function* get_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return &functions[i];
        }
    }
    return NULL;
}

int call_function(Function* func, int args[], int arg_count) {
    if (arg_count != func->param_count) {
        printf("Error: Function '%s' expects %d arguments, but got %d\n", func->name, func->param_count, arg_count);
        exit(1);
    }
    Variable saved_vars[MAX_VARIABLES];
    int saved_var_count = variable_count;
    memcpy(saved_vars, variables, sizeof(variables));
    for (int i = 0; i < func->param_count; i++)
        set_variable_value(func->params[i], args[i]);
    int saved_current_token = current_token;
    Token saved_tokens[MAX_TOKENS];
    int saved_token_count = token_count;
    memcpy(saved_tokens, tokens, sizeof(tokens));
    memcpy(tokens, func->body, sizeof(Token) * func->body_token_count);
    token_count = func->body_token_count;
    current_token = 0;
    int result = 0;
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "RETURN") == 0) {
            current_token++;
            result = evaluate_expression();
            break;
        }
        evaluate_expression();
    }
    memcpy(tokens, saved_tokens, sizeof(tokens));
    token_count = saved_token_count;
    current_token = saved_current_token;
    memcpy(variables, saved_vars, sizeof(variables));
    variable_count = saved_var_count;
    return result;
}

int evaluate_factor() {
    Token token = tokens[current_token];
    current_token++;
    if (strcmp(token.type, "NUMBER") == 0) {
        return atoi(token.value);
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
        } else {
            return get_variable_value(token.value);
        }
    } else if (strcmp(token.type, "STRING") == 0) {
        printf("%s\n", token.value);
        return 0;
    } else if (strcmp(token.type, "PRINT") == 0 || strcmp(token.type, "RESOLVER") == 0) {
        int result = evaluate_expression();
        if (strcmp(token.type, "PRINT") == 0) {
            printf("%d\n", result);
        }
        return result;
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

void define_function() {
    if (function_count >= MAX_FUNCTIONS) {
        printf("Error: Too many functions\n");
        exit(1);
    }
    Function* func = &functions[function_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
        printf("Error: Expected function name\n");
        exit(1);
    }
    strcpy(func->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0 || strcmp(tokens[current_token].value, "(") != 0) {
        printf("Error: Expected '(' after function name\n");
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
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        printf("Error: Expected 'then' after function declaration\n");
        exit(1);
    }
    current_token++;
    func->body_token_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        func->body[func->body_token_count++] = tokens[current_token];
        current_token++;
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of function\n");
        exit(1);
    }
    current_token++;
}

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

void interpret() {
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "FUNC") == 0) {
            current_token++;
            define_function();
        } else if (strcmp(tokens[current_token].type, "IF") == 0) {
            interpret_if_statement();
        } else {
            evaluate_expression();
        }
    }
}

int main() {
    FILE * fp = fopen("main.ty","r");
    if(fp == NULL){
    	perror("");
    	return -1;
    }
    char code[1024];
    fread(code, sizeof(char), 1024, fp);
    fclose(fp);
    tokenize(code);
    interpret();
    return 0;
}
