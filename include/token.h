#ifndef TOKEN_H
#define TOKEN_H

typedef struct {
    char type[20];
    char value[MAX_TOKEN_LENGTH];
} Token;

Token tokens[MAX_TOKENS];
int token_count = 0;
int current_token = 0;

void add_token(const char* type, const char* value) {
    strcpy(tokens[token_count].type, type);
    strcpy(tokens[token_count].value, value);
    token_count++;
}

void tokenize(const char* code) {
    char buffer[MAX_TOKEN_LENGTH];
    int buffer_pos = 0;
    
    for (int i = 0; code[i] != '\0'; i++) {
        if (isalpha(code[i]) || code[i] == '_') {
            buffer[buffer_pos++] = code[i];
            while (isalnum(code[i+1]) || code[i+1] == '_') {
                buffer[buffer_pos++] = code[++i];
            }
            buffer[buffer_pos] = '\0';
            
            if (strcmp(buffer, "text") == 0 || strcmp(buffer, "print") == 0) {
                add_token("PRINT", buffer);
            } else if (strcmp(buffer, "resolver") == 0) {
                add_token("RESOLVER", buffer);
            } else if (strcmp(buffer, "func") == 0 || strcmp(buffer, "function") == 0) {
                add_token("FUNC", buffer);
            } else if (strcmp(buffer, "then") == 0 || strcmp(buffer, "do") == 0) {
                add_token("THEN", buffer);
            } else if (strcmp(buffer, "end") == 0) {
                add_token("END", buffer);
            } else if (strcmp(buffer, "return") == 0) {
                add_token("RETURN", buffer);
            } else if (strcmp(buffer, "if") == 0) {
                add_token("IF", buffer);
            } else if (strcmp(buffer, "else") == 0) {
                add_token("ELSE", buffer);
            } else if (strcmp(buffer, "for") == 0) {
                add_token("FOR", buffer);
            } else if (strcmp(buffer, "while") == 0) {
                add_token("WHILE", buffer);
            } else if (strcmp(buffer, "set") == 0 || strcmp(buffer, "var") == 0) {
                add_token("SET", buffer);
            } else if (strcmp(buffer, "in_range") == 0 || strcmp(buffer, "wait") == 0) {
                add_token("IN_RANGE", buffer);
            } else if (strcmp(buffer, "ascii") == 0) {
            	add_token("ASCII", buffer);
            } else if (strcmp(buffer,"get_int") == 0){
            	add_token("GET_INT", buffer);
            } else if (strcmp(buffer,"get_str") == 0){
            	add_token("GET_STR", buffer);
            } else if (strcmp(buffer,"random") == 0){
            	add_token("RANDOM", buffer);
            } else {
                add_token("IDENTIFIER", buffer);
            }
            buffer_pos = 0;
        } else if (isdigit(code[i])) {
            buffer[buffer_pos++] = code[i];
            while (isdigit(code[i+1])) {
                buffer[buffer_pos++] = code[++i];
            }
            buffer[buffer_pos] = '\0';
            add_token("NUMBER", buffer);
            buffer_pos = 0;
        } else if (code[i] == '"') {
            buffer[buffer_pos++] = code[i];
            i++;
            while (code[i] != '"' && code[i] != '\0') {
                buffer[buffer_pos++] = code[i++];
            }
            if (code[i] == '"') {
                buffer[buffer_pos++] = code[i];
            }
            buffer[buffer_pos] = '\0';
            add_token("STRING", buffer);
            buffer_pos = 0;
        } else if (strchr("+-*/=(),<>", code[i]) != NULL) {
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
                      code[i] == ',' ? "COMMA" : "OPERATOR", buffer);
        } else if (code[i] == ';') {
            add_token("SEMICOLON", ";");
        } else if (code[i] == ' ' || code[i] == '\t' || code[i] == '\n') {
            // Ignore whitespace
        } else {
            printf("Error: Unexpected character '%c'\n", code[i]);
            exit(1);
        }
    }
}

#endif
