cmake_minimum_required(VERSION 3.7)
project(Ex4)

set(CMAKE_C_STANDARD 90)

set(SOURCE_FILES1 ex41.c)
add_executable(Ex41 ${SOURCE_FILES1})

set(SOURCE_FILES2 ex42.c)
add_executable(Ex42 ${SOURCE_FILES2})

target_link_libraries(Ex42 -lpthread)