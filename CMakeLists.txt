cmake_minimum_required(VERSION 3.27)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(compiler_lib ${SOURCE_FILES})
add_executable(Compiler ${SOURCE_FILES}
        include/types.h)

enable_testing()
add_subdirectory(tests)