#ifndef C97_VECTOR_H
#define C97_VECTOR_H

#include "_c97_stdinclude.h"

/*
1K entries seems like a decent amount that *probably* won't break a page.
*/
#define DEFAULT_VECTOR_CAPACITY 1 << 10

/*
A simple expanding vector. It should be noted, {param:}data is stored as void**,
an array pointing to void pointers.
*/
struct vector {
    size_t size;
    size_t _capacity;
    void** data;
};

/*
Checks a vector's integrity.
*/
e97_int vector_check(struct vector* vec);

/*
Populates a vector if not already.
*/
e97_int vector_init(struct vector* vec);

/*
Inserts an item ({param:}data) into the vector ({param:}vec), 
shifting all items at {param:}index and above to the right by 1.
*/
e97_int vector_insert(struct vector* vec, void* data, size_t index);

/*
Appends {param:}data to the end of {param:}vec.
*/
e97_int vector_append(struct vector* vec, void* data);

/*
Removes an item ({param:}data) from {param:}vec at the {param:}index. It should 
be noted that if {param:}data is equal to NULL, no data will be extracted and
the data will not be freed.
*/
e97_int vector_remove(struct vector* vec, size_t index, void** data);

/* 
Removes {param:}data from the end of {param:}vec. It should be noted that if
{param:}data is equal to NULL, no data will be extracted and the data will not
be freed.
*/
e97_int vector_pop(struct vector* vec, void** data);

/*
Frees the vector.
*/
e97_int free_vec(struct vector* vec, bool freeData);

#endif