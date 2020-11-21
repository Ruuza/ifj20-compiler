#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "return-codes.h"

int return_code;
Token token;

#define NEXT()                            \
    {                                     \
        return_code = next_token(&token); \
        if (return_code == -1)            \
        {                                 \
            return LEXICAL_ERROR;         \
        }                                 \
    }

#define CHECK_AND_LOAD_TOKEN(TOK)    \
    {                                \
        if (token.token_type != TOK) \
        {                            \
            return SYNTAX_ERROR;     \
        }                            \
        NEXT();                      \
    }

#define CHECK_AND_CALL_FUNCTION(FUN) \
    {                                \
        return_code = FUN;           \
        if (return_code != OK)       \
        {                            \
            return SYNTAX_ERROR;     \
        }                            \
    }

int State()
{
}

int Data_type()
{
}

int Types()
{
}

int Params_n()
{
}

int Param()
{
}

int Stat_list()
{
    switch (token.token_type)
    {
    case TT_IDENTIFIER:
    case TT_KEYWORD_IF:
    case TT_KEYWORD_FOR:
    case TT_KEYWORD_RETURN:
        // Rule: <statl> -> <state> <stat_list>
        CHECK_AND_CALL_FUNCTION(State());

        CHECK_AND_CALL_FUNCTION(Stat_list());
        return OK;
        break;
    case TT_CLOSE_BRACES:
        // Rule: <statl> -> eps

        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Ret_types()
{
    switch (token.token_type)
    {
    case TT_OPEN_PARENTHESES:
        // Rule: <ret_types> -> ( <types> )

        CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);

        CHECK_AND_CALL_FUNCTION(Types());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);

        return OK;
        break;

    case TT_OPEN_BRACES:
        // Rule: <ret_types> -> eps
        return OK;
        break;

    case TT_KEYWORD_INT:
    case TT_KEYWORD_STRING:
    case TT_KEYWORD_FLOAT64:
        // Rule: <ret_types> -> <data_type>
        CHECK_AND_CALL_FUNCTION(Data_type());

        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Params()
{
    switch (token.token_type)
    {
    case TT_IDENTIFIER:
        // Rule: <params> -> <param> <params_n>

        CHECK_AND_CALL_FUNCTION(Param());

        CHECK_AND_LOAD_TOKEN(Params_n());

        return OK;

        break;

    case TT_CLOSE_PARENTHESES:
        // Rule: <params> -> eps

        return OK;
        break;

    default:
        return SEMANTIC_ERROR;
        break;
    }
}

int Func()
{
    // Rule: <func> -> func id ( <params> ) <ret_types> { <stat_list> }
    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FUNC);

    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(Params());

    CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(Ret_types());

    CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);

    CHECK_AND_CALL_FUNCTION(Stat_list());

    CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);

    return OK;
}

int Body()
{
    switch (token.token_type)
    {
    case TT_EOF:
        // Rule: <body> -> EOF
        return OK;
        break;

    case TT_KEYWORD_FUNC:
        // Rule: <body> -> <func> <body>
        CHECK_AND_CALL_FUNCTION(Func());
        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Preamble()
{
    // rule <preamble> -> package id

    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_PACKAGE);

    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    return OK;
}

int Start()
{
    // rule <start> -> <preamle> <body>

    CHECK_AND_CALL_FUNCTION(Preamble());

    CHECK_AND_CALL_FUNCTION(Body());

    return OK;
}

int parse()
{
    NEXT();

    return_code = Start();
    return return_code;
}