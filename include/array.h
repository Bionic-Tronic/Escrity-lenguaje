#ifndef ARRAY_H
#define ARRAY_H

#include "interpret.h"
#include "variable.h"

ARRAY * get_ARRAY (const char * index);
Vector * get_vector (const char * name);
void define_vector ();
void define_ARRAY ();
Array* get_array(const char* name);
void define_array();
void * evaluate_array_access(Array *arr, const char *key);

#endif
