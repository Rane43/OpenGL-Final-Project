#ifndef shader_h
#define shader_h

#include <iostream>
#include <string>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>


class Shader {
private:
	unsigned int shaderProgramID;

	unsigned int CompileShaders(const char* vertexShader, const char* fragShader);

	static char* readShaderSource(const char* shaderFile);

	static void AddShader(unsigned int ShaderProgram, const char* pShaderText, GLenum ShaderType);

public:
	Shader(const char* vertexShader, const char* fragShader);

	unsigned int getShaderID();
};

#endif

