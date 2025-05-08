#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "../include/interpret.h"
#include "../include/protocol.h"
#include "../include/errors.h"
#include "../include/variable.h"
#include "../include/function.h"
#include "../include/object.h"
#include "../include/properties.h"
#include "../include/array.h"
#include "../include/enums.h"

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

server servidor;
char buffer[BUFFER_SIZE];
char buff[BUFFER_SIZE];
char http_buffer[BUFFER_SIZE];

Values values[MAX_VALUES];
int values_count = 0;

ARRAY ARRAYS[MAX_ARRAY_SIZE];
int ARRAY_count = 0;

Vector vec[MAX_ARRAY_SIZE];
int vector_count;

Protocol protocol[MAX_VARIABLES];
int protocol_count = 0;

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
        int condition = evaluate_expression();
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
        value = var->value.int_value;
    } else if (strcmp(var->type, "float") == 0) {
        value = (int)(var->value.float_value);
    } else if (strcmp(var->type, "char") == 0) {
        value = var->value.char_value;
    } else if (strcmp(var->type, "bool") == 0) {
        value = var->value.bool_value;
    } else if (strcmp(var->type, "enum") == 0) {
        value = var->value.enum_value;
    } else if (strcmp(var->type, "string") == 0) {
        value = var->value.string_value;
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

void define_for () {
}
void define_array_for () {
}
void interpret_for_loop() {
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
    	show_errors("File is NULL or invalid for read",-1);
    printf("");
	char buffer[s];
	fread(buffer, sizeof(char), s, fp);
	tokenize(buffer);
	fclose(fp);
	current_token++;
}

int open_server (server * server){
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0){
        return ERROR;
    }
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    server->server_addr.sin_port = htons(server->port);
    if (bind(server->server_socket, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) < 0){
        close(server->client_socket);
        return ERROR;
    }
    if (listen(server->server_socket, server->listen) < 0){
        close(server->client_socket);
        return ERROR;
    }
    server->is_error = OK;
    return OK;
}

int accept_conections (server * server){
    server->client_addr_len = sizeof(server->client_addr);
    server->client_socket = accept(server->server_socket, (struct sockaddr *)&server->client_addr, &server->client_addr_len);
    if (server->client_socket < 0){
        close(server->client_socket);
        return ERROR;
    }
    server->is_error = OK;
    return OK;
}

void send_file_response(int client_socket, const char *header, const char *content_type, const char *file_path){
    FILE *file = fopen(file_path, "rb");
    if (file == NULL){
        perror("");
        return;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_content = malloc(file_size);
    if (file_content == NULL){
        perror("");
        fclose(file);
        return;
    }
    fread(file_content, 1, file_size, file);
    fclose(file);
    //send_response(client_socket, header, content_type, file_content, file_size);
    free(file_content);
}

String getExtension(String nombreArchivo){
    String punto = strrchr(nombreArchivo, '.');
    if (!punto || punto == nombreArchivo)
        return "";
    return punto + 1;
}

int compareExtension(String archivo1, String archivo2){
    String extension1 = getExtension(archivo1);
    String extension2 = getExtension(archivo2);
    if (strcmp(extension1, extension2) == 0)
        return OK;
    return ERROR;
}

string get (String palabra, string texto, char caracterLimite){
    char palabra_copy[MAX_STR];
    snprintf(palabra_copy, MAX_STR, "%s", palabra);
    char *encontrado = strstr(texto, palabra_copy);
    if (encontrado != NULL){
        size_t posicionFinal = encontrado - texto + strlen(palabra_copy);
        const char *limite = strchr(texto + posicionFinal, caracterLimite);
        if (limite != NULL){
            size_t longitud = limite - (texto + posicionFinal);
            char *subcadena = (char *)malloc(longitud + 1);
            strncpy(subcadena, texto + posicionFinal, longitud);
            subcadena[longitud] = '\0';
            return subcadena;
        }
        else
            return NULL;
    }
    else
        return NULL;
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
        } else if (strcmp(tokens[current_token].type, "OBJECT") == 0) {
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
        } else if (strcmp(tokens[current_token].type, "array") == 0) {
            current_token++;
            define_ARRAY();
        } else if (strcmp(tokens[current_token].type, "ADD_CODE") == 0) {
            current_token++;
            add_code_file();
        } else if (strcmp(tokens[current_token].type, "VALUES") == 0) {
            current_token++;
            define_values();
        } else if (strcmp(tokens[current_token].type, "VECTOR") == 0) {
            current_token++;
            define_vector();
        } else if (strcmp(tokens[current_token].type, "PROTOCOL") == 0) {
            current_token++;
            define_protocol();
        } else {
            evaluate_expression();
        }
    }
}
