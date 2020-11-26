#ifndef _PRECEDENCE_STACK_H_
#define _PRECEDENCE_STACK_H_

#include "token.h"
#include <stdbool.h>

#define MAX_STACK 1024

typedef struct
{
    Token tokens[MAX_STACK];     /* Array of tokens */
    bool shift[MAX_STACK];       /* Array of shifts */
    bool nonterminal[MAX_STACK]; /* Array of nonterminals */
    int top;                     /* Index of element on top of the stack */
} tPrecedenceStack;

/**
 * Initialize stack. Set top to -1.
 * 
 * @param s pointer to stack
 * @return -1 if *s is NULL
 */
int precedenceStackInit(tPrecedenceStack *s);

/**
 * return nonzero value if stack is empty, else return 0.
 * @param s pointer to stack
 * @return 1 if *s is NULL else 0
*/
int precedenceStackEmpty(const tPrecedenceStack *s);

/**
 * Return nonzero value, if stack is full. Else return 0.
 * 
 * @param s pointer to stack
 * @return 1 if stack is full, else 0
 */
int precedenceStackFull(const tPrecedenceStack *s);

/**
 *  Set the value from top of stack in variables.
 * 
 * @param s pointer to stack
 * @param token pointer to token, which value will be set as value in stack
 * @param isShift pointer, which value will be set as value in stack
 * @param isNonterminal pointer, which value will be set as value in stack
 * @return 1 if succes, 0 if stack is empty
*/
int precedenceStackTop(const tPrecedenceStack *s, Token *token, bool *isShift, bool *isNonterminal);

/**
 *  Set the value from first terminal closest to top of stack in variables.
 * 
 * @param s pointer to stack
 * @param token pointer to token, which value will be set as value in stack
 * @return 1 if succes, 0 if stack is empty or there is none terminal in stack
*/
int precedenceStackTopTerminal(const tPrecedenceStack *s, Token *token);

/**
 * Removes the element from top of the stack
 * @param s pointer to stack
 * @return 1 if succes, 0 when stack is empty
*/
int precedenceStackPop(tPrecedenceStack *s);

/**
 * Insert element to top of the stack
 * @param s pointer to stack
 * @param c character to insert
 * @return 1 if succes, 0 when stack is empty
*/
int precedenceStackPush(tPrecedenceStack *s, Token token, bool shift, bool nonterminal);

#endif //_PRECEDENCE_STACK_H_