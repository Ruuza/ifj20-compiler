#include <stdio.h>
#include "token.h"

char str[100];

char* to_string_attribute(const Token* token){
    switch (token->token_type){
        case TT_ERR:
            return "err";
        case TT_IDENTIFIER:
            return token->attribute.string;
        case TT_STRING_LITERAl:
            return token->attribute.string;
        case TT_EOF:
            return "EOF";
        case TT_INTEGER_LITERAL:
            sprintf(str, "%ld", token->attribute.integer);
            return str;
        case TT_FLOATING_LITERAL:
            sprintf(str, "%f", token->attribute.floating);
            return str;
        case TT_PLUS:
            return "+";
        case TT_MINUS:
            return "-";
        case TT_KEYWORD:
            switch (token->attribute.keyword) {
                case KEYWORD_NOT_A_KEYWORD:
                    return "INVALID-NOT A KEYWORD-INVALID";
                case KEYWORD_ELSE:
                    return "else";
                case KEYWORD_FLOAT64:
                    return "float64";
                case KEYWORD_FOR:
                    return "for";
                case KEYWORD_FUNC:
                    return "func";
                case KEYWORD_IF:
                    return "if";
                case KEYWORD_INT:
                    return "int";
                case KEYWORD_PACKAGE:
                    return "package";
                case KEYWORD_RETURN:
                    return "return";
                case KEYWORD_STRING:
                    return "string";
            }
        case TT_EOL:
            return "EOL";
        case TT_ASSIGNMENT:
            return "=";
        case TT_DECLARATION_ASSIGNMENT:
            return ":=";
        case TT_ASTERISK:
            return "*";
        case TT_SLASH:
            return "/";
        case TT_COMMA:
            return ",";
        case TT_SEMICOLON:
            return ";";
        case TT_EQUALS:
            return "==";
        case TT_NOT_EQUALS:
            return "!=";
        case TT_LESS_OR_EQUALS:
            return "<=";
        case TT_GREATER_OR_EQUALS:
            return ">=";
        case TT_LESS:
            return "<";
        case TT_GREATER:
            return ">";
        case TT_OPEN_PARENTHESES:
            return "(";
        case TT_CLOSE_PARENTHESES:
            return ")";
        case TT_OPEN_BRACES:
            return "{";
        case TT_CLOSE_BRACES:
            return "}";
    }
}
