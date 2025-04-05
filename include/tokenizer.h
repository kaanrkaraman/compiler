//
// Created by Kaan Karaman on 05/04/2025.
//

#ifndef COMPILER_TOKENIZER_H
#define COMPILER_TOKENIZER_H

#include <string>
#include <vector>

#include "token.h"
#include "token_type.h"

class Tokenizer {
public:
    explicit Tokenizer(const std::string &source);

    std::vector<Token> tokenize();

private:
    const std::string &source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;

    [[nodiscard]] char peek() const;

    [[nodiscard]] char peekNext() const;

    char advance();

    bool match(char expected);

    void skipWhitespace();

    [[nodiscard]] Token makeToken(TokenType type) const;

    Token string();

    Token number();

    Token identifier();
};

#endif //COMPILER_TOKENIZER_H
