cmake_minimum_required(VERSION 3.7)
project(simplesh)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c defs.h)
add_executable(simplesh ${SOURCE_FILES} defs.h)