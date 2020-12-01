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
#include "precedence.c"
#include "symstack.h"
#include "symtablestack.h"

bool is_EOL = false;
bool EOL_allowed = true;
bool second_token = false;
int return_code;
Token token;
Token prev_token;
Symtable_node_ptr global_symbol_table;
Symtable_item* current_function;
int param_counter;
Symstack* symtable_stack;

// Load next token, check the return code and check if EOL is allowed.
#define NEXT()                              \
    {                                       \
        is_EOL = false;                     \
        while (1)                           \
        {                                   \
            next_token(&token);             \
            if (token.token_type == TT_ERR) \
            {                               \
                return LEXICAL_ERROR;       \
            }                               \
            if (token.token_type == TT_EOL) \
            {                               \
                if (EOL_allowed)            \
                {                           \
                    is_EOL = true;          \
                    continue;               \
                }                           \
                else                        \
                {                           \
                    break;                  \
                }                           \
            }                               \
            break;                          \
        }                                   \
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
            return return_code;      \
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
int fast_param(Symtable_item *function);
int fast_params(Symtable_item *function);
int fast_params_n(Symtable_item *function);
int fast_ret_types(Symtable_item *function);
int fast_types(Symtable_item *function);
int fast_state_data_type(Symtable_item *function);
int fast_types_n(Symtable_item *function);
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
        Symtable_node_ptr localtab_else = NULL;
        Symtable_init(&localtab_else);
            Symtable_stack_insert(symtable_stack, localtab_else);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);
        Symtable_node_ptr helper = Symtable_stack_pop(symtable_stack);
        Symtable_dispose(&helper);

        return OK;
        break;

    default:
        return SYNTAX_ERROR;

        break;
    }
}


Symtable_item* create_shift(){
    Symtable_item* shift = create_item();
    shift->token.token_type = TT_SHIFT;
    return shift;
}

Symtable_item* find_terminal_top(Symstack* symstack){
    for(int i = symstack->top; i >= 0; i--){
        Symtable_item* item = *symstack->stack+symstack->top;
        switch (item->token.token_type) {
            case TT_SHIFT:
            case TT_NONTERMINAL:
                break;
            default:
                return item;
        }
    }
}

Symtable_item* insert_shift(Symstack* symstack, Symtable_item* item){
    Symstack* temp_stack;
    Symstack_init(&temp_stack);
    for(int i = symstack->top; i > 0; i--){
        Symtable_item* current_item = *(symstack->stack+i);
        if (current_item == item){
            Symstack_insert(symstack, create_shift());
            break;
        } else {
            Symstack_insert(temp_stack, Symstack_pop(symstack));
        }
    }
    while (!Symstack_empty(symstack)){
        Symstack_insert(symstack, Symstack_pop(temp_stack));
    }
    Symstack_dispose(&temp_stack);
}

int shift_position(Symstack* symstack){
    for(int i = symstack->top; i > 0; i--){
        Symtable_item* current_item = *(symstack->stack+i);
        if (current_item->token.token_type != TT_STOP){
            continue;
        }
        return i;
    }
    return -1;
}

int parse_expresion_rule(Symstack* symstack, int shift_pos){
    int current_pos = shift_pos+1;
    if (current_pos > symstack->top){
        return SYNTAX_ERROR;
    }
    switch ((*symstack->stack+current_pos)->token.token_type) {
        case TT_NONTERMINAL:
            current_pos = shift_pos+1;
            if (current_pos > symstack->top){
                return SYNTAX_ERROR;
            }
            switch ((*symstack->stack+current_pos)->token.token_type) {
                case TT_PLUS:
                    //E -> E+E
                    break;
                case TT_MINUS:
                    //E -> E-E
                    break;
                case TT_ASTERISK:
                    //E -> E*E
                    break;
                case TT_SLASH:
                    //E -> E/E
                    break;
                case TT_LESS:
                case TT_LESS_OR_EQUALS:
                case TT_GREATER:
                case TT_GREATER_OR_EQUALS:
                case TT_EQUALS:
                    // E -> E compare E
                default:
                    return SYNTAX_ERROR;
            }
        case TT_OPEN_PARENTHESES:
            //E -> (E)
        case TT_IDENTIFIER:
            // E -> id
            // MAYBE FUNEXP?
        default:
            return SYNTAX_ERROR;
    }
}

bool is_precedence_end(Token_type tokenType){
    switch (tokenType) {
        case TT_IDENTIFIER:
        case TT_STRING_LITERAl:
        case TT_INTEGER_LITERAL:
        case TT_FLOATING_LITERAL:
        case TT_PLUS:
        case TT_MINUS:
        case TT_ASTERISK:
        case TT_SLASH:
        case TT_SEMICOLON:
        case TT_EQUALS:
        case TT_NOT_EQUALS:
        case TT_LESS_OR_EQUALS:
        case TT_GREATER_OR_EQUALS:
        case TT_LESS:
        case TT_GREATER:
            return false;
        default:
            return true;
    }
}

int Expresion()
{
    Symstack* symstack;
    Symstack_init(&symstack);
    Symtable_item* stop = create_item();
    stop->token.token_type = TT_STOP;
    Symstack_insert(symstack, stop);
    int shift_pos;

    bool end_found = false;
    Symtable_item* a, *b;
    NEXT()
    do{
        a = find_terminal_top(symstack);
        b = create_item();
        if (is_precedence_end(token.token_type)){
            b->token.token_type = TT_STOP;
            end_found = true;
        } else {
            b->token.token_type = token.token_type;
        }
        switch (precedence_lookup(a->token.token_type, b->token.token_type)) {
            case PRECEDENCE_E:
                Symstack_insert(symstack, b);
                if (!end_found){
                    NEXT()
                }
                break;
            case PRECEDENCE_L:
                insert_shift(symstack, a);
                Symstack_insert(symstack, b);
                if (!end_found){
                    NEXT()
                }
                break;
            case PRECEDENCE_G:
                shift_pos = shift_position(symstack);
                if (shift_pos != -1){
                    parse_expresion_rule(symstack, shift_pos);
                } else{
                    return SYNTAX_ERROR;
                }
                free_symtable_item(b);
                break;
            case PRECEDENCE_X:
                return SYNTAX_ERROR;
        }
    } while (!end_found && find_terminal_top(symstack)->token.token_type != TT_STOP);

    Symstack_dispose(&symstack);
    free_symtable_item(stop);
    return OK;
}

int Declr()
{
    // Rule: <declr> -> id := <expr>
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);

    char* identifier_name = malloc(sizeof(*token.attribute.string)*strlen(token.attribute.string)+1);;
    Symtable_item* identifier = create_item();
    identifier->parameters->identifier = identifier_name;

    CHECK_AND_LOAD_TOKEN(TT_DECLARATION_ASSIGNMENT);

    CHECK_AND_CALL_FUNCTION(Expresion());
    Symtable_node_ptr helper = Symtable_stack_head(symtable_stack);
    Symtable_insert(&helper, identifier_name, identifier);
    free(identifier_name);

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
    Symtable_node_ptr localtab_if_for = NULL;
    Symtable_init(&localtab_if_for);
    Symtable_node_ptr stack_pop_helper = NULL;
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
            Symtable_stack_insert(symtable_stack, localtab_if_for);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);
        stack_pop_helper = Symtable_stack_pop(symtable_stack);
        Symtable_dispose(&stack_pop_helper);
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
            Symtable_stack_insert(symtable_stack, localtab_if_for);

        CHECK_AND_CALL_FUNCTION(Stat_list());

        CHECK_AND_LOAD_TOKEN(TT_CLOSE_BRACES);
        stack_pop_helper = Symtable_stack_pop(symtable_stack);
        Symtable_dispose(&stack_pop_helper);
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
    generate_func_param(current_function->parameters[param_counter].identifier, param_counter+1);
    param_counter++;

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
    char* function_identifier = malloc(sizeof(*token.attribute.string) * strlen(token.attribute.string) + 1);
    current_function = Symtable_search(global_symbol_table, token.attribute.string);
    strcpy(function_identifier, token.attribute.string);
    generate_func_top(token.attribute.string);
    generate_return_values(current_function);

    CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);

    param_counter = 0;

    CHECK_AND_CALL_FUNCTION(Params());

    CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(Ret_types());

    EOL_allowed = true;

    CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);
    Symtable_node_ptr localtab_func = NULL;
    Symtable_init(&localtab_func);
    Symtable_stack_insert(symtable_stack, localtab_func);

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
    generate_func_bottom(function_identifier);
    free(function_identifier);

    Symtable_node_ptr helper = Symtable_stack_pop(symtable_stack);
    Symtable_dispose(&helper);
 
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
    char* package_id_name = "main";
    if (strcmp(token.attribute.string, package_id_name) != 0){
        return SEMANTIC_ERROR_OTHERS;
    }

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
    while (token.token_type != TT_EOF)
    {
        NEXT();
        if (token.token_type == TT_KEYWORD_FUNC)
        {
            CHECK_AND_CALL_FUNCTION(fast_func());
        }
    }
    rewind_file();
    return OK;
}

int fast_func()
{
    // Rule: <func> -> func id ( <params> ) <ret_types> { <stat_list> }
    char *function_name;
    Symtable_item *function = create_item();
    EOL_allowed = false;

    CHECK_AND_LOAD_TOKEN(TT_KEYWORD_FUNC);
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);
    function_name = malloc(sizeof(*token.attribute.string) * strlen(token.attribute.string) + 1);
    strcpy(function_name, token.attribute.string);

    CHECK_AND_LOAD_TOKEN(TT_OPEN_PARENTHESES);
    CHECK_AND_CALL_FUNCTION(fast_params(function));
    CHECK_AND_LOAD_TOKEN(TT_CLOSE_PARENTHESES);

    CHECK_AND_CALL_FUNCTION(fast_ret_types(function));

    // Skip function body
    EOL_allowed = true;
    CHECK_AND_LOAD_TOKEN(TT_OPEN_BRACES);
    if (is_EOL == false)
    {
        return SYNTAX_ERROR;
    }
    is_EOL = false;
    int indent_count = 1;
    while (indent_count)
    {
        if (token.token_type == TT_EOF)
        {
            return SYNTAX_ERROR;
        }
        if (token.token_type == TT_OPEN_BRACES)
        {
            indent_count++;
        }
        if (token.token_type == TT_CLOSE_BRACES)
        {
            indent_count--;
        }
        if (indent_count > 0)
        {
            NEXT();
        }
    }

    Symtable_insert(&global_symbol_table, function_name, function);
    free(function_name);
    return OK;
}

int fast_params(Symtable_item *function)
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

int fast_param(Symtable_item *function)
{
    // Rule: <param> -> id <data_type>
    CHECK_AND_LOAD_TOKEN(TT_IDENTIFIER);
    function->parameter_count++;
    if (function->parameters == NULL)
    {
        function->parameters = malloc(sizeof(Parameter) * function->parameter_count);
    }
    else
    {
        function->parameters = realloc(function->parameters, sizeof(Parameter) * function->parameter_count);
    }
    function->parameters[function->parameter_count - 1].identifier = malloc(sizeof(*token.attribute.string) * strlen(token.attribute.string) + 1);
    strcpy(function->parameters[function->parameter_count - 1].identifier, token.attribute.string);
    switch (token.token_type)
    {
    case TT_KEYWORD_FLOAT64:
        function->parameters[function->parameter_count - 1].dataType = DT_FLOAT;
        break;
    case TT_KEYWORD_INT:
        function->parameters[function->parameter_count - 1].dataType = DT_INT;
        break;
    case TT_KEYWORD_STRING:
        function->parameters[function->parameter_count - 1].dataType = DT_STRING;
        break;
    default:
        return SYNTAX_ERROR;
    }
    NEXT();
    return OK;
}

int fast_params_n(Symtable_item *function)
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

int fast_ret_types(Symtable_item *function)
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

int fast_types(Symtable_item *function)
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

int fast_state_data_type(Symtable_item *function)
{
    // Max 10 return values supported
    if (function->return_values_count == 10)
    {
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

int fast_types_n(Symtable_item *function)
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
    Symtable_stack_init(&symtable_stack);
    // First data gathering pass
    return_code = fill_function_table();
    if (return_code != OK)
    {
        return return_code;
    }
    NEXT();

    // Second full pass
    return_code = Start();
    Symtable_stack_dispose(&symtable_stack);
    return return_code;
}