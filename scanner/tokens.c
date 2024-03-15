#include "./tokens.h"

const char* token_type_name(TokenType t) {
    switch (t) {
        // Anchors
        case EndAnchor:
            return "EndAnchor";
        case StartAnchor:
            return "StartAnchor";

        // Slash classes
        case DigitClass:
            return "DigitClass";
        case NonDigitClass:
            return "NonDigitClass";
        case NonWhitespaceClass:
            return "NonWhitespaceClass";
        case NonWordBoundaryClass:
            return "NonWordBoundaryClass";
        case WhitespaceClass:
            return "WhitespaceClass";
        case WordBoundaryClass:
            return "WordBoundaryClass";

        // Special
        case Empty:
            return "Empty";
        case EndMarker:
            return "EndMarker";
        case Literal:
            return "Literal";

        // Lazy Quantifiers
        case LazyMark:
            return "LazyMark";
        case LazyPlus:
            return "LazyPlus";
        case LazyStar:
            return "LazyStar";

        // Basic Quantifiers
        case Mark:
            return "Mark";
        case Star:
            return "Star";
        case Plus:
            return "Plus";

        // Possessive Quantifiers
        case PossessiveMark:
            return "PossessiveMark";
        case PossessivePlus:
            return "PossessivePlus";
        case PossessiveStar:
            return "PossessiveStar";

        case LeftParen:
            return "LeftParen";
        case RightParen:
            return "RightParen";
        case Or:
            return "Or";
    }
    return "UNKNOWN";
}

void print_token(Token t) {
    printf("Token { type = %s, lexeme = %s, length = %u, position = %u }",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}
