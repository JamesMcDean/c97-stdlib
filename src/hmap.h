#ifndef C97_MAP_H
#define C97_MAP_H

#include "_c97_stdinclude.h"
#include "vector.h"

// Defines
#define DEFAULT_MAP_CAPACITY 1 << 10
#define MAP_EXPAND_PERCENTAGE 0.7
typedef uint32_t hash_t;

/* 
The entries hashed to in the map structure.
*/
struct _hmap_entry {
    bool dirty;
    void* key;
    void* value;
};

/*
The metadata for the map structure.
*/
struct hmap {
    bool initialized;

    size_t count;
    size_t _capacity;

    hash_t (*keyHash)(void*);
    int (*keyCompare)(void*, void*);

    struct _hmap_entry* entries;
};

/*
Checks the hashmap ({param:}map) for errors. Note, if the map is NULL an error 
returned.
*/
e97_int hmap_check(struct hmap* map);

/*
Initializes the hasmap ({param:}map) with {param:}keyHash as the hash function
and {param:}keyCompare as the compare function for the keys in the hashmap. Note:
clears the error string {global:}E97_ERRSTR.
*/
e97_int hmap_init(struct hmap* map, hash_t (*keyHash)(void*), 
    int (*keyCompare)(void*, void*));

/*
Gets a {param:}value from the hashmap {param:}map depending on the {param:}key
provided. Note: If any argument is NULL, error is returned.
*/
e97_int hmap_get(struct hmap* map, void* key, void** value);

/*
Checks to see if the hashmap ({param:}map) for {param:}key, and returns the
value to the location pointed to by {param:}result. If the {param:}result pointer
is equal to NULL, an error is returned. Note: Clears the error string 
{global:}E97_ERRSTR.
*/
e97_int hmap_contains(struct hmap* map, void* key, bool* result);

/*
Put the {param:}value at the {param:}key location in the hashmap ({param:} map).
If {param:}key is NULL, an error is returned. If {param:}oldValue is not NULL,
if there was a value at {param:}key, oldValue points to the pointer for the value
that was overwritten.
*/
e97_int hmap_put(struct hmap* map, void* key, void* value, void** oldValue);

/*
Removes the {param:}value at {param:}key from the hashmap ({param:}map). If value
is not equal to NULL, {param:}value points to the pointer that holds the data 
for the value removed.
*/
e97_int hmap_remove(struct hmap* map, void* key, void** value);

/*
Gets all of the {param:}keys from the hashmap ({param:}map), for iteration etc.
If either of the arguments are NULL, an error is returned.
*/
e97_int hmap_getKeys(struct hmap* map, struct vector* keys);

/*
Frees the hashmap ({param:}map) and optionally frees the data with the argument
{param:}freeData.
*/
e97_int free_hmap(struct hmap* map, bool freeData);

#endif