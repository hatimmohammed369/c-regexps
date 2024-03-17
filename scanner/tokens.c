#include "./tokens.h"

const char* token_type_name(TokenType t) {
    switch (t) {
        // Anchors
        case EndAnchor:
            return "EndAnchor";
        case StartAnchor:
            return "StartAnchor";
        case WordBoundaryAnchor:
            return "WordBoundaryAnchor";
        case NonWordBoundaryAnchor:
            return "NonWordBoundaryAnchor";

        // Slash classes
        case DigitClass:
            return "DigitClass";
        case NonDigitClass:
            return "NonDigitClass";
        case WhitespaceClass:
            return "WhitespaceClass";
        case NonWhitespaceClass:
            return "NonWhitespaceClass";
        case WordCharacterClass:
            return "WordCharacterClass";
        case NonWordCharacterClass:
            return "NonWordCharacterClass";

        // Special
        case Empty:
            return "Empty";
        case EndMarker:
            return "EndMarker";
        case Literal:
            return "Literal";
        case Dot:
            return "Dot";
        case Comma:
            return "Comma";
        case Integer:
            return "Integer";
        case Range:
            return "Range";
        case CharacterClassInverter:
            return "CharacterClassInverter";

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

        case LeftBracket:
             return "LeftBracket";
        case RightBracket:
             return "RightBracket";
        case LeftBrace:
             return "LeftBrace";
        case RightBrace:
             return "RightBrace";
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
    printf("Token { type = %s, lexeme = %s, length = %lu, position = %lu }",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}
