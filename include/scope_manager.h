//
// Created by Kaan Karaman on 05/04/2025.
//

#ifndef COMPILER_SCOPE_MANAGER_H
#define COMPILER_SCOPE_MANAGER_H

#include <stack>
#include "scope.h"

class ScopeManager {
public:
    void pushScope() {
       scopes.emplace_back(std::make_unique<Scope>());
    }

    void popScope() {
       if (!scopes.empty()) {
          scopes.pop_back();
       }
    }

    bool declare(const Symbol& sym) {
       if (scopes.empty()) {
          pushScope();
       }
       return scopes.back()->declare(sym);
    }

    Symbol* lookup(const std::string &name) {
       for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
          Symbol* found = (*it)->findLocal(name);
          if (found) return found;
       }
       return nullptr;
    }

private:
    std::vector<std::unique_ptr<Scope>> scopes;
};

#endif //COMPILER_SCOPE_MANAGER_H
