#include <stdio.h>
#include "codegen.h"

FILE* code_output_file;

int set_code_output(FILE *file) {
    if (file == NULL){
        return -1;
    }
    code_output_file = file;
    return 0;
}

int generate_header() {
    fprintf(code_output_file, ".IFJcode20");
    return 0;
}

