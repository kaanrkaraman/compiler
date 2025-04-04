//
// Created by Kaan Karaman on 04/04/2025.
//

#ifndef COMPILER_TOKEN_TYPE_H
#define COMPILER_TOKEN_TYPE_H

#include <cstdint>
#include <string>
#include <utility>

enum class TokenType : std::uint8_t {
    UNKNOWN = 0,

    // Keywords
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    BREAK,
    CONTINUE,
    FUNCTION,
    VAR,

    // Data types
    INT,
    FLOAT,
    STRING,
    BOOL,

    // Operators
    PLUS, // +
    MINUS, // -
    MULTIPLY, // *
    DIVIDE, // /
    ASSIGN, // =
    MATRIX_ASSIGN, // :=
    MATRIX_MULTIPLY, // @

    // Delimiters
    SEMICOLON,
    COMMA,
    DOT,

    // Identifiers
    IDENTIFIER, // Variable names, function names, etc.

    // Literals
    INTEGER_LITERAL, // 123
    FLOAT_LITERAL, // 123.45
    STRING_LITERAL, // "Hello, World!"
    BOOLEAN_LITERAL, // true or false
    NULL_LITERAL, // null

    // Parentheses
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,

    // Comparison operators
    EQUAL, // ==
    NOT_EQUAL, // !=
    STRICT_EQUAL, // ===
    STRICT_NOT_EQUAL, // !==
    GREATER_THAN, // >
    LESS_THAN, // <
    GREATER_THAN_EQUAL, // >=
    LESS_THAN_EQUAL, // <=

    // Logical operators
    AND, // &&
    OR, // ||
    NOT, // !
    BITWISE_AND, // &
    BITWISE_OR, // |
    BITWISE_XOR, // ^
    BITWISE_NOT, // ~

    // Miscellaneous
    COMMENT, // Single line comment
    BLOCK_COMMENT, // Multi-line comment
    WHITESPACE, // Space, tab, newline
    END_OF_FILE, // End of file
};

#endif //COMPILER_TOKEN_TYPE_H
