#include "ui/shaders/shaders.h"
#include "ui/panels/titlebar.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "utils/icon_manager.h"

using namespace ui;

void Titlebar::init() {
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 8.0f;
  style.FrameRounding = 5.0f;
  style.WindowPadding = ImVec2(0.0f, 5.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0, 0, 0, 0);
}

void Titlebar::render() {
    if (!visible()) return;

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet =
        ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
    ImGui::SetNextWindowClass(&window_class);

    // Transparent, no background
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin(name().c_str(), nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    float rounding = 16.0f;
    float titlebarHeight = 50.0f;

    // Draw blurred texture in titlebar area
    draw->AddImageRounded(
        (void*)(intptr_t)Shader::blurColorTex[0],
        pos,
        ImVec2(pos.x + size.x, pos.y + titlebarHeight),
        ImVec2(0, 0),
        ImVec2(1, titlebarHeight / size.y),
        IM_COL32_WHITE,
        rounding
    );

    // Add translucent overlay tint (optional, for contrast)
  ImU32 topColor = IM_COL32(255, 255, 255, 20);
  ImU32 bottomColor = IM_COL32(255, 255, 255, 5);
  draw->AddRectFilledMultiColor(
      pos,
      ImVec2(pos.x + size.x, pos.y + titlebarHeight),
      topColor, topColor, bottomColor, bottomColor
  );


    // Draw icons and buttons
    ImGui::SetCursorPos(ImVec2(10, 6));
    ImGui::Image((void*)(intptr_t)IconManager::GetIcon("lazap"), ImVec2(38, 38));

    ImGui::SameLine(ImGui::GetWindowWidth() - 144);
    if (ImGui::ImageButton("##minimise",
                           (void*)(intptr_t)IconManager::GetIcon("minimise"),
                           ImVec2(24, 24))) {
        glfwIconifyWindow(window);
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("##maximise",
                           (void*)(intptr_t)IconManager::GetIcon("maximise"),
                           ImVec2(24, 24))) {
        glfwMaximizeWindow(window);
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("##close",
                           (void*)(intptr_t)IconManager::GetIcon("close"),
                           ImVec2(24, 24))) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui::End();
}
