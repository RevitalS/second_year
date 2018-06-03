cmake_minimum_required(VERSION 3.6)
project(ex3_files)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    main-test.c
    pstring.h)

add_executable(ex3_files ${SOURCE_FILES})