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

int For_declr();
int Else();
int Expresion();
int Declr();
int Types_n();
int State();
int Data_type();
int Types();
int Params_n();
int Param();
int Stat_list();
int Ret_types();
int Params();
int Func();
int Body();
int Preamble();
int Start();

int For_declr()
{
    switch (token.token_type)
    {
    case TT_IDENTIFIER:
        // Rule: <for_declr> -> <declr> ;
        CHECK_AND_CALL_FUNCTION(Declr());

        CHECK_AND_LOAD_TOKEN(TT_SEMICOLON);
        return OK;
        break;
    case TT_SEMICOLON:
        // Rule: <for_declr> -> ;

        CHECK_AND_LOAD_TOKEN(TT_SEMICOLON);

        return OK;
        break;
    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Else()
{
    switch (token.token_type)
    {
    case TT_IDENTIFIER:
    case TT_CLOSE_BRACES:
    case TT_KEYWORD_IF:
    case TT_KEYWORD_FOR:
    case TT_KEYWORD_RETURN:

        // Rule: <else> -> eps
        return OK;
        break;

    case TT_KEYWORD_ELSE:

        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_ELSE);

        CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);

        // Rule: <else> -> else { <stat_list> }
        return OK;
        break;

    default:
        // Rule: <else> -> eps
        return OK;

        break;
    }
}

int Expresion()
{
}

int Declr()
{
    // Rule: <declr> -> id := <expr>
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    CHECK_AND_LOAD_TOKEN(TT_DECLARATION_ASSIGNMENT);

    CHECK_AND_CALL_FUNCTION(Expresion());

    return OK;
}

int Types_n()
{
    switch (token.token_type)
    {
    case TT_CLOSE_PARENTHESES:
        // Rule: <types_n> -> eps
        return OK;
        break;

    case TT_COMMA:
        // Rule: <types_n> -> , <data_type> <types_n>
        CHECK_AND_LOAD_TOKEN(TT_COMMA);

        CHECK_AND_CALL_FUNCTION(Data_type());

        CHECK_AND_CALL_FUNCTION(Types_n());
        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int State()
{
    switch (token.token_type)
    {
    case TT_IDENTIFIER:
        // Rule: <state> -> <declr>
        CHECK_AND_CALL_FUNCTION(Declr());
        return OK;
        break;

    case TT_KEYWORD_IF:
        // Rule: <state> -> if <expr> { <stat_list> } <else>
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_IF);

        CHECK_AND_CALL_FUNCTION(Expresion());

        CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);

        CHECK_AND_CALL_FUNCTION(Else());

        return OK;
        break;

    case TT_KEYWORD_FOR:
        // Rule: <state> -> for <for_declr> <expr> ; <expr> { <stat_list> }
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FOR);

        CHECK_AND_CALL_FUNCTION(For_declr());

        CHECK_AND_CALL_FUNCTION(Expresion());

        CHECK_AND_LOAD_TOKEN(TT_SEMICOLON);

        CHECK_AND_CALL_FUNCTION(Expresion());

        CHECK_AND_CALL_FUNCTION(TT_OPEN_BRACES);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_CALL_FUNCTION(TT_CLOSE_BRACES);
        return OK;
        break;

    case TT_KEYWORD_RETURN:
        // Rule: <state> -> return <expr>
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_RETURN);

        CHECK_AND_CALL_FUNCTION(Expresion());

        return OK;
        break;

    default:
        // Rule: <state> -> <exrp>
        CHECK_AND_CALL_FUNCTION(Expresion());
        return OK;
        break;
    }
}

int Data_type()
{
    switch (token.token_type)
    {
    case TT_KEYWORD_INT:
        // Rule: <data_type> -> int
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_INT);
        return OK;
        break;

    case TT_KEYWORD_STRING:
        // Rule: <data_type> -> string
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_STRING);
        return OK;
        break;

    case TT_KEYWORD_FLOAT64:
        // Rule: <data_type> -> float
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FLOAT64);
        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Types()
{
    switch (token.token_type)
    {
    case TT_KEYWORD_INT:
    case TT_KEYWORD_STRING:
    case TT_KEYWORD_FLOAT64:
        // Rule: <types> -> <data_type> <types_n>

        CHECK_AND_CALL_FUNCTION(Data_type());
        CHECK_AND_CALL_FUNCTION(Types_n());
        return OK;
        break;

    default:
        // Rule: <types> -> eps
        return OK;
        break;
    }
}

int Params_n()
{
    switch (token.token_type)
    {
    case TT_CLOSE_PARENTHESES:
        // Rule: <params_n> -> eps
        return OK;
        break;

    case TT_COMMA:
        // Rule: <params_n> -> , <types> <params_n>
        CHECK_AND_LOAD_TOKEN(TT_COMMA);

        CHECK_AND_CALL_FUNCTION(Types());

        CHECK_AND_CALL_FUNCTION(Params_n());
        return OK;
        break;
    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Param()
{
    // Rule: <param> -> id <data_type>
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    CHECK_AND_CALL_FUNCTION(Data_type());
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
        // Rule: <statl> -> <state> <statl>
        CHECK_AND_CALL_FUNCTION(State());

        CHECK_AND_CALL_FUNCTION(Stat_list());

        return OK;
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