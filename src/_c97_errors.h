#ifndef C97__ERRORS_H
#define C97__ERRORS_H

#include <stdint.h>
#include <stdio.h>

typedef uint32_t c97_int;
typedef int64_t e97_int;

#define E97_ERROR_STRING_MAX_CHAR 4 * (1 << 10) - 1
char E97_ERRSTR[E97_ERROR_STRING_MAX_CHAR + 1] = {'\0'};

#define ERR_CONV(x) ((1 << 31) | (x)) << 32

enum C97_ERRORS {
    E97_NONE = 0,

    E97_INVALID_DATATYPE = ERR_CONV(1ul << 0ul),
    E97_BROKEN_DATATYPE = ERR_CONV((1ul << 1ul) | E97_INVALID_DATATYPE),

    E97_NULL_POINTER = ERR_CONV(1ul << 4ul),
    
    E97_ARGUMENT_ERROR = ERR_CONV(1ul << 8ul),
    E97_ARGUMENT_NULL = ERR_CONV(1ul << 9ul) | E97_ARGUMENT_ERROR,
    
    E97_LLIST_BAD_CLOSE = ERR_CONV(1ul << 16ul),
    E97_LLIST_BROKEN_LINK = ERR_CONV(1ul << 17ul),
    E97_LLIST_EMPTY = ERR_CONV(1ul << 18ul) | E97_ARGUMENT_ERROR
};

#undef ERR_CONV(x)

enum C97_WARNINGS {
    W97_NONE = 0ul,

    W97_LIST_TERMINATION = 1ul << 0ul
};

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
    }

    return NULL;
}

#endif