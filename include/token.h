//
// Created by Kaan Karaman on 04/04/2025.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <utility>
#include <string>

enum class TokenType : std::uint8_t;

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t, std::string  l, int ln, int col)
            : type(t), lexeme(std::move(l)), line(ln), column(col) {}
};

#endif //COMPILER_TOKEN_H
