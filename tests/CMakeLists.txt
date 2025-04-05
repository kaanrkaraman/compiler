add_executable(CompilerTests
        test_main.cpp
        parser_test.cpp
)

target_link_libraries(CompilerTests
        compiler_lib
        gtest
        gtest_main
)

target_include_directories(CompilerTests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

if (APPLE)
    message(STATUS "Configuring for macOS")
    target_include_directories(CompilerTests PRIVATE /opt/homebrew/include)
    target_link_directories(CompilerTests PRIVATE /opt/homebrew/lib)
elseif (UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux")
elseif (WIN32)
    message(STATUS "Configuring for Windows")
    if (DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
        message(STATUS "Using VCPKG toolchain at $ENV{VCPKG_ROOT}")
    endif()
endif()

add_test(NAME CompilerTests COMMAND CompilerTests)