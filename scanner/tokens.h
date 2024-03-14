#ifndef TOKENS_H
#define TOKENS_H

#include "../common.h"

enum _TokenType {
    EndMarker,
};

typedef enum _TokenType TokenType;

struct _Token {
    TokenType type;
    char* lexeme;
    size_t length;
    size_t position;
};

typedef struct _Token Token;

#endif
