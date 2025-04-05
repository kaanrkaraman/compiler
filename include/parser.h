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
#include "scope_manager.h"

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens);

    ScopeManager scopeManager;

    std::unique_ptr<Expr> parse();

private:
    const std::vector<Token> &tokens;
    size_t current = 0;

    [[nodiscard]] const Token &peek() const;

    const Token &advance();

    bool match(TokenType type);

    [[nodiscard]] bool check(TokenType type) const;

    [[nodiscard]] bool isAtEnd() const;

    // Helper methods
    [[nodiscard]] static int getPrecedence(TokenType type);

    [[nodiscard]] static int getAssociativity(TokenType type);

    std::unique_ptr<Expr> expression();

    std::unique_ptr<Expr> primary();

    std::unique_ptr<Expr> parsePostfix(std::unique_ptr<Expr> left);

    std::unique_ptr<Expr> parseBinaryExpression(int minPrecedence);

    std::unique_ptr<Expr> parseUnary();

    // Declaration parsing methods
    std::unique_ptr<Expr> declaration();

    std::unique_ptr<Expr> statement();

    std::unique_ptr<Expr> block();

    std::unique_ptr<Expr> statementOrBlock();

    // Statement parsing methods
    std::unique_ptr<Expr> ifStatement();

    std::unique_ptr<Expr> whileStatement();

    std::unique_ptr<Expr> forStatement();

    std::unique_ptr<Expr> functionDeclaration();

    std::unique_ptr<Expr> returnStatement();

    std::unique_ptr<Expr> switchStatement();

    std::unique_ptr<Expr> doWhileStatement();

    std::unique_ptr<Expr> tryStatement();
};

#endif //COMPILER_PARSER_H
