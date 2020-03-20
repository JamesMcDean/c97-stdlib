#ifndef C97_LLIST_H
#define C97_LLIST_H

#include "_c97_stdinclude.h"

struct _llist_node {
    void* data;

    struct _llist_node* next;
    struct _llist_node* prev;
};

struct llist {
    size_t size;
    bool circular;

    struct _llist_node* head;
    struct _llist_node* tail;
};

e97_int llist_check(struct llist* list);
e97_int llist_add(struct llist* list, void* data, size_t index);
e97_int llist_append(struct llist* list, void* data);
e97_int llist_remove(struct llist* list, size_t index);
e97_int free_llist(struct llist* list, bool freeData);
e97_int free_llist_precise(struct llist* list, struct llist* freeList);

#endif