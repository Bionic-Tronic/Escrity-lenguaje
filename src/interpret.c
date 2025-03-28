#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "../include/interpret.h"

Token tokens[MAX_TOKENS];
int token_count = 0;
int current_token = 0;
int current_line = 1;

Variable variables[MAX_VARIABLES];
int variable_count = 0;

Function functions[MAX_FUNCTIONS];
int function_count = 0;

Struct structs[MAX_STRUCTS];
int struct_count = 0;

struct Array arrays[MAX_ARRAY_SIZE];
int array_count = 0;

Enum enums[MAX_ENUMS];
int enum_count = 0;

Properties pro[MAX_PROPERTIES];
int properties_count;

Variable* get_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            if (variables[i].is_wait == 1)
            	return NULL;
            return &variables[i];
        }
    }
    return NULL;
}

void show_errors (const char * msg, int code){
	printf("\033[31m");
	printf("Error: %s in line: %i\n",msg,current_line);
	printf("\033[0m");
	exit(code);
}

void expect_token(const char *expected_type, const char *expected_value) {
    if (current_token >= token_count) {
        show_errors("Error: ", -1);
    }
    if (strcmp(tokens[current_token].type, expected_type) != 0 ||
        strcmp(tokens[current_token].value, expected_value) != 0) {
        printf("\033[31m");
        printf("Error: Expeced '%s' '%s'\n",
                expected_value, tokens[current_token].value);
        printf("\033[0m");
        exit(1);
    }
    
}

int get_type_variable (const char * name){
        for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            if(strcmp(variables[i].type,"int") == 0)
                return 105;
            else if(strcmp(variables[i].type,"float") == 0)
                return 102;
            else if(strcmp(variables[i].type,"string") == 0)
                return 115;
            else if(strcmp(variables[i].type,"char") == 0)
                return 99;
            else if(strcmp(variables[i].type,"function") == 0)
                return 219;
            else if(strcmp(variables[i].type,"array") == 0)
                return 97;
        }
    }
    return -1;
}

int search_word (String word, String texto){
    string text_copy = strdup(word);
    if (text_copy == NULL)
        return -1;
    string line = strtok(text_copy, "\n");
    while (line != NULL){
        if (strstr(line, texto) != NULL){
            free(text_copy);
            return 1;
        }
        line = strtok(NULL, "\n");
    }
    free(text_copy);
    return -1;
}

void set_variable(const char* name, const char* type, void* value) {
    Variable* var = get_variable(name);
    if (var == NULL) {
        if (variable_count >= MAX_VARIABLES)
            show_errors ("Too many variables",-1);
        var = &variables[variable_count++];
        strcpy(var->name, name);
    }
    strcpy(var->type, type);
    var->is_wait = 0;
    if (strcmp(type, "int") == 0) {
        var->value.int_value = *(int*)value;
    } else if (strcmp(type, "float") == 0) {
        var->value.float_value = *(double*)value;
    } else if (strcmp(type, "string") == 0) {
        strcpy(var->value.string_value, (char*)value);
    } else if (strcmp(type, "char") == 0) {
        var->value.char_value = (char)value;
    } else if (strcmp(type, "function") == 0) {
        var->value.func_value = (Function*)value;
    } else if (strcmp(type, "array") == 0) {
        var->value.array_value = (Array*)value;
    } else if (strcmp(type, "bool") == 0) {
        int x = (int)value;
        if (x < 0 || x > 1)
        	x = 0;
        var->value.bool_value = x;
    } else if (strcmp(type, "wait") == 0) {
        var->is_wait = 1;
        var->value.wait_value = 0;
    } else if (strcmp(type, "empty") == 0) {
        var->value.int_value = 0;
    }
}

Function* get_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0)
            return &functions[i];
    }
    return NULL;
}


void error_funcs_args (const char * name, int params, int args){
	printf("\033[31m");
	printf("Error: Function '%s' expects %d arguments, but got %d\n", name, params, args);
    printf("\033[0m");
    exit(1);
}

int call_function(Function* func, void * args[], int arg_count) {
    char * type = "int";
    if (arg_count != func->param_count)
        error_funcs_args(func->name, func->param_count, arg_count);
    Variable saved_vars[MAX_VARIABLES];
    int saved_var_count = variable_count;
    memcpy(saved_vars, variables, sizeof(variables));
    for (int i = 0; i < func->param_count; i++) {
        set_variable(func->params[i], type, &args[i]);
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

Struct* get_struct(const char* name) {
    for (int i = 0; i < struct_count; i++) {
        if (strcmp(structs[i].name, name) == 0) {
            return &structs[i];
        }
    }
    return NULL;
}

Properties * get_properties (const char* name){
	for (int i = 0; i < struct_count; i++) {
        if (strcmp(pro[i].name, name) == 0) {
            return &pro[i];
        }
    }
    return NULL;
}

void define_struct() {
    if (struct_count >= MAX_STRUCTS)
        show_errors ("Too many objects",-1);
    Struct* s = &structs[struct_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
        show_errors ("Expeced object name",-1);
    strcpy(s->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "DECLARE") != 0)
        show_errors ("Expeced 'declare' after object name",-1);
    current_token++;
    s->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
            show_errors ("Expeced member name in object",-1);
        strcpy(s->members[s->member_count].name, tokens[current_token].value);
        current_token++;
        if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0)
            show_errors ("Expeced ':' after member name in object",-1);
        current_token++;
        if (current_token >= token_count)
            show_errors ("Ecpeced member value in object",-1);
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
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].value, "_") != 0)
                show_errors ("Expeced '_' for anonymous function in object",-1);
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0 || strcmp(tokens[current_token].value, "(") != 0)
                show_errors ("Expeced '(' after '_' in object",-1);
            current_token++;
            func->param_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                    show_errors ("Expeced parameter name in function a object",-1);
                strcpy(func->params[func->param_count++], tokens[current_token].value);
                current_token++;
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
                show_errors ("Expeced ')' after function parameter in function object",-1);
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
                show_errors ("Expeced then after function declaration a function object",-1);
            current_token++;
            func->body_token_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                func->body[func->body_token_count++] = tokens[current_token];
                current_token++;
            }
            if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
                show_errors ("Expected 'end' at then end of function a function object",-1);
        } else
            show_errors ("Invalid member value in object",-1);
        current_token++;
        s->member_count++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of object",-1);
    current_token++;
}

void define_properties() {
    if (properties_count >= MAX_PROPERTIES)
        show_errors ("Too many properties",-1);
    Properties * s = &pro[properties_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
        show_errors ("Expected properties name",-1);
    strcpy(s->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expected 'then' after properties name",-1);
    current_token++;
    s->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type,"ACESS") == 0) {
        	if (strcmp(tokens[current_token].value,"priv") == 0)
        		s->visible[s->member_count] = 0;
        	else if (strcmp(tokens[current_token].value,"pub") == 0)
        		s->visible[s->member_count] = 1;
        	else
        		show_errors ("Expected 'priv' or 'pub' a member properties",-1);
        	current_token++;
        } else
        	show_errors ("Expected a visivility member of properties",-1);
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
            show_errors ("Expected member name in properties",-1);
        strcpy(s->members[s->member_count].name, tokens[current_token].value);
        current_token++;
        if (current_token >= token_count || strcmp(tokens[current_token].type, "AT") != 0)
            show_errors ("Expected 'at' after member name in properties",-1);
        current_token++;
        if (current_token >= token_count)
            show_errors ("Expected member value in properties",-1);
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
        } else if (strcmp(tokens[current_token].type, "PAREN") == 0) {
            strcpy(s->members[s->member_count].type, "function");
            Function* func = &functions[function_count++];
            s->members[s->member_count].value.func_value = func;
            current_token++;
            func->param_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                    show_errors ("Expected parameter name in properties",-1);
                strcpy(func->params[func->param_count++], tokens[current_token].value);
                current_token++;
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                    current_token++;
                }
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
                show_errors ("Expected ')' after function parameters in properties",-1);
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
                show_errors ("Exoected 'then' after fubction declaration in properties",-1);
            current_token++;
            func->body_token_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                func->body[func->body_token_count++] = tokens[current_token];
                current_token++;
            }
            if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
                show_errors ("Expected 'end' at the end of function in properties",-1);
        } else {
            strcpy(s->members[s->member_count].type, "int");
            s->members[s->member_count].value.char_value = 0;
        }
        current_token++;
        s->member_count++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of properties",-1);
    current_token++;
}

void interpret_if_statement() {
    current_token++;
    int condition = evaluate_expression();
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expected 'then' after if condition",-1);
    current_token++;
    if (condition) {
    	while (current_token < token_count != 0 && strcmp(tokens[current_token].type, "END") != 0){
               evaluate_expression();
               if (strcmp(tokens[current_token].type, "ELSE") == 0)
                   current_token+=token_count;
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
    current_token++;
}

void interpret_while_loop() {
    current_token++;
    int condicion = evaluate_expression();
    while (condicion) {
        current_token++;
        while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
            evaluate_expression();
        }
        if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
            show_errors ("Expected 'end' at the end of while loop",-1);
        current_token = current_token - (token_count - 1);
    }
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0)
        current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of while loop",-1);
    current_token++;
}

void interpret_where_statement() {
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
        show_errors ("Expected variable name after where statement",-1);
    char var_name[MAX_TOKEN_LENGTH];
    strcpy(var_name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0)
        show_errors ("Expected 'then' after variable name in where statement",-1);
    current_token++;
    Variable* var = get_variable(var_name);
    if (var == NULL) {
        printf("\033[31m");
        printf("Error: Variable '%s' not defined\n", var_name);
        printf("\033[0m");
        exit(1);
    }
    int value;
    if (strcmp(var->type, "int") == 0) {
        value = var->value.int_value;
    } else if (strcmp(var->type, "float") == 0) {
        value = (var->value.float_value);
    } else if (strcmp(var->type, "char") == 0) {
        value = var->value.char_value;
    } else if (strcmp(var->type, "enum") == 0) {
        value = var->value.enum_value;
    } else {
        printf("Error: Cannot use variable '%s' of type '%s' in where statement\n", var_name, var->type);
        exit(1);
    }
    bool case_matched = false;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "CASE") == 0) {
            current_token++;
            int case_value = evaluate_expression();
            if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0)
                show_errors ("Expected ':' after case value",-1);
            current_token++;
            if (value == case_value && !case_matched) {
                case_matched = true;
                while (current_token < token_count && strcmp(tokens[current_token].type, "STOP") != 0 &&
                       strcmp(tokens[current_token].type, "CASE") != 0 && strcmp(tokens[current_token].type, "DEFAULT") != 0 &&
                       strcmp(tokens[current_token].type, "END") != 0) {
                    evaluate_expression();
                    interpret_if_statement();
                }
            } else {
                while (current_token < token_count && strcmp(tokens[current_token].type, "STOP") != 0 &&
                       strcmp(tokens[current_token].type, "CASE") != 0 && strcmp(tokens[current_token].type, "DEFAULT") != 0 &&
                       strcmp(tokens[current_token].type, "END") != 0) {
                    current_token++;
                }
            }

            if (current_token < token_count && strcmp(tokens[current_token].type, "STOP") == 0) {
                current_token++;
            }
        } else if (strcmp(tokens[current_token].type, "DEFAULT") == 0) {
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "COLON") != 0)
                show_errors ("Expected ':' after default",-1);
            current_token++;
            if (!case_matched) {
                while (current_token < token_count && strcmp(tokens[current_token].type, "STOP") != 0 &&
                       strcmp(tokens[current_token].type, "END") != 0) {
                    evaluate_expression();
                    interpret_if_statement();
                }
            } else {
                while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
                    current_token++;
                }
            }
            if (current_token < token_count && strcmp(tokens[current_token].type, "STOP") == 0) {
                current_token++;
            }
        } else {
            printf("Error: Unexpected token '%s' in where statement\n", tokens[current_token].type);
            exit(1);
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0)
        show_errors ("Expected 'end' at the end of where statatement",-1);
    current_token++;
}

void define_for () {
}
void define_array_for () {
}

void interpret_for_loop() {
    current_token++;
    if (strcmp(tokens[current_token].type,"PAREN") != 0) {
    	show_errors("Expected '(' at for statement",-1);
    }
    current_token++;
    evaluate_expression();
    if (strcmp(tokens[current_token].type,"AT") != 0) {
    	show_errors("Expected 'at' at for statement",-1);
    }
    current_token++;
    while(evaluate_expression()) {
    	if (strcmp(tokens[current_token].type,"AT") != 0) {
    		puts("Error: Expected 'at' at for statement");
    	    exit(-1);
    	}
    	current_token++;
    	int loop_start = current_token;
    	int paren_count = 1;
    	while(paren_count > 0 && current_token < token_count) {
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
            while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0){
                evaluate_expression();
            }
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
    current_token++;
}

Array* get_array(const char* name) {
    for (int i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) {
            return &arrays[i];
        }
    }
    return NULL;
}


void define_classic_array () {
}
void define_new_array () {
}

void define_array() {
    if (array_count >= MAX_ARRAY_SIZE) {
        show_errors ("",-1);
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
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        printf("Error: Expected 'then' after array name\n");
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
        if (current_token >= token_count || strcmp(tokens[current_token].type, "IS") != 0) {
            printf("Error: Expected 'is' after member name\n");
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
        } else if (strcmp(tokens[current_token].type, "ARRAY") == 0) {
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

void add_code_file (){
	char archivo[MAX_STR];
	if(strcmp(tokens[current_token].type,"PAREN") != 0)
		show_errors("Expected '(' at add_code function", -1);
	current_token++;
	if(strcmp(tokens[current_token].type,"STRING") != 0)
		show_errors("Expected 'string' at add_code function", -1);
	removeQuotes(tokens[current_token].value, archivo);
	current_token++;
	if(strcmp(tokens[current_token].type,"PAREN") != 0)
		show_errors("Expected ')' at add_code function", -1);
	FILE * fp = fopen(archivo,"r");
	if (fp == NULL)
		show_errors("Error a read file and add this a code", -1);
	struct stat st;
	int s = 0;
    if (stat(archivo, &st) == 0)
         s = st.st_size*2;
    if (s == 0)
    	show_errors("File is empty or invalid for read",-1);
    printf("");
	char buffer[s];
	fread(buffer, sizeof(char), s, fp);
	tokenize(buffer);
	fclose(fp);
	current_token++;
}


void interpret() {
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "FUNC") == 0) {
            current_token++;
            define_function();
        } else if (strcmp(tokens[current_token].type, "IF") == 0 || strcmp(tokens[current_token].type, "ELSE") == 0) {
            interpret_if_statement();
        } else if (strcmp(tokens[current_token].type, "FOR") == 0) {
            interpret_for_loop();
        } else if (strcmp(tokens[current_token].type, "WHILE") == 0) {
            interpret_while_loop();
        } else if (strcmp(tokens[current_token].type, "STRUCT") == 0) {
            current_token++;
            define_struct();
        } else if (strcmp(tokens[current_token].type, "PROPERTIES") == 0) {
            current_token++;
            define_properties();
        } else if (strcmp(tokens[current_token].type, "ENUM") == 0) {
            current_token++;
            define_enum();
        } else if (strcmp(tokens[current_token].type, "WHERE") == 0) {
            interpret_where_statement();
        } else if (strcmp(tokens[current_token].type, "ARRAY") == 0) {
            current_token++;
            define_array();
        } else if (strcmp(tokens[current_token].type, "ADD_CODE") == 0) {
            current_token++;
            add_code_file();
        } else {
            evaluate_expression();
        }
    }
}