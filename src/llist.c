#include "llist.h"

e97_int __llist_check(struct llist* list, struct _llist_node** target, ssize_t index) {
    // Running data
    e97_int result = E97_NONE;

    // Is there a list
    if (list == NULL) {
        char* error = __common_errors(E97_ARGUMENT_NULL);
        sprintf(E97_ERRSTR, error);
        return E97_ARGUMENT_NULL;
    }

    // Is the list valid
    if (list->size == 0) {
        if (list->head == NULL && list->tail == NULL) return E97_NONE;

        sprintf(E97_ERRSTR, "Error: Index is incorrect, llist broken.");
        return E97_BROKEN_DATATYPE | E97_LLIST_BROKEN_LINK;
    }

    if (list->head == NULL && list->size > 0) {
        sprintf(E97_ERRSTR, "Error: Head broken on llist.");
        e97_int result = E97_OR(E97_INVALID_DATATYPE, E97_BROKEN_DATATYPE);
        return E97_OR(result, E97_NULL_POINTER);
    }
    
    // Check completion
    size_t jndex = 0;
    struct _llist_node* current = list->head;

    // Initial links
    do {
        if (current->next == NULL) {
            sprintf(E97_ERRSTR, "Error: The node at llist[%lu] is missing the next link.", jndex);
            return E97_LLIST_BROKEN_LINK;
        }

        // Return data if requested
        if (index >= 0 && index == jndex) {
            if (target == NULL) {
                sprintf(E97_ERRSTR, __common_errors(E97_ARGUMENT_NULL));
                return E97_ARGUMENT_NULL;
            }

            *target = current;
            return E97_NONE;
        }

        current = current->next;
    } while (++index < list->size - 1);

    // Tail
    // Circular
    if (list->circular) {
        if (current->next != list->head) {
            sprintf(E97_ERRSTR, "Error: List is not circular.");
            return E97_BROKEN_DATATYPE | E97_LLIST_BROKEN_LINK;
        }

        return E97_NONE;
    }
    
    // Linear
    if (current->next != NULL) {
        sprintf(E97_ERRSTR, "Warning: The tail of the list is not terminated.");
        result |= W97_LIST_TERMINATION;
    }
    if (list->tail != current) {
        sprintf(E97_ERRSTR, "%sError: The tail of the string is not equal to the last index.",
            result == E97_NONE ? "" : ": ");

        return result | E97_LLIST_BROKEN_LINK;
    }

    return result;
}

e97_int llist_check(struct llist* list) {
    return __llist_check(list, NULL, -1);
}

e97_int llist_add(struct llist* list, void* data, size_t index) {
    e97_int result = E97_NONE;

    // Appending
    if (index == list->size) {
        // Check list to make sure tail exists
        result = __llist_check(list, NULL, -1);
        if (result < 0) return result;

        // Create node
        struct _llist_node* newNode = calloc(1, sizeof(struct _llist_node));
        newNode->data = data;
        
        // TODO - Fix all below for zero case
        // Link to next if circular
        if (list->circular) {
            list->tail->next->prev = newNode;
            newNode->next = list->tail->next;
        }

        // Link to previous
        newNode->prev = list->tail;
        list->tail->next = newNode;
        list->tail = newNode;
    }
    // Inserting at 0
    else if (index == 0) {
        // Error checking
        struct _llist_node* target;
        result = __llist_check(list, &target, 0);
        if (result < 0) return result;


    }
}