#include "../include/error.h"

void white_color (){
	printf("\033[0m");
}

void red_color (){
	printf("\033[31m");
}

void show_errors (const char * msg, int code){
	red_color();
	printf("Error: %s in line: %i\n",msg,current_line);
	white_color();
	exit(code);
}

void simple_error_msg (const char * msg, int code) {
	red_color();
	printf("Error: %s\n",msg);
	white_color();
	exit(code);
}

void expected(const char *paren, const char *function, const char *token){
	char msg[1024];
	snprintf(msg, 1024, "Expected '%s' in %s function", paren, function);
	if (strcmp(tokens[current_token].type, token) != 0)
		show_errors(msg, -1);
	current_token++;
}

void undefined_variable (const char * var_name, const char * in, int code){
	red_color();
	printf("Error: Undefined variable '%s' in '%s'\n",var_name,in);
	white_color();
	exit(code);
}

void unexpected_token (const char * token, int line, int code){
	red_color();
	printf("Error: Unexpected token '%s' in line %i\n", token, line);
	white_color();
	exit(code);
}

void unexpected_character (const char character, int code){
	red_color();
	printf("Error: Unexpected character '%c'\n", character);
    white_color();
    exit(code);
}

void error_funcs_args (const char * name, int params, int args){
	red_color();
	printf("Error: Function '%s' expects %d arguments, but got %d\n", name, params, args);
    white_color();
    exit(1);
}

void undefined_member (const char * member, const char * in, int code){
	red_color();
	printf("Undefined member '%s' name in '%s'",member,in);
	white_color();
	exit(code);
}

void invalid_var (const char * name, int code){
	red_color();
	printf("error: use of undeclared identifier '%s'\n",name);
	white_color();
	exit(code);
}

void notEvaluateStructure (const char * member, const char * structure, const char * name, int code){
	red_color();
	printf("Error: Cannot evaluate member '%s' of %s '%s'\n", member, structure, name);
	white_color();
	exit(code);
}

void notEvaluateVariable (const char * variable, const char * type, int code){
	red_color();
	printf("Error: Cannot evaluate variable '%s' of type '%s'\n", variable, type);
	white_color();
	exit(code);
}

void notDefined (const char * name, const char * token, int code){
	red_color();
	printf("Error: %s '%s' not defined\n", token, name);
	white_color();
	exit(code);
}

