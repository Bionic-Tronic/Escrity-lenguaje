#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/interpret.h"

#define MAX_STRING 1024

int evaluate_array_access(Array* arr, const char* key) {
    char clean_key[MAX_TOKEN_LENGTH];
    strncpy(clean_key, key + 1, strlen(key) - 2);
    clean_key[strlen(key) - 2] = '\0';
    for (int i = 0; i < arr->member_count; i++) {
        if (strcmp(arr->members[i].name, clean_key) == 0) {
            if (strcmp(arr->members[i].type, "int") == 0) {
                return arr->members[i].value.int_value;
            } else if (strcmp(arr->members[i].type, "float") == 0) {
                return arr->members[i].value.float_value;
            } else if (strcmp(arr->members[i].type, "char") == 0) {
                return arr->members[i].value.char_value;
            } else if (strcmp(arr->members[i].type, "enum") == 0) {
                return arr->members[i].value.enum_value;
            } else if (strcmp(arr->members[i].type, "array") == 0) {
                return 0;
            } else if (strcmp(arr->members[i].type, "string") == 0) {
                set_variable("_tmp_str", "string", arr->members[i].value.string_value);
                return 0;
            } else {
                printf("Error: Cannot evaluate array member '%s' of type '%s'\n", clean_key, arr->members[i].type);
                exit(1);
            }
        }
    }
    printf("Error: Array key '%s' not found\n", clean_key);
    exit(1);
}
double evaluate_term() {
    int left = evaluate_factor();
    while (current_token < token_count &&
           strcmp(tokens[current_token].type, "OPERATOR") == 0 &&
           (tokens[current_token].value[0] == '*' || tokens[current_token].value[0] == '/')) {
        char op = tokens[current_token].value[0];
        current_token++;
        int right = evaluate_factor();
        if (op == '*')
            left *= right;
        else {
            if (right == 0) {
                printf("Error: Division by zero\n");
                exit(1);
            }
            left /= right;
        }
    }
    return left;
}

double evaluate_expression() {
    if (current_token + 2 < token_count &&
        strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
        strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
        tokens[current_token + 1].value[0] == '=') {
        char var_name[MAX_TOKEN_LENGTH];
        strcpy(var_name, tokens[current_token].value);
        current_token += 2;
        char var_str[MAX_STR*2];
        if (strcmp(tokens[current_token].type,"NUMBER") == 0){
        	int int_v = atoi(tokens[current_token].value);
        	set_variable(var_name, "int", &int_v);
        } else if (strcmp(tokens[current_token].type,"STRING") == 0){
        	removeQuotes(tokens[current_token].value,var_str);
        	set_variable(var_name, "string", var_str);
        } else if (strcmp(tokens[current_token].type,"CHAR") == 0){
        	int char_ = tokens[current_token].value[1];
        	set_variable(var_name, "char", &char_);
        } else if (strcmp(tokens[current_token].type,"FLOAT") == 0){
        	double float_v = atof(tokens[current_token].value);
        	set_variable(var_name, "float", &float_v);
        } else if (strcmp(tokens[current_token].type,"TRUE") == 0){
            int true_v = 1;
        	set_variable(var_name, "bool", &true_v);
        } else if (strcmp(tokens[current_token].type,"FALSE") == 0){
            int false_v = 0;
        	set_variable(var_name, "bool", &false_v);
        } else if (strcmp(tokens[current_token].type,"EMPTY") == 0) {
        	set_variable(var_name, "empty", NULL);
        } else {
        	int bool_v = evaluate_expression();
        	set_variable(var_name, "int", &bool_v);
        }
        double v = evaluate_expression();
        return v;
    }
    int left = evaluate_term();
    while (current_token < token_count &&
           strcmp(tokens[current_token].type, "OPERATOR") == 0 &&
           (tokens[current_token].value[0] == '+' || tokens[current_token].value[0] == '-' ||
            strcmp(tokens[current_token].value, "==") == 0 || strcmp(tokens[current_token].value, "<") == 0 ||
            strcmp(tokens[current_token].value, ">") == 0 || strcmp(tokens[current_token].value, "<=") == 0 ||
            strcmp(tokens[current_token].value, ">=") == 0 || strcmp(tokens[current_token].value, "!=") == 0 ||
            strcmp(tokens[current_token].value, "is") == 0)) {
        char op[3];
        strcpy(op, tokens[current_token].value);
        current_token++;
        int right = evaluate_term();
        if (strcmp(op, "+") == 0) {
            left += right;
        } else if (strcmp(op, "-") == 0) {
            left -= right;
        } else if (strcmp(op, "*") == 0) {
        	left *= right;
        } else if (strcmp(op, "/") == 0) {
        	left /= right;
        } else if (strcmp(op, "==") == 0) {
            left = (right == left);
        } else if (strcmp(op, "is") == 0) {
            left = (right == left);
        } else if (strcmp(op, "<") == 0) {
            left = (left < right);
        } else if (strcmp(op, ">") == 0) {
            left = (left > right);
        } else if (strcmp(op, "<=") == 0) {
            left = (left <= right);
        } else if (strcmp(op, ">=") == 0) {
            left = (left >= right);
        } else if (strcmp(op, "!=") == 0) {
            left = (left != right);
        }
    }
    return left;
}

double evaluate_factor() {
    Token token = tokens[current_token];
    current_token++;
    if (strcmp(token.type, "NUMBER") == 0) {
        return atoi(token.value);
    } else if (strcmp(token.type, "FLOAT") == 0) {
        return atof(token.value);
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
            return call_function(func, &args, arg_count);
        } else if (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") == 0 && strcmp(tokens[current_token].value, "[") == 0) {
            Array* arr = get_array(token.value);
            if (arr == NULL) {
                printf("Error: Array '%s' not defined\n", token.value);
                exit(1);
            }
            current_token++;
            if (current_token >= token_count || (strcmp(tokens[current_token].type, "STRING") != 0 && strcmp(tokens[current_token].type, "IDENTIFIER") != 0)) {
                printf("Error: Expected string key or identifier for array access\n");
                exit(1);
            }
            char* key = tokens[current_token].value;
            current_token++;
            if (current_token >= token_count || strcmp(tokens[current_token].type, "BRACKET") != 0 || strcmp(tokens[current_token].value, "]") != 0) {
                printf("Error: Expected ']' after array key\n");
                exit(1);
            }
            current_token++;
            return evaluate_array_access(arr, key);
        } else if (current_token < token_count && strcmp(tokens[current_token].type, "DOT") == 0) {
            Struct* s = get_struct(token.value);
            Enum* e = get_enum(token.value);
            Properties * p = get_properties(token.value);
            //Objects
            if (s != NULL) {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
                    printf("Error: Expected member name after '.'\n");
                    exit(1);
                }
                char* member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < s->member_count; i++) {
                    if (strcmp(s->members[i].name, member_name) == 0) {
                        if (strcmp(s->members[i].type, "int") == 0) {
                            return s->members[i].value.int_value;
                        } else if (strcmp(s->members[i].type, "float") == 0) {
                            return s->members[i].value.float_value;
                        } else if (strcmp(s->members[i].type, "char") == 0) {
                            return s->members[i].value.char_value;
                        } else if (strcmp(s->members[i].type, "function") == 0) {
                            Function* func = s->members[i].value.func_value;
                            int args[MAX_PARAMS];
                            int arg_count = 0;
                            if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0) {
                                current_token++;
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
                            }
                            return call_function(func, &args, arg_count);
                        } else {
                            printf("Error: Cannot evaluate member '%s' of object '%s'\n", member_name, token.value);
                            exit(1);
                        }
                    }
                }
                printf("Error: Member '%s' not found in object '%s'\n", member_name, token.value);
                exit(1);
            }
            //enums
            else if (e != NULL) {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
                    printf("Error: Expected enum value after '.'\n");
                    exit(1);
                }
                char* enum_value = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < e->member_count; i++) {
                    if (strcmp(e->members[i], enum_value) == 0) {
                        return i;
                    }
                }
                printf("Error: Enum value '%s' not found in enum '%s'\n", enum_value, token.value);
                exit(1);
            }
            //Properties
            else if (p != NULL){
            	current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
                    printf("Error: Expected member name after '.'\n");
                    exit(1);
                }
                char* member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < p->member_count; i++) {
                    if (strcmp(p->members[i].name, member_name) == 0) {
                        if (p->visible[i] == 1){
                        	if (strcmp(tokens[current_token++].type,"IDENTIFIER") != 0){
                        		if (strcmp(tokens[current_token].type, "NUMBER") == 0) {
            strcpy(p->members[i].type, "int");
            p->members[i].value.int_value = atoi(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "FLOAT") == 0) {
            strcpy(p->members[i].type, "float");
            p->members[i].value.float_value = atof(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "STRING") == 0) {
            strcpy(p->members[i].type, "string");
            strcpy(p->members[i].value.string_value, tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type, "CHAR") == 0) {
            strcpy(p->members[i].type, "char");
            p->members[i].value.char_value = tokens[current_token].value[1];
        } 
                        	}
                        if (current_token + 1 < token_count &&
        strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
        strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
        tokens[current_token + 1].value[0] == '=') {
        }
                        	if (strcmp(p->members[i].type, "int") == 0) {
                            return p->members[i].value.int_value;
                        } else if (strcmp(p->members[i].type, "float") == 0) {
                            return p->members[i].value.float_value;
                        } else if (strcmp(p->members[i].type, "char") == 0) {
                            return p->members[i].value.char_value;
                        } else if (strcmp(p->members[i].type, "function") == 0) {
                            Function* func = p->members[i].value.func_value;
                            int args[MAX_PARAMS];
                            int arg_count = 0;
                            if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0) {
                                current_token++;
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
                            }
                            return call_function(func, &args, arg_count);
                        } else {
                            printf("Error: Cannot evaluate member '%s' of propeties '%s'\n", member_name, token.value);
                            exit(1);
                        }
                        } else {
                        	printf("Error: Cannot access at member '%s' of properties '%s' this is a 'priv'\n", member_name, token.value);
                            exit(-1);
                        }
                    }
                }
                printf("Error: Member '%s' not found in propeties'%s'\n", member_name, token.value);
                exit(1);
            }
             else {
                printf("Error: '%s' is not a object, enum o properties\n", token.value);
                exit(1);
            }
        } else {
            Variable* var = get_variable(token.value);
            if (var == NULL) {
                printf("Error: Variable '%s' not defined\n", token.value);
                exit(1);
            }
            if (strcmp(var->type, "int") == 0) {
                return var->value.int_value;
            } else if (strcmp(var->type, "float") == 0) {
                return var->value.float_value;
            } else if (strcmp(token.type, "CHAR") == 0) {
                return var->value.char_value;
    } else if (strcmp(token.type, "STRING") == 0) {
        char result_string[MAX_STRING];
        removeQuotes(token.value, result_string);
        return 0;
    } else if(strcmp(token.type, "GET_STR") == 0){
        char v[MAX_STRING];
        fflush(stdin);
        fgets(v,MAX_STRING,stdin);
        fflush(stdin);
        return * v;
    } else if (strcmp(var->type, "enum") == 0) {
                return var->value.enum_value;
            } else {
                printf("Error: Cannot evaluate variable '%s' of type '%s'\n", token.value, var->type);
                exit(1);
            }
        }
    } else if (strcmp(token.type, "STRING") == 0) {
        return 0;
    } else if (strcmp(token.type, "GETCHAR") == 0){
        char x = getch();
        return x;
    } else if (strcmp(token.type, "PUTS") == 0){
        printf("\n");
        return 0;
    } else if(strcmp(token.type, "PI") == 0) {
        return 3.14;
    } else if(strcmp(token.type, "EXIT") == 0) {
        exit(1);
    } else if(strcmp(token.type, "RANDOM") == 0){
        srand(time(NULL));
        int r = rand()%atoi(tokens[current_token].value);
        return r;
    } else if (strcmp(token.type, "DREAM") == 0) {
    	int i = atoi(tokens[current_token].value);
    	usleep(i * i);
    	return 0;
    } else if (strcmp(token.type, "SETCOLORTEXT") == 0) {
    	int c;
    	if (strcmp(tokens[current_token].type, "PAREN") != 0)
    		show_errors("Expected '(' in setColorText function", -1);
    	current_token++;
    	if (strcmp(tokens[current_token].type, "NUMBER") == 0)
    	    c = atoi(tokens[current_token].value);
    	else if (strcmp(tokens[current_token].type, "IDENTIFIER") == 0){
    	    Variable * v = get_variable(tokens[current_token].value);
    	    if(strcmp(v->type,"int") == 0)
    	       c = v->value.int_value;
    	    else
    	       c = 118;
    	} else
    	      c = 118;
    	current_token++;
    	if (strcmp(tokens[current_token].type, "PAREN") != 0)
    		show_errors("Expected ')' in setColorText function", -1);
        switch(c){
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
    } else if (strcmp(token.type, "LENGTH") == 0) {
    	int len = 0;
    	if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected '(' in length function");
        	exit(-1);
        }
        current_token++;
        Array * arr = get_array(tokens[current_token].value);
        if (arr == NULL){
        	puts("Error: Expected 'array' in length function");
        	exit(-1);
        }
        len = arr->member_count;
        current_token++;
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected ')' in length function");
        	exit(-1);
        }
        current_token++;
        return len;
    } else if (strcmp(token.type,"GOTOXY") == 0) {
        int x,y;
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected '(' in gotoxy function");
        	exit(-1);
        }
        current_token++;
        if (strcmp(tokens[current_token].type,"NUMBER") == 0) {
        	x = atoi(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0) {
        	Variable * v = get_variable(tokens[current_token].value);
        	if(strcmp(v->type,"int") == 0) {
        		x = v->value.int_value;
        	} else {
        		puts("Error: Expected a 'integer' value in gotoxy function");
        	    exit(-1);
        	}
        } else {
        	puts("Error: Expected a 'integer' value in gotoxy function");
        	exit(-1);
        }
        current_token++;
        if (strcmp(tokens[current_token].type,"COMMA") != 0) {
        	puts("Error: Expected ',' in gotoxy function");
        	exit(-1);
        }
        current_token++;
        if (strcmp(tokens[current_token].type,"NUMBER") == 0) {
        	y = atoi(tokens[current_token].value);
        } else if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0) {
        	Variable * v = get_variable(tokens[current_token].value);
        	if(strcmp(v->type,"int") == 0) {
        		y = v->value.int_value;
        	} else {
        		puts("Error: Expected a 'integer' value in gotoxy function");
        	    exit(-1);
        	}
        } else {
        	puts("Error: Expected a 'integer' value in gotoxy function");
        	exit(-1);
        }
        current_token++;
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected ')' in gotoxy function");
        	exit(-1);
        }
        printf("\033[%d;%dH", y + 1, x + 1);
        fflush(stdout);
        current_token++;
        return 0;
    } else if(strcmp(token.type,"CLEAR") == 0) {
        system("clear");
        return 0;
    } else if (strcmp(token.type, "GETTYPE") == 0){
        if(get_type_variable(tokens[current_token].value) == 105 || strcmp(tokens[current_token].type, "NUMBER") == 0)
             return 105;
        else if(get_type_variable(tokens[current_token].value) == 102 || strcmp(tokens[current_token].type, "FLOAT") == 0)
                return 102;
        else if(get_type_variable(tokens[current_token].value) == 115 || strcmp(tokens[current_token].type, "STRING") == 0)
                return 115;
        else if(get_type_variable(tokens[current_token].value) == 219)//Funcion
                return 219;
        else if(get_type_variable(tokens[current_token].value) == 97)//Arreglo
                return 97;
        else if(get_type_variable(tokens[current_token].value) == 99)//char
                return 99;
        else
           return 0;
    } else if (strcmp(token.type, "CHAR") == 0) {
        int x = tokens[current_token].value[1];
        return x;
    } else if(strcmp(token.type,"PRINT_") == 0) {
        char text[MAX_STRING];
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected '(' in sendText function");
        	exit(-1);
        }
        current_token++;
        if (strcmp(tokens[current_token].type,"STRING") == 0) {
        	removeQuotes(tokens[current_token].value,text);
        	printf("%s",text);
        } else if (strcmp(tokens[current_token].type,"NUMBER") == 0) {
        	int r = evaluate_expression();
        	printf("%d",r);
        } if (strcmp(tokens[current_token].type,"FLOAT") == 0) {
        	printf("%.2f",atof(tokens[current_token].value));
        } else if (strcmp(tokens[current_token].type,"CHAR") == 0) {
        	printf("%c",tokens[current_token].value[1]);
        } if (strcmp(tokens[current_token].type,"IDENTIFIER") == 0) {
        	Variable * v = get_variable(tokens[current_token].value);
        	if(strcmp(v->type,"int") == 0) {
        		printf("%i",v->value.int_value);
        	} else if (strcmp(v->type,"float") == 0) {
        		printf("%.2f",v->value.float_value);
        	} if (strcmp(v->type,"char") == 0) {
        		printf("%i",v->value.char_value);
        	} else if (strcmp(v->type,"string") == 0) {
        		printf("%s",v->value.string_value);
        	} if (strcmp(v->type,"float") == 0) {
        		printf("%d",v->value.enum_value);
        	} else if (strcmp(v->type,"bool") == 0) {
        		printf("%i",v->value.bool_value);
        	}
        }
        current_token++;
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected ')' in sendText function");
        	exit(-1);
        }
        current_token++;
        return 0;
    }else if (strcmp(token.type, "MEMORY") == 0) {
        printf("%p",get_variable(tokens[current_token].value));
        return 0;
    } else if(strcmp(token.type, "GET_INT") == 0){
        int v;
        fflush(stdin);
        scanf("%d",&v);
        fflush(stdin);
        return v;
    } else if (strcmp(token.type,"CREATE_FILE") == 0) {
        char result_string[MAX_STRING];
        if (get_type_variable(tokens[current_token].value) == 115) {
        	removeQuotes(tokens[current_token].value, result_string);
        }
        removeQuotes(tokens[current_token].value, result_string);
        FILE * fp = fopen(result_string,"w");
        if(fp == NULL) {
            perror("");
            return -1;
        }
        fclose(fp);
        return 0;
    } else if (strcmp(token.type,"FILE_WRITE") == 0){
    	char filename[MAX_STRING];
    	char scriture[MAX_STRING];
    	char content[MAX_STRING];
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected '(' in write_file function");
        	exit(-1);
        }
        current_token++;
        if(strcmp(tokens[current_token].type,"STRING") != 0) {
        	puts("Error: Expected a string in filename of write_file");
        	exit(-1);
        }
        removeQuotes(tokens[current_token].value,filename);
        current_token++;
        if (strcmp(tokens[current_token].type,"COMMA") != 0) {
        	puts("Error: Expected ',' in write_file function");
        	exit(-1);
        }
        current_token++;
        if(strcmp(tokens[current_token].type,"STRING") != 0) {
        	puts("Error: Expected a string in lecture file of write_file");
        	exit(-1);
        }
        removeQuotes(tokens[current_token].value, scriture);
        current_token++;
        if (strcmp(tokens[current_token].type,"COMMA") != 0) {
        	puts("Error: Expected ',' in write_file function");
        	exit(-1);
        }
        current_token++;
        if(strcmp(tokens[current_token].type,"STRING") != 0) {
        	puts("Error: Expected a string in content file of write_file");
        	exit(-1);
        }
        removeQuotes(tokens[current_token].value,content);
        current_token++;
        if (strcmp(tokens[current_token].type,"PAREN") != 0) {
        	puts("Error: Expected ')' in write_file function");
        	exit(-1);
        }
        FILE * fp = fopen(filename,scriture);
        if (fp != NULL) {
        	fprintf(fp,"%s",content);
        	fclose(fp);
        	current_token++;
        	return 1;
        } else {
        	current_token++;
        	return 0;
        }
    } else if (strcmp(token.type,"STRLEN") == 0) {
    	char text[MAX_STRING];
        removeQuotes(tokens[current_token].value,text);
        return strlen(text);
    } else if (strcmp(token.type,"STRCMP") == 0) {
    	char string1[MAX_STRING], string2[MAX_STRING];
    	if (strcmp(tokens[current_token].value,"PAREN") == 0) {
        	puts("Error: Expected '(' in strcmp function");
        	exit(-1);
        }
        current_token++;
        removeQuotes(tokens[current_token].value,string1);
        current_token++;
        if (strcmp(tokens[current_token].value,"COMMA") == 0) {
        	puts("Error: Expected ',' in strcmp function");
        	exit(-1);
        }
        current_token++;
        removeQuotes(tokens[current_token].value,string2);
        current_token++;
        if (strcmp(tokens[current_token].value,"PAREN") == 0) {
        	puts("Error: Expected ')' in strcmp function");
        	exit(-1);
        }
        current_token++;
        if(strcasecmp(string1, string2) == 0)
           return 1;
        else
           return 0;
    } else if (strcmp(token.type,"TRUE") == 0) {
    	return 1;
    } else if (strcmp(token.type,"FALSE") == 0) {
    	return 0;
    } else if (strcmp(token.type,"EMPTY") == 0) {
    	return (int)NULL;
    } else if (strcmp(token.type,"REMOVE") == 0) {
    	char content[MAX_STRING];
    	removeQuotes(tokens[current_token].value,content);
    	if(remove(content) == -1)
    	   return 0;
        current_token++;
        return 1;
    } else if (strcmp(token.type,"GETSIZE") == 0) {
    	char content[MAX_STRING];
    	removeQuotes(tokens[current_token].value,content);
    	struct stat st;
        if (stat(content, &st) == 0)
            return st.st_size;
        return 0;
    } else if (strcmp(token.type,"STRCMP") == 0) {
    	return 1;
    } else if (strcmp(token.type,"STRLENG") == 0) {
    	return 1;
    } else if (strcmp(token.type,"STRSEARCH") == 0) {
    	return 1;
    } else if (strcmp(token.type,"STRREV") == 0) {
    	return 1;
    } else if (strcmp(token.type, "BRACKET") == 0 && strcmp(token.value, "[") == 0) {
        int array[MAX_ARRAY_SIZE];
        int size = 0;
        while (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") != 0) {
            array[size++] = evaluate_expression();
            if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
                current_token++;
            }
        }
        if (current_token >= token_count || strcmp(tokens[current_token].value, "]") != 0) {
            printf("Error: Expected ']' after array literal\n");
            exit(1);
        }
        current_token++;
        char array_name[MAX_TOKEN_LENGTH];
        sprintf(array_name, "_array_%d", variable_count);
        Variable* var = &variables[variable_count++];
        strcpy(var->name, array_name);
        strcpy(var->type, "array");
        var->value.array_value = malloc(size * sizeof(int));
        memcpy(var->value.array_value, array, size * sizeof(int));
        var->value.array_size = size;
        return 0;
    } else {
        printf("Error: Unexpected token %s in line %i\n", token.type,current_line);
        exit(1);
    }
}