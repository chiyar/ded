#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>

enum Command {
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT,

    JUMP,
    JE,
    JN,
    JL,
    JG,

    PUSH,
    POP,
    MOV_STOA,
    MOV_STOB,
    MOV_STOC,
    MOV_STOD,
    MOV_STOMEM,
    MOV_ATOS,
    MOV_BTOS,
    MOV_CTOS,
    MOV_DTOS,
    MOV_MEMTOS,

    IN,
    OUT,

    CALL,
    RET,
    END,

    LABEL
};

std::unordered_set<Command> no_arg_commands = {
        ADD,
        SUB,
        MUL,
        DIV,
        SQRT,
        POP,
        MOV_STOA,
        MOV_STOB,
        MOV_STOC,
        MOV_STOD,
        MOV_ATOS,
        MOV_BTOS,
        MOV_CTOS,
        MOV_DTOS,
        IN,
        OUT,
        RET,
        END
};

std::unordered_set<Command> one_arg_commands = {
        MOV_STOMEM,
        MOV_MEMTOS,
        JUMP,
        JE,
        JN,
        JL,
        JG,
        PUSH,
        CALL,
        LABEL
};

std::unordered_map<std::string, Command> command_by_name = {
        {"ADD", ADD},
        {"SUB", SUB},
        {"MUL", MUL},
        {"DIV", DIV},
        {"SQRT", SQRT},

        {"JUMP", JUMP},
        {"JE", JE},
        {"JN", JN},
        {"JL", JL},
        {"JG", JG},

        {"PUSH", PUSH},
        {"POP", POP},
        {"MOV_STOA", MOV_STOA},
        {"MOV_STOB", MOV_STOB},
        {"MOV_STOC", MOV_STOC},
        {"MOV_STOD", MOV_STOD},
        {"MOV_STOMEM", MOV_STOMEM},
        {"MOV_ATOS", MOV_ATOS},
        {"MOV_BTOS", MOV_BTOS},
        {"MOV_CTOS", MOV_CTOS},
        {"MOV_DTOS", MOV_DTOS},
        {"MOV_MEMTOS", MOV_MEMTOS},

        {"IN", IN},
        {"OUT", OUT},

        {"CALL", CALL},
        {"RET", RET},
        {"END", END},

        {"LABEL", LABEL}
};

std::unordered_map<Command, std::string> name_by_command {
        {ADD, "ADD"},
        {SUB, "SUB"},
        {MUL, "MUL"},
        {DIV, "DIV"},
        {SQRT, "SQRT"},

        {JUMP, "JUMP"},
        {JE, "JE"},
        {JN, "JN"},
        {JL, "JL"},
        {JG, "JG"},

        {PUSH, "PUSH"},
        {POP, "POP"},
        {MOV_STOA, "MOV_STOA"},
        {MOV_STOB, "MOV_STOB"},
        {MOV_STOC, "MOV_STOC"},
        {MOV_STOD, "MOV_STOD"},
        {MOV_STOMEM, "MOV_STOMEM"},
        {MOV_ATOS, "MOV_ATOS"},
        {MOV_BTOS, "MOV_BTOS"},
        {MOV_CTOS, "MOV_CTOS"},
        {MOV_DTOS, "MOV_DTOS"},
        {MOV_MEMTOS, "MOV_MEMTOS"},

        {IN, "IN"},
        {OUT, "OUT"},

        {CALL, "CALL"},
        {RET, "RET"},
        {END, "END"},

        {LABEL, "LABEL"}
};

std::unordered_set<Command> require_label = {
        JUMP,
        JE,
        JN,
        JL,
        JG,
        CALL
};

bool HasNoArg(Command command) {
    return no_arg_commands.find(command) != no_arg_commands.end();
}

bool HasOneArg(Command command) {
    return one_arg_commands.find(command) != one_arg_commands.end();
}

bool RequiresLabel(Command command) {
    return require_label.find(command) != require_label.end();
}