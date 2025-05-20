#include "../include/str.h"
#include "../include/error.h"
#include "../include/variable.h"

void print (){
  expected("(", "puts", "L_PAREN");
  if(verify_token("STRING")){
    char str[MAX_TOKEN_LENGTH];
    removeQuotes(get_token_value(), str);
    printf("%s", str);
  } else if (verify_token("NUMBER")){
    printf("%s", get_token_value());
  } else if (verify_token("FLOAT")){
    printf("%.2f", atof(get_token_value()));
  } else if (verify_token("CHAR")){
    printf("%c", get_char_token_value());
  }
  current_token++;
  expected(")", "puts", "R_PAREN");
  current_token++;
}

