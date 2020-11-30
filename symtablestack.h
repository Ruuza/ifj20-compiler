#ifndef IFJ20C_SYMTABLESTACK_H
#define IFJ20C_SYMTABLESTACK_H

#include "symtable.h"

typedef struct{
    Symtable_node_ptr* stack;
    int capacity;
    int top;
} Symstack;

/**
 * Initializes symbol stack
 * @param symstack
 */
void Symtable_stack_init(Symstack** symstack);

/**
 * Removes top symbol table from stack and returns it
 * Stack transverse ownership of symtable to caller
 * @param symstack
 * @return Symtable_node_ptr or NULL if stack is empty
 */
Symtable_node_ptr Symtable_stack_pop(Symstack* symstack);

/**
 * Inserts symtable to the top of the stack. Stack capacity grows if necessary.
 * Stack takes ownership of symtable
 * @param symstack
 * @param symtable
 */
void Symtable_stack_insert(Symstack* symstack, Symtable_node_ptr symtable);

/**
 * Returns pointer to Symtable_node_ptr at top of the stack
 * Stack keeps ownership
 * @param symstack
 * @return non-owning Symbol table pointer
 */
Symtable_node_ptr Symtable_stack_head(Symstack *symstack);

/**
 * Frees all stack memory and memory of stored objects
 * @param symstack
 */
void Symtable_stack_dispose(Symstack **symstack);

#endif //IFJ20C_SYMTABLESTACK_H
