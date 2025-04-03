#pragma once

#include <imgui.h>

#include <functional>

#include "app.hh"

namespace ui {
void renderUI(App& app);
void renderDisplay(const App::State& state, ImFont* large_font);
void button(const char* label, const ImVec4& color, ImFont* button_font,
            float width, float height, std::function<void()> action);
void calculator(App::State& state, int window_width, int window_height,
                ImFont* large_font, ImFont* button_font);
void variableTable(App::State& state, int window_width, int window_height,
                   ImFont* large_font, ImFont* button_font);
}

;  // namespace ui
