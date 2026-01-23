#pragma once

#include "fileReader.hpp"
#include <iostream>
#include <glad/glad.h>

// Shader class - shader loading and activation for OpenGL
class Shader
{
public:
	Shader(const char* vertexFile, const char* fragmentFile); // load shaders into shaderProgram
	~Shader();

	void activate(); // activates the shader
	GLuint getID(); // returns the shader ID so programs can pass uniforms

private:
	GLuint ID; // Shader program ID
};