#include <iostream>

#include "tokenizer.h"
#include "token_type.h"
#include "parser.h"

void printExpr(const Expr *expr) {
   if (!expr) {
      std::cout << "[null expr]\n";
      return;
   }

   std::cout << expr->toString() << "\n";
}

int main() {
   std::string sourceCode = R"(
if (x > 0) { return x; }
)";

   Tokenizer tokenizer(sourceCode);
   std::vector<Token> tokens = tokenizer.tokenize();

   Parser parser(tokens);

   parser.scopeManager.declare(Symbol(
           "x",
           SymbolType::Variable,
           std::make_shared<Type>(TypeKind::Int),
           true,
           0,
           0
   ));

   parser.scopeManager.declare(Symbol(
           "print",
           SymbolType::Function,
           std::make_shared<Type>(TypeKind::Void),
           false,
           0,
           0
   ));

   std::unique_ptr<Expr> ast = parser.parse();

   if (ast) {
      std::cout << "=== AST ===\n";
      printExpr(ast.get());
   } else std::cerr << "Parsing failed.";

   return 0;
}