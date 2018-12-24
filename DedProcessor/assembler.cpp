#include <iostream>
#include <cstdio>
#include <string>
#include <vector>


#include "commands.h"
#include "utils.h"


std::string ExtractLine(const std::string &buffer, size_t &begin) {
    size_t end = begin;
    while (end < buffer.size() && buffer[end] != '\n') {
        ++end;
    }

    std::string line = buffer.substr(begin, end - begin);
    begin = end + 1;
    return line;
}

std::vector<std::string> ParseLine(const std::string &line) {
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

enum class AssemblyStatus {
    OK,
    INVALID_NAME,
    INVALID_ARGS_CNT
};

std::pair<AssemblyStatus, std::vector<double>>
AssemblyCommand(const std::vector<std::string> &parts) {
    for (const auto &part : parts) {
        std::cout << part << " ";
    }
    std::cout << "\n";

    auto found = command_by_name.find(parts[0]);
    if (found == command_by_name.end()) {
        return {AssemblyStatus::INVALID_NAME, std::vector<double>()};
    }

    Command command = found->second;
    if (HasNoArg(command) && parts.size() != 1) {
        return {AssemblyStatus::INVALID_ARGS_CNT, std::vector<double>()};
    }

    if (HasOneArg(command) && parts.size() != 2) {
        return {AssemblyStatus::INVALID_ARGS_CNT, std::vector<double>()};
    }

    std::vector<double> instruction;
    instruction.emplace_back(command);
    for (size_t i = 1; i < parts.size(); ++i) {
        instruction.emplace_back(std::stod(parts[i]));
    }
    return {AssemblyStatus::OK, instruction};
}

int ScanInstructions(const std::string &buffer, bool is_for_label, const std::string &output_name,
                     std::unordered_map<int, size_t> &instruction_number_by_label) {

    FILE *output;
    if (!is_for_label) {
        output = fopen(output_name.data(), "w");
    }

    for (size_t begin = 0, cur_program_size = 0; begin < buffer.size();) {
        std::string line = ExtractLine(buffer, begin);
        if (line.empty()) {
            continue;
        }

        auto parts = ParseLine(line);
        auto[status, instruction] = AssemblyCommand(parts);
        if (status == AssemblyStatus::INVALID_NAME) {
            std::cout << "Invalid command: " << parts[0] << "\n";
            std::cout << "Assembling terminated\n";
            if (!is_for_label) {
                fclose(output);
            }
            return -1;
        } else if (status == AssemblyStatus::INVALID_ARGS_CNT) {
            std::cout << "Invalid number of arguments for command: " << parts[0] << "\n";
            std::cout << "Assembling terminated\n";
            if (!is_for_label) {
                fclose(output);
            }
            return -1;
        } else {
            Command command = static_cast<Command>(instruction[0]);
            if (is_for_label) {
                if (command == LABEL) {
                    instruction_number_by_label[static_cast<int>(instruction[1])] = cur_program_size;
                } else {
                    cur_program_size += instruction.size();
                }
            } else {
                if (RequiresLabel(command)) {
                    instruction[1] = instruction_number_by_label[instruction[1]];
                }
                if (command != LABEL) {
                    fwrite(instruction.data(), sizeof(instruction[0]), instruction.size(), output);
                }
            }
        }
    }
    if (!is_for_label) {
        fclose(output);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Invalid count of arguments.\n Enter name of input file\n";
        return 0;
    }

    std::string input_name(argv[1]);
    std::string output_name("a.o");

    std::string buffer;
    if (ReadFile(input_name, buffer) == -1) {
        std::cout << "Invalid filename\n";
        return 0;
    }

    std::unordered_map<int, size_t> instruction_number_by_label;

    if (ScanInstructions(buffer, true, "", instruction_number_by_label) == -1) {
        return 0;
    }

    ScanInstructions(buffer, false, output_name, instruction_number_by_label);
    return 0;
}