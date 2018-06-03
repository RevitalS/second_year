cmake_minimum_required(VERSION 3.7)
project(untitled)

set(CMAKE_C_STANDARD 90)

set(SOURCE_FILES1 ex32.c)
add_executable(untitled1 ${SOURCE_FILES1})

set(SOURCE_FILES2 ex31.c)
add_executable(untitled2 ${SOURCE_FILES2})