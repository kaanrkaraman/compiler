//
// Created by Kaan Karaman on 04/04/2025.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <vector>
#include <memory>

#include "token.h"
#include "token_type.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    [[nodiscard]] const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
    [[nodiscard]] bool check(TokenType type) const;
    [[nodiscard]] bool isAtEnd() const;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> parsePostfix(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> parseBinaryExpression(int minPrecedence);
    std::unique_ptr<Expr> parseAssignment();

    std::unique_ptr<Expr> declaration();
    std::unique_ptr<Expr> statement();
    std::unique_ptr<Expr> block();
    std::unique_ptr<Expr> ifStatement();
    std::unique_ptr<Expr> whileStatement();
    std::unique_ptr<Expr> forStatement();
    std::unique_ptr<Expr> functionDeclaration();
    std::unique_ptr<Expr> returnStatement();
};

#endif //COMPILER_PARSER_H
