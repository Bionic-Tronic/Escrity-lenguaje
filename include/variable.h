#ifndef VARIABLE_H
#define VARIABLE_H

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    int value;
} Variable;

Variable variables[MAX_VARIABLES];
int variable_count = 0;

int get_variable_value(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    printf("Error: Variable '%s' not defined\n", name);
    exit(1);
}

void set_variable_value(const char* name, int value) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            variables[i].value = value;
            return;
        }
    }
    if (variable_count < MAX_VARIABLES) {
        strcpy(variables[variable_count].name, name);
        variables[variable_count].value = value;
        variable_count++;
    } else {
        printf("Error: Too many variables\n");
        exit(1);
    }
}

#endif
