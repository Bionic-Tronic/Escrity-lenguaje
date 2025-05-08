#include "../include/function.h"
#include "../include/variable.h"

void define_function() {
    if (function_count >= MAX_FUNCTIONS)
        show_errors ("Too many functions",-1);
    Function* func = &functions[function_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
        show_errors ("Expeced function name",-1);
    strcpy(func->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0 || strcmp(tokens[current_token].value, "(") != 0)
        show_errors ("Expeced '(' after function name",-1);
    current_token++;
    func->param_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
            show_errors ("Expected parameter name",-1);
        strcpy(func->params[func->param_count++], tokens[current_token].value);
        current_token++;
        if (strcmp(tokens[current_token].type, "COLON") != 0)
            show_errors ("Expected ':' for type variable",-1);
        current_token++;
        if (strcmp(tokens[current_token].type,"STRING_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "string");
        }
        if (strcmp(tokens[current_token].type,"FLOAT_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "float");
        }
        if (strcmp(tokens[current_token].type,"INTEGER_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "int");
        }
        if (strcmp(tokens[current_token].type,"CHAR_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "char");
        }
        if (strcmp(tokens[current_token].type,"BOOL_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "bool");
        }
        if (strcmp(tokens[current_token].type,"WAIT_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "wait");
        }
        if (strcmp(tokens[current_token].type,"CONST_PARAM") == 0) {
        	strcpy(func->params_type[func->param_count], "const");
        }
        current_token++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
            current_token++;
    }
    if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
        show_errors ("Expeced ')' after function parameters",-1);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expeced 'then' after function declaration",-1);
    current_token++;
    func->body_token_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        func->body[func->body_token_count++] = tokens[current_token];
        current_token++;
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expeced 'end' at the end of function",-1);
    current_token++;
}

void * call_function(Function* func, void * args[], int arg_count) {
    if (arg_count != func->param_count)
        error_funcs_args(func->name, func->param_count, arg_count);
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
    void * result = 0;
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

Function* get_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0)
            return &functions[i];
    }
    return NULL;
}
