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

static bool is_valid_escape_char(char c) {
    return is_anchor_char(c) || is_slash_class_char(c) || is_metacharacter(c);
}

static bool is_basic_quantifier(char c) {
    return c == '?' || c == '*' || c == '+';
}

Scanner new_scanner(char* source, size_t length) {
    char* source_copy = malloc(length);
    memcpy(source_copy, source, length);
    return (Scanner) {
        .source = source_copy,
        .source_length = length,
        .current = 0,
        .found_empty_string = false,
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

    char peek = get_peek_char(s);
    if (!is_previous_char_escaped && !s->found_empty_string) {
        // We did the check
        // Do not attempt to generated empty string token in next iteration
        s->found_empty_string = true;
        char previous = get_previous_char(s);
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

    char next = get_next_char(s);
    switch (peek) {
        case '[':
            if (s->inside_brackets) {
                char* caret = malloc(s->source_length);
                for (size_t i = 0;i < s->current;i++) caret[i] = ' ';
                caret[s->current] = '^';
                for (size_t i = s->current+1;i < s->source_length;i++) caret[i] = ' ';
                fprintf(
                    stderr,
                    "Nested [ at position %u" "\n"
                    "%s" "\n" "%s" "\n"
                    "Use \\[ to match a literal [ inside a character class" "\n"
                    "Use \\] to match a literal ] inside a character class" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            }
            s->inside_brackets = true;
            next_token.type = LeftBracket;
            break;

        case ']':
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
                    "Nested { at position %u" "\n"
                    "%s" "\n" "%s" "\n",
                    s->current, s->source, caret
                );
                exit(1);
            }
            s->inside_braces = true;
            next_token.type = LeftBrace;
            break;

        case '}':
            s->inside_braces = false;
            next_token.type = RightBrace;
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

        case '.':
            next_token.type = Dot;
            break;

        case '\\':
            s->current += 2;
            next_token.length = 2;
            if (next == 'A') {
                next_token.type = StartAnchor;
                next_token.lexeme = "\\A";
            } else if (next == 'b') {
                next_token.type = WordBoundaryAnchor;
                next_token.lexeme = "\\b";
            } else if (next == 'B') {
                next_token.type = NonWordBoundaryAnchor;
                next_token.lexeme = "\\B";
            } else if (next == 'd') {
                next_token.type = DigitClass;
                next_token.lexeme = "\\d";
            } else if (next == 'D') {
                next_token.type = NonDigitClass;
                next_token.lexeme = "\\D";
            } else if (next == 'w') {
                next_token.type = WordCharacterClass;
                next_token.lexeme = "\\w";
            } else if (next == 'W') {
                next_token.type = NonWordCharacterClass;
                next_token.lexeme = "\\W";
            } else if (next == 's') {
                next_token.type = WhitespaceClass;
                next_token.lexeme = "\\s";
            } else if (next == 'S') {
                next_token.type = NonWhitespaceClass;
                next_token.lexeme = "\\S";
            } else if (next == 'Z') {
                next_token.type = EndAnchor;
                next_token.lexeme = "\\Z";
            } else if (!has_next(s)) {
                fprintf(
                    stderr,
                    "Trailing \\" "\n"
                    "\\ must be followed by something" "\n"
                    "Use \"\\\\\\\\\" in your pattern to match a literal \\" "\n"
                );
                exit(1);
            } else if(!is_metacharacter(next)) {
                fprintf(
                    stderr,
                    "Bad escape \\%c at position %u" "\n"
                    "Use \"\\\\\\\\%1$c\" in your pattern to match a literal \\ followed by %1$c" "\n",
                    s->source[s->current],
                    s->current == 0 ? 0 : s->current-1
                );
                exit(1);
            }

            char c0 = next_token.lexeme[0];
            char c1 = next_token.lexeme[1];
            if (c0 != '\\' || !is_anchor_char(c1)) {
                s->current -= 2;
            }

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (s->inside_braces) {
                size_t digits = 0;
                while ('0' <= peek && peek <= '9') {
                    digits++;
                    s->current++;
                    peek = get_peek_char(s);
                }
                next_token.type = Integer;
                next_token.lexeme = malloc(digits);
                memcpy(next_token.lexeme, s->source+(s->current - digits), digits);
                next_token.length = digits;
                return next_token;
            }

        case ',':
            if (s->inside_braces) {
                next_token.type = Comma;
                break;
            }

        default:
            if (s->inside_brackets) {
                if (peek == '^' &&
                    !s->found_brackets_inverter &&
                    get_previous_char(s) == '['
                ) {
                    s->current++;
                    s->found_brackets_inverter = true;
                    next_token.type = CharacterClassInverter;
                } else {
                    char low = get_char(s, s->current);
                    char separator = get_char(s, s->current + 1);
                    char high = get_char(s, s->current + 2);

                    next_token.type = Range;
                    next_token.lexeme = malloc(2);
                    next_token.lexeme[0] = low;

                    if (
                        separator == '-'
                    ) {
                        if (
                            low <= high &&
                            (
                                isdigit(low) && isdigit(high) ||
                                islower(low) && islower(high) ||
                                isupper(low) && isupper(high)
                            )
                        ) {
                            next_token.lexeme[1] = high;
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
                                "Invalid range %c-%c at position %u" "\n"
                                "%s" "\n" "%s" "\n",
                                low, high, s->current, s->source, caret
                            );
                            exit(1);
                        }
                    } else {
                        next_token.lexeme[1] = low;
                        next_token.length = 1;
                    }
                    s->current += next_token.length;
                }
            } else {
                size_t chars_count = 0;
                if (!is_metacharacter(peek)) {
                    s->current++;
                    chars_count++;
                } else if (peek == '\\' && is_metacharacter(next)) {
                    s->current += 2;
                    chars_count++;
                }

                while (has_next(s)) {
                    char next_chars[] = {
                        get_char(s, s->current),
                        get_char(s, s->current + 1),
                        get_char(s, s->current + 2),
                    };

                    bool quantified_char = !is_metacharacter(next_chars[0])
                        && is_basic_quantifier(next_chars[1]);
                    bool quantified_slashed_item = next_chars[0] == '\\'
                        && is_valid_escape_char(next_chars[1])
                        && is_basic_quantifier(next_chars[2]);

                    if (quantified_char || quantified_slashed_item) {
                        break;
                    } else {
                        peek = next_chars[0];
                        next = next_chars[1];
                        if (peek == '\\') {
                            if (!has_next(s)) {
                                // Trailing slash
                                fprintf(
                                    stderr,
                                    "Trailing \\\n"
                                    "\\ must be followed by something\n"
                                    "Use \"\\\\\\\\\" in your pattern to match a literal \\\n"
                                );
                                exit(1);
                            } else if (!is_valid_escape_char(next)) {
                                // Invalid escape, something like \H
                                fprintf(
                                    stderr,
                                    "Bad escape \\%c at position %u\n"\
                                    "Use \"\\\\\\\\%1$c\" in your pattern to match a \\ followed by %1$c\n",
                                    s->source[s->current],
                                    s->current == 0 ? 0 : s->current-1
                                );
                                exit(1);
                            } else if (is_metacharacter(next)) {
                                // Escaped metacharacter, something \+
                                s->current += 2;
                                chars_count += 1;
                            } else {
                                // Any other valid escape sequence like \A or \d
                                break;
                            }
                        } else if (!is_metacharacter(peek)) {
                            // An ordinary character like `z`
                            s->current++;
                            chars_count++;
                        } else {
                            // Any other metacharacter
                            break;
                        }
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
            }
            return next_token;
    }

    s->current++;

    return next_token;
}
