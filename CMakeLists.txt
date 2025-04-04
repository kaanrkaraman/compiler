cmake_minimum_required(VERSION 3.27)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(Compiler ${SOURCE_FILES}
        src/parser.cpp
        include/parser.h
        include/ast.h)