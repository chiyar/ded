#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "utils.h"
#include "expression_evaluation.h"

struct Scope {
    size_t cur_label = 0;
    size_t cur_memory_index = 0;
    std::unordered_map<std::string, size_t> var_index_in_memory;
};

bool IsDefined(const std::string& var, Scope* scope) {
    return (scope->var_index_in_memory.find(var) != scope->var_index_in_memory.end());
}

struct BoolExpression {
    std::string lhs;
    std::string rhs;
    std::string sign;
};

BoolExpression ParseBoolExpression(const std::string& line) {
    BoolExpression expr;
    size_t i = 0;
    while (i < line.size() && line[i] != '=' && line[i] != '<' && line[i] != '>'
        && line[i] != '!') {
        expr.lhs += line[i];
        ++i;
    }
    if (i + 1 >= line.size()) {
        std::cout << "Invalid bool expression: " << line << "\n";
        exit(0);
    }
    if (line[i] == '=') {
        expr.sign = "JE";
    } else if (line[i] == '<') {
        expr.sign = "JL";
    } else if (line[i] == '>') {
        expr.sign = "JG";
    } else if (line[i] == '!' && line[i + 1] == '=') {
        expr.sign = "JN";
        if (i + 2 == line.size()) {
            std::cout << "Invalid bool expression: " << line << "\n";
            exit(0);
        }
        ++i;
    }
    ++i;
    expr.rhs = line.substr(i);
    return expr;
}


void Compile(const std::string& buffer, size_t& cur_pos, Scope* scope, std::ostream* out);

void CompileExpression(const std::string& str, Scope* scope, std::ostream* out) {
    std::stringstream in{str};
    Tokenizer tokenizer(&in);
    auto expr = StringToExpression(&tokenizer);

    //SimplifyExpression(expr);

    ExpressionToAsm(expr, scope->var_index_in_memory, out);
}

void CompileDef(const std::string& str, Scope* scope, std::ostream* out) {
    auto parts = ParseLine(str);
    if (parts.size() != 2) {
        std::cout << "Invalid definition: " << str << "\n";
        exit(0);
    }

    if (IsDefined(parts[1], scope)) {
        std::cout << "Double definition: " << str << "\n";
        exit(0);
    }

    scope->var_index_in_memory[parts[1]] = scope->cur_memory_index;
    ++scope->cur_memory_index;
}

void CompileAssign(const std::string& str, Scope* scope, std::ostream* out) {
    auto parts = ParseLine(str);
    if (parts.size() != 3) {
        std::cout << "Invalid assignment: " << str << "\n";
        exit(0);
    }

    if (!IsDefined(parts[1], scope)) {
        std::cout << "Undefined variable: " << str << "\n";
        exit(0);
    }

    CompileExpression(parts[2], scope, out);
    *out << "MOV_STOMEM " << scope->var_index_in_memory[parts[1]] << "\n";
}

void CompileScan(const std::string& str, Scope* scope, std::ostream* out) {
    auto parts = ParseLine(str);
    if (parts.size() != 2) {
        std::cout << "Invalid scan statement: " << str << "\n";
        exit(0);
    }

    if (!IsDefined(parts[1], scope)) {
        std::cout << "Undefined variable: " << str << "\n";
        exit(0);
    }

    *out << "IN\n";
    *out << "MOV_STOMEM " << scope->var_index_in_memory[parts[1]] << "\n";
}

void CompilePrint(const std::string& str, Scope* scope, std::ostream* out) {
    auto parts = ParseLine(str);
    if (parts.size() != 2) {
        std::cout << "Invalid print statement: " << str << "\n";
        exit(0);
    }

    CompileExpression(parts[1], scope, out);
    *out << "OUT\n";
}

void CompileIf(const std::string& buffer, size_t& cur_pos, Scope* scope, std::ostream* out) {
    auto line = ExtractLine(buffer, cur_pos);
    auto parts = ParseLine(line);
    if (parts.size() != 2) {
        std::cout << "Invalid if clause: " << line << "\n";
        exit(0);
    }

    auto open_brace_line = ExtractLine(buffer, cur_pos);
    auto open_brace_parts = ParseLine(open_brace_line);
    if (open_brace_parts.size() != 1 || open_brace_parts[0] != "{") {
        std::cout << "Expected '{' after if statement: " << open_brace_line << "\n";
        exit(0);
    }

    auto bool_expr = ParseBoolExpression(parts[1]);
    CompileExpression(bool_expr.lhs, scope, out);
    CompileExpression(bool_expr.rhs, scope, out);
    size_t cur_label = scope->cur_label;
    *out << bool_expr.sign << " " << cur_label << "\n";
    *out << "JUMP " << cur_label + 1 << "\n";
    *out << "LABEL " << cur_label << "\n";
    scope->cur_label += 2;
    Compile(buffer, cur_pos, scope, out);
    *out << "LABEL " << cur_label + 1 << "\n";

    auto close_brace_line = ExtractLine(buffer, cur_pos);
    auto close_brace_parts = ParseLine(close_brace_line);
    if (close_brace_parts.size() != 1 || close_brace_parts[0] != "}") {
        std::cout << "Expected '}' after if body: " << close_brace_line << "\n";
        exit(0);
    }
}

void CompileWhile(const std::string& buffer, size_t& cur_pos, Scope* scope, std::ostream* out) {
    auto line = ExtractLine(buffer, cur_pos);
    auto parts = ParseLine(line);
    if (parts.size() != 2) {
        std::cout << "Invalid while loop: " << line << "\n";
        exit(0);
    }

    auto open_brace_line = ExtractLine(buffer, cur_pos);
    auto open_brace_parts = ParseLine(open_brace_line);
    if (open_brace_parts.size() != 1 || open_brace_parts[0] != "{") {
        std::cout << "Expected '{' after while statement: " << open_brace_line << "\n";
        exit(0);
    }

    auto bool_expr = ParseBoolExpression(parts[1]);

    size_t cur_label = scope->cur_label;
    scope->cur_label += 3;
    *out << "LABEL " << cur_label << "\n";
    CompileExpression(bool_expr.lhs, scope, out);
    CompileExpression(bool_expr.rhs, scope, out);

    *out << bool_expr.sign << " " << cur_label + 1 << "\n";
    *out << "JUMP " << cur_label + 2 << "\n";
    *out << "LABEL " << cur_label + 1 << "\n";
    Compile(buffer, cur_pos, scope, out);
    *out << "JUMP " << cur_label << "\n";
    *out << "LABEL " << cur_label + 2 << "\n";

    auto close_brace_line = ExtractLine(buffer, cur_pos);
    auto close_brace_parts = ParseLine(close_brace_line);
    if (close_brace_parts.size() != 1 || close_brace_parts[0] != "}") {
        std::cout << "Expected '}' after while body: " << close_brace_line << "\n";
        exit(0);
    }
}

void Compile(const std::string& buffer, size_t& cur_pos, Scope* scope, std::ostream* out) {
    while (cur_pos < buffer.size()) {
        std::string first_part = PeekAtFirstPart(buffer, cur_pos);
        if (first_part.empty()) {
            continue;
        } else if (first_part == "}") {
            return;
        }

        if (first_part == "def") {
            auto line = ExtractLine(buffer, cur_pos);
            CompileDef(line, scope, out);
        } else if (first_part == "assign") {
            auto line = ExtractLine(buffer, cur_pos);
            CompileAssign(line, scope, out);
        } else if (first_part == "scan") {
            auto line = ExtractLine(buffer, cur_pos);
            CompileScan(line, scope, out);
        } else if (first_part == "print") {
            auto line = ExtractLine(buffer, cur_pos);
            CompilePrint(line, scope, out);
        } else if (first_part == "if") {
            CompileIf(buffer, cur_pos, scope, out);
        } else if (first_part == "while") {
            CompileWhile(buffer, cur_pos, scope, out);
        } else {
            std::cout << "Unknown instruction: " << first_part << "\n";
            exit(0);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid count of arguments.\nEnter name of input file\n";
        return 0;
    }

    std::string input_name(argv[1]);
    std::string output_name("a.asm");

    std::string buffer;
    if (ReadFile(input_name, buffer) == -1) {
        std::cout << "Invalid filename\n";
        return 0;
    }

    size_t cur_pos = 0;
    Scope scope;
    std::stringstream out;
    Compile(buffer, cur_pos, &scope, &out);

    FILE* output = std::freopen(output_name.data(), "w", stdout);
    std::cout << out.rdbuf();
    fclose(output);
    return 0;
}