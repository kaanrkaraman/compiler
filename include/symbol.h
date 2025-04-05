//
// Created by Kaan Karaman on 05/04/2025.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include <string>
#include <optional>

#include "types.h"

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
    SymbolType type;
    std::shared_ptr<Type> declaredType;

    bool isMutable;
    int line;
    int column;

    Symbol(std::string n, SymbolType k, std::shared_ptr<Type> t, bool mut, int ln, int col)
            : name(std::move(n)), type(k), declaredType(std::move(t)), isMutable(mut), line(ln), column(col) {}
};

#endif //COMPILER_SYMBOL_H
