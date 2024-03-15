#include "./tokens.h"

const char* token_type_name(TokenType t) {
    switch (t) {
    case Empty:
        return "Empty";
    case EndMarker:
        return "EndMarker";
    case LeftParen:
        return "LeftParen";
    case Mark:
        return "Mark";
    case Or:
        return "Or";
    case Plus:
        return "Plus";
    case RightParen:
        return "RightParen";
    case Star:
        return "Star";
    }
    return "Unknown";
}

void print_token(Token t) {
    printf("Token { type = %s, lexeme = %s, length = %u, position = %u }",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}
