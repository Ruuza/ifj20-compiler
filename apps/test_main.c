#include "scanner.h"
#include "testData.h"

FILE* test_file = NULL;

int setup(char * string){
    if (test_file == NULL){
        test_file = tmpfile();
    }
    fprintf(test_file, "%s", string);
    rewind(test_file);
    if (set_file(test_file) == -1){
        fprintf(stderr, "Invalid input file");
        return 1;
    }
    return 0;
}

int test_no_errors(char * string){
    if (setup(string)){
        fprintf(stderr, "Error setting up test environment");
        return 1;
    }
    Token token;
    for(int i = 0;next_token(&token);i++){
        if (token.token_type == TT_ERR){
            return 1;
        }
        if (i > 1000){
            fprintf(stderr, "Over 1000 tokens generated. Probably stuck in an infinite loop");
            return 1;
        }
    }
    if (token.token_type != TT_EOF){
        return 1;
    }
    return 0;
}

int main(){
    if(test_no_errors(TESTDATA_FACTORIAL_ITERATIVE)){
        printf("TEST 1 FAILED");
        return 1;
    }
    if(test_no_errors(TESTDATA_FACTORIAL_RECURSIVE)){
        printf("TEST 2 FAILED");
        return 1;
    }
    if(test_no_errors(TESTDATA_STRING_FUNCTION_EXAMPLE)){
        printf("TEST 3 FAILED");
        return 1;
    }
    return 0;
}