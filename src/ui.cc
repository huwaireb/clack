#include "ui.hh"

namespace ui {
void renderDisplay(const App::State& state, ImFont* large_font) {
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 16));
  ImGui::Dummy(ImVec2(0, 15));
  ImGui::PushFont(large_font);
  float text_width = ImGui::CalcTextSize(state.display.c_str()).x;
  float avail_width = ImGui::GetContentRegionAvail().x - 10;
  ImGui::SetCursorPosX(std::max(0.0f, avail_width - text_width));
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
  ImGui::Text("%s", state.display.c_str());
  ImGui::PopStyleColor();
  ImGui::PopFont();
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}

void button(const char* label, const ImVec4& color, ImFont* button_font,
            float width, float height, std::function<void()> action) {
  ImGui::PushFont(button_font);
  ImGui::PushStyleColor(ImGuiCol_Button, color);
  ImGui::PushStyleColor(
      ImGuiCol_ButtonHovered,
      ImVec4(color.x * 1.2f, color.y * 1.2f, color.z * 1.2f, color.w));
  ImGui::PushStyleColor(
      ImGuiCol_ButtonActive,
      ImVec4(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, color.w));
  if (ImGui::Button(label, ImVec2(width, height))) action();
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::PopFont();
}

void calculator(App::State& state, int window_width, int window_height,
                ImFont* large_font, ImFont* button_font) {
  const ImVec4 number_color(0.2f, 0.2f, 0.2f, 1.0f);
  const ImVec4 operation_color(0.8f, 0.4f, 0.0f, 1.0f);
  const ImVec4 clear_color(0.3f, 0.3f, 0.3f, 1.0f);
  const ImVec4 var_color(0.1f, 0.4f, 0.7f, 1.0f);
  const float button_width = 42.0f;
  const float button_height = 48.0f;

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window_width),
                                  static_cast<float>(window_height)));
  ImGui::Begin("Calculator", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

  renderDisplay(state, large_font);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
  ImGui::Dummy(ImVec2(0, 10));
  ImGui::Indent(2.0f);

  auto numberButton = [&](const char* label) {
    button(label, number_color, button_font, button_width, button_height,
           [&]() { state.updateExpression(state.expression + label); });
  };

  auto operationButton = [&](const char* label, char op) {
    button(label, operation_color, button_font, button_width, button_height,
           [&]() { state.updateExpression(state.expression + op); });
  };

  button("C", clear_color, button_font, button_width, button_height,
         [&]() { state.updateExpression(""); });
  button("+/-", clear_color, button_font, button_width, button_height, [&]() {
    if (!state.expression.empty() && state.expression[0] != '-') {
      state.updateExpression("-" + state.expression);
    } else if (!state.expression.empty()) {
      state.updateExpression(state.expression.substr(1));
    }
  });
  button("%", clear_color, button_font, button_width, button_height,
         [&]() { state.updateExpression(state.expression + "%"); });
  operationButton("÷", '/');
  ImGui::NewLine();

  numberButton("7");
  numberButton("8");
  numberButton("9");
  operationButton("×", '*');
  ImGui::NewLine();

  numberButton("4");
  numberButton("5");
  numberButton("6");
  operationButton("-", '-');
  ImGui::NewLine();

  numberButton("1");
  numberButton("2");
  numberButton("3");
  operationButton("+", '+');
  ImGui::NewLine();

  button("Var", var_color, button_font, button_width, button_height,
         [&]() { state.show_var_table = true; });
  numberButton("0");
  button(".", number_color, button_font, button_width, button_height,
         [&]() { state.updateExpression(state.expression + "."); });
  button("=", operation_color, button_font, button_width, button_height,
         [&]() { state.evaluate(); });
  ImGui::PopStyleVar();
  ImGui::End();
}

void variableTable(App::State& state, int window_width, int window_height,
                   ImFont* large_font, ImFont* button_font) {
  const ImVec4 back_color(0.1f, 0.4f, 0.7f, 1.0f);
  const ImVec4 add_color(0.2f, 0.6f, 0.2f, 1.0f);
  const ImVec4 use_color(0.8f, 0.4f, 0.0f, 1.0f);
  const ImVec4 delete_color(0.7f, 0.1f, 0.1f, 1.0f);

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window_width),
                                  static_cast<float>(window_height)));
  ImGui::Begin("Variables", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

  ImGui::PushFont(button_font);
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Variables");
  ImGui::SameLine(ImGui::GetWindowWidth() - 90);
  button("Back", back_color, button_font, 80, 0,
         [&]() { state.show_var_table = false; });
  ImGui::PopFont();
  ImGui::Separator();

  static std::string var_name = "";
  static double var_value = 0.0;

  ImGui::PushFont(button_font);
  ImGui::SetNextItemWidth(50);
  ImGui::InputText("##VarName", &var_name);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(70);
  ImGui::InputDouble("##VarValue", &var_value);
  ImGui::SameLine();
  button("Add", add_color, button_font, 45, 0, [&]() {
    if (!var_name.empty()) {
      state.evaluator.setVariable(var_name, var_value);
    }
  });
  ImGui::PopFont();
  ImGui::Separator();

  ImGui::BeginChild("ScrollingRegion", ImVec2(0, 220), false,
                    ImGuiWindowFlags_AlwaysVerticalScrollbar);
  ImGui::Columns(3, "variablesTable", false);
  ImGui::SetColumnWidth(0, 50);
  ImGui::SetColumnWidth(1, 50);
  ImGui::PushFont(button_font);

  auto vars = state.evaluator.variables;
  for (auto& [name, value] : vars) {
    ImGui::Text("%s", name.c_str());
    ImGui::NextColumn();

    double temp_value = value;
    std::string id = "##val" + name;
    ImGui::SetNextItemWidth(85);
    if (ImGui::InputDouble(id.c_str(), &temp_value)) {
      state.evaluator.setVariable(name, temp_value);
    }
    ImGui::NextColumn();

    button("Use", use_color, button_font, 40, 0, [&]() {
      state.updateExpression(state.expression + name);
      state.show_var_table = false;
    });
    ImGui::SameLine(0, 2);
    button("X", delete_color, button_font, 20, 0,
           [&]() { state.evaluator.variables.erase(name); });
    ImGui::NextColumn();
  }

  ImGui::PopFont();
  ImGui::Columns(1);
  ImGui::EndChild();
  ImGui::End();
}
}  // namespace ui
