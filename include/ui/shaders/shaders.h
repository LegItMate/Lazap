#pragma once
#include "glad/glad.h"

class Shader {
 public:
	static GLuint sceneFBO;
	static GLuint sceneColorTex;
	static GLuint blurFBO[2];
	static GLuint blurColorTex[2];
	static GLuint blurShadowProgram;
	static GLuint quadVAO;
	static GLuint quadVBO;
	static void InitBlurBuffers(int width, int height);
	static void ApplyBlur(GLuint shader, GLuint quadVAO);
	static GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
	static GLuint InitFullscreenQuad();
};
