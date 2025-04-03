#include "app.cc"
#include "evaluator.cc"
#include "parser.cc"
#include "printer.cc"
#include "stack.cc"
#include "ui.cc"

int main() {
  App app;
  if (!app.initialize()) return -1;
  app.run();
  return 0;
}
