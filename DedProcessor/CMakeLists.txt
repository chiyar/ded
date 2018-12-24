cmake_minimum_required(VERSION 3.12)
project(DedProcessor)

set(CMAKE_CXX_STANDARD 17)


add_executable(assembler assembler.cpp commands.h)

add_executable(disassembler disassembler.cpp commands.h)

add_executable(processor processor.cpp)