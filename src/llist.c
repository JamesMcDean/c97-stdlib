#include "llist.h"

e97_int __llist_check(struct llist* list, struct _llist_node** target, ssize_t index) {
    // Running data
    E97_ERRSTR[0] = '\0';
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

e97_int llist_insert(struct llist* list, void* data, size_t index) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;

    // Appending
    if (index == list->size) {
        // Check list to make sure tail exists
        result = __llist_check(list, NULL, -1);
        if (result < 0) return result;

        // Create node
        struct _llist_node* newNode = calloc(1, sizeof(struct _llist_node));
        newNode->data = data;
        
        // Empty list
        if (list->size == 0) {
            // Link to root
            list->head = newNode;
            list->tail = newNode;

            // Link self if circular
            if (list->circular) {
                newNode->next = newNode;
                newNode->prev = newNode;
            }
        }
        // Not empty list
        else {
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
    }
    // Inserting at 0
    else if (index == 0) {
        // Error checking
        struct _llist_node* temp;
        result = __llist_check(list, &temp, 0);
        if (result < 0) return result;

        // Build node
        struct _llist_node* newNode = calloc(1, sizeof(struct _llist_node));
        newNode->data = data;

        // Link
        if (list->circular) {
            newNode->prev = list->tail;
            list->tail->next = newNode;
            list->tail = newNode;
        }

        // Shwoop
        newNode->next = list->head;
        list->head->prev = newNode;
        list->head = newNode;
    }
    // Inserting in general
    else {
        // Get & test
        struct _llist_node* temp;
        result = __llist_check(list, &temp, index);
        if (result < 0) return result;

        // Build
        struct _llist_node* newNode = calloc(1, sizeof(struct _llist_node));
        newNode->data = data;

        // Link
        newNode->prev = temp->prev;
        temp->prev->next = newNode;
        temp->prev = newNode;
        newNode->next = temp;
    }

    // Finish up
    list->size += 1;
    return result;
}

e97_int llist_append(struct llist* list, void* data) {
    return llist_insert(list, data, list->size);
}

e97_int llist_remove(struct llist* list, size_t index, void** data) {
    // Error init
    E97_ERRSTR[0] = '\0';

    // Get and error check
    if (list->size == 0) {
        sprintf(E97_ERRSTR, "Error: Nothing to remove, llist is empty.");
        return E97_LLIST_EMPTY;
    } 

    // Running data
    struct _llist_node* current;
    e97_int result = __llist_check(list, &current, index);
    if (result < 0) return result;

    // Unlink
    if (data != NULL) *data = current->data;
    // Final remove
    if (list->size == 1) {
        free(current);
        list->head = NULL;
        list->tail = NULL;
    }
    // Remove tail
    else if (index == list->size - 1) {
        list->tail = current->prev;

        if (list->circular) {
            list->head->prev = list->tail;
            list->tail->next = list->head;
        }
        else {
            list->tail->next = NULL;
        }

        free(current);
    }
    // Remove regular
    else {
        current->prev->next = current->next;
        current->next->prev = current->prev;

        free(current);
    }

    list->size -= 1;
    return result;
}

e97_int llist_pop(struct llist* list, void** data) {
    return llist_remove(list, list->size - 1, data);
}

e97_int free_llist(struct llist* list, bool freeData) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;
    void* data;
    size_t dataIndex = list->size;

    // Free data
    if (freeData) {
        while (dataIndex > 0 && (result |= llist_remove(list, 0, &data)) >= 0) {
            free(data);
            dataIndex -= 1;
        }
    }
    else {
        while (dataIndex > 0 && (result |= llist_remove(list, 0, NULL)) >= 0) {
            dataIndex -= 1;
        }
    }

    free(list);
    return result;
}