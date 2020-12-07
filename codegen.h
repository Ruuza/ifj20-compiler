#ifndef IFJ20C_CODEGEN_H
#define IFJ20C_CODEGEN_H

#include <stdio.h>
#include "symtable.h"

int set_code_output(FILE* file);

int generate_header();

int generate_func_top(char* function_identifier);

int generate_func_param(char* param_identifier, int param_pos);

int generate_return_values(Symtable_item* function);

int generate_func_bottom(char* function_identifier);

int generate_move(char* dest_frame, char* identifier, char* source_frame, char* source);

int generate_return_move(char* identifier, int position);

int generate_declaration(char* dest_frame, char* identifier);

int generate_frame();

int generate_push(char* dest_frame, char* variable);

int generate_function_call(char * function_name);

int generate_arithmetic_operation_string(Token_type operation, char* result, char* lhs, char* rhs);

int generate_arithmetic_operation_int(Token_type operation, char* result, char* lhs, char* rhs);

int generate_arithmetic_operation_float(Token_type operation, char* result, char* lhs, char* rhs);
#endif //IFJ20C_CODEGEN_H
