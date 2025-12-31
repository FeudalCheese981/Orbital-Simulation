#pragma once

#include <vector>
#include <glad/glad.h>

// Element Buffer Object
class EBO
{
public:
	EBO(std::vector<unsigned int>& indices); // Creates EBO with given vector list of indices
	~EBO(); // Deletes EBO

	void bind(); // binds EBO to OpenGL
	void unbind(); // unbind EBO from OpenGL

private:
	GLuint ID; // EBO ID
};