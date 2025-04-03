
#include "printer.hh"

#include <sstream>

std::string Printer::print(const Expr& expr) {
  return std::visit([this](const auto& node) { return visit(node); },
                    expr.node);
}

std::string Printer::visit(const Number& number) {
  std::ostringstream oss;
  oss << number.value;
  return oss.str();
}

std::string Printer::visit(const Variable& variable) { return variable.name; }

std::string Printer::visit(const Binary& binary) {
  std::string left = print(*binary.left);
  std::string right = print(*binary.right);
  return "(" + left + " " + binary.op + " " + right + ")";
}

std::string Printer::visit(const Unary& unary) {
  std::string op = std::string(1, unary.op);
  std::string operand = print(*unary.operand);

  return "(" + op + operand + ")";
}
