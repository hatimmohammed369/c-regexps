#include "./scanner.h"

Scanner new_scanner(char* source, size_t length) {
    char* source_copy = malloc(length);
    memcpy(source_copy, source, length);
    Scanner new_scanner = {
      .source = source_copy,
      .length = length,
      .current = 0,
      .found_empty_string = false,
    };
    return new_scanner;
}

static const char* token_type_name(TokenType t) {
    switch (t) {
    case EndMarker:
        return "EndMarker";
    }
}

void print_token(Token t) {
    printf("Token { type = %s, lexeme = %s, length = %d, position = %d}",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}

static bool has_next(Scanner s) {
    return s.current < s.length;
}

static Token make_end_marker(size_t source_length) {
    return (Token){.type = EndMarker, .lexeme="", .length=0, .position=source_length};
}

Token get_next_token(Scanner s) {
    s.current += 1;
    return make_end_marker(s.length);
}
