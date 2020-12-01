#include "symtablestack.h"

#define SYMSTACK_MIN_SIZE 10

void Symtable_stack_init(Symtable_stack** symstack){
    *symstack = malloc(sizeof(Symtable_stack));
    (*symstack)->stack = malloc(sizeof(Symtable_node_ptr)*SYMSTACK_MIN_SIZE);
    (*symstack)->capacity = SYMSTACK_MIN_SIZE;
    (*symstack)->top = -1;
}
Symtable_node_ptr Symtable_stack_pop(Symtable_stack* symstack){
    if (symstack->top == -1){
        fprintf(stderr, "Empty pop");
        return NULL;
    }
    Symtable_node_ptr topPtr = *(symstack->stack+symstack->top);
    symstack->top--;
    return topPtr;
}

void Symtable_stack_insert(Symtable_stack* symstack, Symtable_node_ptr symtable){
    if (symstack->top == symstack->capacity){
        symstack->capacity = (int)(symstack->capacity * 1.5);
        symstack->stack = realloc(symstack->stack, sizeof(Symtable_node_ptr)*symstack->capacity);
    }
    symstack->top++;
    *(symstack->stack+symstack->top) = symtable;
}

Symtable_node_ptr Symtable_stack_head(Symtable_stack* symstack){
    if (symstack->top == -1){
        return NULL;
    }
    return *(symstack->stack+symstack->top);
}

void Symtable_stack_dispose(Symtable_stack** symstack){
    for (int i = 0; i <= (*symstack)->top; ++i) {
        free_symtable_node(*((*symstack)->stack+i));
        free(*((*symstack)->stack+i));
        *((*symstack)->stack+i) = NULL;
    }
    free((*symstack)->stack);
    free(*symstack);
    *symstack = NULL;
}
