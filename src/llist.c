#include "llist.h"
#include "_c97_stdinclude.h"

e97_int __llist_check(struct llist* list, struct _llist_node** target, ssize_t index) {
    // Running data
    e97_int result = E97_NONE;
    char temp[E97_ERROR_STRING_MAX_CHAR];

    // Is there a list
    if (list == NULL) {
        E97_ERRSTR_WRITE(__common_errors(E97_ARGUMENT_NULL));
        return E97_ARGUMENT_NULL;
    }

    // Is the list valid
    if (list->size == 0) {
        if (list->head == NULL && list->tail == NULL) return E97_NONE;

        E97_ERRSTR_WRITE("Error: Index is incorrect, llist broken.");
        return E97_BROKEN_DATATYPE | E97_LLIST_BROKEN_LINK;
    }

    if (list->head == NULL && list->size > 0) {
        E97_ERRSTR_WRITE("Error: Head broken on llist.");
        return result | E97_INITIALIZED_DATATYPE | E97_NULL_POINTER;
    }
    
    // Check completion
    size_t jndex = 0;
    struct _llist_node* current = list->head;

    // Initial links
    do {
        if (current->next == NULL) {
            sprintf(temp, "Error: The node at llist[%lu] is missing the next link.", jndex);
            E97_ERRSTR_WRITE(temp);
            return E97_LLIST_BROKEN_LINK;
        }

        // Return data if requested
        if (index >= 0 && index == jndex) {
            if (target == NULL) {
                E97_ERRSTR_WRITE(__common_errors(E97_ARGUMENT_NULL));
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
            E97_ERRSTR_WRITE("Error: List is not circular.");
            return E97_BROKEN_DATATYPE | E97_LLIST_BROKEN_LINK;
        }

        return E97_NONE;
    }
    
    // Linear
    if (current->next != NULL) {
        E97_ERRSTR_WRITE("Warning: The tail of the list is not terminated.");
        result |= W97_LIST_TERMINATION;
    }
    if (list->tail != current) {
        E97_ERRSTR_WRITE("Error: The tail of the string is not equal to the last index.");

        return result | E97_LLIST_BROKEN_LINK;
    }

    return result;
}

e97_int llist_check(struct llist* list) {
    E97_ERRSTR_CLR();
    return __llist_check(list, NULL, -1);
}

e97_int llist_insert(struct llist* list, void* data, size_t index) {
    // Running data
    E97_ERRSTR_CLR();
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

e97_int __llist_remove(struct llist* list, size_t index, void** data) {
    // Get and error check
    if (list->size == 0) {
        E97_ERRSTR_WRITE("Error: Nothing to remove, llist is empty.");
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

e97_int llist_remove(struct llist* list, size_t index, void** data) {
    E97_ERRSTR_CLR();
    return __llist_remove(list, index, data);
}

e97_int llist_pop(struct llist* list, void** data) {
    return llist_remove(list, list->size - 1, data);
}

e97_int free_llist(struct llist* list, bool freeData) {
    // Running data
    E97_ERRSTR_CLR();
    e97_int result = E97_NONE;
    void* data;
    size_t dataIndex = list->size;

    // Free data
    if (freeData) {
        while (dataIndex > 0 && (result |= __llist_remove(list, 0, &data)) >= 0) {
            free(data);
            dataIndex -= 1;
        }
    }
    else {
        while (dataIndex > 0 && (result |= __llist_remove(list, 0, NULL)) >= 0) {
            dataIndex -= 1;
        }
    }

    free(list);
    return result;
}