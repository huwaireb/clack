#include "parser.hh"

#include <cctype>
#include <map>
#include <optional>
#include <string>

#include "stack.hh"

static const std::map<char, int> precedence = {
    {'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}, {'%', 2}, {'^', 3}, {'(', 0}};

static int getPrecedence(const Operator& op) {
  if (op.isUnary) {
    return 4;  // Higher than any binary operator
  }

  return precedence.at(op.op);
}

static bool isLeftAssociative(char op) {
  return op == '+' || op == '-' || op == '*' || op == '/' || op == '%';
}

Variable::Variable(std::string name) : name(std::move(name)) {}
Binary::Binary(char op, ExprPtr left, ExprPtr right)
    : op(op), left(std::move(left)), right(std::move(right)) {}
Unary::Unary(char op, ExprPtr operand) : op(op), operand(std::move(operand)) {}

ExprPtr Expr::makeNumber(double value) {
  return std::make_unique<Expr>(Number{value});
}

ExprPtr Expr::makeVariable(std::string name) {
  return std::make_unique<Expr>(Variable{std::move(name)});
}

ExprPtr Expr::makeBinary(char op, ExprPtr left, ExprPtr right) {
  return std::make_unique<Expr>(Binary{op, std::move(left), std::move(right)});
}

ExprPtr Expr::makeUnary(char op, ExprPtr operand) {
  return std::make_unique<Expr>(Unary{op, std::move(operand)});
}

static std::expected<ExprPtr, Expr::Error> parseToken(std::string_view& input,
                                                      size_t& i) {
  std::string token;

  // Skip whitespace
  while (i < input.length() && std::isspace(input[i])) i++;
  if (i >= input.length())
    return std::unexpected(Expr::Error::InvalidExpression);

  // Parse number
  if (std::isdigit(input[i]) || input[i] == '.') {
    while (i < input.length() && (std::isdigit(input[i]) || input[i] == '.')) {
      token += input[i++];
    }
    try {
      size_t pos;
      double value = std::stod(token, &pos);
      // Ensure the entire token is consumed (e.g., reject "3.14.15")
      if (pos != token.length()) {
        return std::unexpected(Expr::Error::InvalidExpression);
      }
      return Expr::makeNumber(value);
    } catch (...) {
      return std::unexpected(Expr::Error::InvalidExpression);
    }
  }

  // Parse variable
  if (std::isalpha(input[i])) {
    while (i < input.length() && std::isalnum(input[i])) {
      token += input[i++];
    }
    return Expr::makeVariable(token);
  }

  return std::unexpected(Expr::Error::InvalidExpression);
}

// NOTE: Shunting Yard Algorithm
std::expected<ExprPtr, Expr::Error> parseString(std::string_view infix) {
  Stack<ExprPtr> expr_stack;  // Stack for expressions
  Stack<Operator> op_stack;   // Stack for operators
  size_t i = 0;
  bool expect_operand = true;  // Tracks whether we expect an operand next

  // Helper function to apply an operator from the stack
  auto applyOperator = [&]() -> std::expected<void, Expr::Error> {
    if (op_stack.isEmpty())
      return std::unexpected(Expr::Error::InvalidExpression);

    Operator op = op_stack.pop().value();
    if (op.isUnary) {
      if (expr_stack.size() < 1)
        return std::unexpected(Expr::Error::InvalidExpression);
      auto operand = std::move(expr_stack.pop().value());
      expr_stack.push(Expr::makeUnary(op.op, std::move(operand)));
    } else {
      if (expr_stack.size() < 2)
        return std::unexpected(Expr::Error::InvalidExpression);
      auto right = std::move(expr_stack.pop().value());
      auto left = std::move(expr_stack.pop().value());
      expr_stack.push(
          Expr::makeBinary(op.op, std::move(left), std::move(right)));
    }
    return {};
  };

  while (i < infix.length()) {
    char c = infix[i];

    // Skip whitespace
    if (std::isspace(c)) {
      i++;
      continue;
    }

    // Handle opening parenthesis
    if (c == '(') {
      op_stack.push(Operator{'(', false});  // '(' is never unary
      expect_operand = true;  // After '(', expect an operand or unary operator
      i++;
    }
    // Handle closing parenthesis
    else if (c == ')') {
      while (!op_stack.isEmpty() && op_stack.top().value().op != '(') {
        if (auto result = applyOperator(); !result) {
          return std::unexpected(result.error());
        }
      }
      if (op_stack.isEmpty()) {
        return std::unexpected(Expr::Error::UnbalancedParentheses);
      }
      op_stack.pop();          // Remove '('
      expect_operand = false;  // After ')', expect an operator or end
      i++;
    }
    // Handle operators
    else if (precedence.contains(c)) {  // Check if it's a valid operator
      if (expect_operand && (c == '-' || c == '+')) {
        // Treat as unary operator
        op_stack.push(Operator{c, true});
        expect_operand = true;  // Still expect an operand after unary operator
      } else {
        // Treat as binary operator
        Operator currOp = {c, false};
        while (!op_stack.isEmpty() && op_stack.top().value().op != '(') {
          Operator topOp = op_stack.top().value();
          // Apply operators with higher precedence or equal precedence if
          // left-associative
          if (getPrecedence(currOp) < getPrecedence(topOp) ||
              (getPrecedence(currOp) == getPrecedence(topOp) &&
               isLeftAssociative(topOp.op))) {
            if (auto result = applyOperator(); !result) {
              return std::unexpected(result.error());
            }
          } else {
            break;
          }
        }
        op_stack.push(currOp);
        expect_operand = true;  // After binary operator, expect an operand
      }
      i++;
    }
    // Handle numbers and variables
    else {
      if (!expect_operand) {
        return std::unexpected(Expr::Error::InvalidExpression);
      }
      auto tokenResult = parseToken(infix, i);
      if (!tokenResult) {
        return std::unexpected(tokenResult.error());
      }
      expr_stack.push(std::move(*tokenResult));
      expect_operand = false;  // After operand, expect an operator or end
    }
  }

  // Apply any remaining operators
  while (!op_stack.isEmpty()) {
    if (op_stack.top().value().op == '(') {
      return std::unexpected(Expr::Error::UnbalancedParentheses);
    }
    if (auto result = applyOperator(); !result) {
      return std::unexpected(result.error());
    }
  }

  // Ensure exactly one expression remains
  if (expr_stack.size() != 1) {
    return std::unexpected(Expr::Error::InvalidExpression);
  }

  return std::move(expr_stack.pop().value());
}

// Error message conversion
constexpr const std::string_view errorToString(Expr::Error error) {
  using EError = Expr::Error;
  switch (error) {
    case EError::InvalidExpression:
      return "Invalid Expr";
    case EError::InvalidOperator:
      return "Invalid Operator";
    case EError::UnbalancedParentheses:
      return "Unbalanced Paren";
    case EError::UndefinedVariable:
      return "Unknown Variable";
    case EError::DivisionByZero:
      return "Division by Zero";
  }
}

std::ostream& operator<<(std::ostream& os, const Expr::Error error) {
  return os << errorToString(error);
}
