#ifndef ENUMS_H
#define ENUMS_H

#include "interpret.h"

Values * get_values (const char * name);
void define_values ();
Enum * get_enum(const char* name);
void define_enum();

#endif

