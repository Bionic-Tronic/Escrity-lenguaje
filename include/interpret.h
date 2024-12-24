#ifndef INTERPRET_H
#define INTERPRET_H

#define MAX_TOKEN_LENGTH 1000
#define MAX_TOKENS 1000
#define MAX_VARIABLES 1000
#define MAX_FUNCTIONS 1000
#define MAX_PARAMS 1000
#define MAX_BODY_TOKENS 1000
#define MAX_INPUT_LENGTH 1000
#define MAX_STRUCT_MEMBERS 1000
#define MAX_STRUCTS 1000
#define MAX_ARRAY_SIZE 100
#define MAX_ENUMS 50
#define MAX_ENUM_MEMBERS 20
#define MAX_ARRAY_MEMBERS 50

#include <stdbool.h>
#include <time.h>
#include <string.h>

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
        char char_value;
        struct Struct* struct_value;
        Function* func_value;
        int* array_value;
        int array_size;
        int enum_value;
    } value;
} Variable;

struct Function {
    char name[MAX_TOKEN_LENGTH];
    char params[MAX_PARAMS][MAX_TOKEN_LENGTH];
    int param_count;
    Token body[MAX_BODY_TOKENS];
    int body_token_count;
};

typedef struct Struct {
    char name[MAX_TOKEN_LENGTH];
    Variable members[MAX_STRUCT_MEMBERS];
    int member_count;
} Struct;

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

void add_token(const char* type, const char* value);
void tokenize(const char* code);
Variable* get_variable(const char* name);
int get_type_variable (const char * name);
void set_variable(const char* name, const char* type, void* value);
int evaluate_expression();
Function* get_function(const char* name);
Struct* get_struct(const char* name);
int call_function(Function* func, int args[], int arg_count);
Array* get_array(const char* name);
void define_array();
Enum* get_enum(const char* name);
void define_enum();
char* unescape_string(const char* input);
void removeQuotes(const char *source, char *dest);
int evaluate_factor();
int evaluate_term();
void define_function();
void define_struct();
void interpret_if_statement();
void interpret();
void interpret_line(const char* line);

#endif
