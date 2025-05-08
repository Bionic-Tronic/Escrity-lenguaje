#include "../include/errors.h"

void show_errors (const char * msg, int code){
	printf("\033[31m");
	printf("Error: %s in line: %i\n",msg,current_line);
	printf("\033[0m");
	exit(code);
}

void white_color (){
	printf("\033[0m");
}

void red_color (){
	printf("\033[31m");
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
	printf("\033[31m");
	printf("Error: Function '%s' expects %d arguments, but got %d\n", name, params, args);
    printf("\033[0m");
    exit(1);
}


void undefined_member (const char * member, const char * in, int code){
	red_color();
	printf("Undefined member '%s' name in '%s'",member,in);
	white_color();
	exit(code);
}
