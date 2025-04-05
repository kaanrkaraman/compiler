//
// Created by Kaan Karaman on 05/04/2025.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include <string>
#include <optional>

enum class SymbolType {
    Variable,
    Function,
    Parameter,
    Class,
    Enum,
    Struct,
    Interface
};

struct Symbol {
    std::string name;
    SymbolType  type;

    bool isMutable;

    int line;
    int column;

    Symbol(std::string n, SymbolType k, const std::string& t, bool mut, int ln, int col)
            : name(std::move(n)), type(k), isMutable(mut), line(ln), column(col) {}
};

#endif //COMPILER_SYMBOL_H
