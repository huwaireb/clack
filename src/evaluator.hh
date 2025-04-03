#pragma once

#include <expected>
#include <map>
#include <string>

#include "parser.hh"

class Evaluator {
 public:
  std::map<std::string, double> variables;
  using Result = std::expected<double, Expr::Error>;

  Evaluator() = default;

  Result evaluate(const Expr& expr);

  void setVariable(const std::string& name, double value);
  void clearVariables();

 private:
  Result visit(const Number& number);
  Result visit(const Variable& variable);
  Result visit(const Binary& binary);
  Result visit(const Unary& unary);
};
