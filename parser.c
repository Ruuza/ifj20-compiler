/**
 * parser.c
 * 
 * @brief Syntax and semantic analysis. Parsing expressions.
 * 
 * @author Petr Růžanský <xruzan00>
 * 
 */

#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "return-codes.h"
#include "stdbool.h"
#include "codegen.h"
#include "symtable.h"
#include "string.h"
#include "precedence-stack.h"
#include "precedence.c"

bool is_EOL = false;
bool EOL_allowed = true;
bool second_token = false;
int return_code;
Token token;
Token prev_token;
Symtable_node_ptr global_symbol_table;

// Load next token, check the return code and check if EOL is allowed.
#define NEXT()                                \
    {                                         \
        is_EOL = false;                       \
        while (1)                             \
        {                                     \
            next_token(&token);               \
            if (token.token_type == TT_ERR)   \
            {                                 \
                return LEXICAL_ERROR;         \
            }                                 \
            if (token.token_type == TT_EOL)   \
            {                                 \
                if (EOL_allowed)              \
                {                             \
                    is_EOL = true;            \
                    continue;                 \
                }                             \
                else                          \
                {                             \
                    break;                    \
                }                             \
            }                                 \
            break;                            \
        }                                     \
    }

// Compare actual token with TOK and then call and compare next token.
#define CHECK_AND_LOAD_TOKEN(TOK)             \
    {                                         \
        if (second_token == true)             \
        {                                     \
            if (prev_token.token_type != TOK) \
            {                                 \
                return SYNTAX_ERROR;          \
            }                                 \
            second_token = false;             \
        }                                     \
        else                                  \
        {                                     \
            if (token.token_type != TOK)      \
            {                                 \
                return SYNTAX_ERROR;          \
            }                                 \
            NEXT();                           \
        }                                     \
    }

// Call function FUN and check return code.
#define CHECK_AND_CALL_FUNCTION(FUN) \
    {                                \
        return_code = FUN;           \
        if (return_code != OK)       \
        {                            \
            return return_code;     \
        }                            \
    }

// Init nonterminal states:
int Func_param_n();
int For_declr();
int Else();
int Id_n();
int Expresion();
int Declr();
int Types_n();
int Func_param();
int State();
int State_data_type();
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

int fast_func();
int fast_param(Symtable_item* function);
int fast_params(Symtable_item* function);
int fast_params_n(Symtable_item* function);
int fast_ret_types(Symtable_item* function);
int fast_types(Symtable_item* function);
int fast_state_data_type(Symtable_item* function);
int fast_types_n(Symtable_item* function);
/////////////////////////////

// NONTERMINAL STATES

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
int Id_n()
{
    switch (token.token_type)
    {
    case TT_COMMA:
        // Rule: <id_n> -> , id <id_n>

        CHECK_AND_LOAD_TOKEN(TT_COMMA);

        CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

        CHECK_AND_CALL_FUNCTION(Id_n());

        return OK;
        break;

    case TT_ASSIGNMENT:
        // Rule: <id_n> -> eps

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
        // case TT_IDENTIFIER:
        // case TT_CLOSE_BRACES:
        // case TT_KEYWORD_IF:
        // case TT_KEYWORD_FOR:
        // case TT_KEYWORD_RETURN:

        //     // Rule: <else> -> eps
        //     return OK;
        //     break;

    case TT_KEYWORD_ELSE:
        // Rule: <else> -> else { <stat_list> }

        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_ELSE);

        CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);

        return OK;
        break;

    default:
        return SYNTAX_ERROR;

        break;
    }
}

int Expresion()
{
    Token a,b;
    a.token_type,b.token_type = TT_ERR;
    tPrecedenceStack stack;
    Precedence_sign table;
    precedenceStackInit(&stack);
    b.attribute.string = "$";
    precedenceStackPush(&stack,b,false,false);
    bool firstCycle = true;
    prev_token = a ;
    while (!((a.attribute.string/*top*/ != "$" && b.attribute.string != "$") && firstCycle == false)){
        if (firstCycle){
            firstCycle = false;
        }
        prev_token = a ;
        precedenceStackTopTerminal(&stack,&a);
        NEXT();
        b = token;
        // shift and nonterminal contro//
        /*if (b.token_type == TT_LESS){
            precedenceStackPush(&stack,b,true,false);
        }
        else if (b.token_type == TT_IDENTIFIER){
            precedenceStackPush(&stack,b,false,true);
        }
        else{*/
        //////////////////////////////////
        Precedence_sign c = precedence_lookup(a.token_type,b.token_type);
        
            switch (c)
            {
            case PRECEDENCE_L:
                a.token_type = TT_LESS;
                a.attribute.string = "<";
                precedenceStackPush(&stack,a,false,false);
                precedenceStackPush(&stack,b,false,false); 
                break;
            case PRECEDENCE_G: // need help
                if ((prev_token.attribute.string == "<" && prev_token.token_type == TT_STRING_LITERAl) && a.token_type == TT_IDENTIFIER){
                    precedenceStackPop(&stack);// IDENTIFIER
                    precedenceStackPop(&stack);// '<'
                    CHECK_AND_LOAD_TOKEN(b.token_type);
                    b = token; 
                    precedenceStackPush(&stack,b,false,false);
                    //print rule by which we get A --todo--
                }
                else{
                    return SYNTAX_ERROR;
                }
                break;
            case PRECEDENCE_E:
                precedenceStackPush(&stack,b,false,false);
                break;    
            default:
                return SYNTAX_ERROR;
                break;
            }
        //}
    }
    //CHECK_AND_LOAD_TOKEN(TT_INTEGER_LITERAL);
    return OK;
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

        CHECK_AND_CALL_FUNCTION(State_data_type());

        CHECK_AND_CALL_FUNCTION(Types_n());
        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Func_param_n()
{
    switch (token.token_type)
    {
    case TT_CLOSE_PARENTHESES:

        // Rule: <Func_param_n> -> eps

        return OK;
        break;

    case TT_COMMA:
        // Rule: <Func_param> -> , <expression> <func_param_n>

        CHECK_AND_LOAD_TOKEN(TT_COMMA);
        CHECK_AND_CALL_FUNCTION(Expresion());
        CHECK_AND_CALL_FUNCTION(Func_param_n());
        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Func_param()
{
    switch (token.token_type)
    {
    case TT_IDENTIFIER:
    case TT_OPEN_PARENTHESES:
    case TT_PLUS:
    case TT_MINUS:
    case TT_ASTERISK:
    case TT_SLASH:
    case TT_INTEGER_LITERAL:
    case TT_STRING_LITERAl:
    case TT_FLOATING_LITERAL:

        // Rule: <Func_param> -> <expr> <func_param_n>

        CHECK_AND_CALL_FUNCTION(Expresion());
        CHECK_AND_CALL_FUNCTION(Func_param_n());
        return OK;
        break;

    case TT_CLOSE_PARENTHESES:
        // Rule: <Func_param> -> eps
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

        prev_token = token;
        NEXT();
        second_token = true;

        if (token.token_type == TT_DECLARATION_ASSIGNMENT)
        {
            // Rule: <state> -> <declr>
            CHECK_AND_CALL_FUNCTION(Declr());
            return OK;
        }
        else if (token.token_type == TT_OPEN_PARENTHESES)
        {
            // Rule: <state> -> id ( <func_param> )

            CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);
            CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);
            CHECK_AND_CALL_FUNCTION(Func_param());
            CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);
            return OK;
        }
        else
        {
            // Rule: <state> -> id <Id_n> = <expr>

            CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);
            CHECK_AND_CALL_FUNCTION(Id_n());
            CHECK_AND_LOAD_TOKEN(TT_ASSIGNMENT);
            CHECK_AND_CALL_FUNCTION(Expresion());
            return OK;
        }

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

        CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);
        return OK;
        break;

    case TT_KEYWORD_RETURN:
        // Rule: <state> -> return <expr>
        CHECK_AND_LOAD_TOKEN(TT_KEYWORD_RETURN);

        CHECK_AND_CALL_FUNCTION(Expresion());

        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int State_data_type()
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
    case TT_CLOSE_PARENTHESES:
        // Rule: <types> -> eps

        return OK;
        break;

    case TT_KEYWORD_INT:
    case TT_KEYWORD_STRING:
    case TT_KEYWORD_FLOAT64:
        // Rule: <types> -> <data_type> <types_n>

        CHECK_AND_CALL_FUNCTION(State_data_type());
        CHECK_AND_CALL_FUNCTION(Types_n());
        return OK;
        break;

    default:

        return SYNTAX_ERROR;
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
        // Rule: <params_n> -> , <param> <params_n>
        CHECK_AND_LOAD_TOKEN(TT_COMMA);

        CHECK_AND_CALL_FUNCTION(Param());

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

    CHECK_AND_CALL_FUNCTION(State_data_type());

    return OK;
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

        if (is_EOL != true)
        {
            return SYNTAX_ERROR;
        }
        else
        {
            is_EOL = false;
        }

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
        CHECK_AND_CALL_FUNCTION(State_data_type());

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

        CHECK_AND_CALL_FUNCTION(Params_n());

        return OK;

        break;

    case TT_CLOSE_PARENTHESES:
        // Rule: <params> -> eps

        return OK;
        break;

    default:
        return SYNTAX_ERROR;
        break;
    }
}

int Func()
{
    // Rule: <func> -> func id ( <params> ) <ret_types> { <stat_list> }

    EOL_allowed = false;

    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FUNC);

    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(Params());

    CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(Ret_types());

    EOL_allowed = true;

    CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);

    if (is_EOL != true)
    {
        return SYNTAX_ERROR;
    }
    else
    {
        is_EOL = false;
    }

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

        CHECK_AND_CALL_FUNCTION(Body())
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
    EOL_allowed = false;

    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_PACKAGE);

    EOL_allowed = true;

    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    if (is_EOL != true)
    {
        return SYNTAX_ERROR;
    }
    else
    {
        is_EOL = false;
    }

    return OK;
}

int Start()
{
    // rule <start> -> <preamle> <body>

    CHECK_AND_CALL_FUNCTION(Preamble());
    generate_header();

    CHECK_AND_CALL_FUNCTION(Body());

    return OK;
}

int fill_function_table()
{
    Symtable_init(&global_symbol_table);
    while (token.token_type != TT_EOF){
        NEXT();
        if (token.token_type == TT_KEYWORD_FUNC){
            CHECK_AND_CALL_FUNCTION(fast_func());
        }
    }
    rewind_file();
    return OK;
}

int fast_func()
{
    // Rule: <func> -> func id ( <params> ) <ret_types> { <stat_list> }
    char* function_name;
    Symtable_item* function = create_item();
    EOL_allowed = false;

    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FUNC);
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);
    function_name = malloc(sizeof(*token.attribute.string)*strlen(token.attribute.string)+1);
    strcpy(function_name, token.attribute.string);

    CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);
    CHECK_AND_CALL_FUNCTION(fast_params(function));
    CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(fast_ret_types(function));

    // Skip function body
    EOL_allowed = true;
    CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);
    if (is_EOL == false){
        return SYNTAX_ERROR;
    }
    is_EOL = false;
    int indent_count = 1;
    while (indent_count){
        if (token.token_type == TT_EOF){
            return SYNTAX_ERROR;
        }
        if (token.token_type == TT_OPEN_BRACES){
            indent_count++;
        }
        if (token.token_type == TT_CLOSE_BRACES){
            indent_count--;
        }
        if (indent_count > 0){
            NEXT();
        }
    }

    Symtable_insert(&global_symbol_table, function_name, function);
    free(function_name);
    return OK;
}


int fast_params(Symtable_item* function)
{
    switch (token.token_type)
    {
        case TT_IDENTIFIER:
            // Rule: <params> -> <param> <params_n>
            CHECK_AND_CALL_FUNCTION(fast_param(function));
            CHECK_AND_CALL_FUNCTION(fast_params_n(function));
            return OK;
        case TT_CLOSE_PARENTHESES:
            // Rule: <params> -> eps
            return OK;
        default:
            return SYNTAX_ERROR;
    }
}

int fast_param(Symtable_item* function)
{
    // Rule: <param> -> id <data_type>
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);
    function->parameter_count++;
    if (function->parameters == NULL){
        function->parameters = malloc(sizeof(Parameter)*function->parameter_count);
    } else {
        function->parameters = realloc(function->parameters, sizeof(Parameter)*function->parameter_count);
    }
    function->parameters[function->parameter_count-1].identifier
    = malloc(sizeof(*token.attribute.string)*strlen(token.attribute.string)+1);
    strcpy(function->parameters[function->parameter_count-1].identifier, token.attribute.string);
    switch (token.token_type) {
        case TT_KEYWORD_FLOAT64:
            function->parameters[function->parameter_count-1].dataType = DT_FLOAT;
            break;
        case TT_KEYWORD_INT:
            function->parameters[function->parameter_count-1].dataType = DT_INT;
            break;
        case TT_KEYWORD_STRING:
            function->parameters[function->parameter_count-1].dataType = DT_STRING;
            break;
        default:
            return SYNTAX_ERROR;
    }
    NEXT();
    return OK;
}

int fast_params_n(Symtable_item* function)
{
    switch (token.token_type)
    {
        case TT_CLOSE_PARENTHESES:
            // Rule: <params_n> -> eps
            return OK;
        case TT_COMMA:
            // Rule: <params_n> -> , <param> <params_n>
            CHECK_AND_LOAD_TOKEN(TT_COMMA);
            CHECK_AND_CALL_FUNCTION(fast_param(function));
            CHECK_AND_CALL_FUNCTION(fast_params_n(function));
            return OK;
        default:
            return SYNTAX_ERROR;
    }
}

int fast_ret_types(Symtable_item* function)
{
    switch (token.token_type)
    {
        case TT_OPEN_PARENTHESES:
            // Rule: <ret_types> -> ( <types> )
            CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);
            CHECK_AND_CALL_FUNCTION(fast_types(function));
            CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);
            return OK;
        case TT_OPEN_BRACES:
            // Rule: <ret_types> -> eps
            function->dataType[0] = DT_VOID;
            function->return_values_count = 1;
            return OK;
        case TT_KEYWORD_INT:
        case TT_KEYWORD_STRING:
        case TT_KEYWORD_FLOAT64:
            // Rule: <ret_types> -> <data_type>
            switch (token.token_type)
            {
                case TT_KEYWORD_INT:
                    // Rule: <data_type> -> int
                    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_INT);
                    function->dataType[0] = DT_INT;
                    function->return_values_count = 1;
                    return OK;
                case TT_KEYWORD_STRING:
                    // Rule: <data_type> -> string
                    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_STRING);
                    function->dataType[0] = DT_STRING;
                    function->return_values_count = 1;
                    return OK;
                case TT_KEYWORD_FLOAT64:
                    // Rule: <data_type> -> float
                CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FLOAT64);
                    function->dataType[0] = DT_FLOAT;
                    function->return_values_count = 1;
                    return OK;
                default:
                    return SYNTAX_ERROR;
            }
            return OK;
        default:
            return SYNTAX_ERROR;
    }
}

int fast_types(Symtable_item* function)
{
    switch (token.token_type)
    {
        case TT_CLOSE_PARENTHESES:
            // Rule: <types> -> eps
            function->dataType[0] = DT_VOID;
            function->return_values_count = 1;
            return OK;
        case TT_KEYWORD_INT:
        case TT_KEYWORD_STRING:
        case TT_KEYWORD_FLOAT64:
            // Rule: <types> -> <data_type> <types_n>
            CHECK_AND_CALL_FUNCTION(fast_state_data_type(function));
            CHECK_AND_CALL_FUNCTION(fast_types_n(function));
            return OK;
        default:
            return SYNTAX_ERROR;
    }
}

int fast_state_data_type(Symtable_item* function)
{
    // Max 10 return values supported
    if (function->return_values_count == 10){
        return INTERNAL_ERROR;
    }
    switch (token.token_type)
    {
        case TT_KEYWORD_INT:
            // Rule: <data_type> -> int
            CHECK_AND_LOAD_TOKEN(TT_KEYWORD_INT);
            function->dataType[function->return_values_count++] = DT_INT;
            return OK;
        case TT_KEYWORD_STRING:
            // Rule: <data_type> -> string
            CHECK_AND_LOAD_TOKEN(TT_KEYWORD_STRING);
            function->dataType[function->return_values_count++] = DT_STRING;
            return OK;
        case TT_KEYWORD_FLOAT64:
            // Rule: <data_type> -> float
            CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FLOAT64);
            function->dataType[function->return_values_count++] = DT_FLOAT;
            return OK;
        default:
            return SYNTAX_ERROR;
    }
}

int fast_types_n(Symtable_item* function)
{
    switch (token.token_type)
    {
        case TT_CLOSE_PARENTHESES:
            // Rule: <types_n> -> eps
            return OK;
        case TT_COMMA:
            // Rule: <types_n> -> , <data_type> <types_n>
            CHECK_AND_LOAD_TOKEN(TT_COMMA);
            CHECK_AND_CALL_FUNCTION(fast_state_data_type(function));
            CHECK_AND_CALL_FUNCTION(fast_types_n(function));
            return OK;
        default:
            return SYNTAX_ERROR;
    }
}

int parse()
{
    // First data gathering pass
    return_code = fill_function_table();
    if (return_code != OK){
        return return_code;
    }
    NEXT();

    // Second full pass
    return_code = Start();
    return return_code;
}