#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <imgui_layer.h>
#include <application.h>
#include <clients/epic_games.h>
#include <clients/steam/steam.h>
#include <clients/ubisoft_connect.h>

#include <memory>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "addons/discord_rpc/discord_rpc.h"
#include "utils/icon_manager.h"

double ClockSeconds() {
  using Clock = std::chrono::high_resolution_clock;
  using second = std::chrono::duration<double>;
  static const auto start = Clock::now();
  return std::chrono::duration_cast<second>(Clock::now() - start).count();
}

void IdleBySleeping(FpsIdling &ioIdling) {
  ioIdling.isIdling = false;

  if ((ioIdling.fpsIdle > 0.f) && ioIdling.enableIdling) {
    double beforeWait = ClockSeconds();
    double waitTimeout = 1.0 / static_cast<double>(ioIdling.fpsIdle);

    glfwWaitEventsTimeout(waitTimeout);

    double afterWait = ClockSeconds();
    double waitDuration = afterWait - beforeWait;
    double waitIdleExpected = 1.0 / ioIdling.fpsIdle;

    ioIdling.isIdling = (waitDuration > waitIdleExpected * 0.9);
  }
}

void Application::run() {
  auto startupBegin = std::chrono::high_resolution_clock::now();

  glfwSetErrorCallback([](int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
  });
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  GLFWwindow *window = glfwCreateWindow(1779, 979, "Lazap", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD");
  }

  ImGuiLayer imgui;
  imgui.init(window);

  std::vector<std::unique_ptr<Client>> clients;
  clients.push_back(std::make_unique<Steam>());
  clients.push_back(std::make_unique<EpicGames>());
  clients.push_back(std::make_unique<UbisoftConnect>());

  std::vector<Game> games;
  for (const auto &client : clients) {
    std::vector<Game> clientGames = client->getInstalledGames();
    games.insert(games.end(), std::make_move_iterator(clientGames.begin()),
                 std::make_move_iterator(clientGames.end()));
  }

  imgui.setGames(std::move(games));
  IconManager::LoadAllIcons("src/assets/icons/");
  IconManager::LoadAllIcons("src/assets/img/");

  discord::RichPresence::Initialize("932504287337148417");
  discord::RichPresence::UpdatePresence("Lazap", "In Main Menu");

  printf("Startup took: %lld ms\n",
         std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::high_resolution_clock::now() - startupBegin)
             .count());

  //crap code starts here
  int display_w, display_h;
  Shader::InitFullscreenQuad();
  Shader::InitBlurBuffers(display_w, display_h);
  GLuint blurProgram = Shader::CreateShaderProgram("src/ui/shaders/blur.vert","src/ui/shaders/blur.frag");
  GLuint compositeProgram = Shader::CreateShaderProgram("src/ui/shaders/blur.vert","src/ui/shaders/composite.frag");

  GLuint bgTex;
  glGenTextures(1, &bgTex);
  glBindTexture(GL_TEXTURE_2D, bgTex);
  unsigned char pixel[] = { 90,110,150, 110,130,170, 90,110,150, 110,130,170 };
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,2,2,0,GL_RGB,GL_UNSIGNED_BYTE,pixel);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // enable blending once
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  RunnerState runner;
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0,0,display_w,display_h);

  // Render background into sceneFBO
  glBindFramebuffer(GL_FRAMEBUFFER, Shader::sceneFBO);
  glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
  // draw fullscreen quad with background texture:
  glUseProgram(blurProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, bgTex);
  glUniform1i(glGetUniformLocation(blurProgram,"image"), 0);
  glBindVertexArray(Shader::quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Blur: input = sceneColorTex. ApplyBlur will ping-pong into blurColorTex.
  glUseProgram(blurProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Shader::sceneColorTex);
  glUniform1i(glGetUniformLocation(blurProgram,"image"), 0);
  Shader::ApplyBlur(blurProgram, Shader::quadVAO);

  GLuint finalBlurTex = Shader::blurColorTex[0];

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0,0,0,0); glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(compositeProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, finalBlurTex);
  glUniform1i(glGetUniformLocation(compositeProgram,"blurTex"), 0);

  float titlePx = 48.0f;
  glUniform2f(glGetUniformLocation(compositeProgram,"titlebarRectMin"), 0.0f, 0.0f);
  glUniform2f(glGetUniformLocation(compositeProgram,"titlebarRectMax"), 1.0f, titlePx / (float)display_h);
  glUniform4f(glGetUniformLocation(compositeProgram,"tintColor"), 1.0f, 1.0f, 1.0f, 0.65f);
  glBindVertexArray(Shader::quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // crap code ends here

  imgui.begin();
  imgui.render();
  imgui.end();

  glfwSwapBuffers(window);
  }

  discord::RichPresence::Shutdown();
  imgui.shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();
}
