#ifndef IFJ20C_TOKEN_H
#define IFJ20C_TOKEN_H

typedef enum {
    TT_ERR,
    TT_IDENTIFIER,
    TT_KEYWORD,
    TT_STRING,
    TT_EOF,
    TT_INTEGER_LITERAL,
    TT_FLOATING_LITERAL,
    TT_PLUS,
    TT_MINUS,
} Token_type;

typedef enum{
    KEYWORD_NOT_A_KEYWORD,
    KEYWORD_ELSE,
    KEYWORD_FLOAT64,
    KEYWORD_FOR,
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_INT,
    KEYWORD_RETURN,
    KEYWORD_STRING
} Keyword;


union Token_attribute{
    char* string;
    int integer;
    double floating;
    Keyword keyword;
};

struct {
    Token_type token_type;
    union Token_attribute attribute;
} typedef Token;


#endif //IFJ20C_TOKEN_H
