#include "../include/interpret.h"
#include "../include/error.h"
#include "../include/array.h"
#include "../include/variable.h"
#include "../include/enum.h"
#include "../include/protocol.h"
#include "../include/properties.h"
#include "../include/object.h"
#include "../include/function.h"
#include "../include/str.h"

void * evaluate_term(){
  void * left = evaluate_factor();
  int l = (int)left;
  while (current_token < token_count && strcmp(tokens[current_token].type, "OPERATOR") == 0 && (tokens[current_token].value[0] == '*' || tokens[current_token].value[0] == '/')){
    char op = tokens[current_token].value[0];
    current_token++;
    void * right = evaluate_factor();
    int r = (int)right;
    if (op == '*')
      l *= r;
    else{
      if (right == 0)
        simple_error_msg("Division by zero",ERROR);
      l /= r;
    }
  }
return left;
}

void * evaluate_expression(){
  if (current_token + 2 < token_count &&
                strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
                strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
                tokens[current_token + 1].value[0] == '='){
    char var_name[MAX_TOKEN_LENGTH];
    strcpy(var_name, tokens[current_token].value);
    current_token += 2;
    char var_str[MAX_TOKEN_LENGTH * 2];
    if (strcmp(tokens[current_token].type, "NUMBER") == 0){
      int int_v = atoi(tokens[current_token].value);
      set_variable(var_name, "int", &int_v);
    } else if (strcmp(tokens[current_token].type, "STRING") == 0){
      removeQuotes(tokens[current_token].value, var_str);
      set_variable(var_name, "string", var_str);
    } else if (strcmp(tokens[current_token].type, "CHAR") == 0){
      int char_ = tokens[current_token].value[1];
      set_variable(var_name, "char", &char_);
    } else if (strcmp(tokens[current_token].type, "FLOAT") == 0){
      double float_v = atof(tokens[current_token].value);
      set_variable(var_name, "float", &float_v);
    } else if (strcmp(tokens[current_token].type, "TRUE") == 0){
      int true_v = 1;
      set_variable(var_name, "bool", &true_v);
    } else if (strcmp(tokens[current_token].type, "FALSE") == 0){
      int false_v = 0;
      set_variable(var_name, "bool", &false_v);
    } else if (strcmp(tokens[current_token].type, "EMPTY") == 0)
      set_variable(var_name, "empty", NULL);
    else{
      void * returning_value = evaluate_expression();
      set_variable(var_name, type_value_return, &returning_value);
    }
    void * v = evaluate_expression();
    return v;
  }
  int left = (int)evaluate_term();
  while (current_token < token_count &&
      strcmp(tokens[current_token].type, "OPERATOR") == 0 &&
      (tokens[current_token].value[0] == '+' || tokens[current_token].value[0] == '-' ||
      strcmp(tokens[current_token].value, "==") == 0 || strcmp(tokens[current_token].value, "<") == 0 ||
      strcmp(tokens[current_token].value, ">") == 0 || strcmp(tokens[current_token].value, "<=") == 0 ||
      strcmp(tokens[current_token].value, ">=") == 0 || strcmp(tokens[current_token].value, "!=") == 0)){
    char op[3];
    strcpy(op, tokens[current_token].value);
    current_token++;
    int right = (int)evaluate_term();
    if (strcmp(op, "+") == 0) left += right;
    else if (strcmp(op, "-") == 0) left -= right;
    else if (strcmp(op, "*") == 0) left *= right;
    else if (strcmp(op, "/") == 0) left /= right;
    else if (strcmp(op, "==") == 0) left = (right == left);
    else if (strcmp(op, "<") == 0) left = (left < right);
    else if (strcmp(op, ">") == 0) left = (left > right);
    else if (strcmp(op, "<=") == 0) left = (left <= right);
    else if (strcmp(op, ">=") == 0) left = (left >= right);
    else if (strcmp(op, "!=") == 0) left = (left != right);
  }
  return (void *)left;
}

void * evaluate_factor(){
    Token token = tokens[current_token];
  current_token++;
  if (strcmp(token.type, "NUMBER") == 0) return (void *)atoi(token.value);
  else if (strcmp(token.type, "FLOAT") == 0) {
    float float_value = atof(token.value);
    void * float_return = &float_value;
    return float_return;
  } else if (strcmp(token.type, "IDENTIFIER") == 0)
    {
        if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0)
        {
            Function *func = get_function(token.value);
            if (func == NULL)
                notDefined(token.value, "function", ERROR_INTERPRETER);
            current_token++;
            void * args[MAX_PARAMS];
            int arg_count = 0;
            while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0)
            {
                args[arg_count++] = evaluate_expression();
                if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
                    current_token++;
            }
            if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
            {
                printf("Error: Expected ')' after function arguments\n");
                exit(1);
            }
            current_token++;
            return call_function(func, &args, arg_count);
        }
        else if (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") == 0 && strcmp(tokens[current_token].value, "[") == 0)
        {
            void *value = acess_array();
            return value;
        }
        else if (current_token < token_count && strcmp(tokens[current_token].type, "DOT") == 0)
        {
            Object * s = get_object(token.value);
            Enum * e = get_enum(token.value);
            Properties *p = get_properties(token.value);
            Values * valu = get_values(token.value);
            Vector * vectori = NULL;//get_vector(token.value);
            Protocol *protocolito = get_protocol(token.value);
            if (vectori != NULL)
            {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                {
                    printf("Error: Expected vector value after '.'\n");
                    exit(1);
                }
                char *member_name = tokens[current_token].value;
                for (int i = 0; i < vectori->member_count; i++)
                {
                    if (strcmp(vectori->members[i].name, member_name) == 0)
                    {
                        return &vectori->members[i].value.int_value;
                    }
                }
            }
            //Protocolo
            else if (protocolito != NULL)
            {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER"))
                {
                    undefined_member(get_token_value(), "properties", ERROR_INTERPRETER);
                }
                char *member_name = tokens[current_token].value;
                if (protocolito->type_protocol == 0)
                {
                    evalue_http_protocol(protocolito, member_name);
                }
                else
                {
                    show_errors("Undifined type protocol", ERROR_SINTAXIS);
                }
            }
            //Objects
            else if (s != NULL)
            {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                {
                    undefined_member(get_token_value(), "object", ERROR_INTERPRETER);
                }
                char *member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < s->member_count; i++)
                {
                    if (strcmp(s->members[i].name, member_name) == 0)
                    {
                        if (strcmp(s->members[i].type, "int") == 0)
                            return &s->members[i].value.int_value;
                        else if (strcmp(s->members[i].type, "float") == 0)
                            return &s->members[i].value.float_value;
                        else if (strcmp(s->members[i].type, "char") == 0)
                            return &s->members[i].value.char_value;
                        else if (strcmp(s->members[i].type, "function") == 0)
                        {
                            Function *func = s->members[i].value.func_value;
                            void * args[MAX_PARAMS];
                            int arg_count = 0;
                            if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0)
                            {
                                current_token++;
                                while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0)
                                {
                                    args[arg_count++] = evaluate_expression();
                                    if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
                                        current_token++;
                                }
                                if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
                                {
                                    printf("Error: Expected ')' after function arguments\n");
                                    exit(1);
                                }
                                current_token++;
                            }
                            return call_function(func, &args, arg_count);
                        }
                        else
                        {
                            notEvaluateStructure(member_name, "object", token.value, ERROR_INTERPRETER);
                        }
                    }
                }
                printf("Error: Member '%s' not found in object '%s'\n", member_name, token.value);
                exit(1);
            }
            //enums
            else if (e != NULL)
            {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                {
                    printf("Error: Expected enum value after '.'\n");
                    exit(1);
                }
                char *enum_value = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < e->member_count; i++)
                {
                    if (strcmp(e->members[i], enum_value) == 0)
                        return &i;
                }
                printf("Error: Enum value '%s' not found in enum '%s'\n", enum_value, token.value);
                exit(1);
            }
            //Values
            else if (valu != NULL)
            {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                {
                    printf("Error: Expected member name after '.'\n");
                    exit(1);
                }
                char *member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < valu->member_count; i++)
                {
                    if (strcmp(valu->members[i].name, member_name) == 0)
                    {
                        if (strcmp(valu->members[i].type, "int") == 0)
                            return &valu->members[i].value.int_value;
                        else if (strcmp(s->members[i].type, "float") == 0)
                            return &valu->members[i].value.float_value;
                        else if (strcmp(s->members[i].type, "char") == 0)
                            return &valu->members[i].value.char_value;
                        else
                        {
                            printf("Error: Cannot evaluate member '%s' of values '%s'\n", member_name, token.value);
                            exit(1);
                        }
                    }
                }
                printf("Error: Member '%s' not found in values '%s'\n", member_name, token.value);
                exit(1);
            }
            //Properties
            else if (p != NULL)
            {
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                {
                    printf("Error: Expected member name after '.'\n");
                    exit(1);
                }
                char *member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < p->member_count; i++)
                {
                    if (strcmp(p->members[i].name, member_name) == 0)
                    {
                        if (p->visible[i] == 1)
                        {
                            if (strcmp(tokens[current_token++].type, "IDENTIFIER") != 0)
                            {
                                if (strcmp(tokens[current_token].type, "NUMBER") == 0)
                                {
                                    strcpy(p->members[i].type, "int");
                                    p->members[i].value.int_value = atoi(tokens[current_token].value);
                                }
                                else if (strcmp(tokens[current_token].type, "FLOAT") == 0)
                                {
                                    strcpy(p->members[i].type, "float");
                                    p->members[i].value.float_value = atof(tokens[current_token].value);
                                }
                                else if (strcmp(tokens[current_token].type, "STRING") == 0)
                                {
                                    strcpy(p->members[i].type, "string");
                                    strcpy(p->members[i].value.string_value, tokens[current_token].value);
                                }
                                else if (strcmp(tokens[current_token].type, "CHAR") == 0)
                                {
                                    strcpy(p->members[i].type, "char");
                                    p->members[i].value.char_value = tokens[current_token].value[1];
                                }
                            }
                            if (current_token + 1 < token_count &&
                                strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
                                strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
                                tokens[current_token + 1].value[0] == '=')
                            {
                            }
                            if (strcmp(p->members[i].type, "int") == 0)
                                return &p->members[i].value.int_value;
                            else if (strcmp(p->members[i].type, "float") == 0)
                                return &p->members[i].value.float_value;
                            else if (strcmp(p->members[i].type, "char") == 0)
                                return &p->members[i].value.char_value;
                            else if (strcmp(p->members[i].type, "function") == 0)
                            {
                                Function * func = p->members[i].value.func_value;
                                void * args[MAX_PARAMS];
                                int arg_count = 0;
                                if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0)
                                {
                                    current_token++;
                                    while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0)
                                    {
                                        args[arg_count++] = evaluate_expression();
                                        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
                                            current_token++;
                                    }
                                    if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0)
                                    {
                                        printf("Error: Expected ')' after function arguments\n");
                                        exit(1);
                                    }
                                    current_token++;
                                }
                                return call_function(func, &args, arg_count);
                            }
                            else
                            {
                                printf("Error: Cannot evaluate member '%s' of propeties '%s'\n", member_name, token.value);
                                exit(1);
                            }
                        }
                        else
                        {
                            printf("Error: Cannot access at member '%s' of properties '%s' this is a 'priv'\n", member_name, token.value);
                            exit(1);
                        }
                    }
                }
                printf("Error: Member '%s' not found in propeties'%s'\n", member_name, token.value);
                exit(1);
            }
            else
            {
                printf("Error: '%s' is not a object, enum, properties or values\n", token.value);
                exit(1);
            }
        }
        else
        {
            Variable * var = get_variable(token.value);
            if (var == NULL)
            {
                printf("Error: Variable '%s' not defined\n", token.value);
                exit(1);
            }
            if (strcmp(var->type, "int") == 0)
                return &var->value.int_value;
            if (strcmp(var->type, "string") == 0)
                return &var->value.string_value;
            if (strcmp(var->type, "char") == 0)
                return &var->value.char_value;
            else if (strcmp(var->type, "float") == 0)
                return &var->value.float_value;
            else if (strcmp(token.type, "CHAR") == 0)
            {
                return &var->value.char_value;
            }
            else if (strcmp(token.type, "STRING") == 0)
            {
                char result_string[MAX_TOKEN_LENGTH];
                removeQuotes(token.value, result_string);
                return &result_string;
            }
            else if (strcmp(var->type, "enum") == 0)
                return &var->value.enum_value;
            else
            {
                printf("Error: Cannot evaluate variable '%s' of type '%s'\n", token.value, var->type);
                exit(1);
            }
        }
    }
    else if (strcmp(token.type, "GET_INPUT") == 0)
    {
        void * value;
        expected("(", "getInput", "PAREN");
        if (strcmp(tokens[current_token].type, "STRING_PARAM") == 0)
        {
            char str[MAX_TOKEN_LENGTH];
            scanf("%s", &str);
            current_token++;
            expected(")", "getInput", "PAREN");
            value = &str;
            type_value_return = "string";
        }
        else if (strcmp(tokens[current_token].type, "INTEGER_PARAM") == 0)
        {
            int int_value;
            scanf("%i", &int_value);
            current_token++;
            expected(")", "getInput", "PAREN");
            value = &int_value;
            type_value_return = "int";
        }
        else if (strcmp(tokens[current_token].type, "FLOAT_PARAM") == 0)
        {
            float float_value;
            scanf("%f", &float_value);
            current_token++;
            expected(")", "getInput", "PAREN");
            value = &float_value;
            type_value_return = "float";
        }
        else
        {
            show_errors("Invalid type param in getInput", 1);
        }
        return value;
    } else if(strcmp(token.type, "PUTS") == 0) {
        print();
    return 0;
  } else unexpected_token(token.value,current_line,ERROR);
}
