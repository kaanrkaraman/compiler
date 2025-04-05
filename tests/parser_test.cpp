#include <gtest/gtest.h>

#include "tokenizer.h"
#include "parser.h"
#include "types.h"

std::string parseAndPrintAST(const std::string &source) {
   Tokenizer tokenizer(source);
   std::vector<Token> tokens = tokenizer.tokenize();

   Parser parser(tokens);

   parser.scopeManager.declare(
           Symbol("x", SymbolType::Variable, std::make_shared<Type>(TypeKind::Int), true, 0, 0)
   );

   std::unique_ptr<Expr> ast = parser.parse();
   if (!ast) throw std::runtime_error("Failed to parse");

   return ast->toString();
}

TEST(ParserTests, IfStatement) {
   std::string source = R"(if (x > 0) { return x; })";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("IfStatement"), std::string::npos);
}

TEST(ParserTests, ForLoop) {
   std::string source = R"(for (var i = 0; i < 5; i = i + 1) { print(i); })";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("ForStatement"), std::string::npos);
}

TEST(ParserTests, WhileLoop) {
   std::string source = R"(while (x != 0) { x = x - 1; })";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("WhileStatement"), std::string::npos);
}

TEST(ParserTests, DoWhileLoop) {
   std::string source = R"(do { x = x - 1; } while (x > 0);)";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("DoWhileStatement"), std::string::npos);
}

TEST(ParserTests, TryCatchFinally) {
   std::string source = R"(
        try {
            var x = 1;
        } catch (e) {
            print(e);
        } finally {
            print("done");
        })";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("TryCatchFinallyStatement"), std::string::npos);
}

TEST(ParserTests, BinaryExpression) {
   std::string source = R"(var z = 3 * (2 + 1);)";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("Binary(*)"), std::string::npos);
}

TEST(ParserTests, UnaryExpression) {
   std::string source = R"(var x = -y;)";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("Unary(-"), std::string::npos);
}

TEST(ParserTests, Assignment) {
   std::string source = R"(x = y + 1;)";
   auto output = parseAndPrintAST(source);
   EXPECT_NE(output.find("Assignment"), std::string::npos);
}