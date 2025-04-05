//
// Created by Kaan Karaman on 04/04/2025.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include <iostream>

#include "parser.h"
#include "error.h"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

std::unique_ptr<Expr> Parser::parse() {
   scopeManager.pushScope();
   std::vector<std::unique_ptr<Expr>> statements;

   while (!isAtEnd()) {
      if (check(TokenType::END_OF_FILE)) break;

      auto decl = declaration();
      if (decl) {
         statements.push_back(std::move(decl));
      } else {
         if (isAtEnd()) break;
         advance();
         throw CompilerError("Failed to parse declaration");
      }
   }

   scopeManager.popScope();

   return std::make_unique<BlockStatementExpr>(std::move(statements));
}

bool Parser::isAtEnd() const {
   return peek().type == TokenType::END_OF_FILE;
}

const Token &Parser::peek() const {
   return tokens[current];
}

const Token &Parser::advance() {
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

int Parser::getPrecedence(TokenType type) {
   switch (type) {
      case TokenType::OR:
         return 1;
      case TokenType::AND:
         return 2;
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
      default:
         return -1;
   }
}

int Parser::getAssociativity(TokenType type) {
   switch (type) {
      case TokenType::ASSIGN:
         return 2; // RTL
      case TokenType::OR:
      case TokenType::AND:
      case TokenType::EQUAL:
      case TokenType::NOT_EQUAL:
      case TokenType::GREATER_THAN:
      case TokenType::GREATER_THAN_EQUAL:
      case TokenType::LESS_THAN:
      case TokenType::LESS_THAN_EQUAL:
      case TokenType::PLUS:
      case TokenType::MINUS:
      case TokenType::MULTIPLY:
      case TokenType::DIVIDE:
      case TokenType::MATRIX_MULTIPLY:
         return 1; // LTR
      default:
         return -1;
   }
}

std::unique_ptr<Expr> Parser::expression() {
   return parseBinaryExpression(0);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantConditionsOC"

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
            throw CompilerError("Expected ')' after function arguments", peek().line, peek().column);
         }

         if (auto *id = dynamic_cast<IdentifierExpr *>(expr.get())) {
            expr = std::make_unique<FunctionCallExpr>(
                    id->name, std::move(arguments)
            );
         }
      } else {
         break;
      }
   }

   return expr;
}

#pragma clang diagnostic pop

std::unique_ptr<Expr> Parser::parseBinaryExpression(int minPrecedence) {
   std::unique_ptr<Expr> left = parseUnary();

   if (!left) {
      throw CompilerError("Failed to parse primary expression", peek().line, peek().column);
   }

   while (true) {
      TokenType opType = peek().type;
      int precedence = getPrecedence(opType);
      if (precedence == -1 || precedence < minPrecedence) break;

      int associativity = getAssociativity(opType);
      int nextMinPrecedence = (associativity == 1) ? precedence + 1 : precedence;

      Token opToken = advance();
      std::string opLexeme = opToken.lexeme;

      std::unique_ptr<Expr> right = parseBinaryExpression(nextMinPrecedence);
      if (!right) {
         throw CompilerError("Failed to parse right-hand side expression", peek().line, peek().column);
      }

      if (opType == TokenType::ASSIGN) {
         if (auto *id = dynamic_cast<IdentifierExpr *>(left.get())) {
            left = std::make_unique<AssignmentExpr>(id->name, std::move(right));
         } else {
            throw CompilerError("Invalid assignment target", opToken.line, opToken.column);
         }
      } else if (opType == TokenType::MATRIX_MULTIPLY) {
         left = std::make_unique<MatrixMultiplicationExpr>(
                 std::move(left), std::move(right));
      } else {
         left = std::make_unique<BinaryExpr>(
                 std::move(left), opLexeme, std::move(right));
      }
   }

   return left;
}

std::unique_ptr<Expr> Parser::parseUnary() {
   std::vector<Token> unaryOperators;

   while (true) {
      TokenType t = peek().type;
      if (t == TokenType::PLUS || t == TokenType::MINUS ||
          t == TokenType::BANG || t == TokenType::BITWISE_NOT ||
          t == TokenType::INCREMENT || t == TokenType::DECREMENT) {
         unaryOperators.push_back(advance());
      } else {
         break;
      }
   }

   std::unique_ptr<Expr> operand = parsePostfix(primary());

   for (auto it = unaryOperators.rbegin(); it != unaryOperators.rend(); ++it) {
      operand = std::make_unique<UnaryExpr>(it->lexeme, std::move(operand));
   }

   return operand;
}

std::unique_ptr<Expr> Parser::primary() {
   if (match(TokenType::LEFT_PAREN)) {
      auto expr = expression();
      if (!match(TokenType::RIGHT_PAREN)) {
         throw CompilerError("Expected ')' after expression", peek().line, peek().column);
      }
      return expr;
   }

   if (match(TokenType::INTEGER_LITERAL)) {
      const Token &token = tokens[current - 1];

      try {
         int value = std::stoi(token.lexeme);
         return std::make_unique<LiteralExpr>(value);
      } catch (const std::invalid_argument &e) {
         throw CompilerError("Invalid integer literal", token.line, token.column);
      }
   }

   if (match(TokenType::FLOAT_LITERAL)) {
      const Token &token = tokens[current - 1];
      return std::make_unique<LiteralExpr>(std::stof(token.lexeme));
   }

   if (match(TokenType::STRING_LITERAL)) {
      const Token &token = tokens[current - 1];
      return std::make_unique<LiteralExpr>(token.lexeme);
   }

   if (match(TokenType::BOOLEAN_LITERAL)) {
      const Token &token = tokens[current - 1];
      bool value = (token.lexeme == "true");
      return std::make_unique<LiteralExpr>(value);
   }

   if (match(TokenType::NULL_LITERAL)) {
      return std::make_unique<LiteralExpr>(nullptr);
   }

   if (match(TokenType::IDENTIFIER)) {
      const Token &token = tokens[current - 1];

      Symbol *sym = scopeManager.lookup(token.lexeme);
      if (!sym) {
         throw CompilerError("Use of undeclared variable or name: " + token.lexeme,
                             token.line, token.column);
      }

      return std::make_unique<IdentifierExpr>(token.lexeme);
   }

   throw CompilerError("Unexpected token in primary expression", peek().line, peek().column);
}

std::unique_ptr<Expr> Parser::declaration() {
   if (check(TokenType::END_OF_FILE)) {
      std::cerr << "[DEBUG] EOF reached in declaration()\n";
      return nullptr;
   }

   if (match(TokenType::VAR)) {
      if (!match(TokenType::IDENTIFIER)) {
         throw CompilerError("Expected variable name after 'var'", peek().line, peek().column);
      }

      std::string name = tokens[current - 1].lexeme;
      const Token &token = tokens[current - 1];

      std::shared_ptr<Type> declaredType = std::make_shared<Type>(TypeKind::Unknown);

      if (match(TokenType::SEMICOLON)) {
         if (!match(TokenType::IDENTIFIER)) {
            throw CompilerError("Expected type name after ':'", peek().line, peek().column);
         }

         std::string typeName = tokens[current - 1].lexeme;

         if (typeName == "int") declaredType = std::make_shared<Type>(TypeKind::Int);
         else if (typeName == "float")declaredType = std::make_shared<Type>(TypeKind::Float);
         else if (typeName == "bool") declaredType = std::make_shared<Type>(TypeKind::Bool);
         else if (typeName == "string")declaredType = std::make_shared<Type>(TypeKind::String);
         else if (typeName == "null") declaredType = std::make_shared<Type>(TypeKind::Null);
         else declaredType = std::make_shared<Type>(TypeKind::Custom, typeName);
      }

      std::unique_ptr<Expr> initializer = nullptr;
      if (match(TokenType::ASSIGN)) {
         initializer = expression();
      }

      if (!match(TokenType::SEMICOLON)) {
         throw CompilerError("Expected ';' after variable declaration", peek().line, peek().column);
      }

      Symbol sym(name, SymbolType::Variable, declaredType, true, token.line, token.column);
      if (!scopeManager.declare(sym)) {
         throw CompilerError("Variable '" + name + "' already declared in this scope", token.line, token.column);
      }

      return std::make_unique<VarDeclarationExpr>(name, std::move(initializer));
   }

   if (match(TokenType::FUNCTION)) {
      return functionDeclaration();
   }

   if (check(TokenType::CATCH) || check(TokenType::FINALLY)) {
      return nullptr;
   }

   return statement();
}

std::unique_ptr<Expr> Parser::statement() {
   if (check(TokenType::END_OF_FILE)) return nullptr;

   if (match(TokenType::IF)) return ifStatement();
   if (match(TokenType::WHILE)) return whileStatement();
   if (match(TokenType::FOR)) return forStatement();
   if (match(TokenType::DO)) return doWhileStatement();
   if (match(TokenType::SWITCH)) return switchStatement();
   if (match(TokenType::TRY)) return tryStatement();
   if (match(TokenType::RETURN)) return returnStatement();
   if (match(TokenType::BREAK)) {
      if (!match(TokenType::SEMICOLON)) {
         throw CompilerError("Expected ';' after 'break'", peek().line, peek().column);
      }
      return std::make_unique<BreakStatementExpr>();
   }
   if (match(TokenType::CONTINUE)) {
      if (!match(TokenType::SEMICOLON)) {
         throw CompilerError("Expected ';' after 'continue'", peek().line, peek().column);
      }
      return std::make_unique<ContinueStatementExpr>();
   }
   if (match(TokenType::LEFT_BRACE)) return block();

   if (check(TokenType::CATCH) || check(TokenType::FINALLY)) {
      throw CompilerError("Unexpected 'catch' or 'finally' outside of 'try'", peek().line, peek().column);
   }

   std::unique_ptr<Expr> expr = expression();
   if (!match(TokenType::SEMICOLON)) {
      throw CompilerError("Expected ';' after expression statement", peek().line, peek().column);
   }

   return std::make_unique<ExpressionStatementExpr>(std::move(expr));
}

std::unique_ptr<Expr> Parser::block() {
   if (!match(TokenType::LEFT_BRACE)) {
      throw CompilerError("Expected '{' at start of block", peek().line, peek().column);
   }

   scopeManager.pushScope();
   std::vector<std::unique_ptr<Expr>> statements;

   while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      if (check(TokenType::CATCH) || check(TokenType::FINALLY)) {
         break;
      }

      size_t prevIndex = current;
      auto decl = declaration();
      if (decl) {
         statements.push_back(std::move(decl));
      } else {
         if (current == prevIndex) {
            throw CompilerError("Unexpected token in block", peek().line, peek().column);
         }
         break;
      }
   }

   if (!match(TokenType::RIGHT_BRACE)) {
      throw CompilerError("Expected '}' after block", peek().line, peek().column);
   }

   scopeManager.popScope();
   return std::make_unique<BlockStatementExpr>(std::move(statements));
}

std::unique_ptr<Expr> Parser::statementOrBlock() {
   if (check(TokenType::LEFT_BRACE)) {
      std::cout << "[DEBUG] Block is called from statementOrBlock()\n";
      return block();
   }
   std::cout << "[DEBUG] Statement is called from statementOrBlock()\n";
   return statement();
}

std::unique_ptr<Expr> Parser::ifStatement() {
   if (!match(TokenType::LEFT_PAREN)) {
      throw CompilerError("Expected '(' after 'if'", peek().line, peek().column);
   }

   auto condition = expression();

   if (!match(TokenType::RIGHT_PAREN)) {
      throw CompilerError("Expected ')' after 'if' condition", peek().line, peek().column);
   }

   auto thenBranch = statementOrBlock();
   std::unique_ptr<Expr> elseBranch = nullptr;

   if (match(TokenType::ELSE)) {
      elseBranch = statementOrBlock();
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
      throw CompilerError("Expected '(' after 'for'", peek().line, peek().column);
   }

   std::unique_ptr<Expr> initializer = nullptr;
   if (match(TokenType::VAR)) {
      initializer = declaration();
   } else if (!check(TokenType::SEMICOLON)) {
      initializer = expression();
      if (!match(TokenType::SEMICOLON)) {
         throw CompilerError("Expected ';' after for-loop initializer", peek().line, peek().column);
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
      throw CompilerError("Expected ';' after for-loop condition", peek().line, peek().column);
   }

   std::unique_ptr<Expr> increment = nullptr;
   if (!check(TokenType::RIGHT_PAREN)) {
      increment = expression();
   }
   if (!match(TokenType::RIGHT_PAREN)) {
      throw CompilerError("Expected ')' after for-loop increment", peek().line, peek().column);
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
   if (!match(TokenType::IDENTIFIER)) {
      throw CompilerError("Expected function name after 'function'", peek().line, peek().column);
   }

   std::string name = tokens[current - 1].lexeme;

   if (!match(TokenType::LEFT_PAREN)) {
      throw CompilerError("Expected '(' after function name", peek().line, peek().column);
   }

   std::vector<std::string> params;
   std::vector<std::shared_ptr<Type>> paramTypes;

   if (!check(TokenType::RIGHT_PAREN)) {
      do {
         if (!match(TokenType::IDENTIFIER)) {
            throw CompilerError("Expected parameter name", peek().line, peek().column);
         }

         std::string paramName = tokens[current - 1].lexeme;

         auto paramType = std::make_shared<Type>(TypeKind::Unknown);

         Symbol paramSym(paramName, SymbolType::Parameter, paramType, true,
                         tokens[current - 1].line, tokens[current - 1].column);

         if (!scopeManager.declare(paramSym)) {
            throw CompilerError("Parameter '" + paramName + "' already declared",
                                tokens[current - 1].line, tokens[current - 1].column);
         }

         params.push_back(paramName);
         paramTypes.push_back(std::move(paramType));

      } while (match(TokenType::COMMA));
   }

   if (!match(TokenType::RIGHT_PAREN)) {
      throw CompilerError("Expected ')' after function parameters", peek().line, peek().column);
   }

   auto returnType = std::make_shared<Type>(TypeKind::Unknown);

   Symbol functionSym(name, SymbolType::Function,
                      Type::makeFunction(paramTypes, returnType),
                      false,
                      tokens[current - 1].line,
                      tokens[current - 1].column);

   if (!scopeManager.declare(functionSym)) {
      throw CompilerError("Function '" + name + "' already declared", peek().line, peek().column);
   }

   scopeManager.pushScope();

   auto body = block();

   scopeManager.popScope();

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
      throw CompilerError("Expected ';' after return statement", peek().line, peek().column);
   }

   return std::make_unique<ReturnStatementExpr>(std::move(value));
}

std::unique_ptr<Expr> Parser::switchStatement() {
   if (!match(TokenType::LEFT_PAREN)) {
      throw CompilerError("Expected '(' after 'switch'", peek().line, peek().column);
   }

   auto switchExpr = expression();

   if (!match(TokenType::RIGHT_PAREN)) {
      throw CompilerError("Expected ')' after switch expression", peek().line, peek().column);
   }

   if (!match(TokenType::LEFT_BRACE)) {
      throw CompilerError("Expected '{' after switch()", peek().line, peek().column);
   }

   std::vector<std::unique_ptr<Expr>> caseClauses;
   std::unique_ptr<Expr> defaultClause = nullptr;

   while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      if (match(TokenType::CASE)) {
         auto caseValue = expression();
         if (!match(TokenType::SEMICOLON)) {
            throw CompilerError("Expected ':' after case expression", peek().line, peek().column);
         }
         auto stmt = statement();
         caseClauses.push_back(
                 std::make_unique<CaseClauseExpr>(std::move(caseValue), std::move(stmt))
         );
      } else if (match(TokenType::DEFAULT)) {
         if (!match(TokenType::SEMICOLON)) {
            throw CompilerError("Expected ':' after 'default'", peek().line, peek().column);
         }
         defaultClause = statement();
      } else {
         throw CompilerError("Expected 'case' or 'default'", peek().line, peek().column);
      }
   }

   if (!match(TokenType::RIGHT_BRACE)) {
      throw CompilerError("Expected '}' at end of switch block", peek().line, peek().column);
   }

   return std::make_unique<SwitchStatementExpr>(
           std::move(switchExpr),
           std::move(caseClauses),
           std::move(defaultClause)
   );
}

std::unique_ptr<Expr> Parser::doWhileStatement() {
   auto body = statementOrBlock();

   if (!match(TokenType::WHILE)) {
      throw CompilerError("Expected 'while' after do block", peek().line, peek().column);
   }
   if (!match(TokenType::LEFT_PAREN)) {
      throw CompilerError("Expected '(' after 'while'", peek().line, peek().column);
   }

   auto condition = expression();

   if (!match(TokenType::RIGHT_PAREN)) {
      throw CompilerError("Expected ')' after condition", peek().line, peek().column);
   }
   if (!match(TokenType::SEMICOLON)) {
      throw CompilerError("Expected ';' after do-while statement", peek().line, peek().column);
   }

   return std::make_unique<DoWhileStatementExpr>(
           std::move(condition),
           std::move(body)
   );
}

std::unique_ptr<Expr> Parser::tryStatement() {
   if (!check(TokenType::LEFT_BRACE)) {
      throw CompilerError("Expected '{' after 'try'", peek().line, peek().column);
   }

   auto tryBlock = block();

   std::vector<std::unique_ptr<Expr>> catches;

   while (match(TokenType::CATCH)) {
      if (!match(TokenType::LEFT_PAREN)) {
         throw CompilerError("Expected '(' after 'catch'", peek().line, peek().column);
      }

      if (!match(TokenType::IDENTIFIER)) {
         throw CompilerError("Expected exception variable name after 'catch('", peek().line, peek().column);
      }

      std::string exceptionVarName = tokens[current - 1].lexeme;

      if (!match(TokenType::RIGHT_PAREN)) {
         throw CompilerError("Expected ')' after catch variable", peek().line, peek().column);
      }

      scopeManager.pushScope();

      Symbol catchSym(
              exceptionVarName,
              SymbolType::Variable,
              std::make_shared<Type>(TypeKind::Unknown),
              true,
              tokens[current - 1].line,
              tokens[current - 1].column
      );

      if (!scopeManager.declare(catchSym)) {
         throw CompilerError("Exception variable '" + exceptionVarName + "' already declared",
                             tokens[current - 1].line, tokens[current - 1].column);
      }

      if (!check(TokenType::LEFT_BRACE)) {
         throw CompilerError("Expected '{' to start catch block", peek().line, peek().column);
      }

      auto catchBlock = block();

      scopeManager.popScope();

      catches.push_back(
              std::make_unique<CatchClauseExpr>(exceptionVarName, std::move(catchBlock))
      );
   }

   std::unique_ptr<Expr> finallyBlock = nullptr;
   if (match(TokenType::FINALLY)) {
      if (!check(TokenType::LEFT_BRACE)) {
         throw CompilerError("Expected '{' after 'finally'", peek().line, peek().column);
      }
      finallyBlock = block();
   }

   if (catches.empty() && !finallyBlock) {
      throw CompilerError(
              "Expected at least one 'catch' or a 'finally' block after 'try'",
              peek().line, peek().column
      );
   }

   return std::make_unique<TryCatchFinallyStatementExpr>(
           std::move(tryBlock),
           std::move(catches),
           std::move(finallyBlock)
   );
}

#pragma clang diagnostic pop