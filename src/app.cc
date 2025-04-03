#include "app.hh"

#include <iostream>

#include "parser.hh"
#include "ui.hh"

App::App(int width, int height, const char* title)
    : window(nullptr),
      large_font(nullptr),
      button_font(nullptr),
      width(width),
      height(height),
      title(title) {}

App::~App() {
  if (window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
  }
}

bool App::initialize() {
  if (!initializeGlfw()) return false;
  window = createWindow();
  if (!window) return false;
  setupImGui();
  return true;
}

void App::run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!state.show_var_table) {
      const ImGuiIO& io = ImGui::GetIO();
      for (int i = 0; i < io.InputQueueCharacters.Size; ++i) {
        ImWchar c = io.InputQueueCharacters[i];
        if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' ||
            c == '/' || c == '(' || c == ')' || c == '.' ||
            (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
          state.updateExpression(state.expression + static_cast<char>(c));
        } else if (c == 8 && !state.expression.empty()) {
          state.updateExpression(
              state.expression.substr(0, state.expression.size() - 1));
        } else if (c == 27) {
          state.updateExpression("");
        }
      }
      const_cast<ImGuiIO&>(io).InputQueueCharacters.resize(0);
    }

    if (state.show_var_table) {
      ui::variableTable(state, 200, 340, this->getLargeFont(),
                        this->getButtonFont());
    } else {
      ui::calculator(state, 200, 340, this->getLargeFont(),
                     this->getButtonFont());
    }

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
}

bool App::initializeGlfw() {
  glfwSetErrorCallback([](int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
  });

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

  return true;
}

GLFWwindow* App::createWindow() {
  GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  return window;
}

void App::setupImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  ImFontConfig font_config;
  font_config.OversampleH = 2;
  font_config.OversampleV = 2;
  font_config.SizePixels = 28.0f;

  large_font = io.Fonts->AddFontFromFileTTF(
      "/System/Library/Fonts/SFNSMono.ttf", 28.0f, &font_config);

  font_config.SizePixels = 18.0f;
  button_font = io.Fonts->AddFontFromFileTTF(
      "/System/Library/Fonts/SFNSMono.ttf", 18.0f, &font_config);

  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.FrameRounding = 20.0f;
  style.GrabRounding = 20.0f;
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.85f);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void App::State::updateExpression(const std::string& new_expr) {
  expression = new_expr;
  display = expression.empty() ? "0" : expression;
}

void App::State::evaluate() {
  auto parseResult = parseString(expression);
  if (!parseResult) {
    display = errorToString(parseResult.error());
    return;
  }
  auto evalResult = evaluator.evaluate(*parseResult.value());
  if (evalResult) {
    display = std::to_string(*evalResult);
    display.erase(display.find_last_not_of('0') + 1, std::string::npos);
    if (display.back() == '.') display.pop_back();
  } else {
    display = errorToString(evalResult.error());
  }
}
