#ifndef FUNCTION_H
#define FUNCTION_H

#include "interpret.h"
#include "errors.h"

void define_function();
Function* get_function(const char* name);
void * call_function(Function* func, void * args[], int arg_count);

#endif
