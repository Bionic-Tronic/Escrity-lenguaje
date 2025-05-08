#ifndef ERRORS_H
#define ERRORS_H

#include "interpret.h"

void expected(const char *paren, const char *function, const char *token);
void undefined_variable (const char * var_name, const char * in, int error);
void show_errors (const char * msg, int code);
void unexpected_token (const char * token, int line, int code);
void unexpected_character (const char character, int codd);
void error_funcs_args (const char * name, int params, int args);
void undefined_member (const char * member, const char * in, int code);
#endif
