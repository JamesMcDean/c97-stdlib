#ifndef C97__ERRORS_H
#define C97__ERRORS_H

#include <stdint.h>
#include <stdio.h>

// Normal return
typedef uint32_t c97_int;

// Error capable return
typedef int64_t e97_int;

// Error String definitions
#define E97_ERROR_STRING_MAX_CHAR 4 * (1 << 10) - 1
char E97_ERRSTR[E97_ERROR_STRING_MAX_CHAR + 1] = {'\0'};
bool E97_BLOCKCLEAR = false;
#define E97_ERRSTR_CLR() { E97_ERRSTR[0] = '\0'; }
#define E97_ERRSTR_ISCLR() (E97_ERRSTR[0] == '\0')
#define E97_ERRSTR_WRITE(newErr) {\
    if (E97_ERRSTR_ISCLR() && !E97_BLOCKCLEAR) {\
        sprintf(E97_ERRSTR, (newErr));\
    }\
    else {\
        sprintf(E97_ERRSTR, "%s: %s", E97_ERRSTR, (newErr));\
    }\
}

// Error definitions
#define ERR_CONV(x) ((1 << 31) | (x)) << 32

enum C97_ERRORS {
    E97_NONE = 0,

    E97_INVALID_DATATYPE = ERR_CONV(1ul << 0ul),
    E97_BROKEN_DATATYPE = ERR_CONV(1ul << 1ul) | E97_INVALID_DATATYPE,
    E97_UNINITIALIZED_DATATYPE = ERR_CONV(1ul << 2ul) | E97_INVALID_DATATYPE,
    E97_INITIALIZED_DATATYPE = ERR_CONV(1ul << 3ul) | E97_INVALID_DATATYPE,
    E97_NULL_POINTER = ERR_CONV(1ul << 4ul),
    E97_BAD_CLOSE = ERR_CONV(1ul << 5ul),
    
    E97_ARGUMENT_ERROR = ERR_CONV(1ul << 8ul),
    E97_ARGUMENT_NULL = ERR_CONV(1ul << 9ul) | E97_ARGUMENT_ERROR,
    E97_ARGUMENT_RANGE = ERR_CONV(1ul << 10ul) | E97_ARGUMENT_ERROR,
    
    E97_LLIST_BAD_CLOSE = ERR_CONV(1ul << 16ul) | E97_BAD_CLOSE,
    E97_LLIST_BROKEN_LINK = ERR_CONV(1ul << 17ul),
    E97_LLIST_EMPTY = ERR_CONV(1ul << 18ul) | E97_ARGUMENT_ERROR,

    E97_VECTOR_BAD_CLOSE = ERR_CONV(1ul << 19ul) | E97_BAD_CLOSE,
    E97_VECTOR_NOT_CLEAR = ERR_CONV(1ul << 20ul),

    E97_HMAP_MISSING_FUNCTION = ERR_CONV(1ul << 21ul),
    E97_HMAP_BAD_CLOSE = ERR_CONV(1ul << 22ul) | E97_BAD_CLOSE
};

#undef ERR_CONV(x)

// Warning definitions
enum C97_WARNINGS {
    W97_NONE = 0ul,

    W97_TERMINATION = 1ul << 0ul,
    W97_NOTFREED = 1ul << 1ul,
    W97_FREED = 1ul << 2ul,
    W97_SIZE_MISMATCH = 1ul << 3ul,
    W97_NOTFOUND = 1ul << 4ul,
    W97_FOUND = 1ul << 5ul,

    W97_HASH_REPLACE = 1ul << 8ul,
    W97_HASH_NOHASH = 1ul << 9ul,
    W97_HASH_NOCOMPARE = 1ul << 10ul,
    W97_LIST_TERMINATION = 1ul << 11ul
};

/*
Turns an {param:}error code into a string that does not need to be freed. If
{param:}error does not have a prebuilt string, NULL is returned.
*/
static inline char* __common_errors(e97_int error) {
    switch (error) {
        case E97_NONE: {
            return "No error.";
        }

        case E97_INVALID_DATATYPE: {
            return "Error: Invalid datatype.";
        }

        case E97_BROKEN_DATATYPE: {
            return "Error: Broken datatype.";
        }

        case E97_ARGUMENT_ERROR: {
            return "Error: Argument error.";
        }

        case E97_ARGUMENT_NULL: {
            return "Error: Argument NULL.";
        }

        case E97_ARGUMENT_RANGE: {
            return "Error: Argument out of range.";
        }

        case E97_UNINITIALIZED_DATATYPE: {
            return "Error: Datatype has not be initialized yet.";
        }
    }

    return NULL;
}

/*
Turns a {param:}warning code into a string that does not need to be freed. If
{param:}warning does not have a prebuilt string, NULL is returned.
*/
static inline char* __common_warnings(e97_int warning) {
    switch (warning) {
        case W97_TERMINATION: {
            return "Warning: This termination could be bad.";
        }
        
        case W97_NOTFREED: {
            return "Warning: Something was not freed.";
        }

        case W97_FREED: {
            return "Warning: This appears to have already been freed.";
        }
    }

    return NULL;
}

#endif