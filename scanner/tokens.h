#ifndef TOKENS_H
#define TOKENS_H

#include "../common.h"

enum _TokenType {
    // Anchors
    EndAnchor, // \Z
    StartAnchor, // \A
    WordBoundaryAnchor, // \b
    NonWordBoundaryAnchor, // \B

    // Slash classes
    DigitClass, // \d
    NonDigitClass, // \D
    WhitespaceClass, // \s
    NonWhitespaceClass, // \S
    WordCharacterClass, // \w
    NonWordCharacterClass, // \W

    // Special
    Empty,
    EndMarker,
    Literal,
    Dot,
    Comma,
    Integer,
    Range,
    CharacterClassInverter, // ^

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

    LeftBracket, // [
    RightBracket, // ]
    LeftBrace, // {
    RightBrace, // }
    LeftParen, // (
    RightParen, // )
    Or, // |
};

typedef enum _TokenType TokenType;

struct _Token {
    TokenType type;
    // Actually characters this token points to
    char* lexeme;
    // Number of characters this token points to in source string
    // NOT number of characters in field 'lexeme'
    size_t length;
    // Position in source string
    size_t position;
};

typedef struct _Token Token;

const char* token_type_name(TokenType t);

void print_token(Token t);

#endif
