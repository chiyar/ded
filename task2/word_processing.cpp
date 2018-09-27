#include <cctype>
#include <stdio.h>
#include <vector>
#include "word_processing.hpp"

void WordProcessing::PrintLines(const std::vector<char*>& lines) {
    size_t i = 0;
    for (const auto& line : lines) {
        if (CountOffset(line) > 0) {
            ++i;
            printf("%zu  %s\n", i, line);
        }
    }
}

bool WordProcessing::StrCmpByBegin(const char* const first, const char* const second) {
    size_t i = 0;
    while (first[i] != '\0' && second[i] != '\0') {
        if (first[i] != second[i]) {
            return first[i] < second[i];
        }
        ++i;
    }
    return (first[i] == '\0' && second[i] != '\0');
}

size_t WordProcessing::CountOffset(const char* const buffer) {
    size_t offset = 0;
    while (buffer[offset] != '\0') {
        ++offset;
    }
    if (offset > 0) {
        --offset;
    }
    return offset ;
}

bool WordProcessing::StrCmpByEnd(const char* const first, const char* const second) {
    size_t first_index = CountOffset(first);
    while (!std::isalpha(first[first_index])) {
        if (first[first_index] == '\0') {
            break;
        }
        --first_index;
    }

    int second_index = CountOffset(second);
    while (!std::isalpha(second[second_index])) {
        if (second[second_index] == '\0') {
            break;
        }
        --second_index;
    }

    while (first[first_index] != '\0' && second[second_index] != '\0') {
        if (first[first_index] != second[second_index]) {
            return first[first_index] < second[second_index];
        }
        --first_index;
        --second_index;
    }

    return (first[first_index] == '\0' && second[second_index] != '\0');
}
