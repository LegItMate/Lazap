#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "ui/shaders/shaders.h"
#include <imgui_layer.h>
#include <imgui_internal.h>
#include <utils/launch_manager.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ui/panel_manager.h"
#include "ui/themes/themes.h"
#include "utils/font_manager.h"
#include "utils/icon_manager.h"
#include "imgui.h"

void ImGuiLayer::init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.IniFilename = nullptr;

  FontManager::Init();
  FontManager::LoadFont("Username", "src/assets/fonts/Oxanium-Regular.ttf",
                        15.0f);
  FontManager::LoadFont("GameInfo:Title",
                        "src/assets/fonts/ZenDots-Regular.ttf", 64.0f);
  FontManager::LoadFont("GameInfo:Paragraph",
                        "src/assets/fonts/Nunito-Medium.ttf", 16.0f);
  FontManager::LoadFont("Left:Button", "src/assets/fonts/Oxanium-Regular.ttf",
                        16.0f);
  FontManager::LoadFont("Title", "src/assets/fonts/ArchivoBlack-Regular.ttf",
                        24.0f);
  FontManager::LoadFont("Game:Title", "src/assets/fonts/Nunito-SemiBold.ttf",
                        16.0f);
  FontManager::LoadFont("Game:Time", "src/assets/fonts/RobotoMono-Medium.ttf",
                        10.0f);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  panel_manager_ = std::make_unique<ui::PanelManager>();
  panel_manager_->initPanels(window);

  shaders_ = Shader();
  Shader::blurShadowProgram = Shader::CreateShaderProgram("src/ui/shaders/blur.vert", "src/ui/shaders/blur.frag");
  Shader::quadVAO = Shader::InitFullscreenQuad();
}

void ImGuiLayer::begin() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);

  ImGuiWindowFlags host_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoMove;

  ImGuiStyle &style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 0.0f;
  style.WindowPadding = ImVec2(2.0f, 2.0f);
  ImGui::Begin("HostWindow", nullptr, host_flags);
  Themes::setDefaultDarkColors();
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
  if (initialized_ == false) {
    initialized_ = true;
    panel_manager_->view_->MainMenu();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, shaders_.sceneFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (panel_manager_->view_->view == ui::ViewType::MainMenu) {
    DrawMainMenuBg(viewport, style);
  }

  ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0.0f, 0.0f),
                   ImGuiDockNodeFlags_None, nullptr);
  ImGui::PopStyleColor();
  ImGui::End();
}

void ImGuiLayer::render() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ImGuiWindowClass window_class;
  window_class.DockNodeFlagsOverrideSet =
      ImGuiDockNodeFlags_NoTabBar |
      (ImGuiDockNodeFlags)ImGuiDockNodeFlags_NoResize;

  panel_manager_->renderPanels(&window_class);
}

void ImGuiLayer::end() {
  shaders_.ApplyBlur(Shader::blurShadowProgram, Shader::quadVAO);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  GLFWwindow *backup_current_context = glfwGetCurrentContext();
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
  glfwMakeContextCurrent(backup_current_context);
}

void ImGuiLayer::shutdown() {
  panel_manager_->endPanels();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiLayer::setGames(std::vector<Game> games) {
  games_ = std::move(games);
  panel_manager_->setGames(&games_);
}

void DrawMainMenuBg(const ImGuiViewport *viewport, ImGuiStyle &style) {
  ImDrawList *bg = ImGui::GetWindowDrawList();
  float gameInfoY = viewport->Size.y * .43f;
  ImVec2 img_pos = ImVec2(0.0f, 0.0f);
  ImVec2 img_size = ImVec2((float)viewport->Size.x, gameInfoY);
  bg->AddImage((ImTextureID)(intptr_t)IconManager::GetIcon("banner"), img_pos,
               img_size, ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE);
  bg->AddRectFilledMultiColor(img_pos, img_size, IM_COL32(0, 0, 0, 0),
                              IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 255),
                              IM_COL32(0, 0, 0, 255));
  style.Colors[ImGuiCol_WindowBg].w = 0.0f;
}
