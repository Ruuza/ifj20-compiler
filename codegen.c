#include <stdio.h>
#include "codegen.h"
#include "string.h"
#include "symtable.h"

FILE* code_output_file;

int set_code_output(FILE *file) {
    if (file == NULL){
        return -1;
    }
    code_output_file = file;
    return 0;
}

int generate_header() {
    fprintf(code_output_file, ".IFJcode20\n");
    fprintf(code_output_file, "JUMP $$main\n\n");
    return 0;
}

int generate_func_top(char* function_identifier){
    if (strcmp(function_identifier, "main") == 0){
        fprintf(code_output_file, "LABEL $$main\n");
        fprintf(code_output_file, "CREATEFRAME\n");
    } else {
        fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    }
    fprintf(code_output_file, "PUSHFRAME\n");
    return 0;
}

int generate_func_param(char* param_identifier, int param_pos){
    fprintf(code_output_file, "DEFVAR LF@%s\n", param_identifier);
    fprintf(code_output_file,"MOVE LF@%s  LF@%%%d\n", param_identifier, param_pos);
    return 0;
}

int generate_return_values(Symtable_item* function){
    char* ifjcode_type;
    char* empty_value;
    for (int i = 0; i < function->return_values_count;i++){
        switch (function->dataType[i]) {
            case DT_VOID:
                ifjcode_type = "nil";
                empty_value = "nil";
                break;
            case DT_STRING:
                ifjcode_type = "string";
                empty_value = "\"\"";
                break;
            case DT_INT:
                ifjcode_type = "int";
                empty_value = "0";
                break;
            case DT_FLOAT:
                ifjcode_type = "float";
                empty_value = "0x0.0p+0";
                break;
        }
        fprintf(code_output_file,"%s%d%s", "DEFVAR LF@%retval", i+1, "\n");
        fprintf(code_output_file, "MOVE LF@%%retval%d %s@%s\n", i+1, ifjcode_type, empty_value);
    }
    return 0;
}

int generate_func_bottom(char* function_identifier){
    if (strcmp(function_identifier, "main") == 0){
        fprintf(code_output_file, "# end of main body\n");
        return 0;
    }
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n\n");
    return 0;
}