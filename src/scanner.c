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
    SCANSTATE_NUMBER,
    SCANSTATE_FLOAT_PRE_EXPONENT,
    SCANSTATE_FLOAT_EXPONENT,
    SCANSTATE_FLOAT,
};

Keyword get_keyword(const char* attribute){
    //else, float64, for, func, if, int, return, string
    if (strcmp(attribute, "else") == 0){
        return KEYWORD_ELSE;
    }
    if (strcmp(attribute, "float64") == 0){
        return KEYWORD_FLOAT64;
    }
    if (strcmp(attribute, "for") == 0){
        return KEYWORD_FOR;
    }
    if (strcmp(attribute, "func") == 0){
        return KEYWORD_FUNC;
    }
    if (strcmp(attribute, "if") == 0){
        return KEYWORD_IF;
    }
    if (strcmp(attribute, "int") == 0){
        return KEYWORD_INT;
    }
    if (strcmp(attribute, "return") == 0){
        return KEYWORD_RETURN;
    }
    if (strcmp(attribute, "string") == 0){
        return KEYWORD_STRING;
    }
    return KEYWORD_NOT_A_KEYWORD;
}

int string_token_to_int(Token *token){
    int64_t value = atol(token->attribute.string);
    free(token->attribute.string);
    token->attribute.integer = value;
    return 0;
}

int string_token_to_double(Token *token){
    double value = atof(token->attribute.string);
    free(token->attribute.string);
    token->attribute.floating = value;
    return 0;
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

        switch (state) {
            case SCANSTATE_ERR:
                break;
            case SCANSTATE_START:
                if (isalpha(ch) || ch == '_') {
                    line[i] = ch;
                    state = SCANSTATE_IDENTIFIER;
                    break;
                }else if(ch == '-'){
                    token->token_type = TT_MINUS;
                    free(line);
                    return 1;
                }else if(ch == '+'){
                    token->token_type = TT_PLUS;
                    free(line);
                    return 1;
                }else if(ch > '0' && ch <= '9'){
                    line[i] = ch;
                    state = SCANSTATE_NUMBER;
                } else if (isspace(ch)){
                    i--;
                    break;
                } else if (ch == EOF){
                    line[i] = 0;
                    token->token_type = TT_EOF;
                    return 0;
                } else {
                    line[i] = 0;
                    token->token_type = TT_ERR;
                    token->attribute.string = line;
                    return 1;
                }
                break;
            case SCANSTATE_IDENTIFIER:
                if (isalnum(ch) || ch == '_') {
                    line[i] = ch;
                    state = SCANSTATE_IDENTIFIER;
                } else {
                    line[i] = 0;
                    ungetc(ch, inputFile);
                    if (get_keyword(line) == KEYWORD_NOT_A_KEYWORD){
                        token->token_type = TT_IDENTIFIER;
                        token->attribute.string = line;
                    } else {
                        token->token_type = TT_KEYWORD;
                        token->attribute.keyword = get_keyword(line);
                    }
                    return 1;
                }
                break;
            case SCANSTATE_NUMBER:
                if (isdigit(ch)){
                    line[i] = ch;
                } else if(ch == '.') {
                    line[i] = ch;
                    state = SCANSTATE_FLOAT_PRE_EXPONENT;
                } else if(tolower(ch) == 'e') {
                    line[i] = ch;
                    state = SCANSTATE_FLOAT_EXPONENT;
                } else {
                    line[i] = 0;
                    ungetc(ch, inputFile);
                    token->token_type = TT_INTEGER_LITERAL;
                    token->attribute.string = line;
                    string_token_to_int(token);
                    return 1;
                }
                break;
            case SCANSTATE_FLOAT_PRE_EXPONENT:
                if (isdigit(ch)) {
                    line[i] = ch;
                } else if (tolower(ch) == 'e'){
                    line[i] = ch;
                    state = SCANSTATE_FLOAT_EXPONENT;
                } else{
                    line[i] = 0;
                    ungetc(ch, inputFile);
                    token->token_type = TT_FLOATING_LITERAL;
                    token->attribute.string = line;
                    string_token_to_double(token);
                    return 1;
                }
                break;
            case SCANSTATE_FLOAT_EXPONENT:
                line[i] = ch;
                if (ch == '+' || ch == '-' || isdigit(ch)){
                    state = SCANSTATE_FLOAT;
                } else {
                    state = SCANSTATE_ERR;
                }
                break;
            case SCANSTATE_FLOAT:
                if (isdigit(ch)){
                    line[i] = ch;
                } else {
                    line[i] = 0;
                    ungetc(ch, inputFile);
                    token->token_type = TT_FLOATING_LITERAL;
                    token->attribute.string = line;
                    string_token_to_double(token);
                    return 1;
                }
                break;
        }
    }
}
