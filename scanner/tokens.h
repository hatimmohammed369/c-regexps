#ifndef TOKENS_H
#define TOKENS_H

#include "../common.h"

enum _TokenType {
    // Anchors
    EndAnchor, // $
    StartAnchor, // ^

    // Slash classes
    DigitClass, // \d
    NonDigitClass, // \D
    NonWhitespaceClass, // \S
    NonWordBoundaryClass,
    WhitespaceClass, // \s
    WordBoundaryClass, // \w

    // Special
    Empty,
    EndMarker,
    Literal,

    // Lazy Quantifiers
    LazyMark, // ??
    LazyPlus, // +?
    LazyStar, // *?

    // Basic Quantifiers
    Mark, // ?
    Star, // *
    Plus, // +

    // Possessive Quantifiers
    PossessiveMark, // ?+
    PossessivePlus, // ++
    PossessiveStar, // *+

    LeftParen, // (
    RightParen, // )
    Or, // |
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
