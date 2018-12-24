#include <iostream>
#include <vector>


#include "commands.h"
#include "utils.h"


std::string FormInstruction(Command command, const std::vector<double>& args) {
    std::string instruction = name_by_command[command];
    for (const auto& arg : args) {
        instruction += " " + std::to_string(arg);
    }

    return instruction;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid count of arguments.\n Enter name of input file\n";
        return 0;
    }

    std::string input_name(argv[1]);
    std::string output_name("da.txt");

    std::vector<double> buffer;
    if (ReadFile(input_name, buffer) == -1) {
        std::cout << "Invalid filename\n";
        return 0;
    }

    for (const auto& it : buffer) {
        std::cout << it << "\n";
    }

    FILE* output = std::freopen(output_name.data(), "w", stdout);
    for (size_t i = 0; i < buffer.size();) {
        Command command = static_cast<Command>(buffer[i]);
        ++i;

        std::vector<double> args;
        if (HasOneArg(command)) {
            args.emplace_back(buffer[i]);
            ++i;
        }

        std::cout << FormInstruction(command, args) << "\n";
    }
    fclose(output);
    return 0;
}