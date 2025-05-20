#ifndef _ERROR_H
#define _ERROR_H

#include "interpret.h"

void expected(const char *paren, const char *function, const char *token);
void undefined_variable (const char * var_name, const char * in, int error);
void show_errors (const char * msg, int code);
void unexpected_token (const char * token, int line, int code);
void unexpected_character (const char character, int codd);
void error_funcs_args (const char * name, int params, int args);
void undefined_member (const char * member, const char * in, int code);
void invalid_var (const char * name, int code);
void notEvaluateVariable (const char * variable, const char * type, int code);
void simple_error_msg (const char * msg, int code);
void notDefined (const char * name, const char * token, int code);
void notEvaluateStructure (const char * member, const char * structure, const char * name, int code);

#endif

