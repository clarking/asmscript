
cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pedantic -Wall -Wextra")
project(asms CXX)

set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

add_executable(asms
        src/common.cpp
        src/parser.cpp
        src/lexer.cpp
        src/compiler.cpp
        src/runtime.cpp
        src/main.cpp)

target_include_directories(asms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

