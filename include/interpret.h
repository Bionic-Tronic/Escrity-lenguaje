#ifndef INTERPRET_H
#define INTERPRET_H

#define MAX_TOKEN_LENGTH 1000
#define MAX_TOKENS 1000
#define MAX_VARIABLES 1000
#define MAX_FUNCTIONS 1000
#define MAX_PARAMS 1000
#define MAX_BODY_TOKENS 1000
#define MAX_INPUT_LENGTH 1000
#define MAX_OBJECT_MEMBERS 1000
#define MAX_PROPERTIES_MEMBERS 1000
#define MAX_VALUES_MEMBERS 1000
#define MAX_VALUES 1000
#define MAX_OBJECTS 1000
#define MAX_PROPERTIES 1000
#define MAX_ARRAY_SIZE 100
#define MAX_ENUMS 50
#define MAX_ENUM_MEMBERS 20
#define MAX_ARRAY_MEMBERS 1000
#define MAX_FILE_READ 1000
#define OK    200
#define BUFFER_SIZE 1000
#define ERROR_UNDEFINED_IDENTIFIER -1
#define ERROR_INTERPRETER -1
#define ERROR_SINTAXIS 1
#define UNDEFINED_ERROR 170
#define ERROR -1

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef _WIN32
#include <conio.h>
#endif
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <ncurses.h>
#include <sys/param.h>

typedef struct {
    char type[20];
    char value[MAX_TOKEN_LENGTH];
} Token;

typedef struct Function Function;
typedef char * string;
typedef const char * String;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char type[20];
    union {
        int int_value;
        double float_value;
        char string_value[MAX_TOKEN_LENGTH];
        int char_value;
        struct Struct* struct_value;
        Function* func_value;
        int* array_value;
        int array_size;
        int enum_value;
        bool bool_value;
    } value;
    int is_wait;
} Variable;

struct Function {
    char name[MAX_TOKEN_LENGTH];
    char params[MAX_PARAMS][MAX_TOKEN_LENGTH];
    char params_type[MAX_PARAMS][MAX_TOKEN_LENGTH];
    int param_count;
    int type_param_count;
    Token body[MAX_BODY_TOKENS];
    int body_token_count;
};

typedef struct Struct {
    char name[MAX_TOKEN_LENGTH];
    Variable members[MAX_OBJECT_MEMBERS];
    int member_count;
} Object;

typedef struct {
	char name[MAX_TOKEN_LENGTH];
	Variable members[MAX_ARRAY_MEMBERS];
	int member_count;
	char type[20];
} Array;

typedef struct {
	char name[MAX_TOKEN_LENGTH];
	Variable members[MAX_PROPERTIES_MEMBERS];
	int member_count;
	int visible[MAX_PROPERTIES_MEMBERS];
}Properties;

typedef struct {
	char name[MAX_TOKEN_LENGTH];
	Variable members[MAX_VALUES_MEMBERS];
	int member_count;
}Values;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char members[MAX_ENUM_MEMBERS][MAX_TOKEN_LENGTH];
    int member_count;
} Enum;

typedef struct {
	char name[MAX_TOKEN_LENGTH];
	Variable members[MAX_ARRAY_MEMBERS];
	int member_count;
} Vector;

typedef struct {
	char name[MAX_TOKEN_LENGTH];
	int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[1024];
    int type_protocol;
    int member_count;
    char protocol_type[MAX_TOKEN_LENGTH];
    char dir[MAX_TOKEN_LENGTH];
    char files_accept[MAX_TOKEN_LENGTH];
    char mimes_accept[MAX_TOKEN_LENGTH];
} Protocol;

extern Token tokens[MAX_TOKENS];
extern int token_count;
extern int current_token;
extern int current_line;
extern char * type_value_return;
extern char * error_msg;

extern Protocol protocol[MAX_VARIABLES];
extern int protocol_count;

extern Variable variables[MAX_VARIABLES];
extern int variable_count;

extern Function functions[MAX_FUNCTIONS];
extern int function_count;

extern Object objects[MAX_OBJECTS];
extern int object_count;

extern Array arrays[MAX_ARRAY_SIZE];
extern int array_count;

extern Enum enums[MAX_ENUMS];
extern int enum_count;

extern Properties pro[MAX_PROPERTIES];
extern int properties_count;

extern char buffer[BUFFER_SIZE];
extern char buff[BUFFER_SIZE];
extern char http_buffer[BUFFER_SIZE];

extern Values values[MAX_VALUES];
extern int values_count;

extern Vector vec[MAX_ARRAY_SIZE];
extern int vector_count;

void add_token(const char* type, const char* value);
void tokenize(const char* code);
void * evaluate_expression();
char * unescape_string(const char* input);
void removeQuotes(const char *source, char *dest);
void * evaluate_factor();
void * evaluate_term();
void interpret_if_statement();
void interpret();
void interpret_line(const char* line);
void files_write_file( const char* filename, const char* content);
bool verify_token_value (const char * token);
bool verify_token (const char * token);
char * get_token_value ();
char * get_token_type ();
int get_int_token_value ();
float get_float_token_value ();
char get_char_token_value ();
char * atos (int i);

#endif

