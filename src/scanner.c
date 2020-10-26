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
    SCANSTATE_COMMENT,
    SCANSTATE_MULTILINE_COMMENT,
    SCANSTATE_IDENTIFIER,
    SCANSTATE_NUMBER,
    SCANSTATE_STRING_LITERAL,
    SCANSTATE_ESCAPE_CHAR,
    SCANSTATE_ESCAPE_CHAR_HEX,
    SCANSTATE_ESCAPE_CHAR_HEX2,
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
    if (strcmp(attribute, "package") == 0){
        return KEYWORD_PACKAGE;
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
        // Having the check here allows first EOF character to go through.
        if (feof(inputFile)){
            return 0;
        }
        char ch = (char)fgetc(inputFile);
        // If there is not an empty line at the end of the file EOF gets consumed by
        // the previous token. Because ungetc() does not unset EOF bit we need to do it manually.
        if (state != SCANSTATE_START && ch == EOF){
            clearerr(inputFile);
        }

        // Increase capacity for line when needed.
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
                }else if(ch == '/'){
                    ch = (char)fgetc(inputFile);
                    if (ch == '/') {
                        state = SCANSTATE_COMMENT;
                    } else if (ch == '*'){
                        state = SCANSTATE_MULTILINE_COMMENT;
                    } else{
                        token->token_type = TT_SLASH;
                        free(line);
                        ungetc(ch, inputFile);
                        return 1;
                    }
                }else if(ch == '-'){
                    token->token_type = TT_MINUS;
                    free(line);
                    return 1;
                }else if(ch == '+'){
                    token->token_type = TT_PLUS;
                    free(line);
                    return 1;
                }else if(ch == '*'){
                    token->token_type = TT_ASTERISK;
                    free(line);
                    return 1;
                }else if(ch == '('){
                    token->token_type = TT_OPEN_PARENTHESES;
                    free(line);
                    return 1;
                }else if(ch == ')'){
                    token->token_type = TT_CLOSE_PARENTHESES;
                    free(line);
                    return 1;
                }else if(ch == '{'){
                    token->token_type = TT_OPEN_BRACES;
                    free(line);
                    return 1;
                }else if(ch == '}'){
                    token->token_type = TT_CLOSE_BRACES;
                    free(line);
                    return 1;
                }else if(ch == ','){
                    token->token_type = TT_COMMA;
                    free(line);
                    return 1;
                }else if(ch == ';'){
                    token->token_type = TT_SEMICOLON;
                    free(line);
                    return 1;
                }else if(ch == '<'){
                    ch = (char)fgetc(inputFile);
                    if (ch == '='){
                        token->token_type = TT_LESS_OR_EQUALS;
                    } else {
                        token->token_type = TT_LESS;
                    }
                    free(line);
                    return 1;
                }else if(ch == '>'){
                    ch = (char)fgetc(inputFile);
                    if (ch == '='){
                        token->token_type = TT_GREATER_OR_EQUALS;
                    } else {
                        token->token_type = TT_GREATER;
                    }
                    free(line);
                    return 1;
                }else if(ch == '='){
                    ch = (char)fgetc(inputFile);
                    if (ch == '='){
                        token->token_type = TT_EQUALS;
                    } else {
                        token->token_type = TT_ASSIGNMENT;
                    }
                    free(line);
                    return 1;
                }else if(ch == ':'){
                    ch = (char)fgetc(inputFile);
                    if (ch == '='){
                        token->token_type = TT_DECLARATION_ASSIGNMENT;
                    } else{
                        token->token_type = TT_ERR;
                    }
                    free(line);
                    return 1;
                }else if(ch == '!'){
                    ch = (char)fgetc(inputFile);
                    if (ch == '='){
                        token->token_type = TT_NOT_EQUALS;
                    } else{
                        token->token_type = TT_ERR;
                    }
                    free(line);
                    return 1;
                }else if(ch == '"'){
                    i--;
                    state = SCANSTATE_STRING_LITERAL;
                }else if(ch >= '0' && ch <= '9'){
                    line[i] = ch;
                    state = SCANSTATE_NUMBER;
                }else if(ch == '\n'){
                    token->token_type = TT_EOL;
                    free(line);
                    return 1;
                } else if (isspace(ch)){
                    i--;
                    break;
                } else if (ch == EOF) {
                    token->token_type = TT_EOF;
                    free(line);
                    return 1;
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
                // Number with redundant leading zeros are not allowed.
                if (isdigit(ch)){
                    if (line[0] == '0' && i == 1){
                        token->token_type = TT_ERR;
                        free(line);
                        return 1;
                    }
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
            case SCANSTATE_STRING_LITERAL:
                line[i] = ch;
                if (ch == '"'){
                    line[i] = 0;
                    token->token_type = TT_STRING_LITERAl;
                    token->attribute.string = line;
                    return 1;
                }
                if (ch <= 31){
                    token->token_type = TT_ERR;
                    free(line);
                    return 1;
                }
                if (ch == '\\') {
                    i--;
                    state = SCANSTATE_ESCAPE_CHAR;
                }
                break;
            case SCANSTATE_ESCAPE_CHAR:
                switch(ch) {
                    case 'n':
                        line[i] = '\n';
                        state = SCANSTATE_STRING_LITERAL;
                        break;
                    case '\\':
                        line[i] = '\\';
                        state = SCANSTATE_STRING_LITERAL;
                        break;
                    case '"':
                        line[i] = '"';
                        state = SCANSTATE_STRING_LITERAL;
                        break;
                    case 't':
                        line[i] = '\t';
                        state = SCANSTATE_STRING_LITERAL;
                        break;
                    case 'x':
                        i--;
                        state = SCANSTATE_ESCAPE_CHAR_HEX;
                        break;
                    default:
                        state = SCANSTATE_STRING_LITERAL;
                        line[i] = ch;
                }
                break;
            case SCANSTATE_ESCAPE_CHAR_HEX:
                if (!isxdigit(ch)){
                    token->token_type = TT_ERR;
                    free(line);
                    return 1;
                }
                line[i] = ch;
                state = SCANSTATE_ESCAPE_CHAR_HEX2;
                break;
            case SCANSTATE_ESCAPE_CHAR_HEX2:
                if (!isxdigit(ch)){
                    token->token_type = TT_ERR;
                    free(line);
                    return 1;
                }
                line[i] = ch;
                i--;
                line[i] = (char)strtol(line+i, NULL, 16);
                state = SCANSTATE_STRING_LITERAL;
                break;
            case SCANSTATE_COMMENT:
                i = -1;
                // Reset line position every time because we don't want line to grow.
                // -1 because it will be incremented to 0
                if (ch == '\n'){
                    state = SCANSTATE_START;
                }
                break;
            case SCANSTATE_MULTILINE_COMMENT:
                i = -1;
                if (ch == '*'){
                    ch = (char)fgetc(inputFile);
                    if (ch == '/'){
                        state = SCANSTATE_START;
                    }
                }
                break;
        }
    }
}
