#ifndef FUNCTION_H
#define FUNCTION_H

struct Function {
    char name[MAX_TOKEN_LENGTH];
    char params[MAX_PARAMS][MAX_TOKEN_LENGTH];
    int param_count;
    Token body[MAX_BODY_TOKENS];
    int body_token_count;
};

Function functions[MAX_FUNCTIONS];
int function_count = 0;

int evaluate_expression();

Function* get_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0)
            return &functions[i];
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
    for (int i = 0; i < func->param_count; i++) {
        set_variable(func->params[i], "int", &args[i]);
    }
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

#endif
