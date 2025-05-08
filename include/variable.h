#ifndef VARIABLE_H
#define VARIABLE_H

#include "interpret.h"
#include "errors.h"

char * get_type_variable (const char * name);
void set_variable(const char* name, const char* type, void* value);
Variable* get_variable(const char* name);

#endif
