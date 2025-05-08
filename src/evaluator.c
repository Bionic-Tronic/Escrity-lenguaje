#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/interpret.h"
#include "../include/protocol.h"
#include "../include/errors.h"
#include "../include/variable.h"
#include "../include/function.h"
#include "../include/object.h"
#include "../include/properties.h"
#include "../include/array.h"
#include "../include/enums.h"

#define MAX_STRING 1024

int verify_token_type(const char *token){
	if (strcmp(tokens[current_token].type, token) == 0)
		return 1;
	return 0;
}

bool verify_token_value (const char * token, int currentToken){
	if (strcmp(tokens[currentToken].value,token) == 0)
		return true;
    else
        return false;
}

bool verify_token (const char * token, int currentToken){
	if (strcmp(tokens[currentToken].type,token) == 0)
		return true;
    else
        return false;
}

double evaluate_term(){
	int left = evaluate_factor();
	while (current_token < token_count &&
		   strcmp(tokens[current_token].type, "OPERATOR") == 0 &&
		   (tokens[current_token].value[0] == '*' || tokens[current_token].value[0] == '/')){
		char op = tokens[current_token].value[0];
		current_token++;
		int right = evaluate_factor();
		if (op == '*')
			left *= right;
		else{
			if (right == 0){
				printf("Error: Division by zero\n");
				exit(1);
			}
			left /= right;
		}
	}
	return left;
}

void * evaluate_expression(){
	if (current_token + 2 < token_count &&
		strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
		strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
		tokens[current_token + 1].value[0] == '='){
		char var_name[MAX_TOKEN_LENGTH];
		strcpy(var_name, tokens[current_token].value);
		current_token += 2;
		char var_str[MAX_STR * 2];
		if (strcmp(tokens[current_token].type, "NUMBER") == 0){
			int int_v = atoi(tokens[current_token].value);
			set_variable(var_name, "int", &int_v);
		}
		else if (strcmp(tokens[current_token].type, "STRING") == 0){
			removeQuotes(tokens[current_token].value, var_str);
			set_variable(var_name, "string", var_str);
		}
		else if (strcmp(tokens[current_token].type, "CHAR") == 0){
			int char_ = tokens[current_token].value[1];
			set_variable(var_name, "char", &char_);
		}
		else if (strcmp(tokens[current_token].type, "FLOAT") == 0){
			double float_v = atof(tokens[current_token].value);
			set_variable(var_name, "float", &float_v);
		}
		else if (strcmp(tokens[current_token].type, "TRUE") == 0){
			int true_v = 1;
			set_variable(var_name, "bool", &true_v);
		}
		else if (strcmp(tokens[current_token].type, "FALSE") == 0){
			int false_v = 0;
			set_variable(var_name, "bool", &false_v);
		}
		else if (strcmp(tokens[current_token].type, "EMPTY") == 0)
			set_variable(var_name, "empty", NULL);
		else if (strcmp(tokens[current_token].type, "SERVER_HTTP") == 0)
			set_variable(var_name, "http_server", 0);
		else{
			void * bool_v = evaluate_expression();
			set_variable(var_name, "string", &bool_v);
		}
		void * v = evaluate_expression();
		return v;
	}
	int left = evaluate_term();
	while (current_token < token_count &&
		   strcmp(tokens[current_token].type, "OPERATOR") == 0 &&
		   (tokens[current_token].value[0] == '+' || tokens[current_token].value[0] == '-' ||
			strcmp(tokens[current_token].value, "==") == 0 || strcmp(tokens[current_token].value, "<") == 0 ||
			strcmp(tokens[current_token].value, ">") == 0 || strcmp(tokens[current_token].value, "<=") == 0 ||
			strcmp(tokens[current_token].value, ">=") == 0 || strcmp(tokens[current_token].value, "!=") == 0 ||
			strcmp(tokens[current_token].value, "is") == 0)){
		char op[3];
		strcpy(op, tokens[current_token].value);
		current_token++;
		int right = evaluate_term();
		if (strcmp(op, "+") == 0)
			left += right;
		else if (strcmp(op, "-") == 0)
			left -= right;
		else if (strcmp(op, "*") == 0)
			left *= right;
		else if (strcmp(op, "/") == 0)
			left /= right;
		else if (strcmp(op, "==") == 0)
			left = (right == left);
		else if (strcmp(op, "<") == 0)
			left = (left < right);
		else if (strcmp(op, ">") == 0)
			left = (left > right);
		else if (strcmp(op, "<=") == 0)
			left = (left <= right);
		else if (strcmp(op, ">=") == 0)
			left = (left >= right);
		else if (strcmp(op, "!=") == 0)
			left = (left != right);
	}
	return left;
}

void * evaluate_factor(){
	Token token = tokens[current_token];
	current_token++;
	if (strcmp(token.type, "NUMBER") == 0)
		return atoi(token.value);
	else if (strcmp(token.type, "FLOAT") == 0)
		return atoi(token.value);
	else if (strcmp(token.type, "IDENTIFIER") == 0){
		if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0){
			Function *func = get_function(token.value);
			if (func == NULL){
				printf("Error: Function '%s' not defined\n", token.value);
				exit(1);
			}
			current_token++;
			int args[MAX_PARAMS];
			int arg_count = 0;
			while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0){
				args[arg_count++] = evaluate_expression();
				if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
					current_token++;
			}
			if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0){
				printf("Error: Expected ')' after function arguments\n");
				exit(1);
			}
			current_token++;
			return call_function(func, &args, arg_count);
		}
		else if (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") == 0 && strcmp(tokens[current_token].value, "[") == 0){
			Array *arr = get_array(token.value);
			if (arr == NULL){
				printf("Error: Array '%s' not defined\n", token.value);
				exit(1);
			}
			current_token++;
			if (current_token >= token_count || (strcmp(tokens[current_token].type, "STRING") != 0 && strcmp(tokens[current_token].type, "IDENTIFIER") != 0)){
				printf("Error: Expected string key or identifier for array access\n");
				exit(1);
			}
			char *key = tokens[current_token].value;
			current_token++;
			if (current_token >= token_count || strcmp(tokens[current_token].type, "BRACKET") != 0 || strcmp(tokens[current_token].value, "]") != 0){
				printf("Error: Expected ']' after array key\n");
				exit(1);
			}
			current_token++;
			return evaluate_array_access(arr, key);
		}
		else if (current_token < token_count && strcmp(tokens[current_token].type, "DOT") == 0){
			Struct *s = get_struct(token.value);
			Enum *e = get_enum(token.value);
			Properties *p = get_properties(token.value);
			Values *valu = get_values(token.value);
			ARRAY * arrr = get_ARRAY(token.value);
			Vector * vectori = get_vector(token.value);
            Protocol * protocolito = get_protocol(token.value);
			//Arreglos
			if (arrr != NULL){
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected array value after '.'\n");
					exit(1);
				}
			} else if (vectori != NULL){
				puts("Vector");
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected vector value after '.'\n");
					exit(1);
				}
				char * member_name = tokens[current_token].value;
				for (int i = 0; i < vectori->member_count; i++){
					if (strcmp(vectori->members[i].name, member_name) == 0){
						return vectori->members[i].value.int_value;
					}
				}
			}
                        //Protocolo
                        else if (protocolito != NULL){
                          current_token++;
                          if(current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER")){
                              show_errors("Expected member name after '.'",1);
                          }
                          char * member_name = tokens[current_token].value;
                          if(protocolito->type_protocol == 0){
                            evalue_http_protocol(protocolito,member_name);
                          } else {
                            show_errors("Undifined type protocol",ERROR_SINTAXIS);
                          }
                        }
			//Objects
			else if (s != NULL){
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected member name after '.'\n");
					exit(1);
				}
				char *member_name = tokens[current_token].value;
				current_token++;
				for (int i = 0; i < s->member_count; i++){
					if (strcmp(s->members[i].name, member_name) == 0){
						if (strcmp(s->members[i].type, "int") == 0)
							return s->members[i].value.int_value;
						else if (strcmp(s->members[i].type, "float") == 0)
							return (int)s->members[i].value.float_value;
						else if (strcmp(s->members[i].type, "char") == 0)
							return s->members[i].value.char_value;
						else if (strcmp(s->members[i].type, "function") == 0){
							Function *func = s->members[i].value.func_value;
							int args[MAX_PARAMS];
							int arg_count = 0;
							if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0){
								current_token++;
								while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0){
									args[arg_count++] = evaluate_expression();
									if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
										current_token++;
								}
								if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0){
									printf("Error: Expected ')' after function arguments\n");
									exit(1);
								}
								current_token++;
							}
							return call_function(func, &args, arg_count);
						}
						else{
							printf("Error: Cannot evaluate member '%s' of object '%s'\n", member_name, token.value);
							exit(1);
						}
					}
				}
				printf("Error: Member '%s' not found in object '%s'\n", member_name, token.value);
				exit(1);
			}
			//enums
			else if (e != NULL){
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected enum value after '.'\n");
					exit(1);
				}
				char *enum_value = tokens[current_token].value;
				current_token++;
				for (int i = 0; i < e->member_count; i++){
					if (strcmp(e->members[i], enum_value) == 0)
						return i;
				}
				printf("Error: Enum value '%s' not found in enum '%s'\n", enum_value, token.value);
				exit(1);
			}
			//Values
			else if (valu != NULL){
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected member name after '.'\n");
					exit(1);
				}
				char *member_name = tokens[current_token].value;
				current_token++;
				for (int i = 0; i < valu->member_count; i++){
					if (strcmp(valu->members[i].name, member_name) == 0){
						if (strcmp(valu->members[i].type, "int") == 0)
							return valu->members[i].value.int_value;
						else if (strcmp(s->members[i].type, "float") == 0)
							return (int)valu->members[i].value.float_value;
						else if (strcmp(s->members[i].type, "char") == 0)
							return valu->members[i].value.char_value;
						else{
							printf("Error: Cannot evaluate member '%s' of values '%s'\n", member_name, token.value);
							exit(1);
						}
					}
				}
				printf("Error: Member '%s' not found in values '%s'\n", member_name, token.value);
				exit(1);
			}
			//Properties
			else if (p != NULL){
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected member name after '.'\n");
					exit(1);
				}
				char *member_name = tokens[current_token].value;
				current_token++;
				for (int i = 0; i < p->member_count; i++){
					if (strcmp(p->members[i].name, member_name) == 0){
						if (p->visible[i] == 1){
							if (strcmp(tokens[current_token++].type, "IDENTIFIER") != 0){
								if (strcmp(tokens[current_token].type, "NUMBER") == 0){
									strcpy(p->members[i].type, "int");
									p->members[i].value.int_value = atoi(tokens[current_token].value);
								}
								else if (strcmp(tokens[current_token].type, "FLOAT") == 0){
									strcpy(p->members[i].type, "float");
									p->members[i].value.float_value = atof(tokens[current_token].value);
								}
								else if (strcmp(tokens[current_token].type, "STRING") == 0){
									strcpy(p->members[i].type, "string");
									strcpy(p->members[i].value.string_value, tokens[current_token].value);
								}
								else if (strcmp(tokens[current_token].type, "CHAR") == 0){
									strcpy(p->members[i].type, "char");
									p->members[i].value.char_value = tokens[current_token].value[1];
								}
							}
							if (current_token + 1 < token_count &&
								strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
								strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
								tokens[current_token + 1].value[0] == '='){
							}
							if (strcmp(p->members[i].type, "int") == 0)
								return p->members[i].value.int_value;
							else if (strcmp(p->members[i].type, "float") == 0)
								return (int)p->members[i].value.float_value;
							else if (strcmp(p->members[i].type, "char") == 0)
								return p->members[i].value.char_value;
							else if (strcmp(p->members[i].type, "function") == 0)
							{
								Function *func = p->members[i].value.func_value;
								void * args[MAX_PARAMS];
								int arg_count = 0;
								if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0){
									current_token++;
									while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0){
										args[arg_count++] = evaluate_expression();
										if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
											current_token++;
									}
									if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0){
										printf("Error: Expected ')' after function arguments\n");
										exit(1);
									}
									current_token++;
								}
								return call_function(func, &args, arg_count);
							}
							else{
								printf("Error: Cannot evaluate member '%s' of propeties '%s'\n", member_name, token.value);
								exit(1);
							}
						}
						else{
							printf("Error: Cannot access at member '%s' of properties '%s' this is a 'priv'\n", member_name, token.value);
							exit(1);
						}
					}
				}
				printf("Error: Member '%s' not found in propeties'%s'\n", member_name, token.value);
				exit(1);
			}
			else{
				printf("Error: '%s' is not a object, enum, properties or values\n", token.value);
				exit(1);
			}
		}
		else{
			Variable *var = get_variable(token.value);
			if (var == NULL){
				printf("Error: Variable '%s' not defined\n", token.value);
				exit(1);
			}
			if (strcmp(var->type, "int") == 0)
				return var->value.int_value;
			if (strcmp(var->type, "string") == 0)
			    return var->value.string_value;
			if (strcmp(var->type, "char") == 0)
			    return var->value.char_value;
			else if (strcmp(var->type, "float") == 0)
				return (int)var->value.float_value;
			else if (strcmp(token.type, "CHAR") == 0){
				return var->value.char_value;
			}
			else if (strcmp(token.type, "STRING") == 0){
				char result_string[MAX_STRING];
				removeQuotes(token.value, result_string);
				return result_string;
			}
			else if (strcmp(token.type, "GET_STR") == 0){
				char v[MAX_STRING];
				fflush(stdin);
				fgets(v, MAX_STRING, stdin);
				fflush(stdin);
				return *v;
			}
			else if (strcmp(var->type, "enum") == 0)
				return var->value.enum_value;
			else{
				printf("Error: Cannot evaluate variable '%s' of type '%s'\n", token.value, var->type);
				exit(1);
			}
		}
	}
	else if (strcmp(token.type,"GET_INPUT") == 0){
		void * value;
		expected("(","getInput","PAREN");
		if (strcmp(tokens[current_token].type,"STRING_PARAM") == 0){
			char str[MAX_TOKEN_LENGTH];
			scanf("%s",&str);
			current_token++;
		    expected(")","getInput","PAREN");
			return str;
		} else if (strcmp(tokens[current_token].type,"INTEGER_PARAM") == 0){
			int int_value;
			scanf("%i",&int_value);
			current_token++;
		    expected(")","getInput","PAREN");
		    return int_value;
		} else if (strcmp(tokens[current_token].type,"FLOAT_PARAM") == 0){
			void * float_value;
			scanf("%f",&float_value);
			current_token++;
		    expected(")","getInput","PAREN");
		    return float_value;
		} else {
			show_errors("Invalid type param in getInput",1);
		}
	}
	else if (strcmp(token.type, "AUM") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			current_token+=3;
			Variable * a = get_variable(var_name);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0){
					a->value.int_value++;
				}
				else if(strcmp(a->type,"float") == 0){
					a->value.float_value++;
				} else
				    show_errors("Invalid type for variable in '++' operator",ERROR_SINTAXIS);
			} else
			      show_errors("Undefined 'variable'",1);
		}
	}
	else if (strcmp(token.type, "SUD") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			current_token+=3;
			Variable * a = get_variable(var_name);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0){
					a->value.int_value--;
				}
				else if(strcmp(a->type,"float") == 0){
					a->value.float_value--;
				} else
				    show_errors("Invalid type for variable in '++' operator",ERROR_SINTAXIS);
			} else
			      show_errors("Undefined 'variable'",1);
		}
	}
	else if (strcmp(token.type, "AUMM") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			float cand = 0;
			current_token+=3;
			Variable * a = get_variable(var_name);
			if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
				Variable * a = get_variable(tokens[current_token].value);
				if (a != NULL){
					if (strcmp(a->type,"int") == 0)
						cand = a->value.int_value;
					else if (strcmp(a->type,"float") == 0)
						cand = a->value.float_value;
					else
				        show_errors("Invalid type for variable in '+=' operator",1);
				    
				}
			}
			else if (strcmp(tokens[current_token].type,"NUMBER") == 0)
				cand = atoi(tokens[current_token].value);
			else if (strcmp(tokens[current_token].type,"FLOAT") == 0)
				cand = atof(tokens[current_token].value);
			else
				show_errors("Invalid value for variable in '+=' operator",1);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0)
					a->value.int_value+=cand;
				else if(strcmp(a->type,"float") == 0)
					a->value.float_value+=cand;
				else
				    show_errors("Invalid type for variable in '+=' operator",ERROR_SINTAXIS);
			}
			else
			    show_errors("Undefined 'variable'",1);
		}
	}
	else if (strcmp(token.type, "SUDD") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			float cand = 0;
			current_token+=3;
			Variable * a = get_variable(var_name);
			if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
				Variable * a = get_variable(tokens[current_token].value);
				if (a != NULL){
					if (strcmp(a->type,"int") == 0)
						cand = a->value.int_value;
					else if (strcmp(a->type,"float") == 0)
						cand = a->value.float_value;
					else
				        show_errors("Invalid type for variable in '+=' operator",1);
				    
				}
			}
			else if (strcmp(tokens[current_token].type,"NUMBER") == 0)
				cand = atoi(tokens[current_token].value);
			else if (strcmp(tokens[current_token].type,"FLOAT") == 0)
				cand = atof(tokens[current_token].value);
			else
				show_errors("Invalid value for variable in '+=' operator",1);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0)
					a->value.int_value-=cand;
				else if(strcmp(a->type,"float") == 0)
					a->value.float_value-=cand;
				else
				    show_errors("Invalid type for variable in '+=' operator",ERROR_SINTAXIS);
			}
			else
			    show_errors("Undefined 'variable'",1);
		}
	}
	else if (strcmp(token.type, "MUL") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			float cand = 0;
			current_token+=3;
			Variable * a = get_variable(var_name);
			if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
				Variable * a = get_variable(tokens[current_token].value);
				if (a != NULL){
					if (strcmp(a->type,"int") == 0)
						cand = a->value.int_value;
					else if (strcmp(a->type,"float") == 0)
						cand = a->value.float_value;
					else
				        show_errors("Invalid type for variable in '*=' operator",1);
				    
				}
			}
			else if (strcmp(tokens[current_token].type,"NUMBER") == 0)
				cand = atoi(tokens[current_token].value);
			else if (strcmp(tokens[current_token].type,"FLOAT") == 0)
				cand = atof(tokens[current_token].value);
			else
				show_errors("Invalid value for variable in '*=' operator",1);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0)
					a->value.int_value*=cand;
				else if(strcmp(a->type,"float") == 0)
					a->value.float_value*=cand;
				else
				    show_errors("Invalid type for variable in '*=' operator",ERROR_SINTAXIS);
			}
			else
			    show_errors("Undefined 'variable'",1);
		}
	}
	else if (strcmp(token.type, "DIV") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			float cand = 0;
			current_token+=3;
			Variable * a = get_variable(var_name);
			if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
				Variable * a = get_variable(tokens[current_token].value);
				if (a != NULL){
					if (strcmp(a->type,"int") == 0)
						cand = a->value.int_value;
					else if (strcmp(a->type,"float") == 0)
						cand = a->value.float_value;
					else
				        show_errors("Invalid type for variable in '/=' operator",1);
				    
				}
			}
			else if (strcmp(tokens[current_token].type,"NUMBER") == 0)
				cand = atoi(tokens[current_token].value);
			else if (strcmp(tokens[current_token].type,"FLOAT") == 0)
				cand = atof(tokens[current_token].value);
			else
				show_errors("Invalid value for variable in '/=' operator",1);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0)
					a->value.int_value/=cand;
				else if(strcmp(a->type,"float") == 0)
					a->value.float_value/=cand;
				else
				    show_errors("Invalid type for variable in '/=' operator",ERROR_SINTAXIS);
			}
			else
			    show_errors("Undefined 'variable'",1);
		}
	}
	else if (strcmp(token.type, "ROW") == 0){
		current_token-=2;
		if(strcmp(tokens[current_token].type,"IDENTIFIER") == 0){
			char * var_name = tokens[current_token].value;
			char * new_var_name;
			current_token+=3;
			if(strcmp(tokens[current_token].type,"IDENTIFIER") != 0)
				show_errors("Expected a variable",1);
			new_var_name = tokens[current_token].value;
			Variable * a = get_variable(var_name);
			if(a != NULL){
				if(strcmp(a->type,"int") == 0){
					set_variable(new_var_name,a->type,&a->value.int_value);
				}
				else if(strcmp(a->type,"float") == 0){
					set_variable(new_var_name,a->type,&a->value.float_value);
				}
				else if(strcmp(a->type,"char") == 0){
					set_variable(new_var_name,a->type,&a->value.char_value);
				}
				else if(strcmp(a->type,"string") == 0){
					set_variable(new_var_name,a->type,&a->value.string_value);
				}
				else if(strcmp(a->type,"bool") == 0){
					set_variable(new_var_name,a->type,&a->value.bool_value);
				} else
				    show_errors("Invalid type for variable in 'row' operator",ERROR_SINTAXIS);
				
			} else
				show_errors("Undefined 'variable'",1);
		} else
			show_errors("Expected a variable",1);
		//printf("Token: %s | valor: %s\n", tokens[current_token].type, tokens[current_token].value);
		return 0;
	}
	else if (strcmp(token.type, "SOCK_STREAM") == 0)
		return SOCK_STREAM;
	else if (strcmp(token.type, "AF_INET") == 0)
		return AF_INET;
    else if (strcmp(token.type, "STRING") == 0){
    	char result_string[MAX_STRING];
	    removeQuotes(token.value, result_string);
	    return *result_string;
    }
	else if (strcmp(token.type, "GETCHAR") == 0)
		return getch();
        else if (strcmp(token.type, "INITSCR") == 0){
          expected("(","initscr","PAREN");
          expected(")","initscr","PAREN");
          initscr();
          return 0;
        } else if (strcmp(token.type, "REFRESH") == 0){
          expected("(","refresh","PAREN");
          expected(")","refresh","PAREN");
          refresh();
          return 0;
        } else if (strcmp(token.type, "NOECHO") == 0){
          expected("(","noecho","PAREN");
          expected(")","noecho","PAREN");
          noecho();
          return 0;
        }
	else if (strcmp(token.type, "MKDIR") == 0){
		char nameDir[MAX_STRING];
		int mode, result;
		expected("(", "mkdir", "PAREN");
		if(strcmp(tokens[current_token].type,"STRING") != 0)
			show_errors("Invalid param in mkdir function: mkdir(string,int)", ERROR_SINTAXIS);
		else
		    removeQuotes(tokens[current_token].value, nameDir);
		current_token++;
		expected(",", "mkdir", "COMMA");
		if(strcmp(tokens[current_token].type,"NUMBER") != 0)
			show_errors("Invalid param in mkdir function: mkdir(string,int)", ERROR_SINTAXIS);
		else
		    mode = atoi(tokens[current_token].value);
		current_token++;
		expected(")", "mkdir", "PAREN");
		result = mkdir(nameDir, mode);
		if(result == 0)
		    return 1;
		else
		    return 0;
    }
	else if (strcmp(token.type, "EXISTS_DIR") == 0){
		int r = 0;
		expected("(", "exists_dir", "PAREN");
		if(strcmp(tokens[current_token].type,"STRING") != 0){
			show_errors("Expected a string value in exists_dir function", ERROR_SINTAXIS);
		} else {
			char result_string[MAX_STRING];
		    removeQuotes(tokens[current_token].value, result_string);
		    struct stat buffer;
		    r = (stat(result_string, &buffer) == 0 && S_ISDIR(buffer.st_mode));
		}
		current_token++;
		expected(")", "exists_dir", "PAREN");
		return r;
	}
	else if (strcmp(token.type, "SOCKET") == 0){
		int a = 0, b = 0, c = 0;
		expected("(", "socket", "PAREN");
		if(strcmp(tokens[current_token].type,"NUMBER") == 0)
			a = atoi(tokens[current_token].value);
		else
		    show_errors("Expected a number value in socket function", ERROR_SINTAXIS);
		current_token++;
		expected(",", "socket", "COMMA");
		if(strcmp(tokens[current_token].type,"NUMBER") == 0)
			b = atoi(tokens[current_token].value);
		else
		    show_errors("Expected a number value in socket function", ERROR_SINTAXIS);
		current_token++;
		expected(",", "socket", "PAREN");
		if(strcmp(tokens[current_token].type,"NUMBER") == 0)
			c = atoi(tokens[current_token].value);
		else
		    show_errors("Expected a number value in socket function", ERROR_SINTAXIS);
		current_token++;
		expected(")", "socket", "PAREN");
		return socket(a,b,c);
	}
	else if (strcmp(token.type, "PUTS") == 0){
		printf("\n");
		return 0;
	}
	else if (strcmp(token.type, "PI") == 0)
		return (int)3.14;
	else if (strcmp(token.type, "ERROR") == 0)
		return 500;
	else if (strcmp(token.type, "OK") == 0)
		return 200;
	else if (strcmp(token.type, "EXIT") == 0)
		exit(1);
	else if (strcmp(token.type, "RANDOM") == 0){
		srand(time(NULL));
		int r = rand() % atoi(tokens[current_token].value);
		return r;
	}
	else if (strcmp(token.type, "DREAM") == 0){
		int i = atoi(tokens[current_token].value);
		usleep(i * i);
		return 0;
	}
	else if (strcmp(token.type, "SETCOLORTEXT") == 0){
		int c;
		if (strcmp(tokens[current_token].type, "PAREN") != 0)
			show_errors("Expected '(' in setColorText function", ERROR_SINTAXIS);
		current_token++;
		if (strcmp(tokens[current_token].type, "NUMBER") == 0)
			c = atoi(tokens[current_token].value);
		else if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0){
			Variable *v = get_variable(tokens[current_token].value);
			if (strcmp(v->type, "int") == 0)
				c = v->value.int_value;
			else
				c = 118;
		}
		else
			c = 118;
		current_token++;
		if (strcmp(tokens[current_token].type, "PAREN") != 0)
			show_errors("Expected ')' in setColorText function", ERROR_SINTAXIS);
		switch (c){
		case 0:
			printf("\033[30m");
			break;
		case 1:
			printf("\033[31m");
			break;
		case 2:
			printf("\033[32m");
			break;
		case 3:
			printf("\033[33m");
			break;
		case 4:
			printf("\033[34m");
			break;
		case 5:
			printf("\033[35m");
			break;
		case 6:
			printf("\033[36m");
			break;
		case 7:
			printf("\033[37m");
			break;
		case 8:
			printf("\033[38m");
			break;
		case 9:
			printf("\033[39m");
			break;
		case 10:
			printf("\033[40m");
			break;
		case 11:
			printf("\033[41m");
			break;
		case 12:
			printf("\033[42m");
			break;
		case 13:
			printf("\033[43m");
			break;
		case 14:
			printf("\033[44m");
			break;
		case 15:
			printf("\033[45m");
			break;
		case 16:
			printf("\033[46m");
			break;
		case 17:
			printf("\033[47m");
			break;
		case 18:
			printf("\033[48m");
			break;
		case 19:
			printf("\033[49m");
			break;
		default:
			printf("\033[31m");
		}
		current_token++;
		return 0;
	}
	else if (strcmp(token.type, "LENGTH") == 0){
		int len = 0;
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected '(' in length function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		Array *arr = get_array(tokens[current_token].value);
		if (arr == NULL){
			puts("Error: Expected 'array' in length function");
			exit(ERROR_SINTAXIS);
		}
		len = arr->member_count;
		current_token++;
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected ')' in length function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		return len;
	}
	else if (strcmp(token.type, "GOTOXY") == 0){
		int x, y;
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected '(' in gotoxy function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "NUMBER") == 0)
			x = atoi(tokens[current_token].value);
		else if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0){
			Variable *v = get_variable(tokens[current_token].value);
			if (strcmp(v->type, "int") == 0)
				x = v->value.int_value;
			else{
				puts("Error: Expected a 'integer' value in gotoxy function");
				exit(ERROR_SINTAXIS);
			}
		}
		else{
			puts("Error: Expected a 'integer' value in gotoxy function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "COMMA") != 0){
			puts("Error: Expected ',' in gotoxy function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "NUMBER") == 0)
			y = atoi(tokens[current_token].value);
		else if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0){
			Variable *v = get_variable(tokens[current_token].value);
			if (strcmp(v->type, "int") == 0)
				y = v->value.int_value;
			else{
				puts("Error: Expected a 'integer' value in gotoxy function");
				exit(ERROR_SINTAXIS);
			}
		}
		else{
			puts("Error: Expected a 'integer' value in gotoxy function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected ')' in gotoxy function");
			exit(ERROR_SINTAXIS);
		}
		printf("\033[%d;%dH", y + 1, x + 1);
		fflush(stdout);
		current_token++;
		return 0;
	}
	else if (strcmp(token.type, "CLEAR") == 0){
		system("clear");
		return 0;
	}
	else if (strcmp(token.type, "GETTYPE") == 0){
		expected("(","gettype","PAREN");
		char * variable;
		if (verify_token("IDENTIFIER",current_token) == true){
			Variable * var = get_variable(tokens[current_token].value);
			variable = tokens[current_token].value;
			if (var == NULL)
				undefined_variable(variable,"gettype",ERROR_INTERPRETER);
		} else 
			show_errors("Expected a variable in gettype function",ERROR_SINTAXIS);
		current_token++;
		expected(")","gettype","PAREN");
		return get_type_variable(variable);
	}
	else if (strcmp(token.type, "CHAR") == 0){
		int x = tokens[current_token].value[1];
		return x;
	}
	else if (strcmp(token.type, "INTERROGATION") == 0){
		return ERROR_SINTAXIS;
	}
	else if (strcmp(token.type, "PRINT_") == 0){
		char text[MAX_STRING];
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected '(' in sendText function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "STRING") == 0){
			removeQuotes(tokens[current_token].value, text);
			printf("%s", text);
		}
		else if (strcmp(tokens[current_token].type, "NUMBER") == 0){
			int r = evaluate_expression();
			printf("%d", r);
			current_token--;
		}
		if (strcmp(tokens[current_token].type, "FLOAT") == 0)
			printf("%.2f", atof(tokens[current_token].value));
		else if (strcmp(tokens[current_token].type, "CHAR") == 0)
			printf("%c", tokens[current_token].value[1]);
		if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0){
			Variable *v = get_variable(tokens[current_token].value);
			Values *valuess = get_values(tokens[current_token].value);
			Struct *structs = get_struct(tokens[current_token].value);
			if (v != NULL){
				if (strcmp(v->type, "int") == 0)
					printf("%i", v->value.int_value);
				else if (strcmp(v->type, "float") == 0)
					printf("%.2f", v->value.float_value);
				else if (strcmp(v->type, "char") == 0)
					printf("%i", v->value.char_value);
				else if (strcmp(v->type, "string") == 0)
					printf("%s", v->value.string_value);
				else if (strcmp(v->type, "float") == 0)
					printf("%d", v->value.enum_value);
				else if (strcmp(v->type, "bool") == 0)
					printf("%i", v->value.bool_value);
			}
			else if (valuess != NULL){
				current_token++;
				if (strcmp(tokens[current_token].type, "DOT") != 0)
					show_errors("Expected '.' after member name", ERROR_SINTAXIS);
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected member name after '.'\n");
					exit(1);
				}
				char *member_name = tokens[current_token].value;
				current_token++;
				for (int i = 0; i < valuess->member_count; i++){
					if (strcmp(valuess->members[i].name, member_name) == 0){
						if (strcmp(valuess->members[i].type, "int") == 0){
							printf("%i", valuess->members[i].value.int_value);
							current_token--;
						}
						else if (strcmp(valuess->members[i].type, "float") == 0){
							printf("%f", valuess->members[i].value.float_value);
							current_token--;
						}
						else if (strcmp(valuess->members[i].type, "char") == 0){
							printf("%c", values->members[i].value.char_value);
							current_token--;
						}
						else if (strcmp(valuess->members[i].type, "string") == 0){
							char text[MAX_STR];
							removeQuotes(values->members[i].value.string_value, text);
							printf("%s", text);
							current_token--;
						}
						else{
							printf("Error: Cannot evaluate member '%s' of values '%s'\n", member_name, token.value);
							exit(1);
						}
					}
				}
			}
			else if (structs != NULL){
				current_token++;
				if (strcmp(tokens[current_token].type, "DOT") != 0)
					show_errors("Expected '.' after member name", ERROR_SINTAXIS);
				current_token++;
				if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
					printf("Error: Expected member name after '.'\n");
					exit(1);
				}
				char *member_name = tokens[current_token].value;
				current_token++;
				for (int i = 0; i < structs->member_count; i++){
					if (strcmp(structs->members[i].name, member_name) == 0){
						if (strcmp(structs->members[i].type, "int") == 0){
							printf("%i", structs->members[i].value.int_value);
							current_token--;
						}
						else if (strcmp(structs->members[i].type, "float") == 0){
							printf("%f", structs->members[i].value.float_value);
							current_token--;
						}
						else if (strcmp(structs->members[i].type, "char") == 0){
							printf("%c", structs->members[i].value.char_value);
							current_token--;
						}
						else if (strcmp(structs->members[i].type, "string") == 0){
							char text[MAX_STR];
							removeQuotes(structs->members[i].value.string_value, text);
							printf("%s", text);
							current_token--;
						}
						else{
							printf("Error: Cannot evaluate member '%s' of object '%s'\n", member_name, token.value);
							exit(1);
						}
					}
				}
			}
			else{
				char vari[100];
				snprintf(vari, 100, "Use of undeclared variable '%s'", tokens[current_token].value);
				show_errors(vari, ERROR_SINTAXIS);
			}
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected ')' in sendText function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		return 0;
	}
	else if (strcmp(token.type, "MEMORY") == 0){
		printf("%p", get_variable(tokens[current_token].value));
		return 0;
	}
	else if (strcmp(token.type, "GET_INT") == 0){
		int v;
		fflush(stdin);
		scanf("%d", &v);
		fflush(stdin);
		return v;
	}
	else if (strcmp(token.type, "CREATE_FILE") == 0){
		char result_string[MAX_STRING];
		if (get_type_variable(tokens[current_token].value) == 115)
			removeQuotes(tokens[current_token].value, result_string);
		removeQuotes(tokens[current_token].value, result_string);
		FILE *fp = fopen(result_string, "w");
		if (fp == NULL){
			perror("");
			return -1;
		}
		fclose(fp);
		return 0;
	}
	else if (strcmp(token.type, "FILE_WRITE") == 0){
		char filename[MAX_STRING];
		char scriture[MAX_STRING];
		char content[MAX_STRING];
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected '(' in write_file function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "STRING") != 0){
			puts("Error: Expected a string in filename of write_file");
			exit(ERROR_SINTAXIS);
		}
		removeQuotes(tokens[current_token].value, filename);
		current_token++;
		if (strcmp(tokens[current_token].type, "COMMA") != 0){
			puts("Error: Expected ',' in write_file function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "STRING") != 0){
			puts("Error: Expected a string in lecture file of write_file");
			exit(ERROR_SINTAXIS);
		}
		removeQuotes(tokens[current_token].value, scriture);
		current_token++;
		if (strcmp(tokens[current_token].type, "COMMA") != 0){
			puts("Error: Expected ',' in write_file function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcmp(tokens[current_token].type, "STRING") != 0){
			puts("Error: Expected a string in content file of write_file");
			exit(ERROR_SINTAXIS);
		}
		removeQuotes(tokens[current_token].value, content);
		current_token++;
		if (strcmp(tokens[current_token].type, "PAREN") != 0){
			puts("Error: Expected ')' in write_file function");
			exit(ERROR_SINTAXIS);
		}
		FILE *fp = fopen(filename, scriture);
		if (fp != NULL){
			fprintf(fp, "%s", content);
			fclose(fp);
			current_token++;
			return 1;
		}
		else{
			current_token++;
			return 0;
		}
	}
	else if (strcmp(token.type, "STRLEN") == 0){
		char text[MAX_STRING];
		removeQuotes(tokens[current_token].value, text);
		return strlen(text);
	}
	else if (strcmp(token.type, "STRCMP") == 0){
		char string1[MAX_STRING], string2[MAX_STRING];
		if (strcmp(tokens[current_token].value, "PAREN") == 0){
			puts("Error: Expected '(' in strcmp function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		removeQuotes(tokens[current_token].value, string1);
		current_token++;
		if (strcmp(tokens[current_token].value, "COMMA") == 0){
			puts("Error: Expected ',' in strcmp function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		removeQuotes(tokens[current_token].value, string2);
		current_token++;
		if (strcmp(tokens[current_token].value, "PAREN") == 0){
			puts("Error: Expected ')' in strcmp function");
			exit(ERROR_SINTAXIS);
		}
		current_token++;
		if (strcasecmp(string1, string2) == 0)
			return 1;
		else
			return 0;
	}
	else if (strcmp(token.type, "TRUE") == 0)
		return 1;
	else if (strcmp(token.type, "FALSE") == 0)
		return 0;
	else if (strcmp(token.type, "EMPTY") == 0)
		return NULL;
	else if (strcmp(token.type, "REMOVE") == 0){
		char content[MAX_STRING];
		removeQuotes(tokens[current_token].value, content);
		if (remove(content) == ERROR_SINTAXIS)
			return 0;
		current_token++;
		return 1;
	}
	else if (strcmp(token.type, "GETSIZE") == 0){
		char content[MAX_STRING];
		removeQuotes(tokens[current_token].value, content);
		struct stat st;
		if (stat(content, &st) == 0)
			return st.st_size;
		return 0;
	}
	else if (strcmp(token.type, "STRCMP") == 0)
		return 1;
	else if (strcmp(token.type, "STRLENG") == 0)
		return 1;
	else if (strcmp(token.type, "STRSEARCH") == 0)
		return 1;
	else if (strcmp(token.type, "STRREV") == 0)
		return 1;
	else if (strcmp(token.type, "BRACKET") == 0 && strcmp(token.value, "[") == 0){
		int array[MAX_ARRAY_SIZE];
		int size = 0;
		while (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") != 0){
			array[size++] = evaluate_expression();
			if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
				current_token++;
		}
		if (current_token >= token_count || strcmp(tokens[current_token].value, "]") != 0){
			printf("Error: Expected ']' after array literal\n");
			exit(1);
		}
		current_token++;
		char array_name[MAX_TOKEN_LENGTH];
		sprintf(array_name, "_array_%d", variable_count);
		Variable *var = &variables[variable_count++];
		strcpy(var->name, array_name);
		strcpy(var->type, "array");
		var->value.array_value = malloc(size * sizeof(int));
		memcpy(var->value.array_value, array, size * sizeof(int));
		var->value.array_size = size;
		return 0;
	}
	else
		unexpected_token(token.value,current_line,ERROR_INTERPRETER);
}
