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
