#include <stdio.h>
#include <stdlib.h>                               
#include <string.h>
#include <ctype.h>
#include "../include/interpret.h"
                                                  void add_token(const char* type, const char* value) {                                                   strcpy(tokens[token_count].type, type);
    strcpy(tokens[token_count].value, value);
    token_count++;
}
                                                  char* unescape_string(const char* input) {
    static char buffer[MAX_TOKEN_LENGTH];
    int i, j;
    for (i = 0, j = 0; input[i] != '\0'; i++, j++) {                                                        if (input[i] == '\\' || input[i] == '@') {
            i++;
            switch (input[i]) {
                case 'n': buffer[j] = '\n'; break;
                case 't': buffer[j] = '\t'; break;
                case 'r': buffer[j] = '\r'; break;
                case '\\': buffer[j] = '\\'; break;
                case '"': buffer[j] = '"'; break;
                default: buffer[j] = input[i];
            }
        } else {
            buffer[j] = input[i];
        }
    }
    buffer[j] = '\0';
    return buffer;
}

void tokenize(const char* code) {
    char buffer[MAX_TOKEN_LENGTH];
    int buffer_pos = 0;
    bool in_comment = false;
    for (int i = 0; code[i] != '\0'; i++) {
        if (in_comment) {
            if (code[i] == '\n') {
                in_comment = false;
            }
            continue;
        }
        if (code[i] == '/' && code[i+1] == '/') {
            in_comment = true;
            i++;
            continue;
        }
        if (isalpha(code[i]) || code[i] == '_') {
            buffer[buffer_pos++] = code[i];
            while (isalnum(code[i+1]) || code[i+1] == '_') {
                buffer[buffer_pos++] = code[++i];
            }
            buffer[buffer_pos] = '\0';
            if (strcmp(buffer, "text") == 0) {
                add_token("PRINT", buffer);
            } else if (strcmp(buffer, "sendText") == 0) {
                add_token("PRINT_", buffer);
            } else if (strcmp(buffer, "resolver") == 0) {
                add_token("RESOLVER", buffer);
            } else if (strcmp(buffer, "func") == 0 || strcmp(buffer, "function") == 0 || strcmp(buffer, "fn") == 0) {
                add_token("FUNC", buffer);
            } else if (strcmp(buffer, "then") == 0) {
                add_token("THEN", buffer);
            } else if (strcmp(buffer, "end") == 0) {
                add_token("END", buffer);
            } else if (strcmp(buffer, "return") == 0 || strcmp(buffer, "ret") == 0) {
                add_token("RETURN", buffer);
            } else if (strcmp(buffer, "if") == 0) {
                add_token("IF", buffer);
            } else if (strcmp(buffer, "else") == 0) {
                add_token("ELSE", buffer);
            } else if (strcmp(buffer, "for") == 0) {
                add_token("FOR", buffer);
            } else if (strcmp(buffer, "while") == 0) {
                add_token("WHILE", buffer);
            } else if (strcmp(buffer, "set") == 0) {
                add_token("SET", buffer);
            } else if (strcmp(buffer, "in_range") == 0 || strcmp(buffer, "inrange") == 0) {
                add_token("IN_RANGE", buffer);
            } else if (strcmp(buffer, "ascii") == 0) {
                add_token("ASCII", buffer);
            } else if (strcmp(buffer,"get_int") == 0){
                add_token("GET_INT", buffer);
            } else if (strcmp(buffer,"get_str") == 0){
                add_token("GET_STR", buffer);
            } else if (strcmp(buffer,"random") == 0){
                add_token("RANDOM", buffer);
            } else if (strcmp(buffer,"PI") == 0){
                add_token("PI", buffer);
            } else if (strcmp(buffer,"exit") == 0){
                add_token("EXIT", buffer);
            } else if (strcmp(buffer,"puts") == 0){
                add_token("PUTS", buffer);
            } else if (strcmp(buffer,"getchar") == 0){
                add_token("GETCHAR", buffer);
            } else if (strcmp(buffer, "Object") == 0 || strcmp(buffer, "object") == 0) {
                add_token("STRUCT", buffer);
            } else if (strcmp(buffer, "declare") == 0) {
                add_token("DECLARE", buffer);
            } else if (strcmp(buffer, "array") == 0 || strcmp(buffer, "Array") == 0) {
                add_token("ARRAY", buffer);
            } else if (strcmp(buffer, "when") == 0 || strcmp(buffer, "case") == 0) {
                add_token("CASE", buffer);
            } else if (strcmp(buffer, "stop") == 0 || strcmp(buffer, "break") == 0) {
                add_token("STOP", buffer);
            } else if (strcmp(buffer, "include") == 0) {
                add_token("INCLUDE", buffer);
            } else if (strcmp(buffer, "default") == 0) {
                add_token("DEFAULT", buffer);
            } else if (strcmp(buffer, "enum") == 0) {
                add_token("ENUM", buffer);
            } else if (strcmp(buffer, "where") == 0) {
                add_token("WHERE", buffer);
            } else if (strcmp(buffer, "gettype") == 0) {
                add_token("GETTYPE", buffer);
            } else if (strcmp(buffer, "open_file") == 0) {
                add_token("OPEN_FILE", buffer);
            } else if (strcmp(buffer, "close_file") == 0) {
                add_token("CLOSE_FILE", buffer);
            } else if (strcmp(buffer, "memory") == 0) {
                add_token("MEMORY", buffer);
            } else if (strcmp(buffer, "using") == 0) {
                add_token("USING", buffer);
            } else if(strcmp(buffer,"write_file") == 0) {
                add_token("FILE_WRITE", buffer);
            } else if (strcmp(buffer,"create_file") == 0){
                add_token("CREATE_FILE", buffer);
            } else if (strcmp(buffer,"dream") == 0){
                add_token("DREAM", buffer);
            } else if (strcmp(buffer,"gotoxy") == 0) {
            	add_token("GOTOXY",buffer);
            } else if (strcmp(buffer, "clear") == 0) {
                add_token("CLEAR", buffer);
            } else if (strcmp(buffer, "empty") == 0) {
                add_token("EMPTY", buffer);
            } else if (strcmp(buffer, "true") == 0) {
                add_token("TRUE", buffer);
            } else if (strcmp(buffer, "false") == 0) {
                add_token("FALSE", buffer);
            } else if (strcmp(buffer, "remove") == 0) {
                add_token("REMOVE", buffer);
            } else if (strcmp(buffer, "get_size") == 0) {
                add_token("GETSIZE", buffer);
            } else if (strcmp(buffer, "strcmp") == 0) {
                add_token("STRCMP", buffer);
            } else if (strcmp(buffer, "strlen") == 0) {
                add_token("STRLEN", buffer);
            } else if (strcmp(buffer, "cmp") == 0) {
                add_token("CMP", buffer);
            } else if (strcmp(buffer, "strsearch") == 0) {
                add_token("STRSEARCH", buffer);
            } else if (strcmp(buffer, "is") == 0) {
                add_token("IS", buffer);
            } else if (strcmp(buffer, "strrev") == 0) {
                add_token("STRREV", buffer);
            } else if (strcmp(buffer, "properties") == 0) {
                add_token("PROPERTIES", buffer);
            } else if (strcmp(buffer, "priv") == 0 || strcmp(buffer, "pub") == 0) {
                add_token("ACESS", buffer);
            } else if (strcmp(buffer, "setColorText") == 0) {
                add_token("SETCOLORTEXT", buffer);
            } else if (strcmp(buffer, "length") == 0) {
                add_token("LENGTH", buffer);
            } else if (strcmp(buffer, "at") == 0) {
                add_token("AT", buffer);
            } else if (strcmp(buffer, "string") == 0 || strcmp(buffer, "str") == 0) {
                add_token("STRING_PARAM", buffer);
            } else if (strcmp(buffer, "integer") == 0 || strcmp(buffer, "int") == 0) {
                add_token("INTEGER_PARAM", buffer);
            } else if (strcmp(buffer, "float") == 0) {
                add_token("FLOAT_PARAM", buffer);
            } else if (strcmp(buffer, "char") == 0) {
                add_token("CHAR_PARAM", buffer);
            } else if (strcmp(buffer, "add_code") == 0) {
                add_token("ADD_CODE", buffer);
            } else if (strcmp(buffer, "bool") == 0 || strcmp(buffer, "boolean") == 0){
            	add_token("BOOL_PARAM", buffer);
            } else if (strcmp(buffer, "wait") == 0 || strcmp(buffer, "empty") == 0){
            	add_token("WAIT_PARAM", buffer);
            } else if (strcmp(buffer, "const") == 0){
            	add_token("CONST_PARAM", buffer);
            } else if (strcmp(buffer, "input") == 0){
            	add_token("INPUT", buffer);
            } else {
                add_token("IDENTIFIER", buffer);
            }
            buffer_pos = 0;
        } else if (isdigit(code[i]) || (code[i] == '.' && isdigit(code[i+1]))) {
            buffer[buffer_pos++] = code[i];
            while (isdigit(code[i+1]) || code[i+1] == '.') {
                buffer[buffer_pos++] = code[++i];
            }
            buffer[buffer_pos] = '\0';
            add_token(strchr(buffer, '.') ? "FLOAT" : "NUMBER", buffer);
            buffer_pos = 0;
        } else if (code[i] == '"') {
            buffer[buffer_pos++] = code[i];
            i++;
            while (code[i] != '"' || (code[i] == '"' && code[i-1] == '\\')) {
                if (code[i] == '\0') {
                    printf("Error: Unterminated string\n");
                    exit(1);
                }
                buffer[buffer_pos++] = code[i++];
            }
            buffer[buffer_pos++] = code[i];
            buffer[buffer_pos] = '\0';
            add_token("STRING", unescape_string(buffer));
            buffer_pos = 0;
        } else if (code[i] == '\'') {
            buffer[buffer_pos++] = code[i];
            i++;
            if (code[i] == '\\' || code[i] == '@') {
                buffer[buffer_pos++] = code[i++];
            }
            if (code[i] != '\0') {
                buffer[buffer_pos++] = code[i++];
            }
            if (code[i] == '\'') {
                buffer[buffer_pos++] = code[i];
            }
            buffer[buffer_pos] = '\0';
            add_token("CHAR", unescape_string(buffer));
            buffer_pos = 0;
        } else if (strchr("+-*/=(),<>:.[]{}", code[i]) != NULL) {
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
                      code[i] == '[' || code[i] == ']' ? "BRACKET" :
                      code[i] == '{' || code[i] == '}' ? "BRACE" :
                      code[i] == ',' ? "COMMA" :
                      code[i] == ':' ? "COLON" :
                      code[i] == '.' ? "DOT" : "OPERATOR", buffer);
        } else if (code[i] == ' ' || code[i] == '\t' || code[i] == '\n') {
        }else {
            printf("Error: Unexpected character '%c'\n", code[i]);
            exit(1);
        }
    }
}

void removeQuotes(const char *source, char *dest) {
    while (*source) {
        if (*source != '\"') {
            *dest++ = *source;
        }
        source++;
    }
    *dest = '\0';
}