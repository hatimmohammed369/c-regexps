#include <ctype.h>
#include "./scanner.h"

#define METACHARACTERS_COUNT 13
static const char METACHARACTERS[METACHARACTERS_COUNT] = {
    '(', ')', '{', '}', '[' , ']', '*', '+', '.', '?', '\\', '|'
};

static bool is_metacharacter(char c) {
    for (size_t i = 0;i < METACHARACTERS_COUNT;i++)
        if (METACHARACTERS[i] == c) return true;
    return false;
}

static bool is_anchor_char(char c) {
    return c == 'A' || c == 'Z' || c == 'b' || c == 'B';
}

static bool is_slash_class_char(char c) {
    return c == 'd' || c == 'D' || c == 's' || c == 'S' || c == 'w' || c == 'W';
}

Scanner new_scanner(const char* source, size_t length) {
    char* source_copy = malloc(length);
    memcpy(source_copy, source, length);
    return (Scanner) {
        .source = source_copy,
        .source_length = length,
        .current = 0,
        .found_empty_string = false,
        .inside_parentheses = false,
        .inside_braces = false,
        .inside_brackets = false,
        .found_brackets_inverter = false,
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

static char get_char(Scanner* s, size_t position) {
    return position < s->source_length ? s->source[position] : '\0';
}

static char get_previous_char(Scanner* s) {
    return s->current == 0 ? '\0' : s->source[s->current - 1];
}

static char get_peek_char(Scanner* s) {
    return get_char(s, s->current);
}

static char get_next_char(Scanner* s) {
    return get_char(s, s->current+1);
}

Token get_next_token(Scanner* s) {
    bool is_previous_char_escaped = false;
    if (s->current >= 2) {
        is_previous_char_escaped = s->source[s->current - 2] == '\\';
    }

    char peek_char = get_peek_char(s);
    if (
        !s->inside_brackets &&
        !is_previous_char_escaped &&
        !s->found_empty_string
    ) {
        // We did the check
        // Do not attempt to generated empty string token in next iteration
        s->found_empty_string = true;
        char previous_char = get_previous_char(s);
        if (
            // The seven places a empty string token can be generated
            // An empty source string
            (s->source_length == 0 ) ||
            // Before the leading | if source string begins with a |
            (s->current == 0 && previous_char == '\0' && peek_char == '|' ) ||
            // After the trailing | if source string ends with a |
            (s->current == s->source_length && previous_char == '|' && peek_char == '\0' ) ||
            // Between two consecutive |'s
            (previous_char == '|' && peek_char == '|' ) ||
            // After a ( which is followed by a |
            (previous_char == '(' && peek_char == '|' ) ||
            // After a | which is followed by )
            (previous_char == '|' && peek_char == ')' ) ||
            // After a ( which is followed )
            (previous_char == '(' && peek_char == ')')
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
    next_token.lexeme[0] = peek_char;
    next_token.lexeme[1] = '\0';

    peek_char = get_peek_char(s);
    char next_char = get_next_char(s);

    if (s->inside_brackets && peek_char != ']') {
        if (peek_char == '^' &&
            !s->found_brackets_inverter &&
            get_previous_char(s) == '['
        ) {
            s->current++;
            s->found_brackets_inverter = true;
            next_token.type = CharacterClassInverter;
        } else {
            char lookahead[] = {
                get_char(s, s->current),
                get_char(s, s->current + 1),
                get_char(s, s->current + 2)
            };

            next_token.type = Range;
            next_token.lexeme = malloc(2);
            next_token.lexeme[0] = lookahead[0];

            if (lookahead[1] == '-') {
                bool ordered = lookahead[0] <= lookahead[2];
                bool two_digits = isdigit(lookahead[0]) && isdigit(lookahead[2]);
                bool two_lower_case_letters = islower(lookahead[0]) && islower(lookahead[2]);
                bool two_upper_case_letters = isupper(lookahead[0]) && isupper(lookahead[2]);
                if (ordered && (two_digits || two_lower_case_letters || two_upper_case_letters)) {
                    next_token.lexeme[1] = lookahead[2];
                    next_token.length = 3;
                } else {
                    // Invalid range
                    char* caret = malloc(s->source_length);
                    for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                    caret[s->current] = '^';
                    caret[s->current + 1] = '^';
                    caret[s->current + 2] = '^';
                    for (size_t i = s->current+3;i < s->source_length;i++) caret[i] = ' ';
                    fprintf(
                        stderr,
                        "Invalid range %c-%c at position %lu" "\n"
                        "%s" "\n" "%s" "\n",
                        lookahead[0], lookahead[2], s->current, s->source, caret
                    );
                    exit(1);
                }
            } else if (lookahead[0] == '\\' && is_metacharacter(lookahead[1])) {
                next_token.lexeme[0] = lookahead[1];
                next_token.lexeme[1] = lookahead[1];
                next_token.length = 2;
            } else {
                next_token.lexeme[1] = lookahead[0];
                next_token.length = 1;
            }
            s->current += next_token.length;
        }
        return next_token;
    } else if (s->inside_braces) {
        if (peek_char == ',') {
            next_token.type = Comma;
            s->current++;
            return next_token;
        } else if (isdigit(peek_char)) {
            size_t digits = 0;
            while (isdigit(get_peek_char(s))) {
                digits++;
                s->current++;
            }
            next_token.type = Integer;
            next_token.lexeme = malloc(digits);
            memcpy(next_token.lexeme, s->source+(s->current - digits), digits);
            next_token.length = digits;
            return next_token;
        }
    } else if (peek_char == '\\') {
        size_t slash_pos = s->current;
        s->current += 1; // Move past slash

        if (!has_next(s)) {
            fprintf(
                stderr,
                "Trailing \\ at end of pattern" "\n"
                "\\ must be followed by something" "\n"
                "Use `\\\\\\\\` in your pattern to match a literal \\" "\n"
            );
            exit(1);
        }

        if (is_anchor_char(next_char) || is_slash_class_char(next_char)) {
            s->current++; // Move past slash class or anchor character
            next_token.length = 2;

            if (next_char == 'A') {
                next_token.type = StartAnchor;
                next_token.lexeme = "\\A";
            } else if (next_char == 'b') {
                next_token.type = WordBoundaryAnchor;
                next_token.lexeme = "\\b";
            } else if (next_char == 'B') {
                next_token.type = NonWordBoundaryAnchor;
                next_token.lexeme = "\\B";
            } else if (next_char == 'd') {
                next_token.type = DigitClass;
                next_token.lexeme = "\\d";
            } else if (next_char == 'D') {
                next_token.type = NonDigitClass;
                next_token.lexeme = "\\D";
            } else if (next_char == 'w') {
                next_token.type = WordCharacterClass;
                next_token.lexeme = "\\w";
            } else if (next_char == 'W') {
                next_token.type = NonWordCharacterClass;
                next_token.lexeme = "\\W";
            } else if (next_char == 's') {
                next_token.type = WhitespaceClass;
                next_token.lexeme = "\\s";
            } else if (next_char == 'S') {
                next_token.type = NonWhitespaceClass;
                next_token.lexeme = "\\S";
            } else if (next_char == 'Z') {
                next_token.type = EndAnchor;
                next_token.lexeme = "\\Z";
            }
        }

        return next_token;
    } 

    switch (peek_char) {
        case '[':
            if (s->inside_brackets) {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                for (size_t i = s->current+1;i < s->source_length;i++) caret[i] = ' ';
                fprintf(
                    stderr,
                    "Nested [ at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use `\\[` to match a literal [ inside a character class" "\n"
                    "Use `\\[` to match a literal ] inside a character class" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            } else if (next_char == ']') {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                caret[s->current + 1] = '^';
                for (size_t i = s->current+2;i < s->source_length;i++) caret[i] = ' ';
                fprintf(
                    stderr,
                    "Empty character class at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use `\\[\\]` to match a [ followed by ]" "\n"
                    "Use `\\]` to match a literal ] inside a character class" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            } else if (next_char == '^' && get_char(s, s->current+2) == ']') {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current+1;i++) caret[i] = ' ';
                caret[s->current+1] = '^';
                for (size_t i = s->current+2;i < s->source_length;i++) caret[i] = ' ';
                fprintf(
                    stderr,
                    "Using ^ alone inside a character class at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Write `[\\^]` to use ^ inside a character class" "\n"
                    "Or make ^ the first character after [ if there are other characters inside [ and ]" "\n",
                    s->current+1, s->source, caret
                );
                exit(1);
            }
            s->inside_brackets = true;
            next_token.type = LeftBracket;
            break;

        case ']':
            if (!s->inside_brackets) {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                for (size_t i = s->current+1;i < s->source_length;i++) caret[i] = ' ';

                fprintf(
                    stderr,
                    "Unmatched ] at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use `\\]` to match a literal ]" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            }
            s->inside_brackets = false;
            s->found_brackets_inverter = false;
            next_token.type = RightBracket;
            break;

        case '{':
            if (s->inside_braces) {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                for (size_t i = s->current+1;i < s->source_length;i++) caret[i] = ' ';
                fprintf(
                    stderr,
                    "Nested { at position %lu" "\n"
                    "%s" "\n" "%s" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            } else if (next_char == '}') {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                caret[s->current + 1] = '^';
                for (size_t i = s->current+2;i < s->source_length;i++) caret[i] = ' ';
                fprintf(
                    stderr,
                    "Empty braces quantifieri at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use `\\{\\}` to match a { followed by }" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            }
            s->inside_braces = true;
            next_token.type = LeftBrace;
            break;

        case '}':
            if (!s->inside_braces) {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                for (size_t i = s->current+1;i < s->source_length;i++) caret[i] = ' ';

                fprintf(
                    stderr,
                    "Unmatched } at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use `\\}` to match a literal }" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            }
            s->inside_braces = false;
            next_token.type = RightBrace;
            break;

        case '(':
            s->inside_parentheses = true;
            next_token.type = LeftParen;
            break;

        case ')':
            if (!s->inside_parentheses) {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                for (size_t i = s->current+1;i < s->source_length;i++) caret[i] = ' ';

                fprintf(
                    stderr,
                    "Unmatched ) at position %lu" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use `\\)` to match a literal )" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            }
            s->inside_parentheses = false;
            next_token.type = RightParen;
            break;

        case '|':
            next_token.type = Or;
            break;

        case '?':
            if (next_char == '?') {
                s->current++;
                next_token.type = LazyMark;
                next_token.lexeme = "??";
                next_token.length = 2;
            } else if (next_char == '+') {
                s->current++;
                next_token.type = PossessiveMark;
                next_token.lexeme = "?+";
                next_token.length = 2;
            } else {
                next_token.type = Mark;
            }
            break;

        case '*':
            if (next_char == '?') {
                s->current++;
                next_token.type = LazyStar;
                next_token.lexeme = "*?";
                next_token.length = 2;
            } else if (next_char == '+') {
                s->current++;
                next_token.type = PossessiveStar;
                next_token.lexeme = "*+";
                next_token.length = 2;
            } else {
                next_token.type = Star;
            }
            break;

        case '+':
            if (next_char == '?') {
                s->current++;
                next_token.type = LazyPlus;
                next_token.lexeme = "+?";
                next_token.length = 2;
            } else if (next_char == '+') {
                s->current++;
                next_token.type = PossessivePlus;
                next_token.lexeme = "++";
                next_token.length = 2;
            } else {
                next_token.type = Plus;
            }
            break;

        case '.':
            next_token.type = Dot;
            break;

        default:
            size_t chars_count = 0;
            // Consume any non-metacharacter or any escaped metacharacter
            while (has_next(s)) {
                char peek = get_peek_char(s);
                char next = get_next_char(s);
                if (!is_metacharacter(peek)) {
                    s->current++;
                    chars_count++;
                } else if (peek == '\\' && is_metacharacter(next)) {
                    s->current += 2;
                    chars_count += 2;
                } else {
                    break;
                }
            }

            next_token.type = Literal;
            next_token.lexeme = malloc(chars_count);
            const size_t old_position = s->current - chars_count;
            size_t lexeme_length = 0;
            for (size_t i = 0, k = old_position;k < s->current;k++,i++) {
                if (is_metacharacter(s->source[k])) {
                    lexeme_length += 2;
                    k++;
                    next_token.lexeme[i] = s->source[k];
                } else {
                    lexeme_length++;
                    next_token.lexeme[i] = s->source[k];
                }
            }
            next_token.length = lexeme_length;
            next_token.position = old_position;
            return next_token;
    }
    s->current++;
    return next_token;
}
