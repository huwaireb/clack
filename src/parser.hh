#pragma once

#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

struct Expr;

using ExprPtr = std::unique_ptr<Expr>;

struct Operator {
  char op;
  bool isUnary;
};

struct Number {
  double value;
};

struct Variable {
  std::string name;
  Variable(std::string name);
};

// e.g., 2 + 3
struct Binary {
  char op;
  ExprPtr left;
  ExprPtr right;
  Binary(char op, ExprPtr left, ExprPtr right);
};

struct Unary {
  char op;
  ExprPtr operand;
  Unary(char op, ExprPtr operand);
};

struct Expr {
  std::variant<Number, Variable, Binary, Unary> node;

  enum class Error {
    InvalidExpression,
    InvalidOperator,
    UnbalancedParentheses,
    UndefinedVariable,
    DivisionByZero
  };

  static ExprPtr makeNumber(double value);
  static ExprPtr makeVariable(std::string name);
  static ExprPtr makeBinary(char op, ExprPtr left, ExprPtr right);
  static ExprPtr makeUnary(char op, ExprPtr operand);
};

constexpr const std::string_view errorToString(Expr::Error error);
std::ostream& operator<<(std::ostream& os, const Expr::Error error);

std::expected<ExprPtr, Expr::Error> parseString(std::string_view infix);
