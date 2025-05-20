#include "../include/interpret.h"
#include "../include/error.h"
#include "../include/array.h"
#include "../include/variable.h"
#include "../include/enum.h"
#include "../include/protocol.h"
#include "../include/properties.h"
#include "../include/object.h"
#include "../include/function.h"

Token tokens[MAX_TOKENS];
int token_count = 0;
int current_token = 0;
int current_line = 1;
char * type_value_return = "int";
char * error_msg = "none";

Protocol protocol[MAX_VARIABLES];
int protocol_count = 0;

Variable variables[MAX_VARIABLES];
int variable_count = 0;

Function functions[MAX_FUNCTIONS];
int function_count = 0;

Object objects[MAX_OBJECTS];
int object_count = 0;

Array arrays[MAX_ARRAY_SIZE];
int array_count = 0;

Enum enums[MAX_ENUMS];
int enum_count = 0;

Properties pro[MAX_PROPERTIES];
int properties_count = 0;

char buffer[BUFFER_SIZE];
char buff[BUFFER_SIZE];
char http_buffer[BUFFER_SIZE];

Values values[MAX_VALUES];
int values_count = 0;

Vector vec[MAX_ARRAY_SIZE];
int vector_count = 0;

bool verify_token_value (const char * token){
  if (strcmp(tokens[current_token].value,token) == 0)
    return true;
  else
    return false;
}

bool verify_token (const char * token){
  if (strcmp(tokens[current_token].type,token) == 0)
    return true;
  else
    return false;
}

char * get_token_type (){
  return tokens[current_token].type;
}

char * get_token_value (){
  return tokens[current_token].value;
}

int get_int_token_value (){
  return atoi(tokens[current_token].value);
}
float get_float_token_value (){
  return atof(tokens[current_token].value);
}
char get_char_token_value (){
  return tokens[current_token].value[1];
}

char * atos (int i) {
  char *str = (char *)malloc(20 * sizeof(char));
  if (str != NULL)
    sprintf(str, "%d", i);
  return str;
}

void interpret_if_statement() {
    current_token++;
    int condition = (int)evaluate_expression();
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
    void * value;
    if (strcmp(var->type, "int") == 0) {
        value = &var->value.int_value;
    } else if (strcmp(var->type, "float") == 0) {
        value = &var->value.float_value;
    } else if (strcmp(var->type, "char") == 0) {
        value = &var->value.char_value;
    } else if (strcmp(var->type, "bool") == 0) {
        value = &var->value.bool_value;
    } else if (strcmp(var->type, "enum") == 0) {
        value = &var->value.enum_value;
    } else if (strcmp(var->type, "string") == 0) {
        value = &var->value.string_value;
    } else {
        printf("Error: Cannot use variable '%s' of type '%s' in where statement\n", var_name, var->type);
        exit(1);
    }
    bool case_matched = false;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "CASE") == 0) {
            current_token++;
            void * case_value = evaluate_expression();
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

void update_variable(const char *name, const char *type, void *value) {
    Variable *var = get_variable(name);
    if (!var) invalid_var(name, ERROR_SINTAXIS);
    if (strcmp(type, "int") == 0) {
        var->value.int_value = *(int*)value;
    } else if (strcmp(type, "float") == 0) {
        var->value.float_value = *(float*)value;
    } else if (strcmp(type, "string") == 0) {
        var->value.float_value = *(char*)value;
    } else if (strcmp(type, "char") == 0) {
        var->value.float_value = *(char*)value;
    }
}

int find_matching_end(int start) {
    int depth = 1;
    while (start < token_count) {
        if (strcmp(tokens[start].type, "FOR") == 0) depth++;
        if (strcmp(tokens[start].type, "END") == 0) {
            depth--;
            if (depth == 0) return start;
        }
        start++;
    }
    show_errors("Unclosed loop", start);
    return -1;
}

void interpret_for_loop() {
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0) {
        show_errors("Expected '(' after 'for'", current_token);
    }
    current_token++;
    evaluate_expression();
    if (current_token >= token_count || strcmp(tokens[current_token].type, "AT") != 0) {
        show_errors("Expected 'at' after initialization", current_token);
    }
    current_token++;
    int cond_pos = current_token;
    evaluate_expression();
    if (current_token >= token_count || strcmp(tokens[current_token].type, "AT") != 0) {
        show_errors("Expected 'at' after condition", current_token);
    }
    current_token++;
    int update_pos = current_token;
    if (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "INCREMENT") == 0) {
            current_token++;
        }
        else if (strcmp(tokens[current_token].type, "DECREMENT") == 0) {
            current_token++;
        }
        else if (strcmp(tokens[current_token].type, "COMPOUND_OP") == 0) {
            current_token += 2;
        }
        else {
            evaluate_expression();
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "PAREN") != 0) {
        show_errors("Expected ')' after update expression", current_token);
    }
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        show_errors("Expected 'then' after loop header", current_token);
    }
    current_token++;
    int body_start = current_token;
    int depth = 1;
    int end_pos = -1;
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "FOR") == 0) depth++;
        if (strcmp(tokens[current_token].type, "END") == 0) {
            depth--;
            if (depth == 0) {
                end_pos = current_token;
                break;
            }
        }
        current_token++;
    }
    if (end_pos == -1) show_errors("Unclosed 'for' loop", current_token);
    while (1) {
        int saved_pos = current_token;
        current_token = cond_pos;
        int condition = (int)evaluate_expression();
        current_token = saved_pos;
        if (!condition) break;
        int body_current = body_start;
        while (body_current < end_pos) {
            current_token = body_current;
            evaluate_expression();
            body_current = current_token;
        }
        saved_pos = current_token;
        current_token = update_pos;
        if (strcmp(tokens[current_token].type, "INCREMENT") == 0) {
            char* var_name = tokens[current_token-1].value;
            Variable* var = get_variable(var_name);
            var->value.int_value++;
            current_token++;
        }
        else if (strcmp(tokens[current_token].type, "DECREMENT") == 0) {
            char* var_name = tokens[current_token-1].value;
            Variable* var = get_variable(var_name);
            var->value.int_value--;
            current_token++;
        }
        else if (strcmp(tokens[current_token].type, "COMPOUND_OP") == 0) {
            char* var_name = tokens[current_token-1].value;
            char* op = tokens[current_token].value;
            int value = atoi(tokens[current_token+1].value);
            Variable* var = get_variable(var_name);
            if (strcmp(op, "+=") == 0) var->value.int_value += value;
            else if (strcmp(op, "-=") == 0) var->value.int_value -= value;
            else if (strcmp(op, "*=") == 0) var->value.int_value *= value;
            else if (strcmp(op, "/=") == 0) var->value.int_value /= value;
            current_token += 2;
        }
        else {
            evaluate_expression();
        }
        current_token = saved_pos;
    }
    current_token = end_pos + 1;
}

void interpret_each_loop() {
    current_token++;
    if (current_token >= token_count || !verify_token("PAREN")) {
        show_errors("Expected '(' after 'each'", current_token);
    }
    current_token++;
    if (current_token >= token_count || !verify_token("IDENTIFIER")) {
        show_errors("Expected iterator variable name", current_token);
    }
    char var_name[MAX_TOKEN_LENGTH];
    strncpy(var_name, get_token_value(), MAX_TOKEN_LENGTH);
    current_token++;
    if (current_token >= token_count || !verify_token("IS")) {
        show_errors("Expected 'is' before array name", current_token);
    }
    current_token++;
    if (current_token >= token_count || !verify_token("IDENTIFIER")) {
        show_errors("Expected array name", current_token);
    }
    char arr_name[MAX_TOKEN_LENGTH];
    strncpy(arr_name, get_token_value(), MAX_TOKEN_LENGTH);
    current_token++;
    if (current_token >= token_count || !verify_token("PAREN")) {
        show_errors("Expected ')' after array name", current_token);
    }
    current_token++;
    if (current_token >= token_count || !verify_token("THEN")) {
        show_errors("Expected 'then' after loop header", current_token);
    }
    current_token++;
    Array * arr = get_array(arr_name);
    if (!arr) show_errors("Array not defined", current_token);
    //set_variable(var_name, arr->type, 0);
    int body_start = current_token;
    int end_pos = find_matching_end(current_token);
    for (int i = 0; i < arr->member_count; i++) {
        update_variable(var_name, arr->type, &arr->members[i].value);
        current_token = body_start;
        while (current_token < end_pos) {
            evaluate_expression();
            //interpret();
        }
    }
    current_token = end_pos + 1;
}

void interpret_while_loop() {
    current_token++;
    int cond_start = current_token;
    int then_pos = -1;
    int end_pos = -1;
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "THEN") == 0) {
            then_pos = current_token;
            break;
        }
        current_token++;
    }
    if (then_pos == -1) show_errors("Expected 'then' after condition", cond_start);
    current_token = then_pos + 1;
    int depth = 1;
    while (current_token < token_count) {
        if (strcmp(tokens[current_token].type, "WHILE") == 0) depth++;
        if (strcmp(tokens[current_token].type, "END") == 0) {
            depth--;
            if (depth == 0) {
                end_pos = current_token;
                break;
            }
        }
        current_token++;
    }
    if (end_pos == -1) show_errors("Unclosed while loop", then_pos);
    while (1) {
        current_token = cond_start;
        int condition = (int)evaluate_expression();
        if (current_token != then_pos)
            show_errors("Condition must end before 'then'", current_token);
        if (!condition) break;
        current_token = then_pos + 1;
        while (current_token < end_pos) {
            evaluate_expression();
        }
    }
    current_token = end_pos + 1;
}

void add_code_file (){
	char archivo[MAX_TOKEN_LENGTH];
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
    	show_errors("File is NULL or invalid for read",-1);
    printf("");
	char buffer[s];
	fread(buffer, sizeof(char), s, fp);
	tokenize(buffer);
	fclose(fp);
	current_token++;
}

void interpret() {
  while (current_token < token_count) {
    if (verify_token("FUNC")) {
      current_token++;
      define_function();
    } else if (verify_token("IF") || verify_token("ELSE")) {
      interpret_if_statement();
    } else if (verify_token("FOR")) {
      interpret_for_loop();
    } else if (verify_token("EACH")) {
      interpret_each_loop();
    } else if (verify_token("WHILE")) {
      interpret_while_loop();
    } else if (verify_token("OBJECT")) {
      current_token++;
      define_object();
    } else if (verify_token("PROPERTIES")) {
      current_token++;
      define_properties();
    } else if (verify_token("ENUM")) {
      current_token++;
      define_enum();
    } else if (verify_token("WHERE")) {
      interpret_where_statement();
    } else if (verify_token("MODULE")) {
      current_token++;
      add_code_file();
    } else if (verify_token("VALUES")) {
      current_token++;
      define_values();
    } else if (verify_token("VECTOR")) {
      current_token++;
      //define_vector();
    } else if (verify_token("PROTOCOL")) {
      current_token++;
      define_protocol();
    } else if (verify_token("CREATE_ARRAY")) {
      current_token++;
      define_array();
    } else {
      evaluate_expression();
    }
  }
}

