#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "string.h"
#include "symtable.h"

FILE* code_output_file;
int generate_inputs();
int generate_inputi();
int generate_inputf();
int generate_print();
int generate_int2float(int number);
int generate_float2int(float number);
int generate_len(char* str);
int generate_subst(char* string, int begin, int size);
int generate_ord(char* string, int number);
int generate_chr(int number);

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
    fprintf(code_output_file, "DEFVAR LF@%s%d\n", param_identifier, param_pos);
    fprintf(code_output_file,"MOVE LF@%s%d  LF@%%%d\n", param_identifier, param_pos, param_pos);
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

bool character_ok(int character) { //
    if ((character >= 48 && character <= 57) || (character >= 65 && character <= 90) ||     //0-9, a-z
        (character >= 97 && character <= 122) || (character == 95) || (character == 45) ||  //A-Z, _, -
        (character >= 36 && character <= 38) || (character == 42) || (character == 33) ||   //$,%,&,*,!
        (character == 63)) {                                                                //?
        return true;
    } else {
        return false;
    }
}

int generate_inputs() {
    char *function_identifier = "inputs";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s\n", function_identifier);
    fprintf(code_output_file, "READ LF@%s string\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%%retval1 LF@%s\n", function_identifier);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_inputi() {
    char *function_identifier = "inputi";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s\n", function_identifier);
    fprintf(code_output_file, "READ LF@%s int\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@helpertype\n");
    fprintf(code_output_file, "TYPE LF@helpertype LF@%s\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $end LF@helpertype string@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 LF@%s\n", function_identifier);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    fprintf(code_output_file, "LABEL $end\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@1\n");
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_inputf() {
    char *function_identifier = "inputf";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s\n", function_identifier);
    fprintf(code_output_file, "READ LF@%s float\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@helpertype\n");
    fprintf(code_output_file, "TYPE LF@helpertype LF@%s\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $end LF@helpertype string@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 LF@%s\n", function_identifier);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    fprintf(code_output_file, "LABEL $end\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@1\n");
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_print(int params_count) {     /// NEMA INPUT IBA VYPIS
    char *function_identifier = "print";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    for (int i = 1; i <= params_count; i++) {
        fprintf(code_output_file, "WRITE LF@%s%d\n", function_identifier, i);
    }
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_int2float(int number) {
    char *function_identifier = "int2float";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "DEFVAR LF@%s\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%s int@%d\n", function_identifier, number);
    fprintf(code_output_file, "INT2FLOAT LF@%%retval1 LF@%s\n", function_identifier);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_float2int(float number) {
    char *function_identifier = "float2int";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "DEFVAR LF@%s\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%s float@%a\n", function_identifier, number);
    fprintf(code_output_file, "FLOAT2INT LF@%%retval1 LF@%s\n", function_identifier);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_len(char *str) {
    char *function_identifier = "len";
    char *str_modify = malloc(sizeof(str));
    int helper_counter = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (character_ok(str[i]) != false) { // character_ok=false - nepovoleny znak
            str_modify[helper_counter] = str[i];
            helper_counter++;
        }
    }
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%s string@%s\n", function_identifier, str_modify);
    fprintf(code_output_file, "STRLEN LF@%%retval1 LF@%s\n", function_identifier);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_substr(char *string, int begin, int size) {
    char *function_identifier = "substr";
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s_string\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%s_string string@%s\n", function_identifier, string);
    fprintf(code_output_file, "DEFVAR LF@%s_begin\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%s_begin int@%d\n", function_identifier, begin);
    fprintf(code_output_file, "DEFVAR LF@%s_size\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%s_size int@%d\n", function_identifier, size);
    fprintf(code_output_file, "DEFVAR LF@LT1return\n");
    fprintf(code_output_file, "LT LF@LT1return LF@%s_begin int@0\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@LT1return bool@true\n", function_identifier); //begin < 0
    fprintf(code_output_file, "DEFVAR LF@LT2return\n");
    fprintf(code_output_file, "LT LF@LT2return LF@%s_size int@0\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@LT2return bool@true\n", function_identifier); //size < 0
    fprintf(code_output_file, "DEFVAR LF@LENreturn\n");
    fprintf(code_output_file, "STRLEN LF@LENreturn LF@%s_string\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@GT1return\n");
    fprintf(code_output_file, "GT LF@GT1return LF@%s_begin LF@LENreturn\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@GT1return bool@true\n", function_identifier); //begin>strlen
    fprintf(code_output_file, "DEFVAR LF@LENreturnMinusOne\n");
    fprintf(code_output_file, "SUB LF@LENreturnMinusOne LF@LENreturn int@1\n");
    fprintf(code_output_file, "DEFVAR LF@GT2return\n");   //size > (strlen(string) - 1)
    fprintf(code_output_file, "GT LF@GT2return LF@%s_size LF@LENreturnMinusOne\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $%s_size_bigger LF@GT2return bool@true\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@cycle_counter\n");
    fprintf(code_output_file, "MOVE LF@cycle_counter LF@%s_size\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@s_output\n");
    fprintf(code_output_file, "DEFVAR LF@stringonechar\n");
    fprintf(code_output_file, "GETCHAR LF@stringonechar LF@%s_string LF@%s_begin\n", function_identifier,
            function_identifier);
    fprintf(code_output_file, "ADD LF@%s_begin LF@%s_begin int@1\n", function_identifier, function_identifier);
    fprintf(code_output_file, "MOVE LF@s_output LF@stringonechar\n");
    fprintf(code_output_file, "SUB LF@cycle_counter LF@cycle_counter int@1\n");
    fprintf(code_output_file, "DEFVAR LF@concatenate\n");
    fprintf(code_output_file, "DEFVAR LF@next_round_result\n");
    fprintf(code_output_file, "GT LF@next_round_result LF@cycle_counter int@0\n");
    fprintf(code_output_file, "JUMPIFEQ $%s_cycle LF@next_round_result bool@true\n", function_identifier);
    fprintf(code_output_file, "JUMP $substr_end\n");

    fprintf(code_output_file, "LABEL $%s_cycle\n", function_identifier);
    fprintf(code_output_file, "GETCHAR LF@stringonechar LF@substr_string LF@substr_begin\n");
    fprintf(code_output_file, "CONCAT LF@concatenate LF@s_output LF@stringonechar\n");
    fprintf(code_output_file, "MOVE LF@s_output LF@concatenate\n");
    fprintf(code_output_file, "ADD LF@substr_begin LF@substr_begin int@1\n");
    fprintf(code_output_file, "SUB LF@cycle_counter LF@cycle_counter int@1\n");
    fprintf(code_output_file, "GT LF@next_round_result LF@cycle_counter int@0\n");
    fprintf(code_output_file, "JUMPIFEQ $substr_cycle LF@next_round_result bool@true\n");
    fprintf(code_output_file, "JUMP $substr_end\n");

    fprintf(code_output_file, "LABEL $%s_size_bigger\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@helper\n");
    fprintf(code_output_file, "MOVE LF@helper LF@LENreturn\n");
    fprintf(code_output_file, "DEFVAR LF@LENminusBegin\n");
    fprintf(code_output_file, "SUB LF@LENminusBegin LF@LENreturn LF@substr_begin\n");
    fprintf(code_output_file, "MOVE LF@substr_size LF@LENminusBegin\n");//size=len(str)-begin
    fprintf(code_output_file, "DEFVAR LF@cycle_counter\n");
    fprintf(code_output_file, "MOVE LF@cycle_counter LF@%s_size\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@s_output\n");
    fprintf(code_output_file, "DEFVAR LF@stringonechar\n");
    fprintf(code_output_file, "GETCHAR LF@stringonechar LF@%s_string LF@%s_begin\n", function_identifier,
            function_identifier);
    fprintf(code_output_file, "ADD LF@%s_begin LF@%s_begin int@1\n", function_identifier, function_identifier);
    fprintf(code_output_file, "MOVE LF@s_output LF@stringonechar\n");
    fprintf(code_output_file, "SUB LF@cycle_counter LF@cycle_counter int@1\n");
    fprintf(code_output_file, "DEFVAR LF@concatenate\n");
    fprintf(code_output_file, "DEFVAR LF@next_round_result\n");
    fprintf(code_output_file, "GT LF@next_round_result LF@cycle_counter int@0\n");
    fprintf(code_output_file, "JUMPIFEQ $%s_cycle LF@next_round_result bool@true\n", function_identifier);
    fprintf(code_output_file, "JUMP $substr_end\n");
    fprintf(code_output_file, "LABEL $%s_exit\n", function_identifier);//NEW LABEL EXIT
    fprintf(code_output_file, "MOVE LF@%%retval2 int@1\n");
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    fprintf(code_output_file, "LABEL $%s_end\n", function_identifier);//NEW LABEL END
    fprintf(code_output_file, "MOVE LF@%%retval1 LF@s_output\n");
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_ord(char *string, int number) {
    char *function_identifier = "ord";
    fprintf(code_output_file, "LABEL $%s_exit\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%%retval2 int@1\n");
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s%d\n", function_identifier, 1);
    fprintf(code_output_file, "MOVE LF@%s%d string@%s\n", function_identifier, 1, string);
    fprintf(code_output_file, "DEFVAR LF@%s%d\n", function_identifier, 2);
    fprintf(code_output_file, "MOVE LF@%s%d int@%d\n", function_identifier, 2, number);
    fprintf(code_output_file, "DEFVAR LF@LTreturn\n");
    fprintf(code_output_file, "LT LF@LTreturn LF@%s%d int@0\n", function_identifier, 2);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@LTreturn bool@true\n", function_identifier);
    fprintf(code_output_file, "DEFVAR LF@LENreturn\n");
    fprintf(code_output_file, "STRLEN LF@LENreturn LF@%s%d\n", function_identifier, 1);
    fprintf(code_output_file, "DEFVAR LF@LENminusONE\n");
    fprintf(code_output_file, "SUB LF@LENminusONE LF@LENreturn int@1\n");
    fprintf(code_output_file, "DEFVAR LF@GTreturn\n");
    fprintf(code_output_file, "GT LF@GTreturn LF@%s%d LF@LENminusONE\n", function_identifier, 2);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@GTreturn bool@true\n", function_identifier);
    fprintf(code_output_file, "STRI2INT LF@%%retval1 LF@%s%d LF@%s%d\n", function_identifier, 1, function_identifier,
            2);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}

int generate_chr(int number) {
    char *function_identifier = "chr";
    fprintf(code_output_file, "LABEL $%s_exit\n", function_identifier);
    fprintf(code_output_file, "MOVE LF@%%retval2 int@1\n");
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    fprintf(code_output_file, "LABEL $%s\n", function_identifier);
    fprintf(code_output_file, "PUSHFRAME\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval1\n");
    fprintf(code_output_file, "DEFVAR LF@%%retval2\n");
    fprintf(code_output_file, "MOVE LF@%%retval1 nil@nil\n");
    fprintf(code_output_file, "MOVE LF@%%retval2 int@0\n");
    fprintf(code_output_file, "DEFVAR LF@%s%d\n", function_identifier, 1);
    fprintf(code_output_file, "MOVE LF@%s%d int@%d\n", function_identifier, 1, number);
    fprintf(code_output_file, "DEFVAR LF@GTreturn\n"); // test number > 255 || number < 0
    fprintf(code_output_file, "DEFVAR LF@LTreturn\n");
    fprintf(code_output_file, "LT LF@LTreturn LF@%s%d int@0\n", function_identifier, 1);
    fprintf(code_output_file, "GT LF@GTreturn LF@%s%d int@255\n", function_identifier, 1);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@LTreturn bool@true\n", function_identifier);
    fprintf(code_output_file, "JUMPIFEQ $%s_exit LF@GTreturn bool@true\n", function_identifier);
    fprintf(code_output_file, "INT2CHAR LF@%%retval1 LF@%s%d\n", function_identifier, 1);
    fprintf(code_output_file, "POPFRAME\n");
    fprintf(code_output_file, "RETURN\n");
    return 0;
}