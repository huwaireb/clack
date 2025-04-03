
#pragma once

#include <string>

#include "parser.hh"

class Printer {
  std::string visit(const Number& number);
  std::string visit(const Variable& variable);
  std::string visit(const Binary& binary);
  std::string visit(const Unary& binary);

 public:
  std::string print(const Expr& expr);
};
