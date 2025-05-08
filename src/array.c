#include "../include/array.h"

ARRAY * get_ARRAY (const char * index){
	for (int i = 0; i < values_count; i++) {
        if (strcmp(ARRAYS[i].name, index) == 0) {
            return &ARRAYS[i];
        }
    }
    return NULL;
}

Vector * get_vector (const char * name){
	for (int i = 0; i < values_count; i++) {
        if (strcmp(vec[i].name, name) == 0) {
            return &vec[i];
        }
    }
    return NULL;
}

void define_vector (){
	if (ARRAY_count >= MAX_ARRAY_SIZE)
		show_errors("Too many vectors",-1);
	Vector * vecto = &vec[vector_count++];
	vecto->member_count = vector_count;
	if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
	   show_errors ("Expected vector name",-1);
	strcpy(vecto->name, tokens[current_token].value);
	current_token++;
	if (tokens[current_token].value[0] != ':')
	   show_errors("Expected ':' in vector struct",-1);
	current_token++;
	if(strcmp(tokens[current_token].type,"ADD") != 0)
	   show_errors ("Expected 'add' in vector struct",-1);
	current_token++;
	if(strcmp(tokens[current_token].type,"PAREN") != 0)
	   show_errors ("Expected '(' before 'add' in vector struct",-1);
	current_token++;
	if(strcmp(tokens[current_token].type, "NUMBER") == 0){
		strcpy(vecto->members[vecto->member_count].name, "x");
		strcpy(vecto->members[vecto->member_count].type, "int");
		vecto->members[vecto->member_count].value.int_value = atoi(tokens[current_token].value);
	} else
		show_errors ("Expected at number before 'add' in vector struct",-1);
	current_token++;
	if(strcmp(tokens[current_token].type,"COMMA") != 0)
	   show_errors ("Expected ',' before 'add' in vector struct",-1);
	current_token++;
	if(strcmp(tokens[current_token].type, "NUMBER") == 0){
		strcpy(vecto->members[vecto->member_count].name, "y");
		strcpy(vecto->members[vecto->member_count].type, "int");
		vecto->members[vecto->member_count].value.int_value = atoi(tokens[current_token].value);
	} else
		show_errors ("Expected at number before 'add' in vector struct",-1);
	current_token++;
	if(strcmp(tokens[current_token].type,"PAREN") != 0)
	   show_errors ("Expected ')' before 'add' in vector struct",-1);
	current_token++;
	vector_count++;
	values_count++;
}

void define_ARRAY () {
	if (ARRAY_count >= MAX_ARRAY_SIZE)
		show_errors("Too many arrays",-1);
	ARRAY * array = &ARRAYS[ARRAY_count++];
	if (tokens[current_token].value[0] != '<')
	   show_errors("Expected '<' in array structure",-1);
	current_token++;
	if (strcmp(tokens[current_token].type,"INTEGER_PARAM") != 0 )
	   show_errors("Not declared a type array",-1);
	current_token++;
	if (tokens[current_token].value[0] != '>')
	   show_errors("Expected '>' in array structure",-1);
	current_token++;
	if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
	   show_errors ("Expected array name",-1);
	strcpy(array->name, tokens[current_token].value);
	current_token++;
	if(strcmp(tokens[current_token].type,"THEN") != 0)
	   show_errors ("Expected 'then' before array name",-1);
	current_token++;
	array->member_count = 0;
	while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
		if (strcmp(tokens[current_token].type,"NUMBER") == 0){
			char index[1024];
            snprintf(index, 1024, "%i", array->member_count);
			strcpy(array->members[array->member_count].name,index);
			strcpy(array->members[array->member_count].type, "int");
            array->members[array->member_count].value.int_value = atoi(tokens[current_token].value);
		} else if (strcmp(tokens[current_token].type,"FLOAT") == 0){
			char index[1024];
            snprintf(index, 1024, "%i", array->member_count);
			strcpy(array->members[array->member_count].name,index);
			strcpy(array->members[array->member_count].type, "float");
            array->members[array->member_count].value.float_value = atof(tokens[current_token].value);
		} else if (strcmp(tokens[current_token].type,"STRING") == 0){
			char index[1024];
            snprintf(index, 1024, "%i", array->member_count);
			strcpy(array->members[array->member_count].name,index);
			strcpy(array->members[array->member_count].type, "string");
			strcpy(array->members[array->member_count].value.string_value, tokens[current_token].value);
		} else if (strcmp(tokens[current_token].type,"TRUE") == 0){
			char index[1024];
            snprintf(index, 1024, "%i", array->member_count);
			strcpy(array->members[array->member_count].name,index);
			strcpy(array->members[array->member_count].type, "bool");
            array->members[array->member_count].value.bool_value = atoi(tokens[current_token].value);
		} else if (strcmp(tokens[current_token].type,"FALSE") == 0){
			char index[1024];
            snprintf(index, 1024, "%i", array->member_count);
			strcpy(array->members[array->member_count].name,index);
			strcpy(array->members[array->member_count].type, "bool");
            array->members[array->member_count].value.bool_value = atoi(tokens[current_token].value);
		} else if (strcmp(tokens[current_token].type,"CHAR") == 0){
			char index[1024];
            snprintf(index, 1024, "%i", array->member_count);
			strcpy(array->members[array->member_count].name,index);
			strcpy(array->members[array->member_count].type, "char");
            array->members[array->member_count].value.char_value = tokens[current_token].value[1];
		} else
		    show_errors ("Invalid value for array",-1);
		current_token++;
        array->member_count++;
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

void * evaluate_array_access(Array *arr, const char *key){
	char clean_key[MAX_TOKEN_LENGTH];
	strncpy(clean_key, key + 1, strlen(key) - 2);
	clean_key[strlen(key) - 2] = '\0';
	for (int i = 0; i < arr->member_count; i++){
		if (strcmp(arr->members[i].name, clean_key) == 0){
			if (strcmp(arr->members[i].type, "int") == 0)
				return arr->members[i].value.int_value;
			else if (strcmp(arr->members[i].type, "float") == 0)
				return (int)arr->members[i].value.float_value;
			else if (strcmp(arr->members[i].type, "char") == 0)
				return arr->members[i].value.char_value;
			else if (strcmp(arr->members[i].type, "enum") == 0)
				return arr->members[i].value.enum_value;
			else if (strcmp(arr->members[i].type, "array") == 0)
				return 0;
			else if (strcmp(arr->members[i].type, "string") == 0){
				return arr->members[i].value.string_value;
			}
			else{
				printf("Error: Cannot evaluate array member '%s' of type '%s'\n", clean_key, arr->members[i].type);
				exit(1);
			}
		}
	}
	printf("Error: Array key '%s' not found\n", clean_key);
	exit(1);
}
