#pragma once

#include "fileReader.hpp"
#include <iostream>
#include <glad/glad.h>

class Shader
{
public:
	Shader(const char* vertexFile, const char* fragmentFile);
	~Shader();

	void activate(); // activates the shader
	GLuint getID(); // returns the shader ID so programs can pass uniforms

private:
	GLuint ID; // ID for shader
};