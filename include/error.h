//
// Created by Kaan Karaman on 05/04/2025.
//

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <stdexcept>
#include <string>

class CompilerError : public std::runtime_error {
public:
    explicit CompilerError(const std::string &message, int line = -1, int column = -1)
            : std::runtime_error(buildErrorMessage(message, line, column)) {}

private:
    static std::string buildErrorMessage(const std::string &msg, int line, int column) {
       if (line >= 0 && column >= 0) {
          return "[CompilerError] At line " + std::to_string(line) +
                 ", column " + std::to_string(column) + ": " + msg;
       }
       return "[CompilerError] " + msg;
    }
};

class SyntaxError : public CompilerError {
public:
    explicit SyntaxError(const std::string &message, int line = -1, int column = -1)
            : CompilerError(message, line, column) {}

private:
    static std::string buildErrorMessage(const std::string &msg, int line, int column) {
       if (line >= 0 && column >= 0) {
          return "[SyntaxError] At line " + std::to_string(line) +
                 ", column " + std::to_string(column) + ": " + msg;
       }
       return "[SyntaxError] " + msg;
    }
};

#endif //COMPILER_ERROR_H
