cmake_minimum_required(VERSION 3.25)
project(clox_programming_language C)

set(CMAKE_C_STANDARD 11)

add_executable(clox_programming_language main.c common.h chunk.h chunk.c memory.h memory.c debug.h debug.c value.h value.c)
