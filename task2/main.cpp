#include <algorithm>
#include <stdio.h>
#include "text.hpp"
#include "word_processing.hpp"

void GreetUser() {
    printf("# This is text sorter.\n# Write filename to sort text.\n# Max length of filename is 256.\n");
}

void SayGoodbye() {
    printf("# Thank you for using!\n# Authored by Yaroslav Chizh\n");
}

int main() {
    GreetUser();

    char filename[256];
    scanf("%s", filename);

    Text text(filename);
    switch (text.GetState()) {
        case Text::State::ERR_LSTAT:
            printf("Error in lstat(\x1B[31m%s\x1B[0m) occured.\nTry another filename.\n", filename);
            SayGoodbye();
            return 0;
        case Text::State::ERR_FOPEN:
            printf("Error in fopen(\x1B[31m%s\x1B[0m) occured.\nTry another filename.\n", filename);
            SayGoodbye();
            return 0;
        case Text::State::ERR_FCLOSE:
            printf("Error in fclose(\x1B[31m%s\x1B[0m) occured.\nTry another filename.", filename);
            SayGoodbye();
            return 0;
        default:
            break;
    }
    std::vector<char*> lines = text.GetLines();
    std::sort(lines.begin(), lines.end(), WordProcessing::StrCmpByBegin);
    printf("\nSorted:\n\n");
    WordProcessing::PrintLines(lines);

    
    lines = text.GetLines();
    std::sort(lines.begin(), lines.end(), WordProcessing::StrCmpByEnd);
    printf("\nRhymed:\n\n");
    WordProcessing::PrintLines(lines);

    lines = text.GetLines();
    printf("\nOriginal:\n\n");
    WordProcessing::PrintLines(lines);

    SayGoodbye();
    return 0;
}
