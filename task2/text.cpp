#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "text.hpp"

Text::Text(const char* const filename) {
    state_ = State::OK;
    struct stat statbuf;
    int code = lstat(filename, &statbuf);
    if (code == -1) {
        state_ = State::ERR_LSTAT;
        return;
    }
    size_t file_size = statbuf.st_size;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        state_ = State::ERR_FOPEN;
        return;
    }
    length_ = file_size + 2;
    buffer_ = new char[length_];
    buffer_[0] = '\0';
    buffer_[length_ - 1] = '\0';
    fread(buffer_ + 1, sizeof(char), file_size, file);
    code = fclose(file);
    if (code != 0) {
        state_ = State::ERR_FCLOSE;
        return;
    }

    InitLines();
}

std::vector<char*> Text::GetLines() const {
    return lines_;
}

Text::State Text::GetState() const {
    return state_;
}

Text::~Text() {
    if (state_ == State::OK || state_ == State::ERR_FCLOSE) {
        delete[] buffer_;
    }
}

void Text::InitLines() {
    for (size_t i = 1; i < length_; ++i) {
        if (buffer_[i] == '\n') {
            buffer_[i] = '\0';
        }
        if (buffer_[i - 1] == '\0') {
            lines_.emplace_back(buffer_ + i);
        }
    }
}
