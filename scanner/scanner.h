#ifndef SCANNER_H
#define SCANNER_H

#include "./tokens.h"

// Scanner data structure
struct _Scanner {
    // Input pattern to be transformed into Tokens
    char* source;
    // Input length, needed to make the scanner stop generating tokens after consuming the whole input
    size_t source_length;
    // Current index to be processed
    size_t current;
    // Flag to allow emitting tokens representing empty string like between ( and )
    // because the empty string can occur anywhere we need to make whether
    // the scanner generated empty string token at current position or not
    // otherwise the scanner will loop endlessly generating empty string token at the position
    bool found_empty_string;
    bool inside_braces;
};

typedef struct _Scanner Scanner;

// Construct a new scanner from a string
Scanner new_scanner(char* source, size_t length);

// Consume character in source and generate a token
Token get_next_token(Scanner* s);

// Print token in this format:
// Token { type = type_name, lexeme = "lexeme_value", length = N, position = I }
void print_token(Token t);

#endif
