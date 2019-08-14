cmake_minimum_required(VERSION 3.7)
project(assembler)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c file_processing.c symbol_tree.c opcode.c error_handling.c extern_table.c utils.h keywords.c keywords.h directives.c directives.h machine_code.c machine_code.h)
add_executable(assembler ${SOURCE_FILES})