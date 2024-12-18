#ifndef ENUM_H
#define ENUM_H

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char members[MAX_ENUM_MEMBERS][MAX_TOKEN_LENGTH];
    int member_count;
} Enum;

Enum enums[MAX_ENUMS];
int enum_count = 0;

Enum* get_enum(const char* name) {
    for (int i = 0; i < enum_count; i++) {
        if (strcmp(enums[i].name, name) == 0) {
            return &enums[i];
        }
    }
    return NULL;
}

void define_enum() {
    if (enum_count >= MAX_ENUMS) {
        printf("Error: Too many enums\n");
        exit(1);
    }
    Enum* e = &enums[enum_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
        printf("Error: Expected enum name\n");
        exit(1);
    }
    strcpy(e->name, tokens[current_token].value);
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) {
        printf("Error: Expected 'then' after enum name\n");
        exit(1);
    }
    current_token++;
    e->member_count = 0;
    while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
        if (strcmp(tokens[current_token].type, "IDENTIFIER") != 0) {
            printf("Error: Expected enum member name\n");
            exit(1);
        }
        strcpy(e->members[e->member_count], tokens[current_token].value);
        e->member_count++;
        current_token++;
        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0) {
            current_token++;
        }
    }
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) {
        printf("Error: Expected 'end' at the end of enum\n");
        exit(1);
    }
    current_token++;
}

#endif
