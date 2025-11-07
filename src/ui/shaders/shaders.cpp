#include "ui/shaders/shaders.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

GLuint Shader::sceneFBO = 0;
GLuint Shader::sceneColorTex = 0;
GLuint Shader::blurFBO[2] = {0, 0};
GLuint Shader::blurColorTex[2] = {0, 0};
GLuint Shader::blurShadowProgram;
GLuint Shader::quadVBO;
GLuint Shader::quadVAO;
void Shader::InitBlurBuffers(int width, int height) {
  // Scene framebuffer
  glGenFramebuffers(1, &sceneFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

  glGenTextures(1, &sceneColorTex);
  glBindTexture(GL_TEXTURE_2D, sceneColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         sceneColorTex, 0);

  // Two blur passes (ping-pong)
  glGenFramebuffers(2, blurFBO);
  glGenTextures(2, blurColorTex);
  for (int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
    glBindTexture(GL_TEXTURE_2D, blurColorTex[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           blurColorTex[i], 0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shader::ApplyBlur(GLuint shader, GLuint quadVAO) {
  bool horizontal = true, first_iteration = true;
  int amount = 8;  // blur strength

  glUseProgram(shader);
  for (int i = 0; i < amount; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[horizontal]);
    glUniform1i(glGetUniformLocation(shader, "horizontal"), horizontal);
    glBindTexture(GL_TEXTURE_2D,
                  first_iteration ? sceneColorTex : blurColorTex[!horizontal]);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    horizontal = !horizontal;
    if (first_iteration) first_iteration = false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Shader::CreateShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    auto LoadShaderSource = [](const char* path) -> std::string {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << path << std::endl;
            return "";
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    };

    std::string vertexCode = LoadShaderSource(vertexPath);
    std::string fragmentCode = LoadShaderSource(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    GLint success;
    char infoLog[512];

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "Vertex shader compile error: " << infoLog << std::endl;
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "Fragment shader compile error: " << infoLog << std::endl;
    }

    // Shader program
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "Shader linking error: " << infoLog << std::endl;
    }

    // Cleanup
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

GLuint Shader::InitFullscreenQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &Shader::quadVAO);
    glGenBuffers(1, &Shader::quadVBO);
    glBindVertexArray(Shader::quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, Shader::quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return quadVAO;
}