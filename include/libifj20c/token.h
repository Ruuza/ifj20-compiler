#ifndef IFJ20C_TOKEN_H
#define IFJ20C_TOKEN_H

typedef enum {
    TT_ERR,
    TT_IDENTIFIER,
    TT_ELSE,
    TT_FLOAT64,
    TT_FOR,
    TT_FUNC,
    TT_IF,
    TT_INT,
    TT_RETURN,
    TT_STRING,
    TT_EOF
} Token_type;

struct {
    Token_type token_type;
    char* attribute;
} typedef Token;


#endif //IFJ20C_TOKEN_H
