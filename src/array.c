#include "../include/array.h"
#include "../include/error.h"

Array * get_array (const char * name) {
  for (int i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) {
            return &arrays[i];
        }
  }
    return NULL;
}

void define_array (){
	int max_elements;
	char * param;
	if (array_count >= MAX_ARRAY_SIZE)
		show_errors("Too many arrays", ERROR_INTERPRETER);
	Array * arr = &arrays[array_count];
	expected("(","create_array","L_PAREN");
	if (verify_token("STRING") == true){
		char str[MAX_TOKEN_LENGTH];
		removeQuotes(get_token_value(), str);
		strcpy(arr->name, str);
	} else if (verify_token("IDENTIFIER")){
		Variable * var = get_variable(get_token_value());
		if (var == NULL){
			strcpy(arr->name, get_token_value());
		} else
			simple_error_msg("Invalid name for array. this name in use",ERROR_INTERPRETER);
	} else
		simple_error_msg("Invalid name for array",ERROR_INTERPRETER);
	current_token++;
	expected(",","create_array","COMMA");
	if (verify_token("NUMBER") == true){
		max_elements = get_int_token_value();
	} else if (verify_token("IDENTIFIER")){
		Variable * var = get_variable(get_token_value());
		if (var != NULL){
			if (strcmp(var->type,"int") == 0){
				max_elements = var->value.int_value;
			} else
			      invalid_var(get_token_value(), ERROR_SINTAXIS);
		}
	} else
		show_errors("Expected a 'int' value in size_array", ERROR_INTERPRETER);
	current_token++;
	expected(",","create_array","COMMA");
	if (verify_token("STRING_PARAM") == true){
		param = "string";
	} else if (verify_token("INTEGER_PARAM") == true){
		param = "int";
	} else if (verify_token("FLOAT_PARAM") == true){
		param = "float";
	} else if (verify_token("CHAR_PARAM") == true){
		param = "char";
	} else if (verify_token("BOOL_PARAM") == true){
		param = "bool";
	} else {
		simple_error_msg("Invalid type param", ERROR_SINTAXIS);
	}
	strcpy(arr->type,param);
	for (int i = 0; i < max_elements; i++){
		strcpy(arr->members[i].name, atos(i));
		strcpy(arr->members[i].type, param);
		if (strcmp(param,"string") == 0)
			strcpy(arr->members[i].value.string_value, "(empty)");
		else if (strcmp(param,"int") == 0)
			arr->members[i].value.int_value = -1;
		else if (strcmp(param,"float") == 0)
			arr->members[i].value.float_value = 0;
		else if (strcmp(param,"char") == 0)
			arr->members[i].value.char_value = 'a';
		else if (strcmp(param,"bool") == 0)
			arr->members[i].value.bool_value = false;
		arr->member_count++;
	}
	current_token++;
	expected(")","create_array","R_PAREN");
	array_count++;
}
void * acess_array (){
current_token--;
			char * name = get_token_value();
			current_token+=2;
			char * index;
			int concidencia;
			if (verify_token("NUMBER")){
				index = get_token_value();
			} else if (verify_token("IDENTIFIER")){
				Variable * var = get_variable(get_token_value());
				if (var != NULL){
					if (strcmp(var->type,"int") != 0)
						notEvaluateVariable(get_token_value(), var->type, ERROR_INTERPRETER);
					index = atos(var->value.int_value);
				} else
					invalid_var(get_token_value(), ERROR_SINTAXIS);
			} else
				simple_error_msg("array index is not an integer",ERROR_SINTAXIS);
			void * value;
			Array * a = get_array(name);
			if (a != NULL) {
				int int_index = atoi(index);
				if (int_index > a->member_count)
					simple_error_msg("array index 'index' is past the end of the array (which contains '9' elements)", ERROR_INTERPRETER);
				for (int i = 0; i < a->member_count; i++){
					if (strcmp(a->members[i].name,index) == 0){
						if (strcmp(a->members[i].type, "int") == 0){
							concidencia = i;
							value = &a->members[i].value.int_value;
							type_value_return = "int";
							break;
						} else if (strcmp(a->members[i].type, "float") == 0){
							value = &a->members[i].value.float_value;
							type_value_return = "float";
							break;
						} else if (strcmp(a->members[i].type, "string") == 0){
							value = &a->members[i].value.string_value;
							type_value_return = "string";
							break;
						} else if (strcmp(a->members[i].type, "char") == 0){
							value = &a->members[i].value.char_value;
							type_value_return = "char";
							break;
						} else if (strcmp(a->members[i].type, "bool") == 0){
							value = (void *)a->members[i].value.bool_value;
							type_value_return = "bool";
							break;
						} else {
							show_errors("Invalid type in array", UNDEFINED_ERROR);
						}
					}
				}
				current_token++;
				expected("]", "array not","BRACKET");
				if (verify_token("OPERATOR") == true){
					if (verify_token_value("=") == true){
						current_token++;
						if (strcmp(a->type,"int") == 0){
							if (verify_token("NUMBER") == true){
								a->members[concidencia].value.int_value = atoi(get_token_value());
								return value;
							} else if (verify_token("IDENTIFIER")){
								Variable * var = get_variable(get_token_value());
								if (var != NULL){
					                 if (strcmp(var->type,"int") == 0){
					                      a->members[concidencia].value.int_value = var->value.int_value;
					                      return 0;
					                 }
					            notEvaluateVariable(get_token_value(), var->type, ERROR_INTERPRETER);
				                } else
				                     invalid_var(get_token_value(), ERROR_SINTAXIS);
							}
						} else if (strcmp(a->type,"float") == 0){
							if (verify_token("FLOAT") == true){
								a->members[concidencia].value.float_value = atof(get_token_value());
								return 0;
							} else if (verify_token("IDENTIFIER")){
								Variable * var = get_variable(get_token_value());
								if (var != NULL){
					                 if (strcmp(var->type,"float") == 0){
					                      a->members[concidencia].value.float_value = var->value.float_value;
					                      return 0;
					                 }
					            notEvaluateVariable(get_token_value(), var->type, ERROR_INTERPRETER);
				                } else
				                     invalid_var(get_token_value(), ERROR_SINTAXIS);
							}
						} else if (strcmp(a->type,"char") == 0){
							if (verify_token("CHAR") == true){
								a->members[concidencia].value.char_value = get_char_token_value();
								return 0;
							} else if (verify_token("IDENTIFIER")){
								Variable * var = get_variable(get_token_value());
								if (var != NULL){
					                 if (strcmp(var->type,"char") == 0){
					                      a->members[concidencia].value.char_value = var->value.char_value;
					                      return 0;
					                 }
					            notEvaluateVariable(get_token_value(), var->type, ERROR_INTERPRETER);
				                } else
				                     invalid_var(get_token_value(), ERROR_SINTAXIS);
							}
						} else if (strcmp(a->type,"string") == 0){
							if (verify_token("STRING") == true){
								char str[MAX_TOKEN_LENGTH];
								removeQuotes(get_token_value(), str);
								strcpy(a->members[concidencia].value.string_value, str);
								return 0;
							} else if (verify_token("IDENTIFIER")){
								Variable * var = get_variable(get_token_value());
								if (var != NULL){
					                 if (strcmp(var->type,"string") == 0){
					                      strcpy(a->members[concidencia].value.string_value, var->value.string_value);
					                      return 0;
					                 }
					            notEvaluateVariable(get_token_value(), var->type, ERROR_INTERPRETER);
				                } else
				                     invalid_var(get_token_value(), ERROR_SINTAXIS);
							}
						}  else if (strcmp(a->type,"bool") == 0){
							if (verify_token("TRUE") == true){
								a->members[concidencia].value.float_value = atoi(get_token_value());
								return 0;
							} else if (verify_token("IDENTIFIER")){
								Variable * var = get_variable(get_token_value());
								if (var != NULL){
					                 if (strcmp(var->type,"bool") == 0){
					                      a->members[concidencia].value.float_value = var->value.float_value;
					                      return 0;
					                 }
					            notEvaluateVariable(get_token_value(), var->type, ERROR_INTERPRETER);
				                } else
				                     invalid_var(get_token_value(), ERROR_SINTAXIS);
							}
						}
					} else {
						show_errors("Expected '=' in array declaration",ERROR_SINTAXIS);
					}
				}
				return value;
			} else {
				show_errors("Array not defined", ERROR_INTERPRETER);
			}
}

