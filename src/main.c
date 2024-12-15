#include "../include/definitions.h"
#include "../include/token.h"
#include "../include/variable.h"
#include "../include/function.h"
#include "../include/evaluate.h"
#include "../include/interpret.h"

int main() {
    FILE * fp = fopen("main.ty","r");
    if(fp == NULL){
    	perror("");
    	return -1;
    }
    char code[1024];
    fread(code, sizeof(char), 1024, fp);
    fclose(fp);
    tokenize(code);
    interpret();
    return 0;
}
