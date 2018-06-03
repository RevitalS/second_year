cmake_minimum_required(VERSION 3.6)
project(ex1_c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES ex1.h ex1.c main.c)
add_executable(ex1_c ${SOURCE_FILES})