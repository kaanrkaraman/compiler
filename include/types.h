#ifndef COMPILER_TYPES_H
#define COMPILER_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <sstream>

enum class TypeKind {
    Int,
    Float,
    Bool,
    String,
    Void,
    Null,
    Unknown,
    Function,
    Struct,
    Class,
    Generic,
    Custom
};

struct StructField {
    std::string name;
    std::shared_ptr<struct Type> type;
};

struct StructType {
    std::string name;
    std::vector<StructField> fields;
};

struct Type {
    TypeKind kind;
    std::string name;
    std::vector<std::shared_ptr<Type>> parameterTypes;
    std::shared_ptr<Type> returnType;
    std::vector<std::shared_ptr<Type>> parameters;
    std::shared_ptr<StructType> structInfo;

    explicit Type(TypeKind kind, std::string name = "")
            : kind(kind), name(std::move(name)) {}

    static std::shared_ptr<Type>
    makeFunction(std::vector<std::shared_ptr<Type>> params, std::shared_ptr<Type> returnType) {
       auto t = std::make_shared<Type>(TypeKind::Function, "function");
       t->parameterTypes = std::move(params);
       t->returnType = std::move(returnType);
       return t;
    }

    static std::shared_ptr<Type> makeGeneric(std::string name, std::vector<std::shared_ptr<Type>> params) {
       auto t = std::make_shared<Type>(TypeKind::Generic, std::move(name));
       t->parameters = std::move(params);
       return t;
    }

    static std::shared_ptr<Type> makeStruct(std::string name, std::vector<StructField> fields) {
       auto t = std::make_shared<Type>(TypeKind::Struct, std::move(name));
       t->structInfo = std::make_shared<StructType>(StructType{t->name, std::move(fields)});
       return t;
    }

    static std::shared_ptr<Type> makeClass(std::string name, std::vector<StructField> fields) {
       auto t = std::make_shared<Type>(TypeKind::Class, std::move(name));
       t->structInfo = std::make_shared<StructType>(StructType{t->name, std::move(fields)});
       return t;
    }

    [[nodiscard]] std::string toString() const {
       switch (kind) {
          case TypeKind::Int:
             return "int";
          case TypeKind::Float:
             return "float";
          case TypeKind::Bool:
             return "bool";
          case TypeKind::String:
             return "string";
          case TypeKind::Void:
             return "void";
          case TypeKind::Null:
             return "null";
          case TypeKind::Unknown:
             return "unknown";
          case TypeKind::Custom:
             return name;

          case TypeKind::Function: {
             std::stringstream ss;
             ss << "fn(";
             for (size_t i = 0; i < parameterTypes.size(); ++i) {
                ss << parameterTypes[i]->toString();
                if (i != parameterTypes.size() - 1) ss << ", ";
             }
             ss << ") -> " << (returnType ? returnType->toString() : "void");
             return ss.str();
          }

          case TypeKind::Generic: {
             std::stringstream ss;
             ss << name << "<";
             for (size_t i = 0; i < parameters.size(); ++i) {
                ss << parameters[i]->toString();
                if (i != parameters.size() - 1) ss << ", ";
             }
             ss << ">";
             return ss.str();
          }

          case TypeKind::Struct:
          case TypeKind::Class: {
             std::stringstream ss;
             ss << (kind == TypeKind::Struct ? "struct " : "class ") << name << " { ";
             for (size_t i = 0; i < structInfo->fields.size(); ++i) {
                const auto &field = structInfo->fields[i];
                ss << field.type->toString() << " " << field.name;
                if (i != structInfo->fields.size() - 1) ss << "; ";
             }
             ss << " }";
             return ss.str();
          }
       }

       return "unknown";
    }

    [[nodiscard]] bool equals(const std::shared_ptr<Type> &other) const {
       if (!other || kind != other->kind || name != other->name) return false;

       if (kind == TypeKind::Generic || kind == TypeKind::Function) {
          if (parameters.size() != other->parameters.size()) return false;
          for (size_t i = 0; i < parameters.size(); ++i) {
             if (!parameters[i]->equals(other->parameters[i])) return false;
          }
       }

       return true;
    }
};

#endif // COMPILER_TYPES_H