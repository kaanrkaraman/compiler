//
// Created by Kaan Karaman on 05/04/2025.
//

#ifndef COMPILER_SCOPE_H
#define COMPILER_SCOPE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "symbol.h"

struct Scope {
    std::unordered_map<std::string, Symbol> symbols;

    bool declare(const Symbol& sym) {
       auto [it, inserted] = symbols.insert({sym.name, sym});
       return inserted;
    }

    Symbol* findLocal(const std::string &name) {
       auto it = symbols.find(name);
       if (it != symbols.end()) {
          return &it->second;
       }
       return nullptr;
    }
};

#endif //COMPILER_SCOPE_H
