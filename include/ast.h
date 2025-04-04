//
// Created by Kaan Karaman on 04/04/2025.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <cstdint>
#include <string>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

enum class ExprType : std::uint8_t {
    Literal,
    Identifier,
    Binary,
    VarDeclaration,
    FunctionDeclaration,
    FunctionCall,
    IfStatement,
    WhileStatement,
    ForStatement,
    ReturnStatement,
    BreakStatement,
    ContinueStatement,
    BlockStatement,
    ExpressionStatement,
    Assignment,
    MatrixAssignment,
    MatrixMultiplication
};

struct Expr {
    ExprType type;
    virtual ~Expr() = default;
};

struct LiteralExpr : Expr {
    std::variant<int, float, std::string, bool, std::nullptr_t> value;

    explicit LiteralExpr(std::variant<int, float, std::string, bool, std::nullptr_t> value)
        : value(std::move(value)) {
        type = ExprType::Literal;
    }
};

struct IdentifierExpr : Expr {
    std::string name;

    explicit IdentifierExpr(std::string name) : name(std::move(name)) {
        type = ExprType::Identifier;
    }
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    std::string op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, std::string op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {
        type = ExprType::Binary;
    }
};

struct VarDeclarationExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> initializer;

    VarDeclarationExpr(std::string name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {
        type = ExprType::VarDeclaration;
    }
};

struct FunctionDeclarationExpr : Expr {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<Expr> body;

    FunctionDeclarationExpr(std::string name, std::vector<std::string> params, std::unique_ptr<Expr> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {
        type = ExprType::FunctionDeclaration;
    }
};

struct FunctionCallExpr : Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> arguments;

    FunctionCallExpr(std::string callee, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {
        type = ExprType::FunctionCall;
    }
};

struct IfStatementExpr : Expr {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> thenBranch;
    std::unique_ptr<Expr> elseBranch;

    IfStatementExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> thenBranch, std::unique_ptr<Expr> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {
        type = ExprType::IfStatement;
    }
};

struct WhileStatementExpr : Expr {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> body;

    WhileStatementExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> body)
        : condition(std::move(condition)), body(std::move(body)) {
        type = ExprType::WhileStatement;
    }
};

struct ForStatementExpr : Expr {
    std::unique_ptr<Expr> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> increment;
    std::unique_ptr<Expr> body;

    ForStatementExpr(std::unique_ptr<Expr> initializer, std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment, std::unique_ptr<Expr> body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {
        type = ExprType::ForStatement;
    }
};

struct ReturnStatementExpr : Expr {
    std::unique_ptr<Expr> value;

    explicit ReturnStatementExpr(std::unique_ptr<Expr> value) : value(std::move(value)) {
        type = ExprType::ReturnStatement;
    }
};

struct BreakStatementExpr : Expr {
    BreakStatementExpr() {
        type = ExprType::BreakStatement;
    }
};

struct ContinueStatementExpr : Expr {
    ContinueStatementExpr() {
        type = ExprType::ContinueStatement;
    }
};

struct BlockStatementExpr : Expr {
    std::vector<std::unique_ptr<Expr>> statements;

    explicit BlockStatementExpr(std::vector<std::unique_ptr<Expr>> statements)
        : statements(std::move(statements)) {
        type = ExprType::BlockStatement;
    }
};

struct ExpressionStatementExpr : Expr {
    std::unique_ptr<Expr> expression;

    explicit ExpressionStatementExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {
        type = ExprType::ExpressionStatement;
    }
};

struct AssignmentExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;

    AssignmentExpr(std::string name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {
        type = ExprType::Assignment;
    }
};

struct MatrixAssignmentExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;

    MatrixAssignmentExpr(std::string name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {
        type = ExprType::MatrixAssignment;
    }
};

struct MatrixMultiplicationExpr : Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    MatrixMultiplicationExpr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : left(std::move(left)), right(std::move(right)) {
        type = ExprType::MatrixMultiplication;
    }
};

#endif //COMPILER_AST_H
