#include "evaluator.hh"

#include <cmath>

using Result = Evaluator::Result;

Result Evaluator::evaluate(const Expr& expr) {
  return std::visit([this](const auto& node) { return visit(node); },
                    expr.node);
}

void Evaluator::setVariable(const std::string& name, double value) {
  variables[name] = value;
}

void Evaluator::clearVariables() { variables.clear(); }

Result Evaluator::visit(const Number& number) { return number.value; }

Result Evaluator::visit(const Variable& variable) {
  auto it = variables.find(variable.name);

  if (it == variables.end()) {
    return std::unexpected(Expr::Error::UndefinedVariable);
  }

  return it->second;
}

Result Evaluator::visit(const Binary& binary) {
  auto left_result = evaluate(*binary.left);
  if (!left_result) return left_result;

  auto right_result = evaluate(*binary.right);
  if (!right_result) return right_result;

  double left = *left_result;
  double right = *right_result;

  switch (binary.op) {
    case '+':
      return left + right;
    case '-':
      return left - right;
    case '*':
      return left * right;
    case '/':
      if (right == 0.0) return std::unexpected(Expr::Error::DivisionByZero);
      return left / right;
    case '%':
      if (right == 0.0) return std::unexpected(Expr::Error::DivisionByZero);
      return std::fmod(left, right);
    case '^':
      return std::pow(left, right);
    default:
      return std::unexpected(Expr::Error::InvalidOperator);
  }
}

Result Evaluator::visit(const Unary& unary) {
  auto operand_result = evaluate(*unary.operand);
  if (!operand_result) return operand_result;

  double operand = *operand_result;

  switch (unary.op) {
    case '-':
      return -operand;
    case '+':
      return operand;
    default:
      return std::unexpected(Expr::Error::InvalidOperator);
  }
}
