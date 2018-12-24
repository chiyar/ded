#include <math.h>
#include <iostream>
#include <string>
#include <vector>

#include "commands.h"
#include "stack.h"
#include "utils.h"

struct ProcessorState {
    size_t instruction_pointer = 0;

    Stack<double> stack;
    Stack<size_t> instruction_stack;

    double ra = 0;
    double rb = 0;
    double rc = 0;
    double rd = 0;

    double memory[4096]{};
};

template <class T>
T ExtractOneElement(Stack<T>* stack) {
    T element = stack->Top();
    stack->Pop();
    return element;
}

std::pair<double, double> ExtractTwoElements(Stack<double>* stack) {
    double first = stack->Top();
    stack->Pop();
    double second = stack->Top();
    stack->Pop();
    return {second, first};
}

void ExecuteAdd(ProcessorState* state) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    state->stack.Push(lhs + rhs);
}

void ExecuteSub(ProcessorState* state) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    state->stack.Push(lhs - rhs);
}

void ExecuteMul(ProcessorState* state) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    state->stack.Push(lhs * rhs);
}

void ExecuteDiv(ProcessorState* state) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    state->stack.Push(lhs / rhs);
}

void ExecuteSqrt(ProcessorState* state) {
    auto number = ExtractOneElement(&state->stack);
    state->stack.Push(std::sqrt(number));
}

void ExecuteJump(ProcessorState* state, size_t arg) {
    state->instruction_pointer = arg;
}

void ExecuteJE(ProcessorState* state, size_t arg) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    if (lhs == rhs) {
        state->instruction_pointer = arg;
    }
}

void ExecuteJN(ProcessorState* state, size_t arg) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    if (lhs != rhs) {
        state->instruction_pointer = arg;
    }
}

void ExecuteJL(ProcessorState* state, size_t arg) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    if (lhs < rhs) {
        state->instruction_pointer = arg;
    }
}

void ExecuteJG(ProcessorState* state, size_t arg) {
    auto [lhs, rhs] = ExtractTwoElements(&state->stack);
    if (lhs > rhs) {
        state->instruction_pointer = arg;
    }
}

void ExecutePush(ProcessorState* state, double arg) {
    state->stack.Push(arg);
}

void ExecutePop(ProcessorState* state) {
    state->stack.Pop();
}

void ExecuteMovSTOA(ProcessorState* state) {
    double number = ExtractOneElement(&state->stack);
    state->ra = number;
}

void ExecuteMovSTOB(ProcessorState* state) {
    double number = ExtractOneElement(&state->stack);
    state->rb = number;
}

void ExecuteMovSTOC(ProcessorState* state) {
    double number = ExtractOneElement(&state->stack);
    state->rc= number;
}

void ExecuteMovSTOD(ProcessorState* state) {
    double number = ExtractOneElement(&state->stack);
    state->rd = number;
}

void ExecuteMovSTOMEM(ProcessorState* state, size_t arg) {
    double number = ExtractOneElement(&state->stack);
    state->memory[arg] = number;
}

void ExecuteMovATOS(ProcessorState* state) {
    state->stack.Push(state->ra);
}

void ExecuteMovBTOS(ProcessorState* state) {
    state->stack.Push(state->rb);
}

void ExecuteMovCTOS(ProcessorState* state) {
    state->stack.Push(state->rc);
}

void ExecuteMovDTOS(ProcessorState* state) {
    state->stack.Push(state->rd);
}

void ExecuteMovMEMTOS(ProcessorState* state, size_t arg) {
    state->stack.Push(state->memory[arg]);
}

void ExecuteIn(ProcessorState* state) {
    double number = 0;
    std::cin >> number;
    state->stack.Push(number);
}

void ExecuteOut(ProcessorState* state) {
    std::cout << state->stack.Top() << "\n";
}

void ExecuteCall(ProcessorState* state, size_t arg) {
    state->instruction_stack.Push(state->instruction_pointer + 1);
    state->instruction_pointer = arg;
}

void ExecuteRet(ProcessorState* state) {
    state->instruction_pointer = ExtractOneElement(&state->instruction_stack);
}

void ExecuteEnd() {
    exit(0);
}

void ExecuteCommand(Command command, const std::vector<double>& args, ProcessorState* state) {
    switch (command) {
        case ADD:
            ExecuteAdd(state);
            break;
        case SUB:
            ExecuteSub(state);
            break;
        case MUL:
            ExecuteMul(state);
            break;
        case DIV:
            ExecuteDiv(state);
            break;
        case SQRT:
            ExecuteSqrt(state);
            break;
        case JUMP:
            ExecuteJump(state, static_cast<size_t>(args[0]));
            break;
        case JE:
            ExecuteJE(state, static_cast<size_t>(args[0]));
            break;
        case JN:
            ExecuteJN(state, static_cast<size_t>(args[0]));
            break;
        case JL:
            ExecuteJL(state, static_cast<size_t>(args[0]));
            break;
        case JG:
            ExecuteJG(state, static_cast<size_t>(args[0]));
            break;
        case PUSH:
            ExecutePush(state, args[0]);
            break;
        case POP:
            ExecutePop(state);
            break;
        case MOV_STOA:
            ExecuteMovSTOA(state);
            break;
        case MOV_STOB:
            ExecuteMovSTOB(state);
            break;
        case MOV_STOC:
            ExecuteMovSTOC(state);
            break;
        case MOV_STOD:
            ExecuteMovSTOD(state);
            break;
        case MOV_STOMEM:
            ExecuteMovSTOMEM(state, static_cast<size_t>(args[0]));
            break;
        case MOV_ATOS:
            ExecuteMovATOS(state);
            break;
        case MOV_BTOS:
            ExecuteMovBTOS(state);
            break;
        case MOV_CTOS:
            ExecuteMovCTOS(state);
            break;
        case MOV_DTOS:
            ExecuteMovDTOS(state);
            break;
        case MOV_MEMTOS:
            ExecuteMovMEMTOS(state, static_cast<size_t>(args[0]));
            break;
        case IN:
            ExecuteIn(state);
            break;
        case OUT:
            ExecuteOut(state);
            break;
        case CALL:
            ExecuteCall(state, static_cast<size_t>(args[0]));
            break;
        case RET:
            ExecuteRet(state);
            break;
        case END:
            ExecuteEnd();
            break;
    }
}



int main(int argc, char* argv[]) {
    ProcessorState state;

    if (argc != 2) {
        std::cout << "Invalid count of arguments.\n Enter name of input file\n";
        return 0;
    }

    std::string input_name(argv[1]);

    std::vector<double> buffer;
    if (ReadFile(input_name, buffer) == -1) {
        std::cout << "Invalid filename\n";
        return 0;
    }

    while(state.instruction_pointer < buffer.size()) {
        Command command = static_cast<Command>(buffer[state.instruction_pointer]);
        ++state.instruction_pointer;

        std::vector<double> args;
        if (HasOneArg(command)) {
            args.emplace_back(buffer[state.instruction_pointer]);
            ++state.instruction_pointer;
        }

        ExecuteCommand(command, args, &state);
    }

    return 0;
}