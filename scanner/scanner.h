#ifndef SCANNER_H
#define SCANNER_H

#include "./tokens.h"

struct _Scanner {
    char* source;
    size_t length;
    size_t current;
    bool found_empty_string;
};

typedef struct _Scanner Scanner;

Scanner new_scanner(char* source, size_t length);

#endif
