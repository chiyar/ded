#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <string>

template <class Container>
int ReadFile(std::string filename, Container &buffer) {
    struct stat statbuf;
    int code = lstat(filename.data(), &statbuf);
    if (code == -1) {
        return -1;
    }
    size_t file_size = statbuf.st_size;

    FILE *file = std::fopen(filename.data(), "r");
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