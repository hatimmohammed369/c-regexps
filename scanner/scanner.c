#include "./scanner.h"

#define METACHARACTERS_COUNT 9
static const char METACHARACTERS[METACHARACTERS_COUNT] = {'(', ')', '*', '+', '?', '\\', '^', '|', '$'};
static bool is_metacharacter(char c) {
    for (size_t i = 0;i < METACHARACTERS_COUNT;i++)
        if (METACHARACTERS[i] == c) return true;
    return false;
}

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

static char next_char(Scanner* s) {
    return s->current + 1 >= s->source_length ? '\0' : s->source[s->current + 1];
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

    char next = next_char(s);
    switch (peek) {
        case '^':
            next_token.type = StartAnchor;
            break;

        case '$':
            next_token.type = EndAnchor;
            break;

        case '(':
            next_token.type = LeftParen;
            break;

        case ')':
            next_token.type = RightParen;
            break;

        case '|':
            next_token.type = Or;
            break;

        case '?':
            if (next == '?') {
                s->current++;
                next_token.type = LazyMark;
                next_token.lexeme = "??";
                next_token.length = 2;
            } else if (next == '+') {
                s->current++;
                next_token.type = PossessiveMark;
                next_token.lexeme = "?+";
                next_token.length = 2;
            } else {
                next_token.type = Mark;
            }
            break;

        case '*':
            if (next == '?') {
                s->current++;
                next_token.type = LazyStar;
                next_token.lexeme = "*?";
                next_token.length = 2;
            } else if (next == '+') {
                s->current++;
                next_token.type = PossessiveStar;
                next_token.lexeme = "*+";
                next_token.length = 2;
            } else {
                next_token.type = Star;
            }
            break;

        case '+':
            if (next == '?') {
                s->current++;
                next_token.type = LazyPlus;
                next_token.lexeme = "+?";
                next_token.length = 2;
            } else if (next == '+') {
                s->current++;
                next_token.type = PossessivePlus;
                next_token.lexeme = "++";
                next_token.length = 2;
            } else {
                next_token.type = Plus;
            }
            break;

        case '\\':
            s->current++;
            next_token.length = 2;
            if (next == 'd') {
                next_token.type = DigitClass;
                next_token.lexeme = "\\d";
            } else if (next == 'D') {
                next_token.type = NonDigitClass;
                next_token.lexeme = "\\D";
            } else if (next == 'w') {
                next_token.type = WordBoundaryClass;
                next_token.lexeme = "\\w";
            } else if (next == 'W') {
                next_token.type = NonWordBoundaryClass;
                next_token.lexeme = "\\W";
            } else if (next == 's') {
                next_token.type = WhitespaceClass;
                next_token.lexeme = "\\s";
            } else if (next == 'S') {
                next_token.type = NonWhitespaceClass;
                next_token.lexeme = "\\S";
            } else if (!has_next(s)) {
                fprintf(
                    stderr,
                    "Trailing \\\n"
                    "\\ must be followed by something\n"
                    "Use \"\\\\\\\\\" in your pattern to match a literal \\\n"
                );
                exit(1);
            } else {
                fprintf(
                    stderr,
                    "Bad escape \\%c at position %u\n"\
                    "Use \"\\\\%c\" in your pattern\n",
                    s->source[s->current],
                    s->current == 0 ? 0 : s->current-1,
                    s->source[s->current]
                );
                exit(1);
            }

        default:
            size_t literal_length = 0;
            size_t old_position = s->current;
            while (!is_metacharacter(peek)) {
                s->current++;
                literal_length++;
                peek = peek_char(s);
            }
            next_token.type = Literal;
            next_token.lexeme = malloc(literal_length);
            memcpy(next_token.lexeme, s->source+old_position, literal_length);
            next_token.length = literal_length;
            next_token.position = old_position;
            return next_token;
    }

    s->current++;

    return next_token;
}
