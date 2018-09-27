#pragma once

#include <vector>

namespace WordProcessing {
    void PrintLines(const std::vector<char*>& lines);

    size_t CountOffset(const char* const buffer);

    bool StrCmpByBegin(const char* const fisrt, const char* const second);

    bool StrCmpByEnd(const char* const first, const char* const second);
}
