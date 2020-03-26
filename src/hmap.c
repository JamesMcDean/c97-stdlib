#include "hmap.h"

#define HASH_TO_INDEX(hash, cap) (((size_t) hash * 113) + 59) % (size_t) cap

e97_int __hmap_check(struct hmap* map) {
    if (map == NULL) {
        E97_ERRSTR_WRITE(__common_errors(E97_ARGUMENT_NULL));
        return E97_ARGUMENT_NULL;
    }

    if (!map->initialized) {
        E97_ERRSTR_WRITE(__common_errors(E97_UNINITIALIZED_DATATYPE));
        return E97_UNINITIALIZED_DATATYPE;
    }

    if (map->count > map->_capacity) {
        E97_ERRSTR_WRITE("Error: {param:}map->count is larger than {param:}map->_capacity.");
        return E97_INVALID_DATATYPE;
    }

    if (map->keyHash == NULL || map->keyCompare == NULL) {
        E97_ERRSTR_WRITE("Error: Missing one of the required key or value functions.");
        return E97_HMAP_MISSING_FUNCTION | E97_NULL_POINTER;
    }

    if (map->entries == NULL) {
        E97_ERRSTR_WRITE("Error: Missing the collection of entries.");
        return E97_NULL_POINTER;
    }

    return E97_NONE;
}

e97_int hmap_check(struct hmap* map) {
    E97_ERRSTR_CLR();
    return __hmap_check(map);
}

hash_t __default_hash(void* key) {
    return (hash_t) key;
}

int __default_compare(void* a, void* b) {
    return ((int) a) - ((int) b);
}

e97_int __hmap_expand(struct hmap* map);

c97_int __hmap_get(struct hmap* map, void* key, struct _hmap_entry** result) {
    // Find the key
    hash_t hash = map->keyHash(key);
    size_t index = HASH_TO_INDEX(hash, map->_capacity);
    size_t offset = 1;
    size_t iterNum = 1;
    struct _hmap_entry* entry = &map->entries[index];

    while (entry->key != NULL && !entry->dirty && 
        map->keyCompare(key, entry->key) != 0) {
        // Increment
        index = (index + offset) % map->_capacity;
        offset += 2 * iterNum;

        // Reference
        entry = &map->entries[index];
    }

    // Return the entry
    *result = entry;

    return entry->key == NULL ? W97_NOTFOUND : W97_FOUND;
}

e97_int __hmap_put(struct hmap* map, void* key, void* value, void** oldValue) {
    // Running data
    e97_int result;

    // Error checks
    if ((result = __hmap_check(map)) < 0) return result;
    if (key == NULL) {
        E97_ERRSTR_WRITE("Error: {param:}key == NULL is not allowed.");
        return E97_ARGUMENT_NULL | result;
    }

    // Initial expansion
    if ((double) map->count / map->_capacity > MAP_EXPAND_PERCENTAGE) {
        if ((result |= __hmap_expand(map)) < 0) return result;
    }

    // Hash and find
    struct _hmap_entry* entry;
    result |= __hmap_get(map, key, &entry);

    // New entry
    if (entry->key == NULL) {
        // Add
        entry->dirty = true;
        entry->key = key;
        entry->value = value;

        // Set return
        if (oldValue != NULL) *oldValue = NULL;
        result |= W97_HASH_REPLACE;

        // Increment
        map->count += 1;
    }
    else {
        // Set return
        if (oldValue != NULL) *oldValue = entry->value;

        // Add value
        entry->value = value;
    }

    return result;
}

e97_int __hmap_expand(struct hmap* map) {
    // Running data
    e97_int result = E97_NONE;
    struct _hmap_entry* oldData = map->entries;
    size_t oldCapacity = map->_capacity;

    // Expand
    map->_capacity *= 2;
    map->entries = calloc(map->_capacity, sizeof(struct _hmap_entry));

    // Rehash
    map->count = 0;
    for (size_t index = 0; index < oldCapacity; index++) {
        struct _hmap_entry* current = &oldData[index];
        if (current->key == NULL) continue;

        result |= __hmap_put(map, current->key, current->value, NULL);
    }
    
    // Close up
    free(oldData);
    return result;
}

e97_int hmap_init(struct hmap* map, hash_t (*keyHash)(void*), 
    int (*keyCompare)(void*, void*)) {
    // Running data
    E97_ERRSTR_CLR();
    e97_int result = E97_NONE;

    // Error checks
    if (map == NULL) {
        E97_ERRSTR_WRITE(__common_errors(E97_ARGUMENT_NULL));
        return E97_ARGUMENT_NULL;
    }
    if (map->initialized) {
        E97_ERRSTR_WRITE("Error: The map is already initialized.");
        return E97_INITIALIZED_DATATYPE;
    }
    if (map->entries != NULL) {
        E97_ERRSTR_WRITE("Error: The map still has data, bad close?");
        return E97_HMAP_BAD_CLOSE;
    }
    if (map->keyHash == NULL) {
        E97_ERRSTR_WRITE("Warning: Null {param:}keyHash, using default.");
        map->keyHash = __default_hash;
        result |= W97_HASH_NOHASH;
    }
    if (map->keyCompare == NULL) {
        E97_ERRSTR_WRITE("Warning: Null {param:}keyCompare, using default.");

        map->keyCompare = __default_compare;
        result |= W97_HASH_NOCOMPARE;
    }

    // Init map
    map->initialized = true;

    map->count = 0;
    if (map->_capacity == 0) map->_capacity = DEFAULT_MAP_CAPACITY;

    map->keyHash = keyHash;
    map->keyCompare = keyCompare;

    map->entries = calloc(map->_capacity, sizeof(struct _hmap_entry));

    return result;
}

e97_int hmap_get(struct hmap* map, void* key, void** value) {
    // Error checking
    E97_ERRSTR_CLR();
    e97_int result = E97_NONE;

    if (key == NULL) {
        E97_ERRSTR_WRITE("Error: The {param:}key cannot be NULL.");
        return result | E97_ARGUMENT_NULL;
    }
    if (value == NULL) {
        E97_ERRSTR_WRITE("Error: The {param:}value cannot be NULL.");
        return result | E97_ARGUMENT_NULL;
    }
    if ((result |= __hmap_check(map)) < 0) return result;

    // Get the data
    struct _hmap_entry* temp;
    result |= __hmap_get(map, key, &temp);
    *value = temp->value;

    return result;
}

e97_int hmap_contains(struct hmap* map, void* key, bool* result) {
    // Running data
    E97_ERRSTR_CLR();
    e97_int iresult = E97_NONE;

    // Error checks
    if (key == NULL || result == NULL) {
        E97_ERRSTR_WRITE(__common_errors(E97_ARGUMENT_NULL));
        return E97_ARGUMENT_NULL;
    }
    if ((iresult = __hmap_check(map)) < 0) {
        return iresult;
    }

    // Search fpr the key's enrty
    struct _hmap_entry* temp;
    iresult |= __hmap_get(map, key, &temp);
    *result = (temp->key != NULL);

    return iresult;
}

e97_int hmap_put(struct hmap* map, void* key, void* value, void** oldValue) {
    E97_ERRSTR_CLR();
    return __hmap_put(map, key, value, oldValue);
}

e97_int hmap_remove(struct hmap* map, void* key, void** value) {
    // Running data
    E97_ERRSTR_CLR();
    e97_int result = E97_NONE;
    struct _hmap_entry* temp;

    // Error checks
    if (key == NULL) {
        E97_ERRSTR_WRITE("Error: {param:}key is not allowed to be NULL.");
        return E97_ARGUMENT_NULL;
    }
    if (value == NULL) {
        E97_ERRSTR_WRITE("Error: {param:}value is not allowed to be NULL.");
        return E97_ARGUMENT_NULL;
    }
    if ((result = __hmap_check(map)) < 0) return result;

    // Get the entry to be deleted
    result |= __hmap_get(map, key, &temp);
    if (temp->key == NULL) {
        *value = NULL;
        return result;
    }
    *value = temp->value;

    // Delete the entry
    temp->value = NULL;
    temp->key = NULL;
    // This is to be explicit, even though it is already set always
    temp->dirty = true;

    // Update the map definition
    map->count -= 1;

    return result;
}

e97_int hmap_getKeys(struct hmap* map, struct vector* keys) {
    // Running data
    E97_ERRSTR_CLR();
    e97_int result = E97_NONE;
    struct _hmap_entry* current;

    // Error checks
    if (keys == NULL) {
        E97_ERRSTR_WRITE(__common_errors(E97_ARGUMENT_NULL));
        return E97_ARGUMENT_NULL;
    }
    if ((result |= __hmap_check(map)) < 0) {
        return result;
    }

    E97_BLOCKCLEAR = true;
    if ((result |= vector_check(keys)) < 0) {
        return result;
    }
    // Set up
    if ((result |= vector_clear(keys, true)) < 0) {
        return result;
    }

    // Collect keys
    for (size_t index = 0; index < map->_capacity; index++) {
        current = &map->entries[index];
        if (current->key != NULL) vector_append(keys, current->key);
    }
    E97_BLOCKCLEAR = false;

    // This shouldn't happen but it might
    assert(keys->size == map->count);

    return result;
}

e97_int free_hmap(struct hmap* map, bool freeData) {
    // Running data
    E97_ERRSTR_CLR();
    e97_int result = 0;
    struct _hmap_entry* entry;

    // Error checking
    if ((result = __hmap_check(map)) < 0) {
        return result;
    }

    // Free data if set
    // Trying a new style
    if (freeData) for (size_t index = 0; index < map->_capacity; index++) {
        entry = &map->entries[index];

        if (entry->key != NULL) {
            if (entry->value != NULL) free(entry->value);
            free(entry->key);
        }
    }
    
    // Free the map
    free(map->entries);
    free(map);

    return result;
}