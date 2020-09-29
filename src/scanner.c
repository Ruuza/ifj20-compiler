#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"

FILE* inputFile;

int set_file(FILE* file){
    if (file == NULL){
        return -1;
    }
    inputFile = file;
    return 0;
}
enum Scan_state{
    SCANSTATE_ERR,
    SCANSTATE_START,
    SCANSTATE_IDENTIFIER,
};

Token_type get_keyword_type(const char* attribute){
    //else, float64, for, func, if, int, return, string
    if (strcmp(attribute, "else") == 0){
        return TT_ELSE;
    }
    if (strcmp(attribute, "float64") == 0){
        return TT_FLOAT64;
    }
    if (strcmp(attribute, "for") == 0){
        return TT_FOR;
    }
    if (strcmp(attribute, "func") == 0){
        return TT_FUNC;
    }
    if (strcmp(attribute, "if") == 0){
        return TT_IF;
    }
    if (strcmp(attribute, "int") == 0){
        return TT_INT;
    }
    if (strcmp(attribute, "return") == 0){
        return TT_RETURN;
    }
    if (strcmp(attribute, "string") == 0){
        return TT_STRING;
    }
    return TT_IDENTIFIER;
}

int next_token(Token* token){
    if (inputFile == NULL){
        fprintf(stderr, "%s", "Invalid input file\n");
        return -1;
    }

    enum Scan_state state = SCANSTATE_START;

    char *line = malloc(sizeof(char)*16);
    int size = 16;
    for (int i = 0;true;i++){
        char ch = (char)fgetc(inputFile);
        if (i >= size){
            size = (int)(size * 1.5);
            line = realloc(line, size*sizeof(char));
        }
        line[i] = ch;

        switch (state) {
            case SCANSTATE_ERR:
                break;
            case SCANSTATE_START:
                if (isalpha(ch) || ch == '_'){
                    line[i] = ch;
                    state = SCANSTATE_IDENTIFIER;
                    break;
                } else if (isspace(ch)){
                    i--;
                    break;
                } else if (ch == EOF){
                    line[i] = 0;
                    token->token_type = TT_EOF;
                    return 0;
                }
                break;
            case SCANSTATE_IDENTIFIER:
                if (isalnum(ch) || ch == '_') {
                    line[i] = ch;
                    state = SCANSTATE_IDENTIFIER;
                } else {
                    line[i] = 0;
                    ungetc(ch, inputFile);
                    token->token_type = get_keyword_type(line);
                    token->attribute = line;
                    return 1;
                }
        break;
        }
    }
}
