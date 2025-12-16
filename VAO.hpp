#pragma once

#include "VBO.hpp"

// Vertex Array Object
class VAO
{
public:
	VAO(); // Creates VAO
	~VAO(); // Deletes VAO

	void linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset); // sets buffer array attributes for given VBO
	void bind(); // Binds VAO to OpenGL
	void unbind(); // Unbind VAO from OpenGL

private:
	GLuint ID;
};