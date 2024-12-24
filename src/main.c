#include "../include/interpret.h"
#include <stdio.h>

#define MAX_FILE 1024

int main(){
	FILE *fp = fopen("main.ty", "r");
	if (fp == NULL){
		perror("Error");
		return -1;
	}
	char code[MAX_FILE];
	fread(code, sizeof(char), MAX_FILE, fp);
	fclose(fp);
	tokenize(code);
	interpret();
	return 0;
}