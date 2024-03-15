#include "./tokens.h"

const char* token_type_name(TokenType t) {
    switch (t) {
    case StartAnchor:
        return "StartAnchor";
    case Empty:
        return "Empty";
    case EndMarker:
        return "EndMarker";
    case EndAnchor:
        return "EndAnchor";
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
    case Or:
        return "Or";
    case Plus:
        return "Plus";
    case PossessiveMark:
        return "PossessiveMark";
    case PossessiveStar:
        return "PossessiveStar";
    case PossessivePlus:
        return "PossessivePlus";
    case RightParen:
        return "RightParen";
    case Star:
        return "Star";
    }
    return "UNKNOWN";
}

void print_token(Token t) {
    printf("Token { type = %s, lexeme = %s, length = %u, position = %u }",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}
