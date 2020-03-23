#include "vector.h"

e97_int vector_check(struct vector* vec) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;

    // Testing
    if (vec == NULL) {
        sprintf(E97_ERRSTR, "Error: Vector is NULL.");
        return E97_ARGUMENT_NULL;
    }
    if (vec->_capacity == 0 && vec->data != NULL) {
        sprintf(E97_ERRSTR, "Warning: Vector not freed when cleared.");
        result |= W97_NOTFREED;
    }
    if (vec->size > vec->_capacity) {
        sprintf(E97_ERRSTR, "Error: Vector did not close correctly.");
        return result | E97_VECTOR_BAD_CLOSE;
    }
    if (vec->_capacity > 0 && vec->data == NULL) {
        sprintf(E97_ERRSTR, "Error: Vector missing data.");
        return result | E97_BROKEN_DATATYPE;
    }

    return result;
}

e97_int _vector_double(struct vector* vec) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result;

    // Error checks
    if ((result = vector_check(vec)) < 0) return result;

    // Expand and 0
    size_t newCap = vec->_capacity * 2;
    vec->data = realloc(vec->data, newCap);
    for (size_t index = vec->_capacity; index < newCap; index++) {
        vec->data[index] = NULL;
    }
    vec->_capacity = newCap;

    return result;
}

e97_int vector_init(struct vector* vec) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;

    // Error checks
    if ((result = vector_check(vec)) < 0) return result;
    if (vec->data != NULL) {
        sprintf(E97_ERRSTR, "Warning: Vector still has data.");
        return W97_NOTFREED | result;
    }

    // Populate
    if (vec->_capacity > 0) {
        vec->data = calloc(vec->_capacity, sizeof(void*));
    }
    else {
        vec->data = calloc(DEFAULT_VECTOR_CAPACITY, sizeof(void*));
        vec->_capacity = DEFAULT_VECTOR_CAPACITY;
    }
    vec->size = 0;

    return result;
}

e97_int vector_insert(struct vector* vec, void* data, size_t index) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;

    // Error checks
    if ((result = vector_check(vec)) < 0) return result;

    // Expand if needed and recurse
    if (index >= vec->size) vec->size = index + 1;
    if (vec->size + 1 >= vec->_capacity) {
        if (vec->size == 0) {
            result |= vector_init(vec);
        }
        else {
            result |= _vector_double(vec);
        }

        return vector_insert(vec, data, index);
    }

    // Shift
    for (size_t jndex = vec->size; jndex > index; jndex--) {
        vec->data[jndex] = vec->data[jndex - 1];
    }
    vec->size += 1;

    // Place data
    vec->data[index] = data;

    return result;
}

e97_int vector_append(struct vector* vec, void* data) {
    return vector_insert(vec, data, vec->size);
}

e97_int vector_remove(struct vector* vec, size_t index, void** data) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;

    // Error checking
    if ((result = vector_check(vec)) < 0) return result;
    if (index >= vec->size) {
        sprintf(E97_ERRSTR, 
            E97_ERRSTR[0] = '\0' ? "%s%s" : "%s: %s", E97_ERRSTR, 
            __common_errors(E97_ARGUMENT_RANGE));
        return result | E97_ARGUMENT_RANGE;
    }

    // Grab the data
    if (data != NULL) *data = vec->data[index];
    
    // Shift
    for (size_t jndex = index; jndex < vec->size - 1; jndex++) {
        vec->data[jndex] = vec->data[jndex + 1];
    }
    vec->data[vec->size - 1] = NULL;

    // Lower size
    vec->size -= 1;

    return result;
}

e97_int vector_pop(struct vector* vec, void** data) {
    // This purposefully throws an error when the vector is empty
    return vec->size == 0 ? vector_remove(vec, DEFAULT_VECTOR_CAPACITY, data) 
        : vector_remove(vec, vec->size - 1, data);
}

e97_int vector_clear(struct vector* vec, bool freeData) {
    // Running data
    E97_ERRSTR[0] = '\0';
    e97_int result = E97_NONE;

    // Error checks
    if ((result = vector_check(vec)) < 0) {
        return result;
    }

    // Free data option
    if (freeData) {
        void* current;
        for (size_t index = 0; index < vec->_capacity; index++) {
            current = vec->data[index];
        }

        if (current != NULL);
    }

    // Set size to zero
    vec->size = 0;

    return result;
}

e97_int free_vec(struct vector* vec, bool freeData) {
    // Error init
    E97_ERRSTR[0] = '\0';

    // Check to make sure that the vector is not NULL
    if (vec == NULL) {
        sprintf(E97_ERRSTR, __common_errors(E97_ARGUMENT_NULL));
        return E97_ARGUMENT_NULL | (freeData ? W97_NOTFREED : E97_NONE);
    }

    // Free data if selected
    if (freeData) {
        for (size_t index = 0; index < vec->size; index++) {
            if (vec->data[index] != NULL) free(vec->data[index]);
        }
    }
    if (vec->data != NULL) free(vec->data);
    else {
        sprintf(E97_ERRSTR, __common_warnings(W97_FREED));
        return W97_FREED;
    }
    
    free(vec);
    return E97_NONE;
}