#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include <string>

#include "evaluator.hh"
#include "printer.hh"

class App {
 public:
  App(int width = 200, int height = 340, const char* title = "Calculator");
  ~App();

  bool initialize();
  void run();

  struct State {
    std::string expression = "";
    std::string display = "0";
    bool show_var_table = false;
    Evaluator evaluator;
    Printer printer;

    void updateExpression(const std::string& new_expr);
    void evaluate();
  };

  State& getState() { return state; }
  GLFWwindow* getWindow() { return window; }
  ImFont* getLargeFont() { return large_font; }
  ImFont* getButtonFont() { return button_font; }

 private:
  GLFWwindow* window;
  ImFont* large_font;
  ImFont* button_font;
  State state;
  const char* glsl_version = "#version 150";
  int width;
  int height;
  const char* title;

  bool initializeGlfw();
  GLFWwindow* createWindow();
  void setupImGui();
};
