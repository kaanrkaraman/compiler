//
// Created by Kaan Karaman on 04/04/2025.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include <iostream>

#include "parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

bool Parser::isAtEnd() const {
   return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
   return tokens[current];
}

const Token& Parser::advance() {
   if (!isAtEnd()) current++;
   return tokens[current - 1];
}

bool Parser::check(TokenType type) const {
   return !isAtEnd() && peek().type == type;
}

bool Parser::match(TokenType type) {
   if (check(type)) {
      advance();
      return true;
   }
   return false;
}

std::unique_ptr<Expr> Parser::parse() {
   std::vector<std::unique_ptr<Expr>> statements;

   while (!isAtEnd()) {
      if (check(TokenType::END_OF_FILE)) break;

      auto decl = declaration();
      if (decl) {
         statements.push_back(std::move(decl));
      } else {
         std::cerr << "[ERROR] Failed to parse declaration at token: "
                   << peek().lexeme << " (type " << static_cast<int>(peek().type) << ")\n";

         if (isAtEnd()) break;
         advance();
      }
   }

   return std::make_unique<BlockStatementExpr>(std::move(statements));
}

std::unique_ptr<Expr> Parser::parseAssignment() {
   auto expr = parseBinaryExpression(0);
   if (!expr) {
      std::cerr << "[ERROR] Failed to parse LHS of assignment\n";
   }

   if (match(TokenType::ASSIGN)) {
      auto value = parseAssignment();
      if (auto *identifier = dynamic_cast<IdentifierExpr*>(expr.get())) {
         std::string name = identifier->name;
         return std::make_unique<AssignmentExpr>(name, std::move(value));
      }

      std::cerr << "[ERROR] Invalid assignment target\n";
      return nullptr;
   }

   return expr;
}

std::unique_ptr<Expr> Parser::expression() {
   return parseAssignment();
}

int getPrecedence(TokenType type) {
   switch (type) {
      case TokenType::OR: return 1;
      case TokenType::AND: return 2;
      case TokenType::EQUAL:
      case TokenType::NOT_EQUAL:
      case TokenType::GREATER_THAN:
      case TokenType::GREATER_THAN_EQUAL:
      case TokenType::LESS_THAN:
      case TokenType::LESS_THAN_EQUAL:
         return 3;
      case TokenType::PLUS:
      case TokenType::MINUS:
         return 4;
      case TokenType::MULTIPLY:
      case TokenType::DIVIDE:
      case TokenType::MATRIX_MULTIPLY:
         return 5;
      default: return -1;
   }
}

std::unique_ptr<Expr> Parser::parsePostfix(std::unique_ptr<Expr> expr) {
   while (true) {
      if (match(TokenType::LEFT_PAREN)) {

         std::vector<std::unique_ptr<Expr>> arguments;

         if (!check(TokenType::RIGHT_PAREN)) {
            do {
               auto arg = expression();

               if (!arg) return nullptr;

               arguments.push_back(std::move(arg));
            } while (match(TokenType::COMMA));
         }

         if (!match(TokenType::RIGHT_PAREN)) {
            std::cerr << "[ERROR] Expected ')' after function arguments\n";
            return nullptr;
         }

         if (auto* id = dynamic_cast<IdentifierExpr*>(expr.get())) {
            expr = std::make_unique<FunctionCallExpr>(
                    id->name, std::move(arguments)
            );
         } else {
            std::cerr << "[ERROR] Can only call functions by name\n";
            return nullptr;
         }
      } else {
         break;
      }
   }

   return expr;
}

std::unique_ptr<Expr> Parser::parseBinaryExpression(int minPrecedence) {
   std::unique_ptr<Expr> left = parsePostfix(primary());

   if (!left) {
      std::cerr << "[ERROR] Failed to parse left-hand side of binary expression\n";
      return nullptr;
   }

   while (true) {
      TokenType opType = peek().type;
      int precedence = getPrecedence(opType);

      if (precedence == -1 || precedence < minPrecedence) break;

      Token opToken = advance();
      std::string opLexeme = opToken.lexeme;

      std::unique_ptr<Expr> right = parseBinaryExpression(precedence + 1);

      if (!right) {
         std::cerr << "[ERROR] Failed to parse right-hand side of binary expression\n";
         return nullptr;
      }

      if (opType == TokenType::MATRIX_MULTIPLY) {
         left = std::make_unique<MatrixMultiplicationExpr>(
                 std::move(left), std::move(right));
      } else {
         left = std::make_unique<BinaryExpr>(
                 std::move(left), opLexeme, std::move(right));
      }
   }

   return left;
}

std::unique_ptr<Expr> Parser::primary() {
   if (isAtEnd()) {
      std::cerr << "[ERROR] primary() called at EOF\n";
      return nullptr;
   }

   if (match(TokenType::LEFT_PAREN)) {
      auto expr = expression();
      if (!match(TokenType::RIGHT_PAREN)) return nullptr; // TODO: error handling
      return expr;
   }

   if (match(TokenType::INTEGER_LITERAL)) {
      const Token& token = tokens[current - 1];

      try {
         int value = std::stoi(token.lexeme);
         return std::make_unique<LiteralExpr>(value);
      } catch (const std::invalid_argument& e) {
         std::cerr << "[ERROR] Invalid integer literal: '" << token.lexeme << "'\n";
         return nullptr;
      }
   }

   if (match(TokenType::FLOAT_LITERAL)) {
      const Token& token = tokens[current - 1];
      return std::make_unique<LiteralExpr>(std::stof(token.lexeme));
   }

   if (match(TokenType::STRING_LITERAL)) {
      const Token& token = tokens[current - 1];
      return std::make_unique<LiteralExpr>(token.lexeme);
   }

   if (match(TokenType::BOOLEAN_LITERAL)) {
      const Token& token = tokens[current - 1];
      bool value = (token.lexeme == "true");
      return std::make_unique<LiteralExpr>(value);
   }

   if (match(TokenType::NULL_LITERAL)) {
      const Token& token = tokens[current - 1];
      return std::make_unique<LiteralExpr>(nullptr);
   }

   if (match(TokenType::IDENTIFIER)) {
      const Token& token = tokens[current - 1];
      std::cout << "[DEBUG] Parsed identifier: " << token.lexeme << "\n";
      return std::make_unique<IdentifierExpr>(token.lexeme);
   }

   std::cerr << "[ERROR] primary() failed to match any expression starting at token: "
             << peek().lexeme << " (type " << static_cast<int>(peek().type) << ")\n";
   return nullptr;
}

std::unique_ptr<Expr> Parser::declaration() {
   if (check(TokenType::END_OF_FILE)) {
      std::cerr << "[DEBUG] EOF reached in declaration()\n";
      return nullptr;
   }

   if (match(TokenType::VAR)) {
      if (!match(TokenType::IDENTIFIER)) {
         std::cerr << "[ERROR] Expected variable name after 'var'\n";
         return nullptr;
      }
      std::string name = tokens[current - 1].lexeme;

      std::unique_ptr<Expr> initializer = nullptr;
      if (match(TokenType::ASSIGN)) {
         initializer = expression();
      }

      if (!match(TokenType::SEMICOLON)) {
         std::cerr << "[ERROR] Expected ';' after variable declaration\n";
         return nullptr;
      }

      return std::make_unique<VarDeclarationExpr>(name, std::move(initializer));
   }

   if (match(TokenType::FUNCTION)) {
      return functionDeclaration();
   }

   return statement();
}

std::unique_ptr<Expr> Parser::statement() {
   if (check(TokenType::END_OF_FILE)) {
      std::cerr << "[DEBUG] Reached end of file, skipping statement\n";
      return nullptr;
   }
   if (match(TokenType::IF)) return ifStatement();
   if (match(TokenType::WHILE)) return whileStatement();
   if (match(TokenType::FOR)) return forStatement();
   if (match(TokenType::LEFT_BRACE)) return block();
   if (match(TokenType::RETURN)) return returnStatement();

   std::unique_ptr<Expr> expr = expression();
   if (!match(TokenType::SEMICOLON)) {
      std::cerr << "[ERROR] Expected ';' after expression statement\n";
      std::exit(1);
      return nullptr;
   }
   return std::make_unique<ExpressionStatementExpr>(std::move(expr));
}

std::unique_ptr<Expr> Parser::block() {
   std::vector<std::unique_ptr<Expr>> statements;

   while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      statements.push_back(declaration());
   }

   if (!match(TokenType::RIGHT_BRACE)) return nullptr; // TODO: error handling

   return std::make_unique<BlockStatementExpr>(std::move(statements));
}

std::unique_ptr<Expr> Parser::ifStatement() {
   if (!match(TokenType::LEFT_PAREN)) {
      std::cerr << "[ERROR] Expected '(' after 'if'\n";
      return nullptr;
   }

   auto condition = expression();

   if (!match(TokenType::RIGHT_PAREN)) {
      std::cerr << "[ERROR] Expected ')' after condition\n";
      return nullptr;
   }

   auto thenBranch = statement();
   std::unique_ptr<Expr> elseBranch = nullptr;

   if (match(TokenType::ELSE)) {
      elseBranch = statement();
   }

   return std::make_unique<IfStatementExpr>(
           std::move(condition),
           std::move(thenBranch),
           std::move(elseBranch)
   );
}

std::unique_ptr<Expr> Parser::whileStatement() {
   if (!match(TokenType::LEFT_PAREN)) return nullptr;
   auto condition = expression();
   if (!match(TokenType::RIGHT_PAREN)) return nullptr;

   auto body = statement();
   return std::make_unique<WhileStatementExpr>(
           std::move(condition),
           std::move(body)
   );
}

std::unique_ptr<Expr> Parser::forStatement() {
   if (!match(TokenType::LEFT_PAREN)) {
      std::cerr << "[ERROR] Expected '(' after 'for'\n";
      return nullptr;
   }

   std::unique_ptr<Expr> initializer = nullptr;
   if (match(TokenType::VAR)) {
      initializer = declaration();
   } else if (!check(TokenType::SEMICOLON)) {
      initializer = expression();
      if (!match(TokenType::SEMICOLON)) {
         std::cerr << "[ERROR] Expected ';' after for-loop initializer\n";
         return nullptr;
      }
      initializer = std::make_unique<ExpressionStatementExpr>(std::move(initializer));
   } else {
      advance();
   }

   std::unique_ptr<Expr> condition = nullptr;
   if (!check(TokenType::SEMICOLON)) {
      condition = expression();
   }
   if (!match(TokenType::SEMICOLON)) {
      std::cerr << "[ERROR] Expected ';' after for-loop condition\n";
      return nullptr;
   }

   std::unique_ptr<Expr> increment = nullptr;
   if (!check(TokenType::RIGHT_PAREN)) {
      increment = expression();
   }
   if (!match(TokenType::RIGHT_PAREN)) {
      std::cerr << "[ERROR] Expected ')' after for-loop increment\n";
      return nullptr;
   }

   auto body = statement();

   std::vector<std::unique_ptr<Expr>> loopBodyStatements;
   loopBodyStatements.push_back(std::move(body));
   if (increment) {
      loopBodyStatements.push_back(std::make_unique<ExpressionStatementExpr>(std::move(increment)));
   }

   auto loop = std::make_unique<WhileStatementExpr>(
           condition ? std::move(condition) : std::make_unique<LiteralExpr>(true),
           std::make_unique<BlockStatementExpr>(std::move(loopBodyStatements))
   );

   if (initializer) {
      std::vector<std::unique_ptr<Expr>> full;
      full.push_back(std::move(initializer));
      full.push_back(std::move(loop));
      return std::make_unique<BlockStatementExpr>(std::move(full));
   }

   return loop;
}

std::unique_ptr<Expr> Parser::functionDeclaration() {
   if (!match(TokenType::IDENTIFIER)) return nullptr;
   std::string name = tokens[current - 1].lexeme;

   if (!match(TokenType::LEFT_PAREN)) return nullptr;

   std::vector<std::string> params;
   if (!check(TokenType::RIGHT_PAREN)) {
      do {
         if (!match(TokenType::IDENTIFIER)) return nullptr;
         params.push_back(tokens[current - 1].lexeme);
      } while (match(TokenType::COMMA));
   }

   if (!match(TokenType::RIGHT_PAREN)) return nullptr;

   if (!match(TokenType::LEFT_BRACE)) {
      std::cerr << "[ERROR] Expected '{' before function body\n";
      return nullptr;
   }

   auto body = block();
   if (!body) {
      std::cerr << "[ERROR] Failed to parse function body\n";
      return nullptr;
   }

   return std::make_unique<FunctionDeclarationExpr>(
           std::move(name),
           std::move(params),
           std::move(body)
   );
}

std::unique_ptr<Expr> Parser::returnStatement() {
   std::unique_ptr<Expr> value = nullptr;

   if (!check(TokenType::SEMICOLON)) {
      value = expression();
   }

   if (!match(TokenType::SEMICOLON)) {
      std::cerr << "[ERROR] Expected ';' after return statement\n";
      return nullptr;
   }

   return std::make_unique<ReturnStatementExpr>(std::move(value));
}
#pragma clang diagnostic pop