#include "./tokens.h"

const char* token_type_name(TokenType t) {
    switch (t) {
    case Digit:
        return "Digit";
    case Empty:
        return "Empty";
    case EndAnchor:
        return "EndAnchor";
    case EndMarker:
        return "EndMarker";
    case LazyMark:
        return "LazyMark";
    case LazyPlus:
        return "LazyPlus";
    case LazyStar:
        return "LazyStar";
    case LeftParen:
        return "LeftParen";
    case Mark:
        return "Mark";
    case NonDigit:
        return "NonDigit";
    case NonWhitespace:
        return "NonWhitespace";
    case NonWordBoundary:
        return "NonWordBoundary";
    case Or:
        return "Or";
    case Plus:
        return "Plus";
    case PossessiveMark:
        return "PossessiveMark";
    case PossessivePlus:
        return "PossessivePlus";
    case PossessiveStar:
        return "PossessiveStar";
    case RightParen:
        return "RightParen";
    case Star:
        return "Star";
    case StartAnchor:
        return "StartAnchor";
    case Whitespace:
        return "Whitespace";
    case WordBoundary:
        return "WordBoundary";
    }
    return "UNKNOWN";
}

void print_token(Token t) {
    printf("Token { type = %s, lexeme = %s, length = %u, position = %u }",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}
