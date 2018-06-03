cmake_minimum_required(VERSION 3.7)
project(ex32)

set(CMAKE_C_STANDARD 90)

set(SOURCE_FILES ex32.c)
add_executable(ex32 ${SOURCE_FILES})


set(SOURCE_FILES2 ex31.c)
add_executable(ex31 ${SOURCE_FILES2})