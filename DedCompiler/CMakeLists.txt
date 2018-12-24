cmake_minimum_required(VERSION 3.12)
project(DedCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(DedCompiler compiler.cpp utils.h tokenizer.h expression_evaluation.h)