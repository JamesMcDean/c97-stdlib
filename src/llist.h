#ifndef C97_LLIST_H
#define C97_LLIST_H

#include "c97errors.h"
#include <stdbool.h>

/*
The node structure involved in a doubly linked list.
*/
struct _llist_node {
    void* data;

    struct _llist_node* next;
    struct _llist_node* prev;
};

/*
A linked linked list metadata structure to be operated on as a linked list.
*/
struct llist {
    size_t size;
    bool circular;

    struct _llist_node* head;
    struct _llist_node* tail;
};

/*
Checks to make sure that the {param:}list is properly built.
*/
e97_int llist_check(struct llist* list);

/*
Stores a piece of {param:}data to the {param:}index in the {param:}list.
*/
e97_int llist_insert(struct llist* list, void* data, size_t index);

/*
Adds a piece of {param:}data to the end of the linked {param:}list.
*/
e97_int llist_append(struct llist* list, void* data);

/*
Stores a piece of data at the {param:}index provided into {param:}data, 
removing it from the linked {param:}list.
*/
e97_int llist_remove(struct llist* list, size_t index, void** data);

/*
Stores the end piece of data from the linked list into {param:}data, removing it
from the linked {param:}list.
*/
e97_int llist_pop(struct llist* list, void** data);

/*
Frees the linked {param:}list, optionally freeing all data ({param:}freeData) 
stored in the list.
*/
e97_int free_llist(struct llist* list, bool freeData);

#endif