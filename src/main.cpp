#include <iostream>

#include "lexer.h"
#include "token_type.h"
#include "parser.h"

void printExpr(const Expr* expr, int indent = 0);

void printIndent(int level) {
   for (int i = 0; i < level; ++i) std::cout << "  ";
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void printExpr(const Expr* expr, int indent) {
   if (!expr) return;

   switch (expr->type) {
      case ExprType::Literal: {
         auto* lit = dynamic_cast<const LiteralExpr*>(expr);
         printIndent(indent);
         std::visit([&](auto&& val) {
             std::cout << "Literal: " << val << "\n";
         }, lit->value);
         break;
      }
      case ExprType::Identifier: {
         auto* id = dynamic_cast<const IdentifierExpr*>(expr);
         printIndent(indent);
         std::cout << "Identifier: " << id->name << "\n";
         break;
      }
      case ExprType::Binary: {
         auto* bin = dynamic_cast<const BinaryExpr*>(expr);
         printIndent(indent);
         std::cout << "Binary: " << bin->op << "\n";
         printExpr(bin->left.get(), indent + 1);
         printExpr(bin->right.get(), indent + 1);
         break;
      }
      case ExprType::VarDeclaration: {
         auto* var = dynamic_cast<const VarDeclarationExpr*>(expr);
         printIndent(indent);
         std::cout << "VarDeclaration: " << var->name << "\n";
         printExpr(var->initializer.get(), indent + 1);
         break;
      }
      case ExprType::ExpressionStatement: {
         auto* stmt = dynamic_cast<const ExpressionStatementExpr*>(expr);
         printIndent(indent);
         std::cout << "ExprStmt:\n";
         printExpr(stmt->expression.get(), indent + 1);
         break;
      }
      case ExprType::BlockStatement: {
         auto* block = dynamic_cast<const BlockStatementExpr*>(expr);
         printIndent(indent);
         std::cout << "Block:\n";
         for (const auto& stmt : block->statements) {
            printExpr(stmt.get(), indent + 1);
         }
         break;
      }
      case ExprType::IfStatement: {
         auto* ifstmt = dynamic_cast<const IfStatementExpr*>(expr);
         printIndent(indent); std::cout << "If:\n";
         printIndent(indent + 1); std::cout << "Condition:\n";
         printExpr(ifstmt->condition.get(), indent + 2);
         printIndent(indent + 1); std::cout << "Then:\n";
         printExpr(ifstmt->thenBranch.get(), indent + 2);
         if (ifstmt->elseBranch) {
            printIndent(indent + 1); std::cout << "Else:\n";
            printExpr(ifstmt->elseBranch.get(), indent + 2);
         }
         break;
      }
      case ExprType::WhileStatement: {
         auto* loop = dynamic_cast<const WhileStatementExpr*>(expr);
         printIndent(indent); std::cout << "While:\n";
         printIndent(indent + 1); std::cout << "Condition:\n";
         printExpr(loop->condition.get(), indent + 2);
         printIndent(indent + 1); std::cout << "Body:\n";
         printExpr(loop->body.get(), indent + 2);
         break;
      }
      case ExprType::FunctionDeclaration: {
         auto* fn = dynamic_cast<const FunctionDeclarationExpr*>(expr);
         printIndent(indent);
         std::cout << "Function: " << fn->name << " (";
         for (size_t i = 0; i < fn->params.size(); ++i) {
            std::cout << fn->params[i];
            if (i + 1 < fn->params.size()) std::cout << ", ";
         }
         std::cout << ")\n";
         printExpr(fn->body.get(), indent + 1);
         break;
      }
      case ExprType::FunctionCall: {
         auto* call = dynamic_cast<const FunctionCallExpr*>(expr);
         printIndent(indent);
         std::cout << "Call: " << call->callee << "\n";
         for (const auto& arg : call->arguments)
            printExpr(arg.get(), indent + 1);
         break;
      }
      case ExprType::ReturnStatement: {
         auto* ret = dynamic_cast<const ReturnStatementExpr*>(expr);
         printIndent(indent); std::cout << "Return:\n";
         printExpr(ret->value.get(), indent + 1);
         break;
      }
      case ExprType::BreakStatement: {
         printIndent(indent); std::cout << "Break\n";
         break;
      }
      case ExprType::ContinueStatement: {
         printIndent(indent); std::cout << "Continue\n";
         break;
      }
      case ExprType::MatrixMultiplication: {
         auto* mm = dynamic_cast<const MatrixMultiplicationExpr*>(expr);
         printIndent(indent); std::cout << "MatrixMultiply:\n";
         printExpr(mm->left.get(), indent + 1);
         printExpr(mm->right.get(), indent + 1);
         break;
      }
      case ExprType::MatrixAssignment: {
         auto* ma = dynamic_cast<const MatrixAssignmentExpr*>(expr);
         printIndent(indent); std::cout << "MatrixAssign: " << ma->name << "\n";
         printExpr(ma->value.get(), indent + 1);
         break;
      }
      case ExprType::Assignment: {
         auto* assign = dynamic_cast<const AssignmentExpr*>(expr);
         printIndent(indent); std::cout << "Assign: " << assign->name << "\n";
         printExpr(assign->value.get(), indent + 1);
         break;
      }
      default:
         printIndent(indent); std::cout << "Unknown node type\n";
         break;
   }
}
#pragma clang diagnostic pop

[[maybe_unused]] std::string tokenTypeToString(TokenType type) {
   switch (type) {
      case TokenType::IF: return "IF";
      case TokenType::ELSE: return "ELSE";
      case TokenType::IDENTIFIER: return "IDENTIFIER";
      case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
      case TokenType::STRING_LITERAL: return "STRING_LITERAL";
      case TokenType::PLUS: return "PLUS";
      case TokenType::EQUAL: return "EQUAL";
      case TokenType::LEFT_PAREN: return "LEFT_PAREN";
      case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
      case TokenType::END_OF_FILE: return "EOF";
      case TokenType::VAR: return "VAR";
      case TokenType::ASSIGN: return "ASSIGN";
      case TokenType::SEMICOLON: return "SEMICOLON";
      case TokenType::GREATER_THAN: return "GREATER_THAN";
      case TokenType::LEFT_BRACE: return "LEFT_BRACE";
      case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
      case TokenType::FOR: return "FOR";
      case TokenType::LESS_THAN: return "LESS_THAN";
      default: return "UNKNOWN";
   }
}

int main() {
   std::string sourceCode = R"(
i = 0;
for (i = 0; i < 10; i = i + 1) {
    print(i);
}
    )";

   Tokenizer tokenizer(sourceCode);
   std::vector<Token> tokens = tokenizer.tokenize();

   for (const auto& token : tokens) {
      std::cout << "Token: "
                << tokenTypeToString(token.type)
                << " | Lexeme: '" << token.lexeme << "'"
                << " | Line: " << token.line
                << " | Column: " << token.column
                << "\n";
   }

   Parser parser(tokens);
   std::unique_ptr<Expr> ast = parser.parse();

   if (ast) {
      std::cout << "=== AST ===\n";
      printExpr(ast.get());
   } else {
      std::cerr << "Parsing failed.\n";
   }

   return 0;
}