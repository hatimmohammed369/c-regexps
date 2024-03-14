#ifndef SCANNER_H
#define SCANNER_H

#include "./tokens.h"

struct _Scanner {
    char* source;
    size_t source_length;
    size_t current;
    bool found_empty_string;
};

typedef struct _Scanner Scanner;

Scanner new_scanner(char* source, size_t length);
Token get_next_token(Scanner* s);
void print_token(Token t);

#endif
