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
    MatrixMultiplication,
    SwitchStatement,
    CaseClause,
    DoWhileStatement,
    TryCatchFinallyStatement,
    CatchClause,
    Unary,
};

struct Expr {
    ExprType type;

    virtual ~Expr() = default;

    [[nodiscard]] virtual std::string toString() const = 0;
};

struct LiteralExpr : Expr {
    std::variant<int, float, std::string, bool, std::nullptr_t> value;

    explicit LiteralExpr(std::variant<int, float, std::string, bool, std::nullptr_t> value)
            : value(std::move(value)) {
       this->type = ExprType::Literal;
    }

    [[nodiscard]] std::string toString() const override {
       return std::visit([](const auto &val) -> std::string {
           using T = std::decay_t<decltype(val)>;
           if constexpr (std::is_same_v<T, std::nullptr_t>) {
              return "Literal(null)";
           } else if constexpr (std::is_same_v<T, std::string>) {
              return "Literal(\"" + val + "\")";
           } else if constexpr (std::is_same_v<T, bool>) {
              return std::string("Literal(") + (val ? "true" : "false") + ")";
           } else {
              return "Literal(" + std::to_string(val) + ")";
           }
       }, value);
    }
};

struct IdentifierExpr : Expr {
    std::string name;

    explicit IdentifierExpr(std::string name) : name(std::move(name)) {
       type = ExprType::Identifier;
    }

    [[nodiscard]] std::string toString() const override {
       return "Identifier(" + name + ")";
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

    [[nodiscard]] std::string toString() const override {
       return "Binary(" + op + ", " + left->toString() + ", " + right->toString() + ")";
    }
};

struct VarDeclarationExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> initializer;

    VarDeclarationExpr(std::string name, std::unique_ptr<Expr> initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {
       type = ExprType::VarDeclaration;
    }

    [[nodiscard]] std::string toString() const override {
       return "VarDeclaration(" + name + (initializer ? ", " + initializer->toString() : "") + ")";
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

    [[nodiscard]] std::string toString() const override {
       std::string result = "FunctionDeclaration(" + name + ", params: [";
       for (size_t i = 0; i < params.size(); ++i) {
          result += params[i];
          if (i < params.size() - 1) result += ", ";
       }
       result += "]";
       result += ", body: " + body->toString() + ")";
       return result;
    }
};

struct FunctionCallExpr : Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> arguments;

    FunctionCallExpr(std::string callee, std::vector<std::unique_ptr<Expr>> arguments)
            : callee(std::move(callee)), arguments(std::move(arguments)) {
       type = ExprType::FunctionCall;
    }

    [[nodiscard]] std::string toString() const override {
       std::string result = "FunctionCall(" + callee + ", args: [";
       for (size_t i = 0; i < arguments.size(); ++i) {
          result += arguments[i]->toString();
          if (i < arguments.size() - 1) result += ", ";
       }
       result += "])";
       return result;
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

    [[nodiscard]] std::string toString() const override {
       return "If(" + condition->toString() +
              ", then: " + thenBranch->toString() +
              (elseBranch ? ", else: " + elseBranch->toString() : "") +
              ")";
    }
};

struct WhileStatementExpr : Expr {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> body;

    WhileStatementExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> body)
            : condition(std::move(condition)), body(std::move(body)) {
       type = ExprType::WhileStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "While(" + condition->toString() + ", body: " + body->toString() + ")";
    }
};

struct ForStatementExpr : Expr {
    std::unique_ptr<Expr> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> increment;
    std::unique_ptr<Expr> body;

    ForStatementExpr(std::unique_ptr<Expr> initializer, std::unique_ptr<Expr> condition,
                     std::unique_ptr<Expr> increment, std::unique_ptr<Expr> body)
            : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)),
              body(std::move(body)) {
       type = ExprType::ForStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "For(init: " + (initializer ? initializer->toString() : "null") +
              ", cond: " + (condition ? condition->toString() : "null") +
              ", incr: " + (increment ? increment->toString() : "null") +
              ", body: " + (body ? body->toString() : "null") + ")";
    }
};

struct ReturnStatementExpr : Expr {
    std::unique_ptr<Expr> value;

    explicit ReturnStatementExpr(std::unique_ptr<Expr> value) : value(std::move(value)) {
       type = ExprType::ReturnStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "Return(" + (value ? value->toString() : "void") + ")";
    }
};

struct BreakStatementExpr : Expr {
    BreakStatementExpr() {
       type = ExprType::BreakStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "Break";
    }
};

struct ContinueStatementExpr : Expr {
    ContinueStatementExpr() {
       type = ExprType::ContinueStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "Continue";
    }
};

struct BlockStatementExpr : Expr {
    std::vector<std::unique_ptr<Expr>> statements;

    explicit BlockStatementExpr(std::vector<std::unique_ptr<Expr>> statements)
            : statements(std::move(statements)) {
       type = ExprType::BlockStatement;
    }

    [[nodiscard]] std::string toString() const override {
       std::string result = "Block(";
       for (size_t i = 0; i < statements.size(); ++i) {
          result += statements[i]->toString();
          if (i < statements.size() - 1) result += ", ";
       }
       result += ")";
       return result;
    }
};

struct ExpressionStatementExpr : Expr {
    std::unique_ptr<Expr> expression;

    explicit ExpressionStatementExpr(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {
       type = ExprType::ExpressionStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "ExprStmt: " + expression->toString();
    }
};

struct AssignmentExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;

    AssignmentExpr(std::string name, std::unique_ptr<Expr> value)
            : name(std::move(name)), value(std::move(value)) {
       type = ExprType::Assignment;
    }

    [[nodiscard]] std::string toString() const override {
       return "Assign: " + name + " = " + value->toString();
    }
};

struct MatrixMultiplicationExpr : Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    MatrixMultiplicationExpr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
            : left(std::move(left)), right(std::move(right)) {
       type = ExprType::MatrixMultiplication;
    }

    [[nodiscard]] std::string toString() const override {
       return "MatrixMultiply(" + left->toString() + ", " + right->toString() + ")";
    }
};

struct SwitchStatementExpr : Expr {
    std::unique_ptr<Expr> switchExpr;
    std::vector<std::unique_ptr<Expr>> caseClauses;
    std::unique_ptr<Expr> defaultClause;

    SwitchStatementExpr(std::unique_ptr<Expr> switchExpr, std::vector<std::unique_ptr<Expr>> caseClauses,
                        std::unique_ptr<Expr> defaultClause)
            : switchExpr(std::move(switchExpr)), caseClauses(std::move(caseClauses)),
              defaultClause(std::move(defaultClause)) {
       type = ExprType::SwitchStatement;
    }

    [[nodiscard]] std::string toString() const override {
       std::string result = "Switch(" + switchExpr->toString() + ") {\n";
       for (const auto &clause: caseClauses) {
          result += "  " + clause->toString() + "\n";
       }
       if (defaultClause) {
          result += "  Default:\n    " + defaultClause->toString() + "\n";
       }
       result += "}";
       return result;
    }
};

struct CaseClauseExpr : Expr {
    std::unique_ptr<Expr> caseExpr;
    std::unique_ptr<Expr> body;

    CaseClauseExpr(std::unique_ptr<Expr> caseExpr, std::unique_ptr<Expr> body)
            : caseExpr(std::move(caseExpr)), body(std::move(body)) {
       type = ExprType::CaseClause;
    }

    [[nodiscard]] std::string toString() const override {
       return "Case " + caseExpr->toString() + ": " + body->toString();
    }
};

struct DoWhileStatementExpr : Expr {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> body;

    DoWhileStatementExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> body)
            : condition(std::move(condition)), body(std::move(body)) {
       type = ExprType::DoWhileStatement;
    }

    [[nodiscard]] std::string toString() const override {
       return "DoWhile(" + body->toString() + ") while (" + condition->toString() + ")";
    }
};

struct TryCatchFinallyStatementExpr : Expr {
    std::unique_ptr<Expr> tryBlock;
    std::vector<std::unique_ptr<Expr>> catches;
    std::unique_ptr<Expr> finallyBlock;

    TryCatchFinallyStatementExpr(std::unique_ptr<Expr> tryBlock,
                                 std::vector<std::unique_ptr<Expr>> catches,
                                 std::unique_ptr<Expr> finallyBlock)
            : tryBlock(std::move(tryBlock)), catches(std::move(catches)), finallyBlock(std::move(finallyBlock)) {
       type = ExprType::TryCatchFinallyStatement;
    }

    [[nodiscard]] std::string toString() const override {
       std::string result = "Try {\n  " + tryBlock->toString() + "\n}";

       for (const auto &c: catches) {
          result += "\n" + c->toString();
       }

       if (finallyBlock) {
          result += "\nFinally {\n  " + finallyBlock->toString() + "\n}";
       }

       return result;
    }
};

struct CatchClauseExpr : Expr {
    std::string exceptionVarName;
    std::unique_ptr<Expr> block;

    CatchClauseExpr(std::string exceptionVarName, std::unique_ptr<Expr> block)
            : exceptionVarName(std::move(exceptionVarName)), block(std::move(block)) {
       type = ExprType::CatchClause;
    }

    [[nodiscard]] std::string toString() const override {
       return "Catch(" + exceptionVarName + ") {\n  " + block->toString() + "\n}";
    }
};

struct UnaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> right;

    UnaryExpr(std::string op, std::unique_ptr<Expr> right)
            : op(std::move(op)), right(std::move(right)) {
       type = ExprType::Unary;
    }

    [[nodiscard]] std::string toString() const override {
       return "Unary: " + op + " " + right->toString();
    }
};

#endif //COMPILER_AST_H
