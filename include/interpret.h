#ifndef INTERPRET_H
#define INTERPRET_H

#include <stdbool.h>
#include <time.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 1000
#define MAX_TOKENS 1000
#define MAX_VARIABLES 1000
#define MAX_FUNCTIONS 1000
#define MAX_PARAMS 1000
#define MAX_BODY_TOKENS 1000
#define MAX_INPUT_LENGTH 1000
#define MAX_STRUCT_MEMBERS 1000
#define MAX_PROPERTIES_MEMBERS 1000
#define MAX_STRUCTS 1000
#define MAX_PROPERTIES 1000
#define MAX_ARRAY_SIZE 100
#define MAX_ENUMS 50
#define MAX_ENUM_MEMBERS 20
#define MAX_ARRAY_MEMBERS 50
#define MAX_FILE_READ 5200
#define MAX_STR 1024

#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <conio.h>
#include <sys/stat.h>

typedef struct {
    char type[20];
    char value[MAX_TOKEN_LENGTH];
} Token;

typedef struct Function Function;
typedef struct Array Array;

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
        void * wait_value;
    } value;
    int is_wait;
} Variable;

struct Function {
    char name[MAX_TOKEN_LENGTH];
    char params[MAX_PARAMS][MAX_TOKEN_LENGTH];
    char params_type[MAX_PARAMS][MAX_TOKEN_LENGTH];
    int param_count;
    Token body[MAX_BODY_TOKENS];
    int body_token_count;
};

typedef struct Struct {
    char name[MAX_TOKEN_LENGTH];
    Variable members[MAX_STRUCT_MEMBERS];
    int member_count;
} Struct;

typedef struct {
	char name[MAX_TOKEN_LENGTH];
	Variable members[MAX_PROPERTIES_MEMBERS];
	int member_count;
	int visible[MAX_PROPERTIES_MEMBERS];
}Properties;

struct Array {
    char name[MAX_TOKEN_LENGTH];
    Variable members[MAX_ARRAY_MEMBERS];
    int member_count;
};

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char members[MAX_ENUM_MEMBERS][MAX_TOKEN_LENGTH];
    int member_count;
} Enum;

typedef char * string;
typedef const char * String;

extern Token tokens[MAX_TOKENS];
extern int token_count;
extern int current_token;
extern int current_line;

extern Variable variables[MAX_VARIABLES];
extern int variable_count;

extern Function functions[MAX_FUNCTIONS];
extern int function_count;

extern Struct structs[MAX_STRUCTS];
extern int struct_count;

extern struct Array arrays[MAX_ARRAY_SIZE];
extern int array_count;

extern Enum enums[MAX_ENUMS];
extern int enum_count;

extern Properties pro[MAX_PROPERTIES];
extern int properties_count;

void add_token(const char* type, const char* value);
void tokenize(const char* code);
Variable* get_variable(const char* name);
int get_type_variable (const char * name);
void set_variable(const char* name, const char* type, void* value);
double evaluate_expression();
Function* get_function(const char* name);
Struct* get_struct(const char* name);
int call_function(Function* func, void * args[], int arg_count);
Array* get_array(const char* name);
void define_array();
Enum* get_enum(const char* name);
void define_enum();
char* unescape_string(const char* input);
void removeQuotes(const char *source, char *dest);
double evaluate_factor();
double evaluate_term();
void define_function();
void define_struct();
void interpret_if_statement();
void interpret();
void interpret_line(const char* line);
void files_write_file( const char* filename, const char* content);
void show_version ();
void define_properties ();
Properties * get_properties (const char* name);
void show_errors (const char * msg, int code);
void expect_token(const char *expected_type, const char *expected_value);

#endif
