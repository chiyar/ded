#pragma once

#include <vector>

class Text {
public:
    enum class State {
        OK,
        ERR_LSTAT,
        ERR_FOPEN,
        ERR_FCLOSE
    };

    Text(const char* const filename);

    std::vector<char*> GetLines() const;

    State GetState() const;

    ~Text();

private:
    void InitLines();

private:
    char* buffer_;
    size_t length_;
    std::vector<char*> lines_, ends_;
    State state_;
};
