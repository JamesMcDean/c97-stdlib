#include "c97errors.h"

char _E97_ERRSTR[E97_ERROR_STRING_MAX_CHAR + 1] = {'\0'};
bool _E97_BLOCKCLEAR = false;

char* E97_ERRSTR() {
    return _E97_ERRSTR;
}

bool* E97_BLOCKCLEAR() {
    return &_E97_BLOCKCLEAR;
}

void E97_ERRSTR_WRITE(char* error) {
    // Quick write
    if (E97_ERRSTR_ISCLR()) {
        sprintf(E97_ERRSTR(), "%s", error);
        return;
    }

    // Write onto the end of the string
    // Running data
    size_t start = strlen(E97_ERRSTR());
    size_t argLength = strlen(error);

    for (size_t index = start, jndex = 0; index < E97_ERROR_STRING_MAX_CHAR 
        && jndex < argLength;) {
        // Copy char
        E97_ERRSTR()[index] = error[jndex];

        // Increment
        index += 1;
        jndex += 1;
    }
}

void E97_ERRSTR_CLR() {
    E97_ERRSTR()[0] = '\0';
}

bool E97_ERRSTR_ISCLR() {
    return E97_ERRSTR()[0] == '\0';
}
