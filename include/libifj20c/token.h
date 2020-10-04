#ifndef IFJ20C_TOKEN_H
#define IFJ20C_TOKEN_H

#include <stdint.h>

typedef enum {
    TT_ERR,
    TT_IDENTIFIER,
    TT_KEYWORD,
    TT_STRING_LITERAl,
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
    int64_t integer;
    double floating;
    Keyword keyword;
};

struct {
    Token_type token_type;
    union Token_attribute attribute;
} typedef Token;

/**
 * Converts attribute value of token into string.
 * @param token
 * @return Non owning string
 */
char* to_string_attribute(const Token* token);

#endif //IFJ20C_TOKEN_H
