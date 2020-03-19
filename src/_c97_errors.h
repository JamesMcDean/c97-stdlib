#ifndef C97__ERRORS_H
#define C97__ERRORS_H

#include <stdint.h>

typedef uint32_t c97_int;
typedef int64_t e97_int;

#define C97_ERROR_STRING_MAX_CHAR 4 * (1 << 10)
char C97_ERRSTR[C97_ERROR_STRING_MAX_CHAR] = {'\0'};

enum C97_ERRORS {
    E97_NONE = 0,

    E97_INVALID_DATATYPE = -(1 << 0),
    E97_BROKEN_DATATYPE = -((1 << 1) & -E97_INVALID_DATATYPE),
    
    E97_ARGUMENT_ERROR = -(1 << 8),
    E97_ARGUMENT_NULL = -((1 << 9) & -E97_ARGUMENT_ERROR)
};

static inline e97_int E97_OR(e97_int errorA, e97_int errorB) {
    return -((-errorA) | (-errorB));
}

static inline e97_int E97_AND(e97_int errorA, e97_int errorB) {
    return -((-errorA) & (-errorB));
}

static inline e97_int E97_XOR(e97_int errorA, e97_int errorB) {
    return -((-errorA) ^ (-errorB));
}

static inline e97_int E97_LS(e97_int error, uint8_t shift) {
    return -((-error) << shift);
}

static inline e97_int E97_RS(e97_int error, uint8_t shift) {
    return -((-error) >> shift);
}

#endif