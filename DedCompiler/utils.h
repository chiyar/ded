#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>

template<class Container>
int ReadFile(std::string filename, Container& buffer) {
    struct stat statbuf;
    int code = lstat(filename.data(), &statbuf);
    if (code == -1) {
        return -1;
    }
    size_t file_size = statbuf.st_size;

    FILE* file = std::fopen(filename.data(), "r");
    if (file == nullptr) {
        return -1;
    }

    buffer.clear();
    buffer.resize(file_size / sizeof(buffer[0]));
    if (typeid(buffer[0]) == typeid(std::string)) {
        buffer[file_size - 1] = '\0';
    }

    fread(&buffer[0], sizeof(buffer[0]), file_size, file);

    code = fclose(file);
    if (code == -1) {
        return -1;
    }

    return 0;
}

std::string ExtractLine(const std::string& buffer, size_t& begin) {
    size_t end = begin;
    while (end < buffer.size() && buffer[end] != '\n') {
        ++end;
    }

    std::string line = buffer.substr(begin, end - begin);
    begin = end + 1;
    return line;
}

std::vector<std::string> ParseLine(const std::string& line) {
    std::vector<std::string> parts;
    for (size_t cur = 0; cur < line.size();) {
        while (cur < line.size() && line[cur] == ' ') {
            ++cur;
        }

        size_t begin = cur;
        while (cur < line.size() && line[cur] != ' ') {
            ++cur;
        }

        size_t length = cur - begin;
        if (cur == ' ') {
            --length;
        }

        parts.emplace_back(line.substr(begin, length));
    }
    return parts;
}

std::string PeekAtFirstPart(const std::string& buffer, size_t& cur_pos) {
    size_t cur = cur_pos;
    while (cur < buffer.size() && buffer[cur] == ' ') {
        ++cur;
    }

    size_t begin = cur;
    std::string first_part;
    while (cur < buffer.size() && buffer[cur] != ' ' && buffer[cur] != '\n') {
        first_part += buffer[cur];
        ++cur;
    }

    if (begin == cur) {
        ++cur_pos;
    }
    return first_part;
}