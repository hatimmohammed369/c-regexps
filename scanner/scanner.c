#include "./scanner.h"

Scanner new_scanner(char* source, size_t length) {
    char* source_copy = malloc(length);
    memcpy(source_copy, source, length);
    return (Scanner) {
      .source = source_copy,
      .source_length = length,
      .current = 0,
      .found_empty_string = false,
    };
}

static const char* token_type_name(TokenType t) {
    switch (t) {
    case Empty:
        return "Empty";
    case EndMarker:
        return "EndMarker";
    case LeftParen:
        return "LeftParen";
    case Or:
        return "Or";
    case RightParen:
        return "RightParen";
    }
    return "Unknown";
}

void print_token(Token t) {
    printf("Token { type = %s, lexeme = %s, length = %u, position = %u }",
           token_type_name(t.type), t.lexeme, t.length, t.position);
    printf("\n");
}

static Token make_end_marker(size_t source_length) {
    return (Token){.type = EndMarker, .lexeme="", .length=0, .position=source_length};
}

static Token make_empty_token(size_t position) {
        return (Token){.type = Empty, .lexeme="", .length=0, .position=position};
}

static bool has_next(Scanner* s) {
    return s->current < s->source_length;
}

static char previous_char(Scanner* s) {
    return s->current == 0 ? '\0' : s->source[s->current - 1];
}

static char peek_char(Scanner* s) {
    return !has_next(s) ? '\0' : s->source[s->current];
}

Token get_next_token(Scanner* s) {
    bool is_previous_char_escaped = false;
    if (s->current >= 2) {
        is_previous_char_escaped = s->source[s->current - 2] == '\\';
    }

    char peek = peek_char(s);
    if (!is_previous_char_escaped && !s->found_empty_string) {
        // We did the check
        // Do not attempt to generated empty string token in next iteration
        s->found_empty_string = true;
        char previous = previous_char(s);
        if (
            // The seven places a empty string token can be generated
            // An empty source string
            s->source_length == 0 ||
            // Before the leading | if source string begins with a |
            s->current == 0 && previous == '\0' && peek == '|' ||
            // After the trailing | if source string ends with a |
            s->current == s->source_length && previous == '|' && peek == '\0' ||
            // Between two consecutive |'s
            previous == '|' && peek == '|' ||
            // After a ( which is followed by a |
            previous == '(' && peek == '|' ||
            // After a | which is followed by )
            previous == '|' && peek == ')' ||
            // After a ( which is followed )
            previous == '(' && peek == ')'
        ) {
            return make_empty_token(s->current);
        }
    }

    // Attempt to generated empty string token in next iteration
    s->found_empty_string = false;

    if (!has_next(s)) {
        return make_end_marker(s->source_length);
    }

    Token next_token = (Token) {
        .type = EndMarker,
        .lexeme = "",
        .length = 1,
        .position = s->current,
    };
    next_token.lexeme = malloc(2);
    next_token.lexeme[0] = peek;
    next_token.lexeme[1] = '\0';

    switch (peek_char(s)) {
        case '(':
            next_token.type = LeftParen;
            break;

        case ')':
            next_token.type = RightParen;
            break;

        case '|':
            next_token.type = Or;
            break;
    }

    s->current++;

    return next_token;
}
