//
// Created by Kaan Karaman on 05/04/2025.
//

#include <cctype>
#include <unordered_map>

#include "token_type.h"
#include "tokenizer.h"
#include "error.h"

Tokenizer::Tokenizer(const std::string &source)
        : source(source) {}

std::vector<Token> Tokenizer::tokenize() {
   std::vector<Token> tokens;

   while (current < source.size()) {
      skipWhitespace();
      start = current;

      char c = peek();

      if (std::isalpha(c) || c == '_') {
         tokens.push_back(identifier());
         continue;
      }

      if (std::isdigit(c)) {
         tokens.push_back(number());
         continue;
      }

      if (c == '"') {
         tokens.push_back(string());
         continue;
      }

      switch (c) {
         // TODO: Duplicated log
         case '(':
            advance();
            tokens.push_back(makeToken(TokenType::LEFT_PAREN));
            continue;
         case ')':
            advance();
            tokens.push_back(makeToken(TokenType::RIGHT_PAREN));
            continue;
         case '{':
            advance();
            tokens.push_back(makeToken(TokenType::LEFT_BRACE));
            continue;
         case '}':
            advance();
            tokens.push_back(makeToken(TokenType::RIGHT_BRACE));
            continue;
         case ';':
            advance();
            tokens.push_back(makeToken(TokenType::SEMICOLON));
            continue;
         case ',':
            advance();
            tokens.push_back(makeToken(TokenType::COMMA));
            continue;
         case '^':
            advance();
            tokens.push_back(makeToken(TokenType::BITWISE_XOR));
            break;
         case '~':
            advance();
            tokens.push_back(makeToken(TokenType::BITWISE_NOT));
            break;
         case '+':
            advance();
            tokens.push_back(makeToken(TokenType::PLUS));
            continue;
         case '-':
            advance();
            tokens.push_back(makeToken(TokenType::MINUS));
            continue;
         case '*':
            advance();
            tokens.push_back(makeToken(TokenType::MULTIPLY));
            continue;
         case '/':
            advance();
            if (peek() == '/') {
               while (peek() != '\n' && peek() != '\0') advance();
               continue;
            }
            if (peek() == '*') {
               advance();
               while (peek() != '*' || peekNext() != '/') {
                  if (peek() == '\0') break;
                  advance();
               }
               advance();
               advance();
               continue;
            }
            tokens.push_back(makeToken(TokenType::DIVIDE));
            continue;
         case '!':
            advance();
            if (match('=')) {
               tokens.push_back(makeToken(match('=') ? TokenType::STRICT_NOT_EQUAL : TokenType::NOT_EQUAL));
            } else {
               tokens.push_back(makeToken(TokenType::NOT));
            }
            continue;
         case '>':
            advance();
            if (match('=')) {
               tokens.push_back(makeToken(TokenType::GREATER_THAN_EQUAL));
            } else {
               tokens.push_back(makeToken(TokenType::GREATER_THAN));
            }
            break;
         case '<':
            advance();
            if (match('=')) {
               tokens.push_back(makeToken(TokenType::LESS_THAN_EQUAL));
            } else {
               tokens.push_back(makeToken(TokenType::LESS_THAN));
            }
            break;
         case '&':
            advance();
            if (match('&')) {
               tokens.push_back(makeToken(TokenType::AND));
            } else {
               tokens.push_back(makeToken(TokenType::BITWISE_AND));
            }
            break;
         case '|':
            advance();
            if (match('|')) {
               tokens.push_back(makeToken(TokenType::OR));
            } else {
               tokens.push_back(makeToken(TokenType::BITWISE_OR));
            }
            break;
         case '=':
            advance();
            tokens.push_back(makeToken(match('=') ? TokenType::EQUAL : TokenType::ASSIGN));
            continue;

         default:
            // TODO: Handle unknown characters
            advance();
            continue;
      }
   }

   tokens.push_back(makeToken(TokenType::END_OF_FILE));
   return tokens;
}

char Tokenizer::peek() const {
   if (current >= source.size()) {
      return '\0';
   }
   return source[current];
}

char Tokenizer::peekNext() const {
   if (current + 1 >= source.size()) {
      return '\0';
   }
   return source[current + 1];
}

char Tokenizer::advance() {
   if (current >= source.size()) {
      return '\0';
   }
   char c = source[current++];
   if (c == '\n') {
      line++;
      column = 1;
   } else {
      column++;
   }
   return c;
}

bool Tokenizer::match(char expected) {
   if (peek() == expected) {
      advance();
      return true;
   }
   return false;
}

void Tokenizer::skipWhitespace() {
   while (current < source.size()) {
      auto c = static_cast<unsigned char>(source[current]);
      if (std::isspace(c)) {
         advance();
         continue;
      }

      if (c == 0xEF && (current + 2 < source.size())) {
         auto c2 = static_cast<unsigned char>(source[current + 1]);
         auto c3 = static_cast<unsigned char>(source[current + 2]);
         if (c2 == 0xBB && c3 == 0xBF) {
            advance();
            advance();
            advance();
            continue;
         }
      }

      break;
   }
}

Token Tokenizer::makeToken(TokenType type) const {
   std::string lexeme = source.substr(start, current - start);
   return {type, lexeme, line, column};
}

Token Tokenizer::string() {
   advance();


   while (peek() != '"' && peek() != '\0') {
      if (peek() == '\n') {
         throw SyntaxError("Unterminated string literal", line, column);
      }

      if (peek() == '\\') {
         advance();
         switch (peek()) {
            case 'n':
               advance();
               break;
            case 't':
               advance();
               break;
            case 'r':
               advance();
               break;
            case '\\':
               advance();
               break;
            case '"':
               advance();
               break;
            default:
               throw SyntaxError("Invalid escape sequence", line, column);
         }
      } else {
         advance();
      }
   }

   if (peek() == '\0') {
      throw SyntaxError("Unterminated string literal", line, column);
   }

   advance();
   return makeToken(TokenType::STRING_LITERAL);
}

Token Tokenizer::number() {
   while (std::isdigit(peek())) {
      advance();
   }
   return makeToken(TokenType::INTEGER_LITERAL);
}

Token Tokenizer::identifier() {
   while (std::isalnum(peek()) || peek() == '_') {
      advance();
   }

   std::string lexeme = source.substr(start, current - start);
   static const std::unordered_map<std::string, TokenType> keywords = {
           {"if",       TokenType::IF},
           {"else",     TokenType::ELSE},
           {"while",    TokenType::WHILE},
           {"for",      TokenType::FOR},
           {"return",   TokenType::RETURN},
           {"function", TokenType::FUNCTION},
           {"var",      TokenType::VAR},
           {"true",     TokenType::BOOLEAN_LITERAL},
           {"false",    TokenType::BOOLEAN_LITERAL},
           {"null",     TokenType::NULL_LITERAL},
           {"int",      TokenType::INT},
           {"float",    TokenType::FLOAT},
           {"string",   TokenType::STRING},
           {"bool",     TokenType::BOOL},
           {"do",       TokenType::DO},
           {"break",    TokenType::BREAK},
           {"continue", TokenType::CONTINUE},
           {"switch",   TokenType::SWITCH},
           {"case",     TokenType::CASE},
           {"default",  TokenType::DEFAULT},
           {"try",      TokenType::TRY},
           {"catch",    TokenType::CATCH},
           {"finally",  TokenType::FINALLY}
   };

   auto it = keywords.find(lexeme);
   TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
   return {type, lexeme, line, column};
}