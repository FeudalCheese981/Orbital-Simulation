#pragma once

#include <iostream>
#include "shader.hpp"
#include <stb/stb_image.h>

class Texture
{
public:
	Texture(const char* image, const char* texType, GLuint slot); // Creates Texture from specifed image file
	~Texture(); // Deletes the Texture

	void textureUniform(Shader& shader, const char* uniform); // passes the texture to shader uniform
	const char* getTexType();
	void bind(); // Binds texture
	void unbind(); // Unbinds texture

private:
	GLuint ID; // Texture ID
	const char* type; // Type of texture for shader
	GLuint unit; // Texture slot for OpenGL Textures 
};