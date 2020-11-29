#include "precedence-stack.h"
#include <stdio.h>

int STACK_SIZE = MAX_STACK;

/**
 * Initialize stack. Set top to -1.
 * 
 * @param tPrecedenceStack *s pointer to stack
 * @return -1 if *s is NULL
 */
int precedenceStackInit(tPrecedenceStack *s)
{

    if (s == NULL)
    {
        return -1;
    }

    //Identifies, that stack is empty
    s->top = -1;
    for (int i = 0; i < MAX_STACK; i++)
    {
        s->shift[i] = false;
        s->nonterminal[i] = false;
    }

    return 1;
}

/**
 * return nonzero value if stack is empty, else return 0.
 * @param s pointer to stack
 * @return 1 if *s is NULL else 0
*/
int precedenceStackEmpty(const tPrecedenceStack *s)
{

    return s->top == -1;
}

/**
 * Return nonzero value, if stack is full. Else return 0.
 * 
 * @param s pointer to stack
 * @return 1 if stack is full, else 0
 */
int precedenceStackFull(const tPrecedenceStack *s)
{

    return (s->top + 1) == STACK_SIZE;
}

/**
 *  Set the value from top of stack in variables.
 * 
 * @param s pointer to stack
 * @param token pointer to token, which value will be set as value in stack
 * @param isShift pointer, which value will be set as value in stack
 * @param isNonterminal pointer, which value will be set as value in stack
 * @return 1 if succes, 0 if stack is empty
*/
int precedenceStackTop(const tPrecedenceStack *s, Token *token, bool *isShift, bool *isNonterminal)
{

    if (precedenceStackEmpty(s))
    {
        return 0;
    }

    //Insert the value from top of the stack into the variable c
    *token = s->tokens[s->top];
    *isShift = s->shift[s->top];
    *isNonterminal = s->nonterminal[s->top];
    return 1;
}

/**
 *  Set the value from top of stack in variables.
 * 
 * @param s pointer to stack
 * @param token pointer to token, which value will be set as value in stack
 * @return 1 if succes, 0 if stack is empty or there is none terminal in stack
*/
int precedenceStackTopTerminal(const tPrecedenceStack *s, Token *token)
{

    if (precedenceStackEmpty(s))
    {
        return 0;
    }

    int currentTop = s->top;

    while (currentTop >= 0)
    {
        if (s->shift[currentTop] || s->nonterminal[currentTop])
        {
            currentTop--;
            continue;
        }

        *token = s->tokens[currentTop];
        break;
    }
    return 1;
}

/**
 * Removes the element from top of the stack
 * @param s pointer to stack
 * @return 1 if succes, 0 when stack is empty
*/
int precedenceStackPop(tPrecedenceStack *s)
{

    if (precedenceStackEmpty(s))
    {
        return 0;
    }

    s->shift[s->top] = NULL;
    s->nonterminal[s->top] = NULL;
    s->top -= 1;

    return 1;
}

/**
 * Insert element to top of the stack
 * @param s pointer to stack
 * @param c character to insert
 * @return 1 if succes, 0 when stack is empty
*/
int precedenceStackPush(tPrecedenceStack *s, Token token, bool shift, bool nonterminal)
{

    if (precedenceStackFull(s))
    {
        return 0;
    }

    //increment top of stack
    s->top += 1;
    //insert element on top of stack
    s->tokens[s->top] = token;
    s->shift[s->top] = shift;
    s->nonterminal[s->top] = nonterminal;

    return 1;
}