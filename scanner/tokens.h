#ifndef TOKENS_H
#define TOKENS_H

#include "../common.h"

enum _TokenType {
    Empty,
    EndMarker,
    LazyMark, // ??
    LazyPlus, // +?
    LazyStar, // *?
    LeftParen, // (
    Mark, // ?
    Or, // |
    Plus, // +
    RightParen, // )
    Star, // *
};

typedef enum _TokenType TokenType;

struct _Token {
    TokenType type;
    // Actually characters this token points to
    char* lexeme;
    // Number of characters this token points to
    size_t length;
    // Position in source string
    size_t position;
};

typedef struct _Token Token;

const char* token_type_name(TokenType t);

void print_token(Token t);

#endif
