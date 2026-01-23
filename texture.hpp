#pragma once

#include <iostream>
#include "shader.hpp"
#include <stb/stb_image.h>

// Texture class - dedicated reusable class that stores a texture
class Texture
{
public:
	Texture(const char* image, const char* texType, GLuint slot); // Creates Texture from specifed image file
	~Texture(); // Deletes the Texture

	void textureUniform(Shader& shader, const char* uniform); // passes the texture to shader uniform
	const char* getTexType(); // getter for the 
	void bind(); // Binds texture to OpenGL context
	void unbind(); // Unbinds texture form OpenGL context

private:
	GLuint ID; // Texture ID
	const char* type; // Type of texture for shader
	GLuint unit; // Texture slot for OpenGL Textures 
};