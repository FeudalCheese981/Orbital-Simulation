#pragma once

#include "vertex.hpp"
#include <vector>
#include <glad/glad.h>

// Vertex Buffer Object
class VBO
{
public:
	VBO(std::vector<Vertex>& vertices); // Creates VBO with given vector list of Vertexes
	~VBO(); // Deletes The VBO

	void bind(); // Binds VBO to OpenGL
	void unbind(); // Unbinds VBO from OpenGL

private:
	GLuint ID; // VBO ID
};