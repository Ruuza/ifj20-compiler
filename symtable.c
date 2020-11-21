#include "symtable.h"
#include "string.h"

void Symtable_init(Symtable_node_ptr *root) {
    *root = NULL;
}

Symtable_item* Symtable_search(Symtable_node_ptr root, char *key) {
    if (root == NULL) {
        return NULL;
    } else {
        int cmp = strcmp(root->Key, key);
        if (cmp == 0) {
            return root->Value;
        } else {
            if (cmp > 0) {
                return Symtable_search(root->LPtr, key);
            } else {
                return Symtable_search(root->RPtr, key);
            }
        }
    }
}


Symtable_item* Symtable_insert(Symtable_node_ptr *RootPtr, char *key, Symtable_item* value) {
    if (*RootPtr == NULL) {
        *RootPtr = malloc(sizeof(struct Symtable_node));
        (*RootPtr)->Key = malloc(sizeof(char)*(strlen(key)+1));
        strcpy((*RootPtr)->Key, key);
        (*RootPtr)->Value = value;
        (*RootPtr)->LPtr = NULL;
        (*RootPtr)->RPtr = NULL;
        return (*RootPtr)->Value;
    } else {
        int cmp = strcmp((*RootPtr)->Key, key);
        if (cmp > 0) {
            return Symtable_insert(&(*RootPtr)->LPtr, key, value);
        } else {
            if (cmp < 0) {
                return Symtable_insert(&(*RootPtr)->RPtr, key, value);
            } else {
                return (*RootPtr)->Value;
            }
        }
    }
}

void ReplaceByRightmost(Symtable_node_ptr PtrReplaced, Symtable_node_ptr *RootPtr) {
    if (*RootPtr == NULL) {
        return;
    }

    if ((*RootPtr)->RPtr != NULL) {
        ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
    } else {
        PtrReplaced->Value = (*RootPtr)->Value;
        PtrReplaced->Key = (*RootPtr)->Key;
        if ((*RootPtr)->LPtr == NULL) {
            free_symtable_node(*RootPtr);
            (*RootPtr) = NULL;
        } else {
            Symtable_node_ptr temp = *RootPtr;
            *RootPtr = temp->LPtr;
            free_symtable_node(temp);
        }
    }

}

void Symtable_delete(Symtable_node_ptr *RootPtr, char *key) {
    if (*RootPtr == NULL) {
        return;
    }


    int cmp = strcmp((*RootPtr)->Key, key);
    if (cmp == 0) {
        if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr == NULL) {
            free_symtable_node(*RootPtr);
            *RootPtr = NULL;
        } else if ((*RootPtr)->RPtr == NULL) {
            Symtable_node_ptr LeftPtr = (*RootPtr)->LPtr;
            free_symtable_node(*RootPtr);
            *RootPtr = LeftPtr;
        } else if ((*RootPtr)->LPtr == NULL) {
            Symtable_node_ptr RightPtr = (*RootPtr)->RPtr;
            free_symtable_node(*RootPtr);
            *RootPtr = RightPtr;
        } else {
            ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
        }
    } else {
        if (cmp > 0 && (*RootPtr)->LPtr != NULL) {
            Symtable_delete(&(*RootPtr)->LPtr, key);
        }
        if (cmp < 0 && (*RootPtr)->RPtr != NULL) {
            Symtable_delete(&(*RootPtr)->RPtr, key);
        }
    }
}

void Symtable_dispose(Symtable_node_ptr *root) {
    if (*root == NULL) {
        return;
    }
    Symtable_dispose(&(*root)->RPtr);
    Symtable_dispose(&(*root)->LPtr);
    free_symtable_node(*root);
    *root = NULL;
}

void free_symtable_item(Symtable_item* item){
    if (item->parameters != NULL){
        free(item->parameters);
    }
    free(item);
}

void free_symtable_node(Symtable_node_ptr node){
    if (node == NULL){
        return;
    }
    free(node->Key);
    free_symtable_item(node->Value);
    free(node);
}

Symtable_item* create_item(){
    Symtable_item* item = malloc(sizeof(Symtable_item));
    item->dataType = DT_VOID;
    item->parameters = NULL;
    return item;
}